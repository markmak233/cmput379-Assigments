#include "thread_man.h"
#include "tands.h"


using namespace std;

vector<inst_kind> translate_txt_to_struct(vector<string> instru){
    vector<inst_kind> transed_instru;
    for (unsigned ind=0;ind<instru.size()-1;ind++){
        struct inst_kind temp1;
        temp1.TS=instru[ind][0];
        if (temp1.TS!="T"&& temp1.TS!="S"){
            continue;
        }
        string temp2 = instru[ind];
        temp2.erase(0,1);

        // if error occer go to next one
        // https://www.educative.io/answers/how-to-convert-a-string-to-an-int-in-cpp
        // https://en.cppreference.com/w/cpp/language/try_catch
        try{
            stringstream ss;
            ss << temp2;
            ss >> temp1.numb;
        } catch (const exception& e){
            cout << "error occur while converting numbers" << endl;
            continue;
        }

        if (temp1.TS=="S" && (temp1.numb<1 || temp1.numb>100)){
            temp1.numb=1;
        }
        transed_instru.push_back(temp1);
    }
    return transed_instru;
}

vector<string> log_event_convert(vector<log_event> tlog2,int nThread){
    vector<string> s1;
    map <string,int> commcount;
    vector <int> tcout;
    // free queue counting 
    for (int i=0;i<nThread+1;i++){
        tcout.push_back(0);
    }
    int free_queue = nThread;
    // convert each log to a string
    for (unsigned i=0;i<tlog2.size();i++){
        if (tlog2[i].Status=="Receive"){
            free_queue--;
        } else if (tlog2[i].Status=="Complete"){
            free_queue++;
        }
        tlog2[i].queue=free_queue;
        // string part
        string tempstr;
        char temp[100];
        if (tlog2[i].Status=="Work" || tlog2[i].Status=="Receive" ){
            sprintf(temp,"%2.3f ID=%3d Q=%3d ",tlog2[i].currentTime,tlog2[i].tid,tlog2[i].queue);
        } else {
            sprintf(temp,"%2.3f ID=%3d       ",tlog2[i].currentTime,tlog2[i].tid);
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
        // couning part for summary
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
    // assing string for submition
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
    auto totaltime=tlog2[tlog2.size()-1].currentTime;
    tps=tps/totaltime;
    char temp4[100];
    sprintf(temp4,"Trabsactions per second: %3.3f \t    //\n",tps);
    s1.push_back(temp4);
    s1.push_back("----------------------------------\n");
    return s1;
}

vector<log_event> log_merge(vector<vector<log_event>> log_2dlist){
    vector<log_event> allevent;
    int totalsize=0;
    vector<unsigned> pivot;
    for(unsigned i=0;i<log_2dlist.size();i++){
        totalsize=totalsize+log_2dlist[i].size();
        pivot.push_back(0);
    }
    cout << "Merging " << totalsize << " record from " << log_2dlist.size()-1 << " + Parent Threads" << endl;
    for (int i=0;i<totalsize;i++){
        struct log_event local_small;
        int current_pivot;
        int empty=1;
        for(unsigned j=0;j<log_2dlist.size();j++){
            if (log_2dlist[j].size() > pivot[j]){
                if (empty){
                    local_small=log_2dlist.at(j).at(pivot[j]);
                    current_pivot=j;
                    pivot[j]++;
                    empty=0;
                } else if(local_small.currentTime>log_2dlist.at(j).at(pivot[j]).currentTime){
                    local_small=log_2dlist.at(j).at(pivot[j]);
                    pivot[current_pivot]--;
                    current_pivot=j;
                    pivot[j]++;
                } else if (local_small.currentTime== log_2dlist[j][pivot[j]].currentTime && local_small.tid>log_2dlist[j][pivot[j]].tid){
                    local_small=log_2dlist.at(j).at(pivot[j]);
                    pivot[current_pivot]--;
                    current_pivot=j;
                    pivot[j]++;
                }
            }
        }
        allevent.push_back(local_small);
    }
    return allevent;
}

void writefiles(string filename,vector<string> outputss){
    // geting a list of string and save it into a file
    //https://www.tutorialspoint.com/how-to-append-text-to-a-text-file-in-cplusplus
    //https://www.codeproject.com/Questions/5299415/I-get-a-warning-when-I-try-to-use-a-for-loop-to-pr
    for (unsigned ind=0;ind<outputss.size();ind++){
        if (ind==outputss.size()){
            break;
        }
        ofstream fout;
        ifstream fin;
        fin.open(filename);
        fout.open(filename,ios_base::app);
        if (fout.is_open()){
            fout << outputss[ind];
        }
        fin.close();
        fout.close();
    }
    return;
}

void *Parent_thread(void *data){
    // rocover from the data
    struct main_kid *data_cp = (struct main_kid*)data;
    struct log_event temp_log;
    struct timeval etime;

    cout << "Starting Parent" << endl;

    sem_wait(&(data_cp->semaph->at(0)));
    int myid=data_cp->tid;
    data_cp->status="Running";
    sem_post(&(data_cp->semaph->at(0)));
    
    
    for (unsigned i=0;i<data_cp->instructions->size();i++) {
        ////https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
        // if sleeping requested
        cout << "working on" << i <<endl;
        if (data_cp->instructions->at(i).TS=="S"){
            // start signaling
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Sleep";
            data_cp->workingnum=data_cp->instructions->at(i).numb;
            temp_log.Status=data_cp->status;
            temp_log.tid=data_cp->tid;
            temp_log.run_num=data_cp->workingnum;
            sem_post(&(data_cp->semaph->at(myid)));

            gettimeofday(&etime,NULL);
            temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
            data_cp->loge->push_back(temp_log);

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
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Work";
            data_cp->workingnum=data_cp->instructions->at(i).numb;
            temp_log.Status=data_cp->status;
            temp_log.run_num=data_cp->workingnum;
            sem_post(&(data_cp->semaph->at(myid)));

            gettimeofday(&etime,NULL);
            temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
            data_cp->loge->push_back(temp_log);

            while (!work_assigned){
                // checking if locking, if it is check back later
                int st;
                sem_getvalue((data_cp->global_sem),&st);
                if (st){
                    sem_wait((data_cp->global_sem));
                    if (data_cp->tasks->size()<data_cp->nth*2){
                        data_cp->tasks->push(data_cp->instructions->at(i).numb);
                        work_assigned=1;
                    }
                    sem_post((data_cp->global_sem));
                } 
            }
            sem_wait(&(data_cp->semaph->at(myid)));
            data_cp->status="Running";
            data_cp->workingnum=0;
            sem_post(&(data_cp->semaph->at(myid)));
        }
    }

    // call all the function ok to terminate
    for(unsigned i=0;i<data_cp->childThread->size();i++){
        sem_wait(&(data_cp->childThread->at(i)->semaph2));
        data_cp->childThread->at(i)->nomorework=1;
        sem_post(&(data_cp->childThread->at(i)->semaph2));
    }

    sem_wait(&(data_cp->semaph->at(myid)));
    data_cp->status="End";
    temp_log.Status=data_cp->status;
    temp_log.tid=data_cp->tid;
    temp_log.run_num=0;
    gettimeofday(&etime,NULL);
    temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
    data_cp->loge->push_back(temp_log);
    sem_post(&(data_cp->semaph->at(myid)));

    cout << "Ending Parent" << endl;
    pthread_exit(NULL);
    return data;
}

void *Children_run_thread(void *data2){
    ////https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
    // rocover from the data
    struct children_thread *data2_cp = (struct children_thread*)data2;
    sem_wait(&(data2_cp->semaph2));
    int myid=data2_cp->tid;
    data2_cp->status="Ask";
    struct log_event templog1;
    struct  timeval etime;
    templog1.Status=data2_cp->status;
    templog1.tid=data2_cp->tid;
    sem_post(&(data2_cp->semaph2));
    // gete time
    gettimeofday(&etime,NULL);
    templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
    data2_cp->loge->push_back(templog1);

    int end_thread=0;
    while (end_thread==0){
        // checking if all the task has been handout,and it is ok to terminate
        int sm;
        sem_getvalue((data2_cp->global_sem),&sm);
        if (sm){
            sem_wait((data2_cp->global_sem));
            if (!(data2_cp->tasks->empty())){
                data2_cp->newWorknum=data2_cp->tasks->front();
                data2_cp->tasks->pop();
                data2_cp->emptyqueue=0;
            } else {
                cout << myid << " " << "empty queue" << endl;
                data2_cp->emptyqueue=1;
            }
            sem_post((data2_cp->global_sem));
            data2_cp->isnewWork=1;
        }



        sem_getvalue(&(data2_cp->semaph2),&sm);
        // finded a work given
        if (sm){
            sem_wait(&(data2_cp->semaph2));
            int worknum=data2_cp->newWorknum;

            if (worknum!=0){
                // data receive logging device
                data2_cp->status="Receive";
                templog1.Status=data2_cp->status;
                sem_post(&(data2_cp->semaph2));
                gettimeofday(&etime,NULL);
                templog1.run_num=worknum;

                templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
                data2_cp->loge->push_back(templog1);

                // work on something
                Trans(worknum);

                // complete logging
                sem_wait(&(data2_cp->semaph2));
                data2_cp->status="Complete";
                templog1.Status=data2_cp->status;
                gettimeofday(&etime,NULL);
                templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
                data2_cp->loge->push_back(templog1);

                // ask logging
                data2_cp->status="Ask";
                data2_cp->isnewWork=0;
                data2_cp->newWorknum=0;
                templog1.Status=data2_cp->status;
                templog1.run_num=0;
                templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
                data2_cp->loge->push_back(templog1);

            }
            sem_post(&(data2_cp->semaph2));

            // changeing from Comlete to Ask will be host's logging work
        }

        int sm2;
        sem_getvalue(&(data2_cp->semaph2),&sm2);
        if (sm2){
            sem_wait(&(data2_cp->semaph2));
            if ((data2_cp->nomorework==1 && data2_cp->newWorknum==0) && data2_cp->emptyqueue==1){
                end_thread=1;
            }
            sem_post(&(data2_cp->semaph2));
        }
    }
    sem_wait(&(data2_cp->semaph2));
    data2_cp->status="End";
    // The code here are for no reason,just for delay the end to make sure all record in the vector<log_event>,the output will remove the following end record
    data2_cp->isnewWork=0;
    data2_cp->newWorknum=0;
    templog1.Status=data2_cp->status;
    templog1.run_num=0;
    templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
    data2_cp->loge->push_back(templog1);
    sem_post(&(data2_cp->semaph2));
    pthread_exit(NULL);
    return data2;
}