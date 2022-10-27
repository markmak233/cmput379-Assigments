#include "thread_man.h"
#include "tands.h"


using namespace std;

vector<inst_kind> translate_txt_to_struct(vector<string> instru){
    vector<inst_kind> transed_instru;
    for (unsigned ind=0;ind<instru.size()-1;ind++){
        struct inst_kind temp1;
        temp1.TS=instru[ind][0];
        string temp2 = instru[ind];
        temp2.erase(0,1);

        // https://www.educative.io/answers/how-to-convert-a-string-to-an-int-in-cpp
        stringstream ss;
        ss << temp2;
        ss >> temp1.numb;

        if (temp1.numb<1 || temp1.numb>100){
            temp1.numb=1;
        }

        transed_instru.push_back(temp1);
    }
    return transed_instru;
}

vector<string> log_event_convert(vector<log_event> tlog2,int nThread){
    vector<string> s1;
    // struct log_event{
    // double currentTime;
    // int tid;
    // int queue;
    // std::string Status;
    // int run_num;
    map <string,int> commcount;
    vector <int> tcout;
    for (int i=0;i<nThread+1;i++){
        tcout.push_back(0);
    }
    
    for (unsigned i=0;i<tlog2.size();i++){
        // string part
        string tempstr;
        char temp[100];
        if (tlog2[i].Status=="Work" || tlog2[i].Status=="Receive" ){
            sprintf(temp,"%2.3f ID=%3d Q=%3d ",tlog2[i].currentTime.count(),tlog2[i].tid,tlog2[i].queue);
        } else {
            sprintf(temp,"%2.3f ID=%3d       ",tlog2[i].currentTime.count(),tlog2[i].tid);
        }
        tempstr.append(temp);

        tempstr.append(tlog2[i].Status);
        while (tempstr.size()<(35*sizeof(char))){
            tempstr.append(" ");
        }

        if (tlog2[i].run_num==0){
            tempstr.append("     //");
        }else{
            char temp2[50];
            sprintf(temp2,"%2d   //",tlog2[i].run_num);
            tempstr.append(temp2);
        }
        tempstr.append("\n");
        s1.push_back(tempstr);
        // couning part
        if(tlog2[i].Status=="Receive" ){
            commcount["Receive"]=commcount["Receive"]+1;
            tcout[tlog2[i].tid]=tcout[tlog2[i].tid]+1;
        } else if  (tlog2[i].Status=="Work" ||
                    tlog2[i].Status=="Ask" ||
                    tlog2[i].Status=="Complete" ||
                    tlog2[i].Status=="Sleep" ){
            commcount[tlog2[i].Status]=commcount[tlog2[i].Status]+1;
        }
        
    }
    s1.push_back("Summary:                                //\n");
    map<string, int>::iterator it;
    for (it = commcount.begin(); it != commcount.end(); it++){
        string temp3 ="    "+it->first;
        while (temp3.size()<(18*sizeof(char))){
            temp3.append(" ");
        }
        char temp4[100];
        sprintf(temp4,"%3d                   // \n",it->second);
        temp3.append(temp4);
        s1.push_back(temp3);
    }
    for (unsigned i=1;i<tcout.size();i++){
        char temp3[100];
        sprintf(temp3,"    Thread %3d \t%3d \t\t\t\t    //\n",i,tcout[i]);
        s1.push_back(temp3);
    }
    double tps=commcount["Receive"];
    auto totaltime=tlog2[tlog2.size()-1].currentTime.count();
    tps=tps/totaltime;
    char temp4[100];
    sprintf(temp4,"Trabsactions per second: %3.3f \t    //\n",tps);
    s1.push_back(temp4);
    s1.push_back("----------------------------------\n");
    return s1;
}

vector<string> event_management(int nThread,vector<string> instru,vector<string>* outputss){
    // this function many logging the event of each device
    vector<inst_kind> translated_instru;
    translated_instru=translate_txt_to_struct(instru);
    //cout << "result" << endl;
    // result output
    for (unsigned i=0;i<translated_instru.size();i++){
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
        temp.status="init";
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
    for (unsigned i=0;i<childthinfo.size();i++){
        childthinfo_vector.push_back(&childthinfo[i]);
    }

    // creating Parent infomation
    struct main_kid* Parent,Parent2;
    Parent=&Parent2;
    Parent->tid=0;
    Parent->nth=nThread;
    Parent->workingnum=0;
    Parent->status="init";
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
    for(unsigned i=0;i<childthinfo.size();i++){
        // start second children sephnore
        // start pthread
        pthread_t cp;
        cout <<i << " cthread"<< childthinfo[i].semaph2.size() << endl;
        pthread_create(&cp,NULL,Children_run_thread,childthinfo_vector[i]);
        bkpt.push_back(cp);
    }

    cout << "start Parent thread" << endl;
    //creating main parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,Parent);
    
    
    int checking=1;
    while (checking){
        auto now_time =chrono::system_clock::now();
        // data the associate with parent
        int st;
        sem_getvalue(&(Parent->semaph->at(0)),&st);
        if (st){
            sem_wait(&(Parent->semaph->at(0)));
            if(Parent->status=="End" && !parent_end){
                terminated_thread++;
                parent_end=1;
            } else if (Parent->status!=Parent->last_saved_status){
                //cout << "status change to " << Parent->status << endl;
                //cout << "working on" << Parent->workingnum << endl;
                if (Parent->status!="Running" && Parent->status!="init"){
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
        }

        // data that associate with 
        for (unsigned i=0;i<childthinfo.size();i++){
            int st,st2;
            sem_getvalue(&(childthinfo[i].semaph2[0]),&st);
            sem_getvalue(&(childthinfo[i].semaph2[0]),&st2);
            if (st&&st2){
                sem_wait(&(sema[i+1]));
                sem_wait(&(childthinfo[i].semaph2[0]));
                if (childthinfo[i].status!=childthinfo[i].last_saved_status && childthinfo[i].status!="End"){   
                    struct log_event temp_log;
                    temp_log.Status=childthinfo[i].status;
                    temp_log.tid=childthinfo[i].tid;
                    //run num does not exist
                    if (childthinfo[i].status!="Ask"){
                        temp_log.run_num=childthinfo[i].newWorknum;
                    }else{
                        temp_log.run_num=0;
                    }
                    chrono::duration<double> durtime=now_time-init_log_time;
                    temp_log.currentTime=durtime;
                    temp_log.queue=free_queue;
                    thlog1.push_back(temp_log);
                    childthinfo[i].last_saved_status=childthinfo[i].status;
                    
                    if(childthinfo[i].status=="Complete"){
                        free_queue++;
                        // logging for parent
                        struct log_event temp_log2;
                        temp_log2.Status="Work";
                        temp_log2.tid=0;
                        //run num does not exist
                        temp_log2.run_num=childthinfo[i].newWorknum;
                        temp_log2.currentTime=durtime;
                        temp_log2.queue=free_queue;
                        thlog1.push_back(temp_log2);
                        // update the statuśof parent that work received
                        Parent->last_saved_status=Parent->status;

                        // clear out the instruction
                        childthinfo[i].newWorknum=0;
                        childthinfo[i].isnewWork=0;

                        childthinfo[i].status="Ask";
                        struct log_event temp_log3;
                        temp_log3.Status=childthinfo[i].status;
                        temp_log3.tid=childthinfo[i].tid;
                        temp_log3.currentTime=durtime;
                        temp_log3.run_num=0;
                        temp_log3.queue=free_queue;
                        thlog1.push_back(temp_log3);
                        childthinfo[i].last_saved_status=childthinfo[i].status;
                        
                    } else if (childthinfo[i].status=="Receive")
                    {
                        free_queue--;
                    }
                    
                    childthinfo[i].last_saved_status=childthinfo[i].status;
                    
                }
                
                sem_post(&(childthinfo[i].semaph2[0]));
                sem_post(&(sema[i+1]));

            }
        }
        
        
        


        //checking if thread ended,if all thread ended stop logging
        // adding untill the parent end start checking
        if (terminated_thread>0){
            for(unsigned i=0;i<childthinfo.size();i++){
                if (!binary_search(ted_thread.begin(),ted_thread.end(),i)){
                    // check if busy,if it is ,check back later
                    int st;
                    sem_getvalue(&(sema[i+1]),&st);
                    int st2;
                    sem_getvalue(&(childthinfo[i].semaph2[0]),&st2);
                    if (st && st2){
                        sem_wait(&(sema[i+1]));
                        sem_wait(&(childthinfo[i].semaph2[0]));
                        if (childthinfo_vector[i]->nomorework && childthinfo_vector[i]->status=="End"){
                            terminated_thread++;
                            ted_thread.push_back(i);
                            cout <<"ID="<<i+1 <<"Thread termed"<< terminated_thread << endl;
                        }
                        sem_post(&(childthinfo[i].semaph2[0]));
                        sem_post(&(sema[i+1]));
                    }
                }
            }
        }
        
        if (terminated_thread>=nThread+1){
            checking=0;
        }
    }
    cout << Parent->tid << endl;

    cout << "loop exited"<< endl;
    vector<string> op1;
    for (unsigned idx=0;idx<op1.size();idx++){
        outputss->push_back(op1[idx]);
    }

    op1=log_event_convert(thlog1,nThread);
    return op1;
    for (unsigned i=0;i<sema.size();i++){
        sem_destroy(&sema[i]);
        if (i!=0){
            sem_destroy(&childthinfo[i-1].semaph2[0]);
        }
    }
    
}

void *Parent_thread(void *data){
    // rocover from the data
    struct main_kid *data_cp = (struct main_kid*)data;
    sem_wait(&(data_cp->semaph->at(0)));
    int myid=data_cp->tid;
    data_cp->status="Running";
    sem_post(&(data_cp->semaph->at(0)));
    
    for (unsigned i=0;i<data_cp->instructions->size();i++) {
        // if sleeping requested
        if (data_cp->instructions->at(i).TS=="S"){
            // start signaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Sleep";
            data_cp->workingnum=data_cp->instructions->at(i).numb;
            sem_post(&(data_cp->semaph->at(myid)));

            // testing purpose showing
            Sleep(data_cp->instructions->at(i).numb);

            //sleeping done singaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Running";
            data_cp->workingnum=0;
            sem_post(&(data_cp->semaph->at(myid)));
        }
        //if trans requested
        else if (data_cp->instructions->at(i).TS=="T"){
            int work_assigned=0;
            //check needed;
            while (!work_assigned){
                for (unsigned tx=0;tx<data_cp->childThread->size();tx++){
                    // checking if locking, if it is check back later
                    int st;
                    sem_getvalue(&(data_cp->semaph->at(tx+1)),&st);
                    int st2;
                    sem_getvalue(&(data_cp->childThread->at(tx)->semaph2.at(0)),&st2);
                    if (st && st2){
                        sem_wait(&(data_cp->semaph->at(tx+1)));
                        sem_wait(&(data_cp->childThread->at(tx)->semaph2.at(0)));
                        
                        if (data_cp->childThread->at(tx)->status=="Ask" && data_cp->childThread->at(tx)->newWorknum==0){
                            data_cp->childThread->at(tx)->isnewWork =1;
                            data_cp->childThread->at(tx)->newWorknum = data_cp->instructions->at(i).numb;
                            work_assigned =1;
                        }
                        sem_post(&(data_cp->childThread->at(tx)->semaph2.at(0)));
                        sem_post(&(data_cp->semaph->at(tx+1)));
                        if (work_assigned){
                            tx=data_cp->childThread->size()-1;
                            
                        }
                        
                    }
                }
            }
        }
    }

    // call all the function ok to terminate
    for(unsigned i=0;i<data_cp->childThread->size();i++){
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
    //pthread_exit(NULL);
    return data;
}

void *Children_run_thread(void *data2){
    // rocover from the data
    cout << "children process started" << endl;
    struct children_thread *data2_cp = (struct children_thread*)data2;

    
    sem_wait(&(data2_cp->semaph2.at(0)));
    //int myid=data2_cp->tid;
    data2_cp->status="Ask";
    sem_post(&(data2_cp->semaph2.at(0)));

    int end_thread=0;
    while (end_thread==0){
        // checking if all the task has been handout,and it is ok to terminate
        int sm;
        sem_getvalue(&(data2_cp->semaph2.at(0)),&sm);
        // finded a work given
        if (sm){
            sem_wait(&(data2_cp->semaph2.at(0)));
            int worknum=data2_cp->newWorknum;

            if (worknum!=0){
                data2_cp->status="Receive";
                sem_post(&(data2_cp->semaph2.at(0)));
                Trans(worknum);
                sem_wait(&(data2_cp->semaph2.at(0)));
                data2_cp->status="Complete";
            }
            sem_post(&(data2_cp->semaph2.at(0)));

            // changeing from Comlete to Ask will be host's logging work
        }

        int sm2;
        sem_getvalue(&(data2_cp->semaph2.at(0)),&sm2);
        if (sm2){
            sem_wait(&(data2_cp->semaph2.at(0)));
            if (data2_cp->nomorework==1 && data2_cp->newWorknum==0){
                end_thread=1;
            }
            sem_post(&(data2_cp->semaph2.at(0)));
        }
    }
    sem_wait(&(data2_cp->semaph2.at(0)));
    data2_cp->status="End";
    sem_post(&(data2_cp->semaph2.at(0)));
    //pthread_exit(NULL);
    return data2;
}