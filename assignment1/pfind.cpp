#include "pfind.h"

using namespace std;



void shell_running(int argc,int bg,vector<string> str_cp_argv,const map <pid_t,pidinfo> *piddict){
    printf("running shell");
    string r_str="";
    bool opt=0;
    string fname;
    for (int i=0;i<argc;i++){
        
        if (bg==1 && i==argc-1){
            continue;
        }else if(str_cp_argv[i][0]=='>' || str_cp_argv[i][0]=='<'){
            fname=str_cp_argv[i].erase(0,1);
            opt=1;
            continue;
        }
        r_str=r_str+str_cp_argv[i];
    }
    
    cout << r_str << endl;

    pid_t pid = fork();
    if(pid<0){
        printf("creating child failed");
        _exit(EXIT_FAILURE);

    }

    if(pid==0){
        //child
         if (opt){
            freopen(fname.c_str(),"w",stdout);
        }
        //converting cmd to char
        
        char* cp_argv[10];
        int cp_c=0;
        for (int i=0;i<argc;i++){
            if (bg==1 && i==argc-1){
                continue;
            }else if(str_cp_argv[i][0]=='>' || str_cp_argv[i][0]=='<'){
                continue;
            }
            const char* temp=str_cp_argv[i].c_str();
            strcpy(cp_argv[cp_c],temp);
            cp_c++;
            
        }
       
        //https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
        if(execvp(str_cp_argv[0].c_str(),cp_argv)<0){
            cout << "run failed";
            _exit(EXIT_FAILURE);
        }

        
    }else{
        //parent
        struct pidinfo temp;
        temp.command=r_str;
        temp.status="Running";
        temp.npid=pid;
        // insert file into piddict
        piddict->insert({pid,temp});
        if(!bg){
            waitpid(pid,nullptr,0);
            piddict->erase(pid);
        }
    }
}
