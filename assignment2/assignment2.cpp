# include "assignment2.h"
# include "thread_man.h"

using namespace std;\

void event_management(int nThread,vector<string> instru,string filename){
    // this function many logging the event of each device
    vector<inst_kind> translated_instru;
    translated_instru=translate_txt_to_struct(instru);

    //start a vector that contain the share memory information
    // where this log devices also able to access
    vector<children_thread> childthinfo;
    vector<children_thread*> childthinfo_vector;
    //std::vector<std::vector<log_event>> childrenlog;
    vector<sem_t> sema;
    vector<int> ted_thread;
    queue<int> tasks;
    queue<log_event> global_log;
    queue<log_event> global_log_cache;
    map <string,int> commcount;
    vector <int> tcout;
    // main Parent semphore init
    sem_t sp;
    sem_init(&sp,1,1);
    sema.push_back(sp);
    // global semphore init
    sem_t gb;
    sem_init(&gb,1,1);
    sem_t gb_log;
    sem_init(&gb_log,1,1);
    int qs=nThread;
    double lastime=0;
    tcout.push_back(0);

    // children info setup and semphore init
    for (int i=0;i<nThread;i++){
        struct children_thread temp;
        temp.tid=i+1;
        // setup semaphore
        sem_t temp3;
        sem_init(&temp3,1,1);
        temp.semaph2=temp3;

        temp.global_sem=&gb;
        temp.tasks=&tasks;

        temp.global_sem_log1=&gb_log;
        temp.gblog1=&global_log;
        
        temp.qsnow=&qs;
        tcout.push_back(0);
        childthinfo.push_back(temp);
    }
    // pointer for readu distribure to each children
    for (unsigned i=0;i<childthinfo.size();i++){
        childthinfo_vector.push_back(&childthinfo[i]);

    }

    vector<log_event> thlog1;
    // creating Parent infomation
    struct main_kid* Parent,Parent2;
    Parent=&Parent2;
    Parent->nth=nThread;
    Parent->childThread=&childthinfo_vector;
    Parent->instructions=&translated_instru;
    Parent->semaph=&sema;
    Parent->loge=&thlog1;
    Parent->global_sem=&gb;
    Parent->tasks=&tasks;
    Parent->gblog2=&global_log;
    Parent->global_sem_log2=&gb_log;
    Parent->qsnow2=&qs;
    
    // //https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
    
    struct timeval stime;
    gettimeofday(&stime,NULL);

    Parent->start_time=stime;

    //preation done from here

    // start children tread    
    vector<pthread_t> bkpt;
    for(unsigned i=0;i<childthinfo_vector.size();i++){
        // start second children sephnore
        // start pthread
        pthread_t cp1;
        childthinfo[i].start_time=stime;
        pthread_create(&cp1,NULL,Children_run_thread,childthinfo_vector[i]);
        bkpt.push_back(cp1);
    }
    // start parent thread
    //creating main parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,Parent);
    //pthread_join(p,NULL);
    

    int checking=1;
    int parent_end=0;
    int terminated_thread=0;
    while (checking){
        int st;
        sem_getvalue(&(Parent->semaph->at(0)),&st);
        if (st && !parent_end){
            sem_wait(&(Parent->semaph->at(0)));
            if(Parent->status=="End"){
                terminated_thread++;
                parent_end=1;
            }
            sem_post(&(Parent->semaph->at(0)));
        }
        //getting the log cache
        sem_getvalue(&(gb_log),&st);
        if (st){
            sem_wait(&(gb_log));
            if(!(global_log.empty())){
                while(!(global_log.empty())){
                    global_log_cache.push(global_log.front());
                    global_log.pop();
                }
                sem_post(&(gb_log));
                while (!(global_log_cache.empty()))
                {
                    rapidwrite(global_log_cache.front(),filename,&qs,&commcount,&tcout);
                    lastime=global_log_cache.front().currentTime;
                    global_log_cache.pop();
                }
                
            } else {
                sem_post(&(gb_log));
            }
           
        }



        // checking if thread ended,if all thread ended stop logging
        // adding untill the parent end start checking
        if (terminated_thread>0){
            for(unsigned i=0;i<childthinfo.size();i++){
                if (!binary_search(ted_thread.begin(),ted_thread.end(),i)){
                    // check if busy,if it is ,check back later
                    int st;
                    sem_getvalue(&(childthinfo[i].semaph2),&st);
                    if (st ){
                        sem_wait(&(childthinfo[i].semaph2));
                        if (childthinfo_vector[i]->nomorework && childthinfo_vector[i]->status=="End"){
                            terminated_thread++;
                            ted_thread.push_back(i);
                        }
                        sem_post(&(childthinfo[i].semaph2));
                        //pthread_join(bkpt[i],0);
                    }
                }
            }
        }
        
        if (terminated_thread>=nThread+1){
            checking=0;
        }
    }

    if (!(global_log.empty())){
        rapidwrite(global_log.front(),filename,&qs,&commcount,&tcout);
        lastime=global_log_cache.front().currentTime;
        global_log_cache.pop();
    }


    cout << "children ended" << endl;
    //childrenlog.push_back(thlog1);
       
    for (unsigned i=0;i<sema.size();i++){
        sem_destroy(&sema[i]);
        if (i!=0){
            sem_destroy(&childthinfo[i-1].semaph2);
        }
    }

    summarywrite(&commcount,&tcout,lastime,filename);
    return;
    
}


int main(int argc,char* argv[]){
    if (argc<2 || argc>3){
        cout << "unexpected arguments inputed";
        return 1;
    }
    // number of thread
    int nThread = atoi(argv[1]);
    //output file name
    string filename;
    if (argc==3){
        string nid=argv[2];
        filename.append("prodcon.");
        filename.append(nid);
        filename.append(".log");
    } else{
        filename = "prodcon.log";
    }
    // inputting instructions
    vector<string> instruct;
    int running=1;
    while (running){
        string as;
        cin >> as;
        instruct.push_back(as);
        if (as.size()==0){
            running=0;
        }
    }
    //cout <<" start running to looging device" << endl;
    event_management(nThread,instruct,filename);
    cout << "Goodbye" <<endl;
    return 1;
}


