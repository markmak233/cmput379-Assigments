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


using namespace std;


int main(int argc,char *argv[]){
    //let a programme sleep for certain time
    if (argc==2){
        int timenum;
        sscanf(argv[1], "%d", &timenum);
        sleep(timenum);
    }
    return 0;
}