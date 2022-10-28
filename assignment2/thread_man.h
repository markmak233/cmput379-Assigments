# include <iostream>
# include <ostream>
# include <fstream>
# include <sstream>

# include <stdio.h>
# include <stdlib.h>
# include <cstring>
# include <sys/time.h>

# include <string>
# include <vector>
# include <algorithm>
# include <map>
# include <queue>

# include <pthread.h>
# include <mutex>
# include <semaphore.h>
#include<sys/timeb.h>




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
    int isnewWork=0;
    int newWorknum=0;
    int nomorework=0;
    int emptyqueue=0;
    sem_t semaph2;
    sem_t *global_sem;
    std::vector<log_event>*loge;
    std::queue<int>* tasks;
    struct timeval start_time;
};

struct main_kid{
    int tid=0;
    int nth;
    std::string status="init";
    int workingnum=0;
    std::vector<children_thread*> *childThread;
    std::vector<inst_kind> *instructions;
    std::vector<log_event> *loge;
    std::vector<sem_t>* semaph;
    std::queue<int>*tasks;
    sem_t *global_sem;
    struct timeval start_time;
    
};



std::vector<inst_kind> translate_txt_to_struct(std::vector<std::string> instru);
std::vector<std::string> log_event_convert(std::vector<log_event> tlog2,int nThread);
std::vector<log_event> log_merge(std::vector<std::vector<log_event>>);
void writefiles(std::string filename,std::vector<std::string> outputss);
void *Parent_thread(void *data);
void *Children_run_thread(void *data2);