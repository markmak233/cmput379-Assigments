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
#include <signal.h>

using namespace std;

void show_jobs(){
    pid_t s1 =fork();
    pid_t subpid = fork();
    switch(subpid){
        case -1:
            cout<< "Process = \t 0 active" << endl;
            break;

        case 0:
            pid_t subpid = getpid();
            cout << "pid=" <<subpid << endl;
            break;
    }
    kill(s1, SIGKILL);
    kill(subpid, SIGKILL);
    return;
}

int main(){
    printf("start\n");
    show_jobs();
    printf("end\n");
    return 0;
}

