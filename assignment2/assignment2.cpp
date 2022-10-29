# include "assignment2.h"
# include "thread_man.h"

using namespace std;\

void event_management(int nThread,vector<string> instru,string filename){
    // this function many logging the event of each device
    vector<inst_kind> translated_instru;
    translated_instru=translate_txt_to_struct(instru);
    if (translated_instru.size()==0){
        return;
    }
    // start a vector that contain the share memory information
    // where this log devices also able to access
    vector<children_thread> childthinfo;
    // init number thread end/number thread instructions.
    vector<int> ted_thread,tcout;
    queue<int> tasks;
    queue<log_event> global_log,global_log_cache;
    map <string,int> commcount;
    // main Parent semphore init
    sem_t sp,gb,gb_log;
    sem_init(&sp,1,1);
    // global semphore init
    sem_init(&gb,1,1);
    sem_init(&gb_log,1,1);
    int qs=nThread;
    vector<double> lastime;
    tcout.push_back(0);

    // children info setup and semphore init
    for (int i=0;i<nThread;i++){
        struct children_thread temp;
        temp.tid=i+1;
        // setup semaphore
        sem_init(&temp.semaph2,1,1);
        // semphore for taking task
        temp.global_sem=&gb;
        temp.tasks=&tasks;
        //semphore for log device
        temp.global_sem_log1=&gb_log;
        temp.gblog1=&global_log;
        //push back in to a vector
        tcout.push_back(0);
        childthinfo.push_back(temp);
    }

    // creating Parent infomation
    struct main_kid Parent;
    Parent.nth=nThread;
    Parent.childThread=&childthinfo;
    Parent.instructions=&translated_instru;
    Parent.semaph=&sp;
    // tasks devices
    Parent.global_sem=&gb;
    Parent.tasks=&tasks;
    // log device
    Parent.gblog2=&global_log;
    Parent.global_sem_log2=&gb_log;
    
    // strt the main time here
    // //https://www.tutorialspoint.com/how-to-get-time-in-milliseconds-using-cplusplus-on-linux
    struct timeval stime;
    gettimeofday(&stime,NULL);
    Parent.start_time=stime;

    // start children tread    
    vector<pthread_t> bkpt;
    for(unsigned i=0;i<childthinfo.size();i++){
        // start second children sephnore
        // start pthread
        pthread_t cp1;
        childthinfo[i].start_time=stime;
        pthread_create(&cp1,NULL,Children_run_thread,&childthinfo[i]);
        bkpt.push_back(cp1);
    }
    // start parent thread
    pthread_t p;
    pthread_create(&p,NULL,Parent_thread,&Parent);
    
    // main thread remains running to collect logs,which prevent wait after all jobs done
    int terminated_thread=0;
    while (terminated_thread<nThread+1){
        int st;
        sem_getvalue((Parent.semaph),&st);
        if (st && terminated_thread==0){
            sem_wait((Parent.semaph));
            if(Parent.status=="End"){
                terminated_thread++;
            }
            sem_post((Parent.semaph));
        }
        //getting the log cache
        sem_getvalue(&(gb_log),&st);
        if (st){
            sem_wait(&(gb_log));
            // if not empty push the log into another queue and release the semphore
            if(!(global_log.empty())){
                while(!(global_log.empty())){
                    global_log_cache.push(global_log.front());
                    global_log.pop();
                }
                sem_post(&(gb_log));
                // translate and write it into files.
                while (!(global_log_cache.empty()))
                {
                    if (global_log_cache.front().currentTime>0){
                        lastime.push_back(global_log_cache.front().currentTime);
                    }
                    rapidwrite(global_log_cache.front(),filename,&qs,&commcount,&tcout);

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
                        if (childthinfo[i].nomorework && childthinfo[i].status=="End"){
                            terminated_thread++;
                            ted_thread.push_back(i);
                        }
                        sem_post(&(childthinfo[i].semaph2));
                    }
                }
            }
        }
    }
    // incase left with empty record,read again
    while (!(global_log.empty())){
        global_log_cache.push(global_log.front());
        global_log.pop();
    }
    while(!(global_log_cache.empty())){
        rapidwrite(global_log_cache.front(),filename,&qs,&commcount,&tcout);
        if (global_log_cache.front().currentTime>0){
            lastime.push_back(global_log_cache.front().currentTime);
        }
        global_log_cache.pop();
    }
    // https://stackoverflow.com/questions/9874802/how-can-i-get-the-maximum-or-minimum-value-in-a-vector
    // avoid garbage value
    const double en_time =*max_element(lastime.begin(),lastime.end());
    summarywrite(commcount,tcout,en_time,filename);
    for (unsigned i=0;i<childthinfo.size();i++){
        sem_destroy(&childthinfo[i].semaph2);
    }
    commcount.clear();
    tcout.clear();
    childthinfo.clear();
    return;
    
}


int main(int argc,char* argv[]){
    // process the input
    if (argc<2 || argc>3){
        cout << "unexpected arguments inputed" << endl;
        return 1;
    }
    // number of thread
    int nThread = atoi(argv[1]);
    //output file name
    string filename;
    if (argc==3){
        filename.append("prodcon.");
        filename.append(argv[2]);
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
    return 1;
}


