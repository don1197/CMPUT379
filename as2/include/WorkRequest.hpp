#pragma once
#ifndef WORKREQUEST_H
#define WORKREQUEST_H
enum class ModeEnum
{
    mode_t,
    mode_s
};

struct WorkRequest
{
    ModeEnum mode;
    int count;
    bool isNull = true;
    WorkRequest()
    {
        // NULL
    }
    WorkRequest(ModeEnum m, int n)
    {
        isNull = false;
        mode = m;
        count = n;
    }
};
#endif