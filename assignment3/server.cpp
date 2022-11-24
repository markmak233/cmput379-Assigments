#include "server.h"

using namespace std;

void init_machine(char* portn){
    queue<log_data> lg1;
    //timecounting
    struct timeval stime;
    struct timeval rtime;
    struct timeval etime;
    // count for each orgin
    map<string,int> orgincout;
    sem_t semph1;
    sem_init(&semph1,1,1);

    // setup file write semphore and writer
    struct sharing_data2 shdata2;
    shdata2.v_op1=&lg1;
    shdata2.v_semph1=&semph1;
    shdata2.ogct=&orgincout;

    // print the top line
    string l1="using port ";
    string filename = "server.log";
    l1.append(portn);
    l1.append("\n");
    writefile(filename,l1);
    l1.clear();

    // start children port
    pthread_t p;
    pthread_create(&p,NULL,file_writer,&shdata2);

    int waittime_notexpire=1;

    //https://www.geeksforgeeks.org/socket-programming-cc/
    int server_fd,new_socket;
    struct sockaddr_in address;

    //timeout settings for count down;
    fd_set fd;
    FD_ZERO(&fd);
    timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;


    int addrlen = sizeof(address);
   
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        string l3 = "socket failed";
        writefile(filename,l3);
        exit(EXIT_FAILURE);
        l3.clear();
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(atoi(portn)); 

    // https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
    // timeout in 30 second
    setsockopt(server_fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv));

    // bind a port
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) {
        string l3 = "bind failed program terminated";
        writefile(filename,l3);
        exit(EXIT_FAILURE);
        l3.clear();
    }

    // allow up to 60 client waiting 
    if (listen(server_fd, 60) < 0) {
        perror("listen program terminated");
        exit(EXIT_FAILURE);
    }

    int Work_done=1;
    string useless = "useless";
    // run infininity until 30 second of no package sent
    while (waittime_notexpire){
        //timeout settings
        struct log_data temp1,temp2;
        temp1.task_id = Work_done;
        temp1.task_type = "start";
        string message =to_string(Work_done);
        char buffer[1024]={0};
        char buffer2[1024]={0};
        // waiting fof new socket connect
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
            // end condition
            if (rtime.tv_usec==0){
                gettimeofday(&rtime,NULL);
                perror("connect");
            }
            waittime_notexpire=0;
            continue;
        }
        // save the start time
        if (Work_done==1){
            gettimeofday(&stime,NULL);
        }
        //read the client name
        if( read(new_socket, buffer, 1024)<0){
            perror("read");
        }
        temp1.orgin = buffer;

        //useless sending,reduce wait time
        send(new_socket, useless.data(), useless.size(), 0);

        // read the "Txxxx" number
        if (read(new_socket, buffer2, 1024)<0){
            perror("read");
        }
        // log && run
        int runnum=atoi(buffer2);
        temp1.task_num=runnum;
        gettimeofday(&etime,NULL);
        temp1.currenttime=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
        // new one just little edit
        temp2=temp1;
        Trans(runnum);

        // sent number back when workdone;
        send(new_socket, message.data(), message.size(), 0);
        // closing the connected socket
        close(new_socket);
        message.clear();

        // closing the listening socket 
        gettimeofday(&etime,NULL);
        temp1.task_type = "Done";
        temp1.currenttime=((etime.tv_sec ) * 1000 + (etime.tv_usec)/1000.0)/1000;
        // sync and let other thread to process
        sem_wait(&semph1);
        lg1.push(temp2);
        lg1.push(temp1);
        sem_post(&semph1);
        Work_done++;
    }

    useless.clear();
    FD_CLR(0,&fd);
    shutdown(server_fd, SHUT_RDWR);
    // tell fileserver shutdown
    sem_wait(&semph1);
    shdata2.terminate=1;
    sem_post(&semph1);
    // waiting writer to end.and avoid memory leak
    // https://stackoverflow.com/questions/17642433/why-pthread-causes-a-memory-leak
    while (pthread_detach(p)!=0);
    // print summary information
    string l2 = "\nSUMMARY\n";
    int total=0;
    writefile(filename,l2);
    l2.clear();
    // recycle code from assignment 2
    map<string, int>::iterator it;
    for (it = orgincout.begin(); it != orgincout.end(); it++){
        char temp[100];
        sprintf(temp,"%4d transactions from %s \n",it->second,it->first.data());
        total=total+it->second;
        string temp2=temp;
        writefile(filename,temp2);
        temp2.clear();
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
 
    sem_destroy(&semph1);
    writefile(filename,l3);
    l3.clear();
    filename.clear();
    orgincout.clear();
}



void *file_writer(void *data2){
    // concurrenery running
    struct sharing_data2 *data2_cp = (struct sharing_data2*)data2;
    queue<log_data> tempqlog;
    string filename = "server.log";
    int ready_to_end=0,st;
    // run infinitly until host till it to stop
    while (!ready_to_end){
        sem_getvalue((data2_cp->v_semph1),&st);
        if(st){
            // copy all the exise not process log
            sem_wait((data2_cp->v_semph1));
            while (!data2_cp->v_op1->empty()){
                tempqlog.push(data2_cp->v_op1->front());
                data2_cp->v_op1->pop();
            }
            ready_to_end=data2_cp->terminate;
            sem_post((data2_cp->v_semph1));
        }
        // translate and write it into file
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
            writefile(filename,dataline);
            tempqlog.pop();
            dataline.clear();
            templg.orgin.clear();
            templg.task_type.clear();
        }
    }
    filename.clear();
    pthread_exit(NULL);
    return data2;
}

void writefile(string filename,string a1){
    //write string to file
    ofstream fout;
    ifstream fin;
    fin.open(filename);
    fout.open(filename,ios_base::app);
    if (fout.is_open()){
        fout << a1;
    }
    fin.close();
    fout.close();
    // clean the data.
    a1.clear();
    filename.clear();
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