//#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>

#include "afs.h"

using namespace std;
int main()
{
    int closeResult;
    //command to execute
    string cmd_ = "pkexec apt update -c " + string(getenv("HOME")) + "/.config/wali/conf/walideb.conf";
    string cmd_1 = "";
    //check if shell is available
    if(system(nullptr) == 0)
    {
        cout << "shell is not available" << std::endl;
        return 1;
    }

    //execute the command
    auto cmdOut = popen(cmd_.c_str(), "r");
    if(cmdOut == nullptr)
    {
        cout << "popen error" << std::endl;
        return 1;
    }

    //get the return value
    closeResult = pclose(cmdOut);
    if (closeResult != 0) {
        cout << "apt update is error" << endl;
        return 1;
    }



    return 0;
}
