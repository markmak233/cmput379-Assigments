#include "client.h"
#include "tands.h"

using namespace std;

void init_machine(vector<string> us1, char* portn, char* ipaddress){

    struct sharing_data children_data;
    vector<log_data> log_d;
    sem_t semph1;
    sem_init(&semph1,1,1);
    children_data.v_semph1=&semph1;
    children_data.v_op1=&log_d;
    children_data.uop1=us1;
    children_data.portn=portn;
    children_data.ip=ipaddress;
        

    pthread_t p;
    pthread_create(&p,NULL,task_sender,&children_data);
    pthread_join(p,NULL);
}

void *task_sender(void *data){
    struct sharing_data *data_cp = (struct sharing_data*)data;
    //setup send format
    int mypid=getpid();
    size_t namelen;
    char hostname[100];
    string mestemplate;
    mestemplate=gethostname(hostname,namelen);
    mestemplate.append(";");
    mestemplate.append(to_string(mypid));
    mestemplate.append(";");
    for (unsigned int i=0;i<data_cp->uop1.size();i++){
        stringstream ss2;
        string strline,typeline;
        ss2 << data_cp->uop1.at(i);
        ss2 >> strline;
        typeline=strline[0];
        
        cout << "Working on "<< i << endl;


        if (typeline !=  "T" && typeline!= "S"){
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

            //Sleep(runnum);
        } else {
            //https://www.geeksforgeeks.org/socket-programming-cc/
            int sockfd =0,valread,client_fd;
            
            struct sockaddr_in serv_addr;
            string message =mestemplate; 
            string temp = strline;
            temp.erase(0,1);
            message.append(temp);

            char buffer[1024]={0};
            if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
                cout << "Socket creadtion uncessful" << endl;
            } 
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(5003); // tempory hardcode ////////////////////////////////

            // // Convert IPv4 and IPv6 addresses from text to binary
            if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<= 0) {
                printf("\nInvalid address/ Address not supported \n");
            }

            int trial=0;
            while (trial==0)
            // try to connect if bussy it will try again
            if ((client_fd = connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))< 0) {
                continue;
            }else {
                send(sockfd, message.data(), message.size(), 0);
                printf("Hello message sent\n");
                valread = read(sockfd, buffer, 1024);
                printf("%s\n", buffer);
                valread=0;
                close(client_fd);
                trial=100;
                trial=1;
            }

        }
        

    }
    pthread_exit(NULL);
    return data;
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





