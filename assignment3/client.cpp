#include "client.h"
#include "tands.h"
#include "tands.cpp"

using namespace std;

void init_machine(vector<string> us1, char* portn, char* ipaddress){

    //set up filename and hostname
    char hostname[100];
    size_t namelen=100;
    int mypid=getpid();
    gethostname(hostname,namelen);
    string filename=(hostname);
    filename.append(".");
    filename.append(to_string(mypid));

    // setup sender info
    struct sharing_data children_data;
    queue<log_data> log_d;
    vector<string> log_s;
    children_data.v_op1=&log_d;
    children_data.uop1=us1;
    children_data.portn=portn;
    children_data.ip=ipaddress;
    children_data.filename=filename;
    
    //write the headline
    string l1 ="Using port ";
    l1.append(portn);
    l1.append("\n");
    string l2 = "Using server address ";
    l2.append(ipaddress);
    l2.append("\n");
    string l3="Host ";
    l3.append(filename);
    l3.append("\n");
    writefile(filename,l1);
    writefile(filename,l2);
    writefile(filename,l3);

    int sentnum=task_sender(children_data);

    // write summary message
    string l4 = "Sent ";
    l4.append(to_string(sentnum));
    l4.append(" transactions\n");
    writefile(filename,l4);

    //clear or the string
    l1.clear();
    l2.clear();
    l3.clear();
    l4.clear();

    log_s.clear();
    filename.clear();
}

void translate(string filename,struct log_data d1){
    // translate the infomation to a line of string
    char temp[70];
    if (d1.task_type!="S"){
        int num=stoi(d1.task_num);
        sprintf(temp,"%.2f: %s (%s %3d)\n",d1.current_time,d1.log_type.data(),
                                        d1.task_type.data(),num);
        string a1=temp;
        writefile(filename,a1);
    } else {
        sprintf(temp,"Sleep %s units\n",d1.task_num.data());
        string a1=temp;
        writefile(filename,a1);
    }
    d1.log_type.clear();
    d1.task_num.clear();
    d1.task_type.clear();
    return;
}

void writefile(string filename,string a1){
    // write a string append to a file
    ofstream fout;
    ifstream fin;
    fin.open(filename);
    fout.open(filename,ios_base::app);
    if (fout.is_open()){
        fout << a1;
    }
    fin.close();
    fout.close();
    a1.clear();
    return;
}

int task_sender(struct sharing_data data_cp){
    //current timing
    struct timeval etime;
    int sentnum=0;
    //setup send format
    string mestemplate=data_cp.filename;
    //runthrough each line
    for (unsigned int i=0;i<data_cp.uop1.size();i++){
        stringstream ss2;
        string strline,typeline;
        ss2 << data_cp.uop1.at(i);
        ss2 >> strline;
        typeline=strline[0];
        
        if (typeline !=  "T" && typeline!= "S"){
            // if either T or S run nextone
            continue;
        } else if (typeline == "S"){
            // string conversion
            string temp = strline;
            temp.erase(0,1);
            int runnum=0;
            stringstream ss3;
            ss3 << temp;
            ss3 >> runnum;
            if (runnum<1 || runnum>100){
                runnum=1;
            }

            // logging
            struct log_data templog;
            templog.task_type="S";
            templog.task_num=temp;
            templog.log_type="Sleep";
            gettimeofday(&etime,NULL);
            templog.current_time=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
            data_cp.v_op1->push(templog);

            // sleep for units
            Sleep(runnum);
        } else {
            //https://www.geeksforgeeks.org/socket-programming-cc/
            int sockfd =0,client_fd;
            
            struct sockaddr_in serv_addr;
            string message = data_cp.uop1.at(i);
            message.erase(0,1);

            // logging
            struct log_data templog;
            templog.task_type="T";
            templog.task_num=message;
            templog.log_type="Send";
            gettimeofday(&etime,NULL);
            templog.current_time=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
            data_cp.v_op1->push(templog);

            char buffer[1024]={0};
            char buffer2[1024]={0};
            if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
                cout << "Socket creadtion uncessful" << endl;
            } 
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(atoi(data_cp.portn)); 

            // // Convert IPv4 and IPv6 addresses from text to binary
            if (inet_pton(AF_INET, data_cp.ip, &serv_addr.sin_addr)<= 0) {
                printf("\nInvalid address/ Address not supported \n");
            }

            int trial=0;
            while (trial==0){
                // try to connect if bussy it will try again
                if ((client_fd = connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))< 0) {
                    Sleep(1);
                    continue;
                }else {
                    // sent client information
                    send(sockfd, mestemplate.data(), mestemplate.size(), 0);
                    
                    // useless receiving.reduce wait time
                    if (read(sockfd, buffer2, 1024)<0){
                        perror("read");
                    }

                    // send working number
                    send(sockfd, message.data(), message.size(), 0);
                    
                    // waiting for workdone
                    if( read(sockfd, buffer, 1024) <0){
                        perror("read");
                    }
                    close(client_fd);

                    trial=1;
                    string runnum=buffer;

                    // logging
                    struct log_data templog;
                    templog.task_type="D";
                    templog.task_num=runnum;
                    templog.log_type="Recv";
                    gettimeofday(&etime,NULL);
                    templog.current_time=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
                    data_cp.v_op1->push(templog);
                }
            }
            //write all the file into log
            while (!data_cp.v_op1->empty()){
                translate(data_cp.filename,data_cp.v_op1->front());
                data_cp.v_op1->pop();
            }
            sentnum++;
        }   
    }
    return sentnum;
}

int main(int argc,char* argv[]){
    if (argc!=3){
        cout << "client port ipaddress"<< endl;
        return 1;
    }
    
    int portn=atoi(argv[1]);
    if (portn>64000 || portn<5000){
        cout << "port out of range" << endl;
        return 1;
    }

    // conver input to string vector
    vector<string> us1;
    int running=1;
    while (running){
        string as;
        cin >> as;
        if (as.size()!=0){
            us1.push_back(as);
        }else{
            running=0;
        }
    }
    init_machine(us1,argv[1],argv[2]);
    return 1;
}





