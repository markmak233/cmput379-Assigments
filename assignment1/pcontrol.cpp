#include "pcontrol.h"



using namespace std;

void kill_process(int argc,vector<string> str_cp_argv,const map <pid_t,pidinfo> *piddict){
    //kill a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    if (piddict->find(pidnum) == piddict->end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGKILL);
    piddict->erase(pidnum);
}

void resume_process(int argc,vector<string> str_cp_argv,const map <pid_t,pidinfo> *piddict){
    // resume a process if they exist
    // string temp =str_cp_argv[1];
    // int pidnum = stoi(temp);
    // if (piddict->find(pidnum) == piddict->end()){
    //    cout << "dne pid"<< endl;
    //     return;
    // }
    // kill(pidnum, SIGCONT);
    // //string temps ="Suspanded";
    // //strcpy(piddict->at(pidnum).status, temps);
    // return;
}

void suspend_process(int argc,vector<string> str_cp_argv,const map <pid_t,pidinfo> *piddict){
    // suspand a process if they exist
    // string temp =str_cp_argv[1];
    // int pidnum = stoi(temp);
    // if (piddict->find(pidnum) == piddict->end()){
    //     cout << "dne pid"<< endl;
    //     return;
    // }
    // kill(pidnum, SIGSTOP);
    // //piddict->at(pidnum).status = "Running";
    // //strcpy(piddict->at(pidnum).status, temps);
    // return;
}

void wait_process(int argc,vector<string> str_cp_argv,const map <pid_t,pidinfo> *piddict){
    // suspand a process if they exist
    // string temp =str_cp_argv[1];
    // int pidnum = stoi(temp);
    // if (piddict->find(pidnum) == piddict->end()){
    //    cout << "dne pid"<< endl;
    //    return;
    // }
    // //https://stackoverflow.com/questions/21248840/example-of-waitpid-in-use
    // waitpid(pidnum,0,WUNTRACED);
    // return;
}

void sleep_process(int argc,vector<string> str_cp_argv){
    int timenum = stoi(str_cp_argv[1]);
    cout << "sleep for "<< timenum << endl;
    sleep(timenum);
    return;
}

