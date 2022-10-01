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

#include <bits/stdc++.h>

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

    vector<string> modified_argv;
    for (int i=0;i<argc;i++){
        if (bg==1 && i==argc-1){
            continue;
        }else if(str_cp_argv[i][0]=='>' || str_cp_argv[i][0]=='<'){
            continue;
        }
        modified_argv.push_back(str_cp_argv[i]);
    }

    vector<char*> pvec(str_cp_argv.size());
    std::transform(str_cp_argv.begin(),str_cp_argv.end(),pvec.begin(),[](auto& str){
    return &str[0];
   });

    execvp(str_cp_argv[0].c_str(),pvec.data());
}