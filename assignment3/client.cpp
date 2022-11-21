#include "client.h"

using namespace std;

void init_machine(){

}

void task_sender(){
    
}

int main(int argc,char* argv[]){
    if (argc!=2){
        cout << "client port ipaddress"<< endl;
        return 1;
    }
    
    vector<string> us1;
    int running=1;
    while (running){
        string as;
        cin >> as;
        if (as.size()!=0){
            us1.push_back(as);
        }else{
            running=0;
        }
    }

    return 1;
}