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

#define STD_INPUT  0    /* file descriptor for standard input  */
#define STD_OUTPUT 1    /* file descriptor for standard output */
#define READ  0         /* read file descriptor from pipe  */
#define WRITE 1         /* write file descriptor from pipe */

using namespace std;

pid_t mainPid;


CommandExecutor::CommandExecutor()
{
    cout << "Main PID " << getpid() << endl;
    mainPid = getpid();
}

char** CommandExecutor::convertToArgv(vector<string> input)
{
    char* argv[input.size() + 1]; // NULL Terminator

    for(int i = 0 ; i < input.size() ; i++ )
    {
        argv[i] = (char*) input[i].c_str();

    }
    argv[input.size()] = NULL; 
    return argv;
}

void CommandExecutor::myExit(vector<string> input)
{
    cout << "Exiting\n";
    exit(0);
}

void CommandExecutor::myJobs(vector<string> input)
{
    // Show processes

    pid_t rc = fork();
    int p[2];
    int status;


    if(rc > 0)
    {
        wait(&rc);
    }
    else if(rc == 0)
    {
        if(pipe(p) < 0)
        {
            cerr << "Failed to create pipe" << endl;
        }

        char *argv1[] = {"ps", "-o", "pid:6,ppid:6,time:8,stat:5,cmd:50", "--ppid", strdup(to_string(mainPid).c_str()) , NULL};
        char *argv2[] = {"grep", "-v", strdup(to_string(getpid()).c_str()) , NULL}; // Parent will be ps, Child will be grep
        rc = fork();

        if(rc > 0 )
        {
            close( STD_INPUT );
            dup(p[READ]);
            close(p[READ]);
            close(p[WRITE]);
            execvp(argv2[0], argv2);
        }
        else if (rc == 0)
        {
            // Child
            close(STD_OUTPUT);

            dup( p[WRITE]);
            close( p[READ]);

            execvp(argv1[0], argv1);
        }
        else
        {
            cerr << "Failed to create pipe" << endl;
        }
        

        // if(execvp((char *) string("ps").c_str(), argv) != 0)
        // {
        //     cerr << "Error: " << strerror(errno) << endl;
        //     exit(1); // Kill child process if child
        // }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}

void CommandExecutor::myKill(vector<string> input)
{

    // Continue Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    pid_t rc = fork();

    if(rc > 0)
    {
        waitpid(rc, NULL, 0);                
    }
    else if(rc == 0)
    { 
        char *argv[] = {"kill", strdup(input[0].c_str()) , NULL};

        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}

void CommandExecutor::myResume(vector<string> input)
{
    // Continue Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    pid_t rc = fork();

    if(rc > 0)
    {
        // Run in background
        // waitpid(rc, NULL, 0);                

    }
    else if(rc == 0)
    { 
        char *argv[] = {"kill", "-CONT", strdup(input[0].c_str()) , NULL};

        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}
void CommandExecutor::mySleep(vector<string> input)
{
    // Create a process that sleeps for x seconds

    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    pid_t rc = fork();

    if(rc > 0)
    {
        waitpid(rc, NULL, 0);                
    }
    else if(rc == 0)
    {
        // cout << "Child PID: " << getpid() << endl;

        char *argv[] = {"sleep", strdup(input[0].c_str()) , NULL};

        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}
void CommandExecutor::mySuspend(vector<string> input)
{
    // Stop Process X
    if(input.size() != 1)
    {
        cerr << "Invalid ammount of args" << endl;
        return;
    }

    pid_t rc = fork();

    if(rc > 0)
    {
        // Run in background
        // waitpid(rc, NULL, 0);                

    }
    else if(rc == 0)
    { 
        char *argv[] = {"kill", "-STOP", strdup(input[0].c_str()) , NULL};
        
        if(execvp(argv[0], argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}
void CommandExecutor::myWait(vector<string> input)
{
    // Wait till X is done executing
    pid_t rc = fork();

    if(rc > 0)
    {
        wait(&rc);
    }
    else if(rc == 0)
    {

        char *argv[] = {"ps", "--no-headers", "-o", "pid:6,cmd:50", "--ppid", strdup(to_string(mainPid).c_str()) , NULL};

        if(execvp((char *) string("ps").c_str(), argv) != 0)
        {
            cerr << "Error: " << strerror(errno) << endl;
            exit(1); // Kill child process if child
        }
    }
    else
    {
        cerr << "Failed to fork\n";
        exit(1);
    }
}

int CommandExecutor::execute(CmdStruct cs)
{
    for(vector<string> processCmd : cs.cmdVector)
    {
        string cmd = processCmd[0];
        if(funcMap.find(cmd) != funcMap.end())
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
                else
                {
                    usleep(50); // To prevent cout collisions with child              
                }
            }
            else if(rc == 0)
            {
                // Childcode
                cout << "Child PID: " << getpid() << endl;
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
    // cout << "Done execute\n";

    return 0;
}