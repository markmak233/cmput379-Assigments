#include "assign1.h"

using namespace std;

int main(int argc,char *argv[]){
    //this programme start a loop, it stop until user ask to exit the programme
    int exit_state=0;
    // create a dictionary save all the running/suspanding children
    map <pid_t,pidinfo> *piddict;
    piddict= new map <pid_t,pidinfo>;
    //while not requested exit keeping looping
    while (!(exit_state)){
        //septarting the code by space
        cout << endl << "SHELL379: ";
        string usr_instruct;
        getline(cin, usr_instruct);

        // create a stream for sepration
        vector<string> usr_s1;
        stringstream arguss(usr_instruct);
        string arg;
            while(arguss >> arg) {
                usr_s1.push_back(arg);
        }

        cout << endl;

        //determinate which programme user requested        
        if ((usr_s1.size()==1) && (usr_s1[0] == "jobs")){
            current_process(piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="kill")){
            kill_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="resume")){
            resume_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="suspend")){
            suspend_process(usr_s1.size(),usr_s1,piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[0]=="wait")){
            wait_process(usr_s1.size(),usr_s1,piddict);
        }
         else if ((usr_s1.size()>=2) && (usr_s1[0]=="sleep")){
            sleep_process(usr_s1.size(),usr_s1);
        }
        else if ((usr_s1.size()==1) && (usr_s1[0]== "exit")){
            exit_state=1;
            exit_programme(piddict);
        }
        else if ((usr_s1.size()>=2) && (usr_s1[usr_s1.size()-1]== "&")){
            shell_running(usr_s1.size(),1,usr_s1,piddict);
        }
        else if (usr_s1.size()>0){
            shell_running(usr_s1.size(),0,usr_s1,piddict);
        }
        // clean the input
        usr_instruct.clear();
        usr_s1.clear();


    }
    return 0;
}