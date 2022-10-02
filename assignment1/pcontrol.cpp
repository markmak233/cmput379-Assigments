#include "pcontrol.h"

using namespace std;

void kill_process(int argc,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    //kill a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    // checking if the process are self created and exist
    if (piddict->find(pidnum) == piddict->end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGKILL);
    piddict->erase(pidnum);
}

void resume_process(int argc,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    //resume a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    // checking if the process are self created and exist
    if (piddict->find(pidnum) == piddict->end()){
       cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGCONT);
    //create a new file for pid
    struct pidinfo temps;
    temps.command = piddict->at(pidnum).status;
    temps.npid = pidnum;
    temps.status ="Running";


    //erase orginal and insert new one
    piddict->erase(pidnum);
    piddict->insert({temps.npid,temps});
    
    return;
}

void suspend_process(int argc,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    //suspand a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    // checking if the process are self created and exist
    if (piddict->find(pidnum) == piddict->end()){
        cout << "dne pid"<< endl;
        return;
    }
    kill(pidnum, SIGSTOP);

    //create a new file for pid
    struct pidinfo temps;
    temps.command = piddict->at(pidnum).status;
    temps.npid = pidnum;
    temps.status ="Suspanded";

    //erase orginal and insert new one
    piddict->erase(pidnum);
    piddict->insert({temps.npid,temps});


    return;
}

void wait_process(int argc,vector<string> str_cp_argv,map <pid_t,pidinfo> *piddict){
    //suspand a process if they exist
    string temp =str_cp_argv[1];
    int pidnum = stoi(temp);
    // checking if the process are self created and exist
    if (piddict->find(pidnum) == piddict->end()){
       cout << "dne pid"<< endl;
       return;
    }
    // //https://stackoverflow.com/questions/21248840/example-of-waitpid-in-use
    waitpid(pidnum,0,WUNTRACED);
    return;
}

void sleep_process(int argc,vector<string> str_cp_argv){
    //let a programme sleep for certain time
    int timenum = stoi(str_cp_argv[1]);
    cout << "sleep for "<< timenum << endl;
    sleep(timenum);
    return;
}

void exit_programme(map<pid_t,pidinfo> *piddict){
    // when user requested exit, tell children termainates and wait for it end.
    if (piddict->size()!=0){
        map <pid_t,pidinfo>::iterator it=piddict->begin();
        for (;it!=piddict->end();it++){
            kill(it->first,SIGTERM);
            waitpid(it->first,nullptr,0);
            int pksig=kill(it->first,SIGKILL);
            if (pksig == 0){
                cout << "fail to kill:" << it->first <<endl;
            }
        }
    }
    // show resources being used during the execuction
    cout << endl << "resources used:"<<endl;
    resources_up_time();
}