#pragma once
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <string>
#include <vector>

#include <CmdStruct.h>

using namespace std;

class InputParser
{
    public:
        InputParser();
        CmdStruct parse(string input);
};
#endif