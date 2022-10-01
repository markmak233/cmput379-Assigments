#include "assign1.h"


using namespace std;
// remember split in to 3 file





//long get_sys_up_time(){
//    //https://stackoverflow.com/questions/1540627/what-api-do-i-call-to-get-the-system-uptime
//    struct sysinfo s_info;
//    
//    int error = sysinfo(&s_info);
//    if(error != 0){
//        cout << "code error" << error << endl;
//    }
//    return s_info.uptime;
//}

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



int main(int argc,char *argv[]){
//this programme start a loop, it stop until user ask to exit the programme
    int exit_state=0;
    map <pid_t,pidinfo> *piddict;
    piddict= new map <pid_t,pidinfo>();
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

        for (int ix=0; ix<((int)usr_s1.size()); ix++){
        cout << ix << " " <<usr_s1[ix] << endl;
        }
        cout << endl;

        
        
        if ((usr_s1.size()==1) && (usr_s1[0] == "jobs")){
            printf("show current");
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="kill")){
            kill_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="resume")){
            resume_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="suspend")){
            suspend_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="wait")){
            wait_process(usr_s1.size(),usr_s1,piddict);
        }
         else if ((usr_s1.size()>=2) && (usr_s1[0]=="sleep")){
            sleep_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()==1) && (usr_s1[0]== "exit")){
            exit_state=1;
        }
        else if ((usr_s1.size()>=2) && (usr_s1[usr_s1.size()-1]== "&")){
            shell_running(argc,1,usr_s1,piddict);
        }
        else{
            shell_running(argc,0,usr_s1,piddict);
        }

        usr_s1.clear();

    }
    return 0;
}