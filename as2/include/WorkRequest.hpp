#pragma once
#ifndef WORKREQUEST_H
#define WORKREQUEST_H

struct WorkRequest
{
    int count = 0;
    WorkRequest()
    {
        // NULL
    }
    WorkRequest(int n)
    {
        count = n;
    }

    bool isInitialized()
    {
        return count != 0;
    }
};
#endif