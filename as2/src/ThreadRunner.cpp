#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unistd.h>

#include "ThreadRunner.hpp"
#include "Tands_Trans.hpp"

using namespace std;

// Import from main.cpp
extern int freeThreads;
extern mutex m_queueAccessor;
extern mutex m_freeThreadsAccessor;
extern queue<WorkRequest> pendingWork;
extern time_t startTime;

ThreadRunner::ThreadRunner()
{
    // Do nothing
}

double ThreadRunner::getTimeSinceStart()
{
    // Get ms between now and startTime
    return (double) difftime(clock(), startTime) / CLOCKS_PER_SEC ;
}

void ThreadRunner::execute(int i)
{
    int threadNum = i + 1; // Keep it local

    WorkRequest work;
    m_freeThreadsAccessor.lock();
    printf("Starting Thread %d\n", i + 1);
    m_freeThreadsAccessor.unlock();
    while(1)
    {
        m_queueAccessor.lock();

        if(pendingWork.empty())
        {
            m_queueAccessor.unlock();
            usleep(10); // Sleep for a bit, then reenter the queue, gives other threads a chance to get in
        }
        else
        {
            // Found
            work = pendingWork.front();
            pendingWork.pop();
            printf("%f | ID = %d | Q = %d | Ask\n", (double) (difftime(clock(), startTime) / CLOCKS_PER_SEC), threadNum, pendingWork.size());

            m_queueAccessor.unlock();

            if(work.isInitialized())
            {
                printf("%f | ID = %d | Q = %d | Work | %d \n", (double) (difftime(clock(), startTime) / CLOCKS_PER_SEC), threadNum, pendingWork.size(), work.count);
                m_freeThreadsAccessor.lock();
                freeThreads--;
                m_freeThreadsAccessor.unlock();

                Tands::Trans(work.count);

                m_freeThreadsAccessor.lock();
                freeThreads++;
                m_freeThreadsAccessor.unlock();
                printf("%f | ID = %d | Q = %d | Complete \n", (double) difftime(clock(), startTime) / CLOCKS_PER_SEC, threadNum, pendingWork.size());

                work = WorkRequest();   // Clear it
            }
        }
    }
}

