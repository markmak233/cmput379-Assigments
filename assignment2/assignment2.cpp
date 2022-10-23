#include "assignment2.h"

using namespace std;
void writefiles(string filename,vector<string> outputss){
    // geting a list of string and save it into a file
    //https://www.tutorialspoint.com/how-to-append-text-to-a-text-file-in-cplusplus
    for (int ind=0;ind<outputss.size();ind++){
        cout << ind;
        ofstream fout;
        ifstream fin;
        fin.open(filename);
        fout.open(filename,ios_base::app);
        if (fout.is_open()){
            fout << outputss[ind];
        }
        fin.close();
        fout.close();
    }
    return;
}


int main(int argc,char* argv[]){
    if (argc<2 || argc>3){
        cout << "unexpected arguments inputed";
        return 1;
    }
    // number of thread
    int nThread = atoi(argv[1]);
    //output file name
    string filename;
    if (argc==3){
        string nid=argv[2];
        filename.append("prodcon.");
        filename.append(nid);
        filename.append(".log");
    } else{
        filename = "prodcon.log";
    }
    // inputting instructions
    vector<string> instruct;
    cout << "The following in input instructions:" << endl;
    int running=1;
    while (running){
        string as;
        cin >> as;
        cout << as << endl;
        instruct.push_back(as);
        if (as.size()==0){
            running=0;
        }
    }

    cout <<nThread << endl;


    vector<string>outputss;

    writefiles(filename,outputss);
    return 1;
}


