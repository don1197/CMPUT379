#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <sstream>

#include <InputParser.h>
#include <CommandExecutor.h>

using namespace std;


void handleSigint(int sig)
{
    // SIGINT will be received by all of the children process, and they will be killed
    // This method prevents the killing of this program.
    // Use CRTL + '\' to kill it

    cout << "Received SIGINT, killing all children processes" << endl;
    cout << "To Kill this program use crtl + \'\\\'" << endl;
}

int main(int argc, char *argv[])
{
    InputParser ip = InputParser();
    CommandExecutor ce = CommandExecutor();
    string inputBuffer;

    while(1)
    {
        signal(SIGINT, handleSigint);
        cout << "Input Cmd:\n-->";
        getline(cin, inputBuffer);

        if(!inputBuffer.empty())
        {
            CmdStruct parsedResults = ip.parse(inputBuffer);
            ce.execute(parsedResults);
        }
    }
}