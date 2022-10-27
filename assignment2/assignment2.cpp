# include "assignment2.h"
# include "thread_man.h"

using namespace std;\

vector<string> event_management(int nThread,vector<string> instru,string filename){
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
    vector<children_thread*> childthinfo_vector;
    //vector<vector<log_event>> childrenlog;
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
        // setup semaphore
        sem_t temp3;
        sem_init(&temp3,1,1);
        vector<sem_t> mysem;
        mysem.push_back(temp3);
        temp.semaph2=mysem;
        cout <<"sem size "<< temp.semaph2.size() << endl;
        //push into list


        childthinfo.push_back(temp);
        
    }
    // pointer for readu distribure to each children
    for (unsigned i=0;i<childthinfo.size();i++){
        childthinfo_vector.push_back(&childthinfo[i]);
    }

    // creating Parent infomation
    struct main_kid* Parent,Parent2;
    Parent=&Parent2;
    Parent->nth=nThread;
    Parent->childThread=&childthinfo_vector;
    Parent->instructions=&translated_instru;
    Parent->semaph=&sema;

    vector<log_event> thlog1;
    // https://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
    auto init_log_time=chrono::system_clock::now();
    //int free_queue=nThread;
    int parent_end=0;
    int terminated_thread=0;
    //preation done from here

    // start children tread    
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

    // start parent thread
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

                    thlog1.push_back(temp_log);
                }
                Parent->last_saved_status=Parent->status;
                
            }
            sem_post(&(Parent->semaph->at(0)));
        }

        // data that associate with children
        for (unsigned i=0;i<childthinfo.size();i++){
            int st;
            sem_getvalue(&(childthinfo[i].semaph2[0]),&st);
            if (st){
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
                    thlog1.push_back(temp_log);
                    childthinfo[i].last_saved_status=childthinfo[i].status;
                    // additional information for the 
                    if(childthinfo[i].status=="Complete"){
                        // clear out the instruction
                        childthinfo[i].newWorknum=0;
                        childthinfo[i].isnewWork=0;

                        childthinfo[i].status="Ask";
                        struct log_event temp_log3;
                        temp_log3.Status=childthinfo[i].status;
                        temp_log3.tid=childthinfo[i].tid;
                        temp_log3.currentTime=durtime;
                        temp_log3.run_num=0;
                        thlog1.push_back(temp_log3);
                        childthinfo[i].last_saved_status=childthinfo[i].status;
                        
                    }
                    childthinfo[i].last_saved_status=childthinfo[i].status;
                    
                }
                
                sem_post(&(childthinfo[i].semaph2[0]));

            }
        }
        
        
        


        //checking if thread ended,if all thread ended stop logging
        // adding untill the parent end start checking
        if (terminated_thread>0){
            for(unsigned i=0;i<childthinfo.size();i++){
                if (!binary_search(ted_thread.begin(),ted_thread.end(),i)){
                    // check if busy,if it is ,check back later
                    int st;
                    sem_getvalue(&(childthinfo[i].semaph2[0]),&st);
                    if (st ){
                        sem_wait(&(childthinfo[i].semaph2[0]));
                        if (childthinfo_vector[i]->nomorework && childthinfo_vector[i]->status=="End"){
                            terminated_thread++;
                            ted_thread.push_back(i);
                            cout <<"ID="<<i+1 <<"Thread termed"<< terminated_thread << endl;
                        }
                        sem_post(&(childthinfo[i].semaph2[0]));
                    }
                }
            }
        }
        
        if (terminated_thread>=nThread+1){
            checking=0;
        }
    }

    // process with the log
    vector<string> op1;

    op1=log_event_convert(thlog1,nThread);
    
    writefiles(filename,op1);

    
    for (unsigned i=0;i<sema.size();i++){
        sem_destroy(&sema[i]);
        if (i!=0){
            sem_destroy(&childthinfo[i-1].semaph2[0]);
        }
    }
    return op1;
    
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
        filename.append(".txt");
    } else{
        filename = "prodcon.txt";
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
    vector<string>outputss;

    outputss=event_management(nThread,instruct,filename);

    cout << outputss[outputss.size()-2] << endl;
    cout << "writting" << endl;
    // write file to dedicated destinaction.
    //writefiles(filename,outputss);
    cout << "entireprograme done" <<endl;
    return 1;
}


