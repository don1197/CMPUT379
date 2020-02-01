#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include <InputParser.h>

using namespace std;

InputParser::InputParser()
{
    // Do Nothing
}

CmdStruct InputParser::parse(string input)
{
    CmdStruct output = CmdStruct();
    vector<string> parsedInput; 
    vector<string> unparsedCmd;

    // & Handling
    output.ampersand = boost::algorithm::ends_with(boost::algorithm::trim_copy(input), "&");
    if(output.ampersand)
    {
        // Assume & is the last character
        input = boost::algorithm::trim_copy(input).substr(0, input.size()-1);
    }

    boost::split(parsedInput, input, boost::is_any_of("|"), boost::token_compress_on);

    for(auto i : parsedInput)
    {
        unparsedCmd.push_back(boost::algorithm::trim_copy(i));
    }

    for(auto i : unparsedCmd)
    {
        vector<string> subCmd;
        vector<string> subCmdTrimed;
        boost::split(subCmd, i, boost::is_any_of(" "), boost::token_compress_on);

        for(auto i : subCmd)
        {
            subCmdTrimed.push_back(boost::algorithm::trim_copy(i));
        }

        output.cmdVector.push_back(subCmdTrimed);
    }

    return output;
}
