#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "assign1.h"
using namespace std;


int input_trans(string usr_input){
    // detacting if the length is out
    if (size(usr_input)>MAX_LENGTH*sizeof(char))
    {
        cout << "length max out of "<< MAX_LENGTH*sizeof(char) << "."<< endl;
        return 2;
    }
    // seprating into different args
    vector <string> str_args;
    string cur_args;
    for (int i=0;i<size(usr_input);i++){
        cout << usr_input[i] << endl;

        // space detected save it to a vector
        if (stoi(usr_input[i])==" "){
            cout << cur_args << endl;
            cur_args.clear();
        }
    }


    return 1;
}


int main(){
//this programme start a loop, it stop until user ask to exit the programme
    int exit_stat=0;

    // 0 for continue loop
    // 1 for exit
    // 2 for unidentified input
    while (!exit_stat){
        string usr_input; // 100 size of char
        cout << "SHELL 379: ";
        getline( cin , usr_input );
        //istringstream iss(usr_input );

        input_trans(usr_input);
        if (exit_stat==2) {
            cout << "unidentified input";
        }
        if (usr_input == "exit") {
            exit_stat = 1;
        }


    }
    return 0;
}