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

void rapidwrite(struct log_event data,string filename,int* qsnow3,map <string,int>* commcount,vector<int>*tcount){
    // string part
    string tempstr;
    char temp[100];
    if (data.Status=="Complete"){
    *qsnow3=*qsnow3+1;
    }
    if(data.Status=="Receive" ){
    *qsnow3=*qsnow3-1;
    }
    data.queue=*qsnow3;
    if (data.Status=="Work" || data.Status=="Receive" ){
        sprintf(temp,"%2.3f ID=%3d Q=%3d ",data.currentTime,data.tid,data.queue);
    } else {
        sprintf(temp,"%2.3f ID=%3d       ",data.currentTime,data.tid);
    }
    tempstr.append(temp);

    tempstr.append(data.Status);
    while (tempstr.size()<(35*sizeof(char))){
        tempstr.append(" ");
    }

    if (data.run_num==0){
        tempstr.append("     //");
    }else{
        char temp2[50];
        sprintf(temp2,"%2d   //",data.run_num);
        tempstr.append(temp2);
    }
    // couning part for summary

    char temp5[100];
    if(data.Status=="Receive" ){
        (*commcount)["Receive"]=(*commcount)["Receive"]+1;
        (tcount->at(data.tid))=(tcount->at(data.tid))+1;
        sprintf(temp5," Thread %2d takes work, n=%2d \n",data.tid,data.run_num);
    } else if  (data.Status=="Work"){
        (*commcount)[data.Status]=(*commcount)[data.Status]+1;
        sprintf(temp5," Parent receives work with n=%2d \n",data.run_num);
    } else if (data.Status=="Ask"){
        (*commcount)[data.Status]=(*commcount)[data.Status]+1;
        sprintf(temp5," Thread %2d asks for work \n",data.tid);
    } else if ( data.Status=="Complete"){
        (*commcount)[data.Status]=(*commcount)[data.Status]+1;
        sprintf(temp5," Thread %2d completes task, n=%2d \n",data.tid,data.run_num);
    } else if (data.Status=="Sleep"){
        (*commcount)[data.Status]=(*commcount)[data.Status]+1;
        sprintf(temp5," Parent sleeps, n=%2d \n",data.run_num);
    } else if (data.Status=="End")
    {
        sprintf(temp5," End of input for producer\n");
    }else{
        sprintf(temp5," \n");
    }
    tempstr.append(temp5);
    
    
    

    ofstream fout;
    ifstream fin;
    fin.open(filename);
    fout.open(filename,ios_base::app);
    if (fout.is_open()){
        fout << tempstr;
    }
    fin.close();
    fout.close();

}

void summarywrite(map <string,int>* commcount,vector<int>*tcount,double lengthtime,string filename){
    vector<string> s1;
    s1.push_back("Summary:                                //\n");
    map<string, int>::iterator it;
    for (it = commcount->begin(); it != commcount->end(); it++){
        string temp3 ="    "+it->first;
        while (temp3.size()<(18*sizeof(char))){
            temp3.append(" ");
        }
        char temp4[100];
        sprintf(temp4,"%3d                   // \n",it->second);
        temp3.append(temp4);
        s1.push_back(temp3);
    }
    for (unsigned i=1;i<tcount->size();i++){
        char temp3[100];
        sprintf(temp3,"    Thread %3d \t%3d \t\t\t\t    //\n",i,tcount->at(i));
        s1.push_back(temp3);
    }

    double tps=commcount->at("Receive")/lengthtime;
    char temp4[100];
    sprintf(temp4,"Trabsactions per second: %3.3f \t    //\n",tps);
    s1.push_back(temp4);
    s1.push_back("----------------------------------\n");
    writefiles(filename,s1);
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

    
    for (unsigned i=0;i<data_cp->instructions->size();i++) {
        ////https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
        // if sleeping requested
        if (data_cp->instructions->at(i).TS=="S"){
            // start signaling
            sem_wait(&(data_cp->semaph->at(0)));
            data_cp->status="Sleep";
            sem_post(&(data_cp->semaph->at(0)));
            data_cp->workingnum=data_cp->instructions->at(i).numb;
            temp_log.Status=data_cp->status;
            temp_log.tid=data_cp->tid;
            temp_log.run_num=data_cp->workingnum;
            

            gettimeofday(&etime,NULL);
            temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
            //data_cp->loge->push_back(temp_log);

            sem_wait((data_cp->global_sem_log2));
            data_cp->gblog2->push(temp_log);
            sem_post((data_cp->global_sem_log2));


            // testing purpose showing
            Sleep(data_cp->instructions->at(i).numb);

            //sleeping done singaling
            sem_wait(&(data_cp->semaph->at(0)));
            data_cp->status="Running";
            sem_post(&(data_cp->semaph->at(0)));
            data_cp->workingnum=0;
            
        }
        //if trans requested
        else if (data_cp->instructions->at(i).TS=="T"){
            int work_assigned=0;


            sem_wait(&(data_cp->semaph->at(0)));
            data_cp->status="Work";
            sem_post(&(data_cp->semaph->at(0)));
            data_cp->workingnum=data_cp->instructions->at(i).numb;
            temp_log.Status=data_cp->status;
            temp_log.run_num=data_cp->workingnum;


            gettimeofday(&etime,NULL);
            temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
            //data_cp->loge->push_back(temp_log);

            sem_wait((data_cp->global_sem_log2));
            data_cp->gblog2->push(temp_log);
            sem_post((data_cp->global_sem_log2));

            while (!work_assigned){
                // checking if locking, if it is check back later
                int st;
                sem_getvalue((data_cp->global_sem),&st);
                if (st){
                    sem_wait((data_cp->global_sem));
                    if (data_cp->tasks->size() <= (unsigned)data_cp->nth*2){
                        data_cp->tasks->push(data_cp->instructions->at(i).numb);
                        work_assigned=1;
                    }
                    sem_post((data_cp->global_sem));
                } 
            }
            sem_wait(&(data_cp->semaph->at(0)));
            data_cp->status="Running";
            sem_post(&(data_cp->semaph->at(0)));
            data_cp->workingnum=0;
        }
    }

    // call all the function ok to terminate
    for(unsigned i=0;i<data_cp->childThread->size();i++){
        sem_wait(&(data_cp->childThread->at(i)->semaph2));
        data_cp->childThread->at(i)->nomorework=1;
        sem_post(&(data_cp->childThread->at(i)->semaph2));
    }

    sem_wait(&(data_cp->semaph->at(0)));
    data_cp->status="End";
    temp_log.Status=data_cp->status;
    temp_log.tid=data_cp->tid;
    temp_log.run_num=0;
    gettimeofday(&etime,NULL);
    temp_log.currentTime=((etime.tv_sec - data_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data_cp->start_time.tv_usec)/1000.0)/1000;
    //data_cp->loge->push_back(temp_log);
    sem_post(&(data_cp->semaph->at(0)));

    sem_wait((data_cp->global_sem_log2));
    data_cp->gblog2->push(temp_log);
    sem_post((data_cp->global_sem_log2));


    cout << "Ending Parent" << endl;
    pthread_exit(NULL);
    return data;
}

void *Children_run_thread(void *data2){
    ////https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
    // rocover from the data
    struct children_thread *data2_cp = (struct children_thread*)data2;
    sem_wait(&(data2_cp->semaph2));
    //int myid=data2_cp->tid;
    data2_cp->status="Ask";
    sem_post(&(data2_cp->semaph2));
    struct log_event templog1;
    struct  timeval etime;
    templog1.Status=data2_cp->status;
    templog1.tid=data2_cp->tid;
    

    // gete time
    gettimeofday(&etime,NULL);
    templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
    //data2_cp->loge->push_back(templog1);

    sem_wait((data2_cp->global_sem_log1));
    data2_cp->gblog1->push(templog1);
    sem_post((data2_cp->global_sem_log1));

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
                // templog1.queue=*data2_cp->qsnow;
            } else {
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
                //data2_cp->loge->push_back(templog1);

                sem_wait((data2_cp->global_sem_log1));
                data2_cp->gblog1->push(templog1);
                sem_post((data2_cp->global_sem_log1));

                // work on something
                Trans(worknum);

                // complete logging
                sem_wait(&(data2_cp->semaph2));
                data2_cp->status="Complete";
                templog1.Status=data2_cp->status;
                gettimeofday(&etime,NULL);
                templog1.queue=0;
                templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
                //data2_cp->loge->push_back(templog1);

                sem_wait((data2_cp->global_sem_log1));
                data2_cp->gblog1->push(templog1);
                sem_post((data2_cp->global_sem_log1));

                // ask logging
                data2_cp->status="Ask";
                data2_cp->isnewWork=0;
                data2_cp->newWorknum=0;
                templog1.Status=data2_cp->status;
                templog1.run_num=0;
                templog1.currentTime= ((etime.tv_sec - data2_cp->start_time.tv_sec) * 1000 + (etime.tv_usec-data2_cp->start_time.tv_usec)/1000.0)/1000;
                //data2_cp->loge->push_back(templog1);

                sem_wait((data2_cp->global_sem_log1));
                data2_cp->gblog1->push(templog1);
                sem_post((data2_cp->global_sem_log1));

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
    sem_post(&(data2_cp->semaph2));
    pthread_exit(NULL);
    return data2;
}