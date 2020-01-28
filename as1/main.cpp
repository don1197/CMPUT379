#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#include <InputParser.h>
#include <CommandExecutor.h>

using namespace std;


int main(int argc, char *argv[])
{
    InputParser ip = InputParser();
    CommandExecutor ce = CommandExecutor();

    while(1)
    {
        string inputBuffer;
        cout << "Input Cmd:\n-->";
        getline(cin, inputBuffer);
        
        CmdStruct parsedResults = ip.parse(inputBuffer);
        ce.execute(parsedResults);
        usleep(50); // 
    }
}