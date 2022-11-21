#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#include<string>
#include <string.h>
#include<vector>
# include <cstring>
# include <sys/time.h>

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
};

struct sharing_data{
    std::vector<log_data>* v_op1;
    std::vector<std::string> uop1;
    sem_t * v_semph1;
    char *portn;
    char *ip;
};

void init_machine(std::vector<std::string> us1, char* portn, char* ipaddress);
void *task_sender(void *data);
int main(int argc,char* argv[]);