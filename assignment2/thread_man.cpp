#include "thread_man.h"
#include "tands.h"


using namespace std;

vector<inst_kind> translate_txt_to_struct(vector<string> instru){
    vector<inst_kind> transed_instru;
    for (int ind=0;(unsigned)ind<instru.size()-1;ind++){
        struct inst_kind temp1;
        temp1.TS=instru[ind][0];
        string temp2 = instru[ind];
        temp2.erase(0,1);

        // https://www.educative.io/answers/how-to-convert-a-string-to-an-int-in-cpp
        stringstream ss;
        ss << temp2;
        ss >> temp1.numb;

        transed_instru.push_back(temp1);
    }
    return transed_instru;
}

vector<string> log_event_convert(vector<log_event> log2){
    vector<string> s1;
    // struct log_event{
    // double currentTime;
    // int tid;
    // int queue;
    // std::string Status;
    // int run_num;
    for (int i=0;(unsigned)i<log2.size();i++){
        //https://www.programiz.com/cpp-programming/library-function/cstdio/sprintf
        char temp[100];
        string temstr;
        stringstream ss2;
        ss2 << log2[i].currentTime << " "
    
        s1.push_back(temstr);
    }
    return s1;
}

vector<string> event_management(int nThread,vector<string> instru){
    // this function many logging the event of each device
    vector<inst_kind> translated_instru;
    translated_instru=translate_txt_to_struct(instru);
    //cout << "result" << endl;
    // result output
    for (int i=0;(unsigned)i<translated_instru.size();i++){
        cout << instru[i] <<" "<< translated_instru[i].TS << " " << translated_instru[i].numb << endl;
    }
    //cout << "data displed" << endl;
    //start a vector that contain the share memory information
    // where this log devices also able to access
    vector<children_thread*> childthinfo;
    vector<sem_t> sema;
    sem_t sp;
    sem_init(&sp,1,1);
    sema.push_back(sp);
    for (int i=0;i<nThread;i++){
        struct children_thread* temp,temp2;
        temp=&temp2;
        temp->tid=i+1;
        temp->status="Ask";
        temp->last_saved_status="None";
        temp->isnewWork=0;
        temp->newWorknum=0;
        childthinfo.push_back(temp);
        // setup semaphore
        sem_t temp3;
        sem_init(&temp3,1,1);
        temp->semaph=&temp3;
        //push into list
        sema.push_back(temp3);
    }

    // creating Parent infomation
    struct main_kid* Parent,Parent2;
    Parent=&Parent2;
    Parent->tid=0;
    Parent->nth=nThread;
    Parent->workingnum=0;
    Parent->status="Running";
    Parent->last_saved_status="None";
    Parent->childThread=&childthinfo;
    Parent->instructions=&translated_instru;
    Parent->semaph=&sema;

    vector<log_event> log1;
    time_t init_log_time=time(0);
    //preation done from here


    cout << "start Parent thread" << endl;
    //creating main parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,Parent);
    //pthread_join(p,NULL);
    int checking=1;
    while (checking){
        sem_wait(&(Parent->semaph->at(0)));
        if(Parent->status=="End"){
            checking=0;
            cout << "thread end" << endl;
        } else if (Parent->status!=Parent->last_saved_status){
            //cout << "status change to " << Parent->status << endl;
            //cout << "working on" << Parent->workingnum << endl;
            if (Parent->status!="Running"){
                struct log_event temp_log;
                temp_log.Status=Parent->status;
                temp_log.tid=Parent->tid;
                temp_log.run_num=Parent->workingnum;
                temp_log.currentTime=time(0)-init_log_time;
                log1.push_back(temp_log);
            }
            Parent->last_saved_status=Parent->status;
            
        }
        sem_post(&(Parent->semaph->at(0)));
    }
    cout << Parent->tid << endl;
    //cout << "end Parent Process" << endl;
    vector<string> op1;
    return op1;

    
}

void *Parent_thread(void *data){
    // rocover from the data
    struct main_kid *data_cp = (struct main_kid*)data;
    sem_wait(&(data_cp->semaph->at(0)));
    int myid=data_cp->tid;
    sem_post(&(data_cp->semaph->at(0)));
    // int en =0;
    // while (en==0){
    //     sem_getvalue(&(data_cp->semaph->at(0)),&en);
    // }
    // cout << en << endl;
    // sem_wait(&(data_cp->semaph->at(0)));
    // data_cp->tid=123;
    // sem_post(&(data_cp->semaph->at(0)));
    // cout<<"asd"<<endl;
    

    
    for (int i=0;(unsigned)i<data_cp->instructions->size();i++) {
        // if sleeping requested
        if (data_cp->instructions->at(i).TS=="S"){
            // start signaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Sleeping";
            data_cp->workingnum=data_cp->instructions->at(0).numb;
            sem_post(&(data_cp->semaph->at(myid)));

            // testing purpose showing
            cout << data_cp->instructions->at(i).TS << " " << data_cp->instructions->at(i).numb << endl;
            Sleep(data_cp->instructions->at(i).numb);
            cout << "Completed next" << endl;

            //sleeping done singaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Running";
            data_cp->workingnum=0;
            sem_post(&(data_cp->semaph->at(myid)));
        }
        //if trans requested
    }
    cout << "term-ing" << endl;

    sem_wait(&(data_cp->semaph->at(myid)));
    data_cp->status="End";
    sem_post(&(data_cp->semaph->at(myid)));
    cout << "bye" << endl;
    pthread_exit(NULL);
    return data;
}
