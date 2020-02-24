#pragma once
#ifndef THREADRUNNER_H
#define THREADRUNNER_H
#include "WorkRequest.hpp"

using namespace std;

class ThreadRunner
{
    public:
        ThreadRunner();
        void execute(int i);
        double getTimeSinceStart();
};

#endif