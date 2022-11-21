#include "server.h"

using namespace std;

void init_machine(char* portn){

}
void *childhandling(void *data){

}

void *file_writer(void *data2){

}

int main(int argc,char* argv[]){
    if (argc!=2){
        cout << "client port"<< endl;
        return 1;
    }

    int portn=atoi(argv[1]);
    if (portn>64000 || portn<5000){
        cout << "out of range" << endl;
        return 1;
    }

    init_machine(argv[1]);
    return 1;
}