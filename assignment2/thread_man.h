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


# include <chrono>
# include <ctime>
# include <cstring>
struct inst_kind{
    std::string TS;
    int numb; 
};

struct children_thread{
    int tid;
    std::string status;
    std::string last_saved_status;
    int isnewWork;
    int newWorknum;
    int nomorework;
    std::vector<sem_t> semaph2;
};

struct main_kid{
    int tid;
    int nth;
    std::string status;
    std::string last_saved_status;
    int workingnum;
    std::vector<children_thread*> *childThread;
    std::vector<inst_kind> *instructions;
    std::vector<sem_t>* semaph;
    
};

struct log_event{
    std::chrono::duration<double> currentTime;
    int tid;
    int queue;
    std::string Status;
    int run_num;
};

std::vector<inst_kind> translate_txt_to_struct(std::vector<std::string> instru);
std::vector<std::string> log_event_convert(std::vector<log_event> tlog2);
std::vector<std::string> event_management(int nThread,std::vector<std::string> instru,std::vector<std::string>* outputss);
void *Parent_thread(void *data);
void *Children_run_thread(void *data2);