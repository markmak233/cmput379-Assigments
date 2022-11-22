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
#include<queue>
# include <cstring>
# include <sys/time.h>

# include <iostream>
# include <ostream>
# include <fstream>
# include <sstream>

# include <pthread.h>
# include <mutex>
# include <semaphore.h>



struct log_data{
    double current_time;
    std::string task_type;
    std::string task_num;
    std::string log_type;
};

struct sharing_data{
    std::queue<log_data>* v_op1;
    std::vector<std::string> uop1;
    std::string filename;
    char *portn;
    char *ip;
    int term=0;
};

void init_machine(std::vector<std::string> us1, char* portn, char* ipaddress);
void translate(std::string filename,struct log_data d1);
void writefile(std::string filename,std::string a1);
int task_sender(struct sharing_data data_cp);
int main(int argc,char* argv[]);