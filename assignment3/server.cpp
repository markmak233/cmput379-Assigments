#include "server.h"

using namespace std;

void init_machine(char* portn){
    queue<log_data> lg1;
    //timecounting
    struct timeval stime;
    struct timeval etime;
    map<string,int> orgincout;
    sem_t semph1;
    sem_init(&semph1,1,1);

    struct sharing_data2 shdata2;
    shdata2.v_op1=&lg1;
    shdata2.v_semph1=&semph1;
    shdata2.ogct=&orgincout;

    string l1="using port ";
    string filename = "server.log";
    l1.append(portn);
    l1.append("\n");
    writefile(filename,l1);

    pthread_t p;
    pthread_create(&p,NULL,file_writer,&shdata2);

    int waittime_notexpire=1;

    cout << "running T" << endl;
    //https://www.geeksforgeeks.org/socket-programming-cc/
    int server_fd,new_socket;
    struct sockaddr_in address;

    //timeout settings;
    fd_set fd;
    timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;


    int addrlen = sizeof(address);
   
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        //exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(portn)); //hardcodeed change later //////////////////////////////////////////

    // https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
    // timeout in 30 second
    setsockopt(server_fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv));

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int Work_done=1;
    string useless = "useless";

    while (waittime_notexpire){
        //timeout settings
        struct log_data temp1,temp2;
        temp1.task_id = Work_done;
        temp1.task_type = "start";
        string message =to_string(Work_done);
        char buffer[1024]={0};
        char buffer2[1024]={0};
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
            perror("accept");
            waittime_notexpire=0;
            continue;
        }
        if (Work_done==1){
            gettimeofday(&stime,NULL);
        }
        if( read(new_socket, buffer, 1024)<0){
            perror("read");
        }
        temp1.orgin = buffer;

        //useless sending,reduce wait time
        send(new_socket, useless.data(), useless.size(), 0);

        if (read(new_socket, buffer2, 1024)<0){
            perror("read");
        }
        int runnum=atoi(buffer2);
        temp1.task_num=runnum;
        gettimeofday(&etime,NULL);
        temp1.currenttime=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
        
        // new one just little edit
        temp2=temp1;

        cout <<(runnum) << endl;
        Trans(runnum);
        // sent number back when workdone;
        
        
        send(new_socket, message.data(), message.size(), 0);
        // closing the connected socket
        close(new_socket);

        // closing the listening socket
        gettimeofday(&etime,NULL);
        temp1.task_type = "Done";
        temp1.currenttime=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
        sem_wait(&semph1);
        lg1.push(temp2);
        lg1.push(temp1);
        sem_post(&semph1);
        Work_done++;
    }

    shutdown(server_fd, SHUT_RDWR);

    sem_wait(&semph1);
    shdata2.terminate=1;
    sem_post(&semph1);

    cout << "wait time for 30 second is now expire" << endl;

    string l2 = "\nSUMMARY\n";
    int total=0;
    writefile(filename,l2);
    map<string, int>::iterator it;
    for (it = orgincout.begin(); it != orgincout.end(); it++){
        char temp[100];
        sprintf(temp,"%4d transactions from %s \n",it->second,it->first.data());
        total=total+it->second;
        string temp2=temp;
        writefile(filename,temp2);
    }
    Work_done--;
    string l3;
    if (Work_done==0){
        l3="transactions/sec unaviable.\n";
    } else {
        double total_runtime=((etime.tv_sec - stime.tv_sec) * 1000 + (etime.tv_usec-stime.tv_usec)/1000.0)/1000;
        double transpersec = Work_done/total_runtime;
        char temp[100];
        sprintf(temp,"%.1f transactions/sec  (%d/%.2f)\n",transpersec,Work_done,total_runtime);
        l3=temp;
    }
    writefile(filename,l3);
}



void *file_writer(void *data2){
    struct sharing_data2 *data2_cp = (struct sharing_data2*)data2;
    queue<log_data> tempqlog;
    int ready_to_end=0,st;
    while (!ready_to_end){
        sem_getvalue((data2_cp->v_semph1),&st);
        if(st){
            sem_wait((data2_cp->v_semph1));
            while (!data2_cp->v_op1->empty()){
                tempqlog.push(data2_cp->v_op1->front());
                data2_cp->v_op1->pop();
            }
            ready_to_end=data2_cp->terminate;
            sem_post((data2_cp->v_semph1));
        }else {
            //Sleep(1);
        }
        while (!tempqlog.empty()){
            struct log_data templg=tempqlog.front();
            char temp[100];
            if (templg.task_type=="start"){
                sprintf(temp,"%.2f: #%3d (T%3d) from %s \n",templg.currenttime,
                templg.task_id,templg.task_num,templg.orgin.data());
            }else{
                sprintf(temp,"%.2f: #%3d (Done) from %s \n",templg.currenttime,
                templg.task_id,templg.orgin.data());
                if (data2_cp->ogct->count(templg.orgin)>0){
                    data2_cp->ogct->at(templg.orgin)=data2_cp->ogct->at(templg.orgin)+1;
                }else{
                    data2_cp->ogct->insert({templg.orgin,1});
                }
            }
            string dataline = temp;
            string filename = "server.log";
            writefile(filename,dataline);
            tempqlog.pop();

        }
    }
    pthread_exit(NULL);
    return data2;
}

void writefile(string filename,string a1){
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


int main(int argc,char* argv[]){
    if (argc!=2){
        cout << "client port"<< endl;
        return 1;
    }

    int portn=atoi(argv[1]);
    if (portn>64000 || portn<5000){
        cout << "out of range" << endl;
        return 1;
    }

    init_machine(argv[1]);
    return 1;
}