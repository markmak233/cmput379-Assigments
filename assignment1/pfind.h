#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <time.h>


#include <bits/stdc++.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <signal.h>

#pragma once

#ifndef ASSIGN1_H
#define ASSIGN1_H

struct pidinfo{
    pid_t npid;
    std::string status;
    std::string command;
};

#endif
void shell_running(int argc,int bg,std::vector<std::string> str_cp_argv,std::map <pid_t,pidinfo> *piddict);
void current_process(std::map <pid_t,pidinfo> *piddict);
void resources_up_time();
long get_sys_up_time();