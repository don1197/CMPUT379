/*
README 

Warning, if reading from a file, then the file must have an empty line at the end, otherwise the process will spin indefinatly
*/

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <time.h>
#include <unistd.h>

#include "ThreadRunner.hpp"
#include "WorkRequest.hpp"
#include "Tands_Sleep.hpp"

using namespace std;

// Global Variables
queue<WorkRequest> pendingWork;
mutex m_queueAccessor;
mutex m_freeThreadsAccessor;
time_t startTime;
int freeThreads;

namespace Tands
{
    /*
    Hack to get around linker re-definition issue. 
    */
    int TransSave = 0;
}

void *createRunner(void *threadarg)
{
    /*
    Creates the thread, and loop it forever
    */
    ThreadRunner tr = ThreadRunner();
    tr.execute((long) threadarg);
    pthread_exit(0); // Should never happen
}

void initializeThreads(pthread_t threads[], int n)
{
    // Initializes the ThreadRunner objects
    m_queueAccessor.lock();
    for(int i = 0; i < n ; i++ )
    {
        freeThreads++;
        pthread_create(&threads[i], NULL, createRunner, (void*) i );
    }
    m_queueAccessor.unlock();
    usleep(10); // Ensures printf isn't disjointed
}

int main(int argc, char *argv[])
{
    printf("Warning, if you are inputing from a file, then the file needs to end in a empty line (EOF), or the program will spin, and execute the last command indefinatly. \n");

    // Local Variables
    string inputBuffer;

    int numThreads = (argc >= 2) ? atoi(argv[1]) : 3;   // Default to 3 if none entered
    int id = (argc >= 3) ? atoi(argv[2]) : 0;           // 

    string outputFileName = (id != 0) ? "prodcom." + to_string(id) + ".log" : "prodcom.log";

    if(id != 0)
    {
        printf("Pipeing output to %s instead of terminal. Too see output on terminal remove 2nd argument\n", outputFileName.c_str());

        if(!freopen( outputFileName.c_str(), "w", stdout))
        {
            // For no enter, pipe it to screen instead.
            printf("Failed to create log file, exiting\n");
            return 1;
        }
    }

    pthread_t threads [numThreads];    

    initializeThreads(threads, numThreads);
    freeThreads++;
    startTime = clock();

    if(id == 0)
    {
        printf("Start Inputing: (T<N>, S<N>, or Q\n");
    }
    
    printf("Timestamp | Thread Number | Items in Queue | Action | Ammount\n");
    while(1)
    {
        getline(cin, inputBuffer);

        m_queueAccessor.lock();
        m_freeThreadsAccessor.lock();
        if(cin.eof() && freeThreads == numThreads + 1 && pendingWork.size() == 0)
        {
            printf("All work completed\n");
            break;
        }
        m_freeThreadsAccessor.unlock();
        m_queueAccessor.unlock();

        if(!inputBuffer.empty())
        {
            string code = inputBuffer.substr(0,1);
            int num = atoi(inputBuffer.substr(1).c_str());

            if(num <= 0)
            {
                cerr << "The numeric must be a real non-zero number" << endl;
            }
            else
            {
                if(code == "T" || code == "t")
                {
                    // Push to queue for threads to access
                    m_queueAccessor.lock();
                    printf("%f | ID = 0 | Q = %d | Receive | %d\n", (double) difftime(clock(), startTime) / CLOCKS_PER_SEC, pendingWork.size(), num);
                    pendingWork.push(WorkRequest(num));
                    m_queueAccessor.unlock();
                }
                else if(code == "S" || code == "s")
                {

                    printf("%f | ID = 0 | Q = %d | Sleep | %d\n", (double) (difftime(clock(), startTime) / CLOCKS_PER_SEC), pendingWork.size(), num);
                    
                    m_freeThreadsAccessor.lock();
                    freeThreads--;
                    m_freeThreadsAccessor.unlock();
                    
                    Tands::Sleep(num);  // Sleep for X cycles

                    m_freeThreadsAccessor.lock();
                    freeThreads++;
                    m_freeThreadsAccessor.unlock();

                    printf("%f | ID = 0 | Q = %d | Complete | %d\n", (double) (difftime(clock(), startTime) / CLOCKS_PER_SEC), pendingWork.size(), num);
                }
                else if(code == "q" || code == "Q")
                {
                    printf("User quit cmd");
                    break;
                }
                else
                {
                    cerr << "The letter must be either \'T\' or \'S\'" << endl;
                }
            }
        }
    }
    usleep(10); // Ensures printf isn't disjointed   
    printf("Killing all threads\nExiting");
    
    for(pthread_t i : threads)
    {
        pthread_kill(i, SIGINT); // Just crtl-c it
    }

    return 0;
}



