#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include<sstream>

#include <unistd.h>
#include <sys/types.h>

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

using namespace std;

int main(){
    //septarting the code by space
    string usr_instruct;
    getline(cin, usr_instruct);

    vector<string> usr_s1;
    stringstream arguss(usr_instruct);
    
    string arg;
        while(arguss >> arg) {
            usr_s1.push_back(arg);
    }
    for (string x:usr_s1){
        cout << x << endl;
    }
    return 0;
}