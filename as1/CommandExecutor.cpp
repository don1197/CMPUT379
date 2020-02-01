#include <boost/algorithm/string.hpp>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <CommandExecutor.h>
#include <InputParser.h>

#define STD_INPUT  0    /* FD for input  */
#define STD_OUTPUT 1    /* FD for output */
#define READ  0         /* Read FD from pipe  */
#define WRITE 1         /* Write FD from pipe */

using namespace std;

pid_t mainPid;
InputParser ip;
struct rusage usage;

CommandExecutor::CommandExecutor()
{
    mainPid = getpid();
    ip = InputParser();
}

void CommandExecutor::myExit(vector<string> input)
{
    // OS will handle the killing all child processes 
    cout << "Exiting\n";
    exit(0);
}

void CommandExecutor::myJobs(vector<string> input)
{
    FILE *file;
    string data;
    string cmd;
    int bufSize = 65536; // 2^16 should be enough
    char buffer[bufSize];

    // Display ps table
    cmd = "ps -o pid:6,time:8,stat:5,cmd:50 | grep -v bash | grep -v defunct | grep -v " + to_string(mainPid) + " | grep -v grep | grep -v ps";
    file = popen(cmd.c_str(),"r");

    if(file)
    {
        while(!feof(file))
        {
            if(fgets(buffer, bufSize, file) != NULL) 
            {
                data.append(buffer);
            }
        }
        pclose(file);
    }

    cout << data << endl; // EOFL is already part of the buffer, endl is for double space

    data = "";                  // Clear Output
    memset(buffer, 0, bufSize); // Clear Buffer

    // Get num of running processes
    // wc -l always return a 1 at the very least. Parant process?
    cmd = "ps -o pid:6,cmd:50 --no-headers | grep -v bash | grep -v defunct | grep -v " + to_string(mainPid) + " | grep -v grep | grep -v ps | wc -l";
    file = popen(cmd.c_str(),"r");

    if(file)
    {
        while(!feof(file))
        {
            if(fgets(buffer, bufSize, file) != NULL) 
            {
                data.append(buffer);
            }
        }
        pclose(file);
    }


    cout << "Running Processes:" << endl << "Active Processes = " << data; // EOFL is already part of the buffer
    
    cout << "Completed Processes:" << endl;

    getrusage (RUSAGE_SELF, &usage);
    
    cout << "--User Time = " << usage.ru_utime.tv_sec + static_cast<double>(usage.ru_utime.tv_usec) / 1000000 << endl;

    cout << "--System Time = " << usage.ru_stime.tv_sec + static_cast<double>(usage.ru_stime.tv_usec) / 1000000 << endl;
}

void CommandExecutor::myKill(vector<string> input)
{
    // Kill Process X
    if(input.size() != 1)
    {
        cerr << "Invalid amount of args" << endl;
        return;
    }

    CommandExecutor().execute(ip.parse("kill " + input[0]), false);
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
    // Wait till process X is done executing
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    int i;
    pid_t p = stoi(input[0]);
    cout << "Start Waiting" << endl;
    waitpid(p , &i, WUNTRACED);
    cout << "Done Waiting" << endl;
    // CommandExecutor().execute(ip.parse("tail --pid=" + input[0] + " -f /dev/null"), false);
}

int CommandExecutor::execute(CmdStruct cs)
{
    // Default method
    return execute(cs, true);
}

int CommandExecutor::execute(CmdStruct cs, bool exeCustomFunc)
{
    // cs - See CmdStruct.h
    // exeCustomFunc - Set to true if the function name is in the map below

    vector<string> processCmdA;
    vector<string> processCmdB;

    int size = cs.cmdVector.size();

    vector<pid_t> spawnedPid;

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

    int fd[2];

    processCmdA = cs.cmdVector[0];
    if(size == 1)
    {
        // One process
    }
    else if(size == 2)
    {
        // A | B
        processCmdB = cs.cmdVector[1];
        if(pipe(fd) < 0)
        {
            cerr << "Failed to pipe" << endl;
            return(1);
        }
    }
    else
    {
        // Talked to TA, dynamic pipeing is not a requirement. This will suffice.
        cerr << "WARNING, piping more then two processes is not supported" << endl;
        return 1;
    }

    if(exeCustomFunc && funcMap.find(processCmdA[0]) != funcMap.end())
    {
        // Special case for pre-defined functions in the map.
        vector<string> argvVector(processCmdA.begin() + 1, processCmdA.end());
        funcMap[processCmdA[0]](argvVector);
        return(0);
    }

    pid_t rc; // Note spawn the last process first

    // Process 2
    if(size == 2)
    {
        rc = fork();
    }

    if(size == 2 && rc == 0)
    {
        // Child
        spawnedPid.push_back(getpid());
        char* argv[processCmdB.size() + 1]; // NULL Terminator

        if(size == 2)
        {
            // Setup input for reading from pipe
            close(fd[WRITE]);
            close(READ);
            dup(fd[0]);
            close(fd[0]);

        }

        // Vector<string> to char**
        for(int i = 0 ; i < processCmdB.size() ; i++ )
        {
            argv[i] = (char*) processCmdB[i].c_str();
        }
        // Char ** EOFL
        argv[processCmdB.size()] = NULL; 

        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else if(size == 2 && rc < 0)
    {
        cerr << "Failed to fork\n";
        return 0;
    }
    
    rc = fork();
    // Process 1
    if(rc == 0)
    {
        // Child
        spawnedPid.push_back(getpid());
        char* argv[processCmdA.size() + 1]; // NULL Terminator

        if(size == 2)
        {
            // Setup output for writing to pipe
            close(fd[0]);
            close(1);
            dup(fd[1]);
            close(fd[1]);

        }

        // Vector<string> to char**
        for(int i = 0 ; i < processCmdA.size() ; i++ )
        {
            argv[i] = (char*) processCmdA[i].c_str();
        }

        // Char ** EOFL
        argv[processCmdA.size()] = NULL; 

        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else if(rc < 0)
    {
        cerr << "Failed to fork\n";
        return 0;
    }

    // Parent


    if(size == 2)
    {
        // Cleanup
        close(fd[READ]);
        close(fd[WRITE]);
    }
    if(!cs.ampersand)
    {
        waitpid(rc, NULL, 0);  
    }
    return 0;
}