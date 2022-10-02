#include <iostream>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <string>
#include <map>

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>
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
int pid_up_time(pid_t pid);
void resources_up_time();