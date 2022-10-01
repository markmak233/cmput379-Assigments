#include "pfind.h"

using namespace std;



void shell_running(int argc,int bg,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    printf("running shell\n");
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
        if (r_str!=""){
            r_str=r_str+" ";
        }
        r_str=r_str+str_cp_argv[i];
    }
    cout <<"length "<< argc << "cmd: " <<r_str<< endl;

    pid_t pid = fork();
    cout << "running pid: "<< pid<<endl;;
    if(pid<0){
        printf("creating child failed");
        _exit(EXIT_FAILURE);

    }

    else if(pid==0){
        cout << "running children" << endl;
        //child
         if (opt){
            freopen(fname.c_str(),"w",stdout);
        }

        cout<< "instru " << str_cp_argv[0].c_str() << endl;

        //converting cmd to char
        char *cp_argv[40];
        int cp_c=0;
        for (int i=1;i<argc;i++){
            cout << "index "<< i <<endl; //1
            if (bg==1 && i==argc-1){
                cout << "bg pass" << endl;
                continue;
            }else if(str_cp_argv[i][0]=='>' || str_cp_argv[i][0]=='<'){
                cout << "fname" << endl;
                continue;
            }
            cout << "index "<< i <<endl;
            const char* temp=str_cp_argv[i].c_str();
            strcpy(cp_argv[cp_c],temp);
            cout << "index "<< i <<endl; //2
            cp_c=cp_c+str_cp_argv[i].length()*2;
            cout << "index "<< i <<endl; //3
        }

        cout << "checking "<<endl;
        //for (int i=1;i<cp_c+1;i++){
        //    cout << i << ":ag-> "<< cp_argv[i]<< endl;
        //}
       
        //https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
        if(execvp(str_cp_argv[0].c_str(),cp_argv)<0){
            cout << "run failed";
            _exit(EXIT_FAILURE);
        }

        fflush(stdout);

        
    }else{
        //parent
        cout << "running parent" << endl;

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
