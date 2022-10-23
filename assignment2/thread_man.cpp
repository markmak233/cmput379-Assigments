#include "thread_man.h"
#include "tands.h"

using namespace std;

vector<inst_kind> translate_txt_to_struct(vector<string> instru){
    vector<inst_kind> transed_instru;
    for (int ind=0;(unsigned)ind<instru.size()-1;ind++){
        struct inst_kind temp1;
        temp1.TS=instru[ind][0];
        string temp2 = instru[ind];
        temp2.erase(0,1);

        // https://www.educative.io/answers/how-to-convert-a-string-to-an-int-in-cpp
        stringstream ss;
        ss << temp2;
        ss >> temp1.numb;

        transed_instru.push_back(temp1);
    }

    return transed_instru;
}

vector<string> thread_management(int nThread,vector<string> instru){
    vector<inst_kind> translated_instru;
    translated_instru=translate_txt_to_struct(instru);

    // result output
    for (int i=0;(unsigned)i<translated_instru.size();i++){
        cout << instru[i] <<" "<< translated_instru[i].TS << " " << translated_instru[i].numb << endl;
    }
    vector<string> op1;
    return op1;

    
}
