#include "server.h"

using namespace std;

void init_machine(char* portn){
    vector<log_data> lg1;
    sem_t semph1,semph2;
    sem_init(&semph1,1,1);
    sem_init(&semph2,1,1);
    int portnum;
    stringstream ss2;
    ss2 << portn;
    ss2 >> portnum;
    queue<int> work;
    struct sharing_data1 shdata1;
    shdata1.v_op1=&lg1;
    shdata1.v_semph1=&semph1;
    shdata1.portn=portnum;


    struct sharing_data2 shdata2;
    shdata2.v_op1=&lg1;
    shdata2.Work=&work;
    shdata2.v_semph1=&semph1;
    shdata2.v_semph2=&semph2;
    shdata2.portn=portnum;

    //pthread_t p1;
    //pthread_create(&p1,NULL,childhandling,&shdata1);
    //pthread_t p2;
    //pthread_create(&p2,NULL,file_writer,&shdata2);

    int waittime_notexpire=1;

    cout << "running T" << endl;
    //https://www.geeksforgeeks.org/socket-programming-cc/
    int server_fd,new_socket,valread;
    struct sockaddr_in address;

    //timeout settings;
    fd_set fd;
    timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;



    int addrlen = sizeof(address);
    char buffer[1024]={0};
   
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        //exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5003); //hardcodeed change later //////////////////////////////////////////

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

    int Work_done=0;

    while (waittime_notexpire){
        //timeout settings
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) {
            perror("accept");
            waittime_notexpire=0;
            continue;
        }
        valread = read(new_socket, buffer, 1024);
        cout << buffer << endl;
        
        Work_done++;
        string message =to_string(Work_done);
        send(new_socket, message.data(), message.size(), 0);
        printf("Hello message sent\n");

        // closing the connected socket
        close(new_socket);
        // closing the listening socket
       
        cout << "new loop" << endl;
    }

    shutdown(server_fd, SHUT_RDWR);
    cout << "wait time for 30 second is now expire" << endl;

}
void *childhandling(void *data){
    pthread_exit(NULL);
    return data;
}

void *file_writer(void *data2){
    pthread_exit(NULL);
    return data2;
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