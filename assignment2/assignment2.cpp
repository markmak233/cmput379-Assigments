# include "assignment2.h"
# include "thread_man.h"

using namespace std;\

void writefiles(string filename,vector<string> outputss){
    // geting a list of string and save it into a file
    //https://www.tutorialspoint.com/how-to-append-text-to-a-text-file-in-cplusplus
    cout << outputss.size() <<endl;
    for (int ind=0;(unsigned)ind<outputss.size();ind++){
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
    int running=1;
    while (running){
        string as;
        cin >> as;
        instruct.push_back(as);
        if (as.size()==0){
            running=0;
        }
    }
    //cout <<" start running to looging device" << endl;
    vector<string>outputss;

    outputss=event_management(nThread,instruct);

    cout << "writting" << endl;
    // write file to dedicated destinaction.
    writefiles(filename,outputss);
    cout << "entireprograme done" <<endl;
    return 1;
}


