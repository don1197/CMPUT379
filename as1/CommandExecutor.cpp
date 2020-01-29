#include <iostream>
#include <string>
#include <CommandExecutor.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <InputParser.h>
#include <sys/time.h>
#include <sys/resource.h>

#define STD_INPUT  0    /* file descriptor for standard input  */
#define STD_OUTPUT 1    /* file descriptor for standard output */
#define READ  0         /* read file descriptor from pipe  */
#define WRITE 1         /* write file descriptor from pipe */

using namespace std;

pid_t mainPid;
InputParser ip;

CommandExecutor::CommandExecutor()
{
    cout << "Main PID " << getpid() << endl;
    mainPid = getpid();
    ip = InputParser();
}

void CommandExecutor::myExit(vector<string> input)
{
    cout << "Exiting\n";
    exit(0);
}

void CommandExecutor::myJobs(vector<string> input)
{
    CommandExecutor().execute(ip.parse("ps -o pid:6,ppid:6,time:8,stat:5,cmd:50 --ppid " + to_string(mainPid) + " | grep -v " + to_string(mainPid)), false);
}

void CommandExecutor::myKill(vector<string> input)
{

    // Continue Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }
    CommandExecutor().execute(ip.parse("kill " + input[0]));
}

void CommandExecutor::myResume(vector<string> input)
{        
    // Continue Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    CommandExecutor().execute(ip.parse("kill -CONT " + input[0]), false);
}
void CommandExecutor::mySleep(vector<string> input)
{
    // Create a process that sleeps for x seconds
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    CommandExecutor().execute(ip.parse("sleep " + input[0]), false);
}
void CommandExecutor::mySuspend(vector<string> input)
{
    // Stop Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    CommandExecutor().execute(ip.parse("kill -STOP " + input[0]), false);
}
void CommandExecutor::myWait(vector<string> input)
{
    // Wait till X is done executing

}

int CommandExecutor::execute(CmdStruct cs)
{
    execute(cs, true);
}

int CommandExecutor::execute(CmdStruct cs, bool exeCustomFunc)
{
    // DO NOT PLACE IN HEADER, ISSUE WITH LINKING FUNC POINTERS
    map<string, function<void(vector<string>)>> funcMap = 
    {
        {"exit",  myExit},
        {"jobs", myJobs},
        {"kill", myKill},
        {"resume", myResume},
        {"sleep", mySleep},
        {"suspend", mySuspend},
        {"wait", myWait},
    };
    struct timeval start, end;
    struct rusage usage;

    for(int index = 0 ; index < cs.cmdVector.size() ; index ++ )
    {
        vector<string> processCmd = cs.cmdVector[index];

        string cmd = processCmd[0];

        if(exeCustomFunc && funcMap.find(cmd) != funcMap.end())
        {
            // Custom Commands
            vector<string> argvVector(processCmd.begin() + 1, processCmd.end());
            funcMap[cmd](argvVector);
        }
        else
        {
            // Execute Command
            int status;

            pid_t rc = fork();

            if(rc > 0)
            {
                // Parent Code
                if(!cs.ampersand)
                {
                    waitpid(rc, NULL, 0);  
                }
            }
            else if(rc == 0)
            {
                // Childcode
                char* argv[processCmd.size() + 1]; // NULL Terminator

                for(int i = 0 ; i < processCmd.size() ; i++ )
                {
                    argv[i] = (char*) processCmd[i].c_str();

                }
                argv[processCmd.size()] = NULL; 

                if(execvp(cmd.c_str(), argv) != 0)
                {
                    cerr << "Error: " << strerror(errno) << endl;
                    exit(1); // Kill child process if child
                }
                exit(0);
            }
            else
            {
                cerr << "Failed to fork\n";
                exit(1);
            }
        }
    }
    getrusage(RUSAGE_SELF, &usage);
    printf("sys seconds without microseconds: %ld\n", usage.ru_stime.tv_sec); 
    printf("sys microseconds: %ld\n", usage.ru_stime.tv_usec); 
    printf("user seconds without microseconds: %ld\n", usage.ru_utime.tv_sec); 
    printf("user microseconds: %ld\n", usage.ru_utime.tv_usec); 
    printf("total user seconds: %e\n", 
	   (double) usage.ru_utime.tv_sec 
	 + (double) usage.ru_utime.tv_usec / (double) 1000000); 

    return 0;
}