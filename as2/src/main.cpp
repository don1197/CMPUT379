#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <queue>
#include <mutex>
#include <algorithm>

#include <pthread.h>
#include "ThreadRunner.hpp"
#include "WorkRequest.hpp"
#include "Tands_Sleep.hpp"
#include <chrono>
#include <time.h>
#include <signal.h>

using namespace std;

// Global Variables
queue<WorkRequest> pendingWork;
mutex m_pendingWork;
mutex m_ThreadSpinTracker;
vector<bool> isThreadSpinning;
time_t startTime;

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
    ThreadRunner tr = ThreadRunner((long) threadarg); // int throws a precision loss err
    tr.execute((long) threadarg);
    pthread_exit(0); // Should never happen
}

int main(int argc, char *argv[])
{
    // Local variables
    string inputBuffer;
    int numThreads = (argc >= 2) ? atoi(argv[1]) : 3;
    int id = (argc >= 3) ? atoi(argv[2]) : 0;
    pthread_t threads [numThreads];    

    m_pendingWork.lock(); // Don't start any thread until all threads are initialized

    for(int i = 0; i < numThreads ; i++ )
    {
        cout << "Creating thread " << i << endl;
        isThreadSpinning.push_back(true);
        pthread_create(&threads[i], NULL, createRunner, (void*) i );
    }
    isThreadSpinning.push_back(true); // isThreadSpinning[-1] is reseerved for the master thread

    startTime = clock();

    m_pendingWork.unlock(); // Start spinning threads

    while(1)
    {
        getline(cin, inputBuffer);

        if(cin.eof() && pendingWork.empty())
        {
            // Only for use if reading from a file, EOL is an empty line at the end of the text file
            bool anyRunning = true;
            for(auto i : isThreadSpinning)
            {
                anyRunning = anyRunning && i; // Want all true
            }
            if(anyRunning)
            {
                cout << "All threads are spinning. Exiting" << endl;
                return 0;
            }
        }

        if(!inputBuffer.empty())
        {
            string code = inputBuffer.substr(0,1);
            int num = atoi(inputBuffer.substr(1).c_str());

            if(num == 0)
            {
                cerr << "The numeric must be a real non-zero number" << endl;
            }
            else
            {
                if(code == "T" || code == "t")
                {
                    // Push to queue for threads to access
                    printf("%f | 0 | Got request for T%d\n", (double) difftime(clock(), startTime) / CLOCKS_PER_SEC, num);

                    m_pendingWork.lock();
                    pendingWork.push(WorkRequest(ModeEnum::mode_t, num));
                    m_pendingWork.unlock();
                }
                else if(code == "S" || code == "s")
                {
                    m_ThreadSpinTracker.lock();
                    isThreadSpinning[numThreads] = false;
                    m_ThreadSpinTracker.unlock();
                    printf("%f | 0 | Main thread sleeping for %d\n", (double) difftime(clock(), startTime) / CLOCKS_PER_SEC, num);
                    Tands::Sleep(num);
                    printf("%f | 0 | Main thread done sleeping\n", (double) difftime(clock(), startTime) / CLOCKS_PER_SEC);
                    m_ThreadSpinTracker.lock();
                    isThreadSpinning[numThreads] = true;
                    m_ThreadSpinTracker.unlock();
                }
                else
                {
                    cerr << "The letter must be either \'T\' or \'S\'" << endl;
                }
            }
        }
    }
    
    pthread_exit(0);

    return 0;

}