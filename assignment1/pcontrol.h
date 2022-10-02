

#include <iostream>
#include <string>
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

//self created

#include "pfind.h"

#pragma once



void kill_process(int argc,std::vector<std::string> str_cp_argv,std::map <pid_t,pidinfo> *piddict);
void resume_process(int argc,std::vector<std::string> str_cp_argv,std::map <pid_t,pidinfo> *piddict);
void suspend_process(int argc,std::vector<std::string> str_cp_argv,std::map <pid_t,pidinfo> *piddict);
void wait_process(int argc,std::vector<std::string> str_cp_argv,std::map <pid_t,pidinfo> *piddict);
void sleep_process(int argc,std::vector<std::string> str_cp_argv);
void exit_programme(std::map<pid_t,pidinfo> *piddict);



