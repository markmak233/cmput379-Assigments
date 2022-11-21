#include "client.h"

using namespace std;

void init_machine(vector<string> us1, char* portn, char* ipaddress){
    for (unsigned int ind=0;ind<us1.size();ind++){
        cout << us1[ind] << endl;
    }

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
    //for i in range(data_cp->uop1.size()){
    //
    //}
    cout <<"asdfdas"<<endl;
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





