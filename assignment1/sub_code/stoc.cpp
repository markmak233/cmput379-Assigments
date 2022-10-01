#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <sstream>
#include <string>
#include <map>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <signal.h>

using namespace std;

int main(){
    vector<string> str_cp_argv;
    str_cp_argv.push_back("sudo");
    str_cp_argv.push_back("apt");
    str_cp_argv.push_back("install");
    str_cp_argv.push_back("nvidia-cuda-toolkit");
    bool bg=0;
    int argc=str_cp_argv.size();



    char *cp_argv[40];
    int cp_c=0;
    for (int i=1;i<argc;i++){
        cout << "index "<< i << " 1" << endl; //1
        if (bg==1 && i==argc-1){
            cout << "bg pass" << endl;
            continue;
        }else if(str_cp_argv[i][0]=='>' || str_cp_argv[i][0]=='<'){
            cout << "fname" << endl;
            continue;
        }
        cout << "index "<< i <<" 2" <<endl; //2
        const char* temp=str_cp_argv[i].c_str();
        cout << "index "<< i <<" 3" <<endl; //3
        cout << "cpc: "<< cp_c << endl;
        strcpy(cp_argv[cp_c],temp);
        cout << "index "<< i <<" 4" <<endl; //4
        cp_c=cp_c+4*str_cp_argv[i].length();
        cout << "index "<< i <<" 5" <<endl; //5
    }

    cout << "checking "<<endl;
    for (int i=0;i<40;i++){
        cout<< cp_argv[i] << endl;
    }
    }