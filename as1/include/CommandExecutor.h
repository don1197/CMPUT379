#pragma once

#include <string>
#include <vector>
#include <CmdStruct.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#include <map>
#include <functional>

using namespace std;

class CommandExecutor
{
    public:
        CommandExecutor();
        int execute(CmdStruct cs);

        map<string, function<void(vector<string>)>> funcMap = 
        {
            {"exit",  myExit},
            {"jobs", myJobs},
            {"kill", myKill}, // Disable me if you want to use kill cmd line
            {"resume", myResume},
            {"sleep", mySleep},
            {"suspend", mySuspend},
            {"wait", myWait},
        };

    private:
        char** convertToArgv(vector<string> input);
        void static myExit(vector<string> input);
        void static myJobs(vector<string> input);
        void static myKill(vector<string> input);
        void static myResume(vector<string> input);
        void static mySleep(vector<string> input);
        void static mySuspend(vector<string> input);
        void static myWait(vector<string> input);

};


