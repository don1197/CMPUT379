#include <iostream>
#include <mutex>
#include <queue>
#include <unistd.h>
#include <thread>

#include "ThreadRunner.hpp"
#include "Tands_Trans.hpp"

using namespace std;

// Import from main.cpp
extern mutex m_pendingWork;
extern queue<WorkRequest> pendingWork;
extern vector<bool> isThreadSpinning;
extern mutex m_ThreadSpinTracker;
extern time_t startTime;

ThreadRunner::ThreadRunner(int i)
{
    cout << "Creating thread runner " << i << endl;
}

double ThreadRunner::getTimeSinceStart()
{
    return (double) difftime(clock(), startTime) / CLOCKS_PER_SEC ;
}

void ThreadRunner::execute(int i)
{
    int threadNum = i; // Keep it local
    WorkRequest work;

    while(1)
    {
        startLabel:
        m_pendingWork.lock();
        time_t timeout = clock();
        while(pendingWork.empty())
        {
            // cout << (double) (difftime(clock(), startTime) / CLOCKS_PER_SEC) << endl;
            if(((double) difftime(clock(), startTime) / CLOCKS_PER_SEC) > 0.01) // If 
            {
                m_pendingWork.unlock();
                usleep(10);
                goto startLabel;
            }
            // Do Nothing
        }
        
        work = pendingWork.front();
        pendingWork.pop();
        m_pendingWork.unlock();

        m_ThreadSpinTracker.lock();
        isThreadSpinning[threadNum] = false;
        m_ThreadSpinTracker.unlock();

        if(!work.isNull)
        {
            switch(work.mode)
            {
                case ModeEnum::mode_t:
                {
                    printf("%f | %d | Thread %d got a T%d\n", getTimeSinceStart(), threadNum, threadNum, work.count);
                    Tands::Trans(work.count);
                    break;
                }
                case ModeEnum::mode_s:
                    cout << "Thread got sleeper" << endl;
                    break;
                default:
                    cerr << "Invalid mode type" << endl;
                    break;
            }
            printf("%f | %d | Thread %d is done working\n", getTimeSinceStart(), threadNum, threadNum);

            work = WorkRequest();
            m_ThreadSpinTracker.lock();
            isThreadSpinning[threadNum] = true;
            m_ThreadSpinTracker.unlock();
        }
    }
}

