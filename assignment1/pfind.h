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

#pragma once

#ifndef ASSIGN1_H
#define ASSIGN1_H

struct pidinfo{
    pid_t npid;
    std::string status;
    std::string command;
};

#endif
void shell_running(int argc,int bg,std::vector<std::string> str_cp_argv,const std::map <pid_t,pidinfo> *piddict);