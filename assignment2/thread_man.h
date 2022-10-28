# include <iostream>
# include <ostream>
# include <stdio.h>
# include <stdlib.h>
# include <string>
# include <vector>
# include <fstream>
# include <sstream>
# include <pthread.h>
# include <mutex>
# include <semaphore.h>
#include<sys/timeb.h>
#include <algorithm>
#include <map>

#include <typeinfo>


# include <cstring>
# include <sys/time.h>

struct inst_kind{
    std::string TS;
    int numb; 
};

struct log_event{
    double currentTime;
    int tid;
    int queue=0;
    std::string Status;
    int run_num=0;
};

struct children_thread{
    int tid;
    std::string status="init";
    std::string last_saved_status="None";
    int isnewWork=0;
    int newWorknum=0;
    int nomorework=0;
    std::vector<sem_t> semaph2;
    std::vector<log_event>*loge;
    struct timeval start_time;
};

struct main_kid{
    int tid=0;
    int nth;
    std::string status="init";
    std::string last_saved_status="None";
    int workingnum=0;
    std::vector<children_thread*> *childThread;
    std::vector<inst_kind> *instructions;
    std::vector<sem_t>* semaph;
    
};



std::vector<inst_kind> translate_txt_to_struct(std::vector<std::string> instru);
std::vector<std::string> log_event_convert(std::vector<log_event> tlog2,int nThread);
void writefiles(std::string filename,std::vector<std::string> outputss);
void *Parent_thread(void *data);
void *Children_run_thread(void *data2);