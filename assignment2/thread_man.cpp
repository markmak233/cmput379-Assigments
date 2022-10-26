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

vector<string> log_event_convert(vector<log_event> tlog2){
    vector<string> s1;
    // struct log_event{
    // double currentTime;
    // int tid;
    // int queue;
    // std::string Status;
    // int run_num;
    for (int i=0;(unsigned)i<tlog2.size();i++){
        string tempstr;
        char temp[100];
        sprintf(temp,"%2.3f ID=%2d Q=%2d ",tlog2[i].currentTime,tlog2[i].tid,tlog2[i].queue);
        tempstr.append(temp);

        tlog2[i].Status.resize(12*sizeof(char));
        tempstr.append(tlog2[i].Status);
        char temp2[50];
        sprintf(temp2,"%2d   //",tlog2[i].run_num);
        tempstr.append(temp2);
        tempstr.append("\n");
        s1.push_back(tempstr);
        
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
    vector<children_thread> childthinfo;
    vector<sem_t> sema;
    vector<int> ted_thread;
    // main Parent semphore init
    sem_t sp;
    sem_init(&sp,1,1);
    sema.push_back(sp);
    // children info setup and semphore init
    for (int i=0;i<nThread;i++){
        struct children_thread temp;
        temp.tid=i+1;
        temp.status="Ask";
        temp.last_saved_status="None";
        temp.isnewWork=0;
        temp.newWorknum=0;
        
        // setup semaphore
        sem_t temp3;
        sem_init(&temp3,1,1);
        vector<sem_t> mysem;
        mysem.push_back(temp3);
        temp.semaph2=mysem;
        cout <<"sem size "<< temp.semaph2.size() << endl;
        //push into list
        sema.push_back(temp3);
        temp.nomorework=0;
        childthinfo.push_back(temp);
    }

    vector<children_thread*> childthinfo_vector;
    for (int i=0;i<(signed)childthinfo.size();i++){
        childthinfo_vector.push_back(&childthinfo[i]);
    }

    // creating Parent infomation
    struct main_kid* Parent,Parent2;
    Parent=&Parent2;
    Parent->tid=0;
    Parent->nth=nThread;
    Parent->workingnum=0;
    Parent->status="Running";
    Parent->last_saved_status="None";
    Parent->childThread=&childthinfo_vector;
    Parent->instructions=&translated_instru;
    Parent->semaph=&sema;

    vector<log_event> thlog1;
    // https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    auto init_log_time=chrono::system_clock::now();
    int free_queue=nThread;
    int parent_end=0;
    int terminated_thread=0;
    //preation done from here

    cout << "starting children thread" << endl;
    
    vector<pthread_t> bkpt;
    cout << "size "<<childthinfo.size() << endl;
    for(int i=0;i<(signed)childthinfo.size();i++){
        // start second children sephnore
        cout << i << endl;
        // start pthread
        pthread_t cp;
        cout <<i << " cthread"<< childthinfo[i].semaph2.size() << endl;
        pthread_create(&cp,NULL,Children_run_thread,childthinfo_vector[i]);
        pthread_join(cp,NULL);
        bkpt.push_back(cp);
        cout << i << endl;
    }

    cout << "start Parent thread" << endl;
    //creating main parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,Parent);
    
    
    int checking=1;
    while (checking){
        auto now_time =chrono::system_clock::now();
        // data the associate with parent
        sem_wait(&(Parent->semaph->at(0)));
        if(Parent->status=="End" && !parent_end){
            cout << "added" << endl;
            terminated_thread++;
            parent_end=1;
            cout << "thread end" << terminated_thread <<endl;
        } else if (Parent->status!=Parent->last_saved_status){
            //cout << "status change to " << Parent->status << endl;
            //cout << "working on" << Parent->workingnum << endl;
            if (Parent->status!="Running"){
                struct log_event temp_log;
                temp_log.Status=Parent->status;
                temp_log.tid=Parent->tid;
                temp_log.run_num=Parent->workingnum;

                //https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
                chrono::duration<double> durtime=now_time-init_log_time;
                temp_log.currentTime=durtime;

                temp_log.queue=free_queue;
                thlog1.push_back(temp_log);
            }
            Parent->last_saved_status=Parent->status;
            
        }
        sem_post(&(Parent->semaph->at(0)));



        //checking if thread ended,if all thread ended stop logging

        for(int i=0;(unsigned)i<childthinfo.size();i++){
            if (!binary_search(ted_thread.begin(),ted_thread.end(),i)){
                cout<< terminated_thread << endl;
                sem_wait(&(sema[i+1]));
                sem_wait(&(childthinfo[i].semaph2[0]));
                cout << i<< " "<<childthinfo_vector[i]->nomorework << " " << childthinfo_vector[i]->status << endl;
                if (childthinfo_vector[i]->nomorework && childthinfo_vector[i]->status=="End"){
                    terminated_thread++;
                    ted_thread.push_back(i);
                    cout <<"ID="<<i+1 <<"Thread termed"<< terminated_thread << endl;
                }
                sem_post(&(childthinfo[i].semaph2[0]));
                sem_post(&(sema[i+1]));
                
                cout<< terminated_thread << endl;
            }
        }

        
        if (terminated_thread==nThread+1){
            checking=0;
        }
    }
    cout << Parent->tid << endl;
    //cout << "end Parent Process" << endl;


    vector<string> op1;
    op1=log_event_convert(thlog1);
    return op1;

    
}

void *Parent_thread(void *data){
    // rocover from the data
    struct main_kid *data_cp = (struct main_kid*)data;
    sem_wait(&(data_cp->semaph->at(0)));
    int myid=data_cp->tid;
    sem_post(&(data_cp->semaph->at(0)));
    
    for (int i=0;(unsigned)i<data_cp->instructions->size();i++) {
        // if sleeping requested
        if (data_cp->instructions->at(i).TS=="S"){
            // start signaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Sleeping";
            data_cp->workingnum=data_cp->instructions->at(i).numb;
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

    // call all the function ok to terminate
    for(int i=0;(unsigned)i<data_cp->childThread->size();i++){
        cout << "waiting" << endl;
        sem_wait(&(data_cp->semaph->at(i+1)));
        sem_wait(&(data_cp->childThread->at(i)->semaph2.at(0)));
        data_cp->childThread->at(i)->nomorework=1;
        sem_post(&(data_cp->semaph->at(i+1)));
        sem_post(&(data_cp->childThread->at(i)->semaph2.at(0)));
        cout << i << " is now allow to terminate"<<  endl;
    }

    sem_wait(&(data_cp->semaph->at(myid)));
    data_cp->status="End";
    sem_post(&(data_cp->semaph->at(myid)));
    cout << "bye" << endl;
    pthread_exit(NULL);
    return data;
}

void *Children_run_thread(void *data2){
    // rocover from the data

    struct children_thread *data2_cp = (struct children_thread*)data2;

    
    sem_wait(&(data2_cp->semaph2.at(0)));
    int myid=data2_cp->tid;
    sem_post(&(data2_cp->semaph2.at(0)));

    // int end_thread=0;
    // cout << myid << endl;
    // while (end_thread==0){
    //     // checking if all the task has been handout,and it is ok to terminate
    //     sem_wait(&(data2_cp->semaph2.at(0)));
    //     if (data2_cp->nomorework==1){
    //         end_thread=1;
    //     }
    //     sem_post(&(data2_cp->semaph2.at(0)));
    // }

    sem_wait(&(data2_cp->semaph2.at(0)));
    data2_cp->status="End";
    sem_post(&(data2_cp->semaph2.at(0)));
    pthread_exit(NULL);
    return data2;
}