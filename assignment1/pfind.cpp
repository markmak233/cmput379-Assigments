#include "pfind.h"

using namespace std;



void shell_running(int argc,int bg,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    // running shell
    string r_str="";
    bool opt=0;
    string fname;
    for (int i=0;i<argc;i++){
        
        if (bg==1 && i==argc-1){
            continue;
        }else if(str_cp_argv[i][0]=='>' ){
            fname=str_cp_argv[i].erase(0,1);
            opt=1;
            continue;
        }
        if (r_str!=""){
            r_str=r_str+" ";
        }
        r_str=r_str+str_cp_argv[i];
    }

    pid_t pid = fork();
    if(pid<0){
        cout << "creating child failed" << endl;
        _exit(EXIT_FAILURE);

    }

    else if(pid==0){
        //child

        //https://stackoverflow.com/questions/40576003/ignoring-warning-wunused-result
         if (opt){
            (void)!freopen(fname.c_str(),"w",stdout);
        }


        
        vector<string> modified_argv;
        for (int i=0;i<argc;i++){
            if (bg==1 && i==argc-1){
                continue;
            }else if(str_cp_argv[i][0]=='>' ){
                continue;
            }else if(str_cp_argv[i][0]=='<' ){
                // read the file and input it into the argv(s)
                ifstream ipfile (str_cp_argv[i].erase(0,1).c_str());
                string filestring,perstr;
                if (getline(ipfile,perstr)){
                    filestring=filestring+perstr+"\n";
                }
                modified_argv.push_back(filestring);
                ipfile.close();
                
            }else{
                modified_argv.push_back(str_cp_argv[i]);
            }
        }
        // convert vector string to char ** 
        // https://stackoverflow.com/questions/71302334/convert-vectorstring-to-char-for-use-in-execvp
        vector<char*> pvec(str_cp_argv.size());
        std::transform(modified_argv.begin(),modified_argv.end(),pvec.begin(),[](auto& str){
        return &str[0];
        });

        // run the programme
        // https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
        if(execvp(str_cp_argv[0].c_str(),pvec.data())<0){
            cout << "run failed,exit unnormaly." << endl;;
            _exit(EXIT_FAILURE);
        }

        if (!opt){
            fflush(stdout);
        }
        
    }else{
        //parent
        //cout << "running parent" << endl;

        struct pidinfo temppid;
        temppid.command=r_str;
        temppid.status="Running";
        temppid.npid=pid;
        // insert file into piddict
        piddict->insert({pid,temppid});
        if(!bg){
            waitpid(pid,nullptr,0);
            piddict->erase(pid);
        }
    }
}


void current_process(map <pid_t,pidinfo> *piddict){
    if (piddict.size()!=0){
        int ct=0;
        map <pid_t,pidinfo>::iterator it=piddict->begin();
        for (;it!=piddict->end();it++){
            cout << ct << " : "<< it->first ;
            cout << " "<< (it->second).status ;
            cout << (it->second).command << endl;
        }
    }
    cout << "Proesss = \t"<< piddict->size() << " Active" <<endl;
    cout << "Complete Process:"<< endl;
};

int pid_up_time(pid_t pid){
    int rtime;
    return rtime;
}

void resources_up_time(){
    cout << "sys time = \t" << (get_sys_up_time()/1000) << " second."<< endl;
}
long get_sys_up_time(){
    //https://stackoverflow.com/questions/1540627/what-api-do-i-call-to-get-the-system-uptime
    struct sysinfo s_info;
    
    int error = sysinfo(&s_info);
    if(error != 0){
       cout << "code error" << error << endl;
    }
    return s_info.uptime;
    
    //https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
}



//void get_states(){
//    pid_t subpid = fork();
//    switch(subpid){
//        case -1:
//            cout<< "Process = \t 0 active" << endl;
//            break;
//
//        case 0:
//            subpid = getpid();
//            cout << "pid=" <<subpid << endl;
//            
//            break;
//        default:
//            kill(subpid, SIGKILL);
//            break;
//    }
//    kill(subpid, SIGKILL);
//
//    return;
//}

// void show_jobs_sum(){
//     cout << "jopbs display" << endl;
//     get_states();

//     //cout << "sys time = \t" << (get_sys_up_time()/1000) << " second."<< endl;

//     return;
// }


