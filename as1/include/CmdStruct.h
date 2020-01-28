#pragma once
#include <string>
#include <vector>

using namespace std;

struct CmdStruct
{
    bool ampersand;
    vector<vector<string>> cmdVector;
    CmdStruct(){};
    CmdStruct(vector<vector<string>> cmdVector, bool ampersand)
    {
        cmdVector=cmdVector;
        ampersand=ampersand;
    }
};