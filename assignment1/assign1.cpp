#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include<sstream>

#include <unistd.h>
#include <sys/types.h>

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

#include "assign1.h"



using namespace std;

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
    pid_t pid=fork();
    fork();
    switch(subpid){
        case -1:
            cout<< "Process = \t 0 active" << endl;
            break;

        case 0:
            subpid = getpid();
            cout << "pid=" <<pid << endl;
            
            break;
        default:
            cout << "sys time = \t" << (get_sys_up_time()/1000) << " second."<< endl;
            break;
    }
    return;
}

void show_jobs_sum(){
    cout << "jopbs display" << endl;
    get_states();
    



    //cout << "sys time = \t" << (get_sys_up_time()/1000) << " second."<< endl;

    return;
}

void shell_running(int argc,vector<string> str_cp_argv){
    printf("running shell");
    string r_str="";
    for (int i=1;i<argc-1;i++){
        r_str=r_str+str_cp_argv[i];
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
        else if ((usr_s1.size()>=2) && (usr_s1[usr_s1.size()-1]== "&")){
            shell_running(argc,usr_s1);
        }
        else if ((usr_s1.size()==1) && (usr_s1[0]== "exit")){
            exit_state=1;
        }

    }
    return 0;
}