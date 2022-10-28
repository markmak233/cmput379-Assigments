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
    int* qsnow;
    sem_t semaph2;
    sem_t *global_sem;
    sem_t *global_sem_log1;
    std::vector<log_event>*loge;
    std::queue<int>* tasks;
    std::queue<log_event>* gblog1;
    struct timeval start_time;
};

struct main_kid{
    int tid=0;
    int nth;
    int* qsnow2;
    std::string status="Running";
    int workingnum=0;
    std::vector<children_thread*> *childThread;
    std::vector<inst_kind> *instructions;
    std::vector<log_event> *loge;
    sem_t* semaph;
    std::queue<int>*tasks;
    std::queue<log_event>* gblog2;
    sem_t *global_sem;
    sem_t *global_sem_log2;
    struct timeval start_time;
    
};



std::vector<inst_kind> translate_txt_to_struct(std::vector<std::string> instru);
std::vector<std::string> log_event_convert(std::vector<log_event> tlog2,int nThread);
std::vector<log_event> log_merge(std::vector<std::vector<log_event>>);
void rapidwrite(struct log_event data,std::string filename,int* qsnow3,std::map <std::string,int>* commcount,std::vector<int>*tcount);
void summarywrite(std::map <std::string,int>* commcount,std::vector<int>*tcount, double lengthtime,std::string filename);
void writefiles(std::string filename,std::vector<std::string> outputss);
void *Parent_thread(void *data);
void *Children_run_thread(void *data2);