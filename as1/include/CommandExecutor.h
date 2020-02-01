#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <CmdStruct.h>

using namespace std;

class CommandExecutor
{
    public:
        CommandExecutor();
        int execute(CmdStruct cs);
        int execute(CmdStruct cs, bool exeCustomFunc);

    private:
        // Method pointer methods
        void static myExit(vector<string> input);
        void static myJobs(vector<string> input);
        void static myKill(vector<string> input);
        void static myResume(vector<string> input);
        void static mySleep(vector<string> input);
        void static mySuspend(vector<string> input);
        void static myWait(vector<string> input);

};


