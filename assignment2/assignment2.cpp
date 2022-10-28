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
    std::vector<std::vector<log_event>> childrenlog;
    vector<sem_t> sema;
    vector<int> ted_thread;
    queue<int> tasks;
    // main Parent semphore init
    sem_t sp;
    sem_init(&sp,1,1);
    sema.push_back(sp);
    // global semphore init
    sem_t gb;
    sem_init(&gb,1,1);

    // children info setup and semphore init
    for (int i=0;i<nThread;i++){
        struct children_thread temp;
        temp.tid=i+1;
        // setup semaphore
        sem_t temp3;
        sem_init(&temp3,1,1);
        temp.semaph2=temp3;
        vector<log_event> temp4;
        temp.global_sem=&gb;
        temp.tasks=&tasks;
        //push into list
        childrenlog.push_back(temp4);
        childthinfo.push_back(temp);
        
    }
    // pointer for readu distribure to each children
    for (unsigned i=0;i<childthinfo.size();i++){
        childthinfo_vector.push_back(&childthinfo[i]);
        childthinfo[i].loge=&(childrenlog[i]);

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
    
    // //https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
    
    struct timeval stime;
    gettimeofday(&stime,NULL);

    Parent->start_time=stime;
    //int free_queue=nThread;

    cout << "starting children" << endl;
    //preation done from here

    // start children tread    
    vector<pthread_t> bkpt;
    for(unsigned i=0;i<childthinfo.size();i++){
        // start second children sephnore
        // start pthread
        pthread_t cp;
        childthinfo[i].start_time=stime;
        pthread_create(&cp,NULL,Children_run_thread,childthinfo_vector[i]);
        bkpt.push_back(cp);
    }

    // start parent thread
    //creating main parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,Parent);
    pthread_join(p,NULL);
    
    int checking=1;
    int terminated_thread=0;
    while (checking){
        //checking if thread ended,if all thread ended stop logging
        // adding untill the parent end start checking
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
        
        
        if (terminated_thread>=nThread){
            checking=0;
        }
    }
    cout << "children ended" << endl;
    childrenlog.push_back(thlog1);
       
    for (unsigned i=0;i<sema.size();i++){
        sem_destroy(&sema[i]);
        if (i!=0){
            sem_destroy(&childthinfo[i-1].semaph2);
        }
    }

    // process with the log
    vector<string> op1;
    vector<log_event> all_log=log_merge(childrenlog);
    op1=log_event_convert(all_log,nThread);
    cout << "writing file"<< endl;
    writefiles(filename,op1);

    //debug for each thread output only
    for (unsigned i=0;i<childrenlog.size();i++){
        vector<string> op2;
        op2=log_event_convert(childrenlog[i],nThread);
        writefiles(filename,op2);
    }
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


