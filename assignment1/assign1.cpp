

#include "assign1.h"

// remember split in to 3 file
map <pid_t,piddict> pinstru;




long get_sys_up_time(){
    //https://stackoverflow.com/questions/1540627/what-api-do-i-call-to-get-the-system-uptime
    struct sysinfo s_info;
    
    int error = sysinfo(&s_info);
    if(error != 0){
        cout << "code error" << error << endl;
    }
    return s_info.uptime;
}

void get_states(){
    pid_t subpid = fork();
    switch(subpid){
        case -1:
            cout<< "Process = \t 0 active" << endl;
            break;

        case 0:
            subpid = getpid();
            cout << "pid=" <<subpid << endl;
            
            break;
        default:
            kill(subpid, SIGKILL);
            break;
    }
    kill(subpid, SIGKILL);

    return;
}

void show_jobs_sum(){
    cout << "jopbs display" << endl;
    get_states();
    



    //cout << "sys time = \t" << (get_sys_up_time()/1000) << " second."<< endl;

    return;
}


void kill_process(int argc,vector<std::string> str_cp_argv){
    // kill a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    if (pinstru.find(pidnum) == pinstru.end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGKILL);
    pinstru.erase(pidnum);
}

void resume_process(int argc,vector<string> str_cp_argv){
    // resume a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    if (pinstru.find(pidnum) == pinstru.end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGCONT);
    pinstru[pidnum].state = 'S';
    return;
}

void suspend_process(int argc,vector<string> str_cp_argv){
    // suspand a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    if (pinstru.find(pidnum) == pinstru.end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGSTOP);
    pinstru[pidnum].state = 'R';
    return;
}

void wait_process(int argc,vector<string> str_cp_argv){
    // suspand a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    if (pinstru.find(pidnum) == pinstru.end()){
        cout << "dne pid"<< endl;
        return;
    }
    //https://stackoverflow.com/questions/21248840/example-of-waitpid-in-use
    waitpid(pidnum,0,WUNTRACED);
    return;
}

void sleep_process(int argc,vector<string> str_cp_argv){
    int timenum = stoi(str_cp_argv[1]);
    unsigned int msecond = 1*10^6;
    cout << "sleep for "<< timenum << endl;
    sleep(timenum);
    return;
}

void shell_running(int argc,int bg,vector<string> str_cp_argv){
    printf("running shell");
    string r_str="";
    if (bg == 1){
        for (int i=1;i<argc-1;i++){
            r_str=r_str+str_cp_argv[i];
        }
    }
    else{
        for (int i=1;i<argc;i++){
            r_str=r_str+str_cp_argv[i];
        }
    }
    cout << r_str << endl;
}
int main(int argc,char *argv[]){
//this programme start a loop, it stop until user ask to exit the programme
    int exit_state=0;
    
    //while not requested exit keeping looping
    while (!(exit_state)){
        //septarting the code by space
        cout << endl << "SHELL379: ";
        string usr_instruct;
        getline(cin, usr_instruct);

        // create a stream for sepration
        vector<string> usr_s1;
        stringstream arguss(usr_instruct);
        
        string arg;
            while(arguss >> arg) {
                usr_s1.push_back(arg);
        }


        cout << "code dispaying:" << endl;

        for (int ix=0; ix<usr_s1.size(); ix++){
        cout << ix << " " <<usr_s1[ix] << endl;
        }
        cout << endl;

        


        if ((usr_s1.size()==1) && (usr_s1[0] == "jobs")){
            printf("show current");
            show_jobs_sum();
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="kill")){
            kill_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="resume")){
            resume_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="suspend")){
            suspend_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="wait")){
            wait_process(usr_s1.size(),usr_s1);
        }
         else if ((usr_s1.size()>=2) && (usr_s1[0]=="sleep")){
            sleep_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()==1) && (usr_s1[0]== "exit")){
            exit_state=1;
        }
        else if ((usr_s1.size()>=2) && (usr_s1[usr_s1.size()-1]== "&")){
            shell_running(argc,1,usr_s1);
        }
        else{
            shell_running(argc,0,usr_s1);
        }

    }
    return 0;
}