#pragma once
#include <string>
#include <vector>

using namespace std;

struct CmdStruct
{
    bool ampersand;
    vector<vector<string> > cmdVector;
    CmdStruct(){};
    CmdStruct(vector<vector<string> > cmdVector, bool ampersand)
    {
        cmdVector=cmdVector; // Cmd line, split by '|' followed by argv's
        ampersand=ampersand; // Ends in ampersand?
    }
};