# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <netdb.h>
# include <stdio.h>

# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <arpa/inet.h> 

# include<string>
# include <string.h>
# include<vector>
# include <cstring>
# include <sys/time.h>
# include <sys/select.h>
# include <queue>

# include <iostream>
# include <ostream>
# include <fstream>
# include <sstream>

# include <pthread.h>
# include <mutex>
# include <semaphore.h>


#include "tands.h"

struct log_data{
    std::string task_type;
    int task_num;
    std::string log_type;
    std::string orgin;
    int task_id;
};
// for file writer
struct sharing_data1{
    std::vector<log_data>* v_op1;
    sem_t * v_semph1;
    int portn;
};
// for children
struct sharing_data2{
    std::vector<log_data>* v_op1;
    std::queue<int> *Work;
    sem_t * v_semph1;// communicate with sharing data1
    sem_t * v_semph2;// communicate with the host
    int portn;
};

void init_machine(std::vector<std::string> us1, char* portn);
void *childhandling(void *data);
void *file_writer(void *data2);
int main(int argc,char* argv[]);