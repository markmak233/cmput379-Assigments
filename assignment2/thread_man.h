# include <iostream>
# include <stdio.h>
# include <string>
# include <vector>
# include <fstream>
# include <sstream>

struct inst_kind{
    std::string TS;
    int numb; 
};


std::vector<std::string> thread_management(int nThread,std::vector<std::string> instru);