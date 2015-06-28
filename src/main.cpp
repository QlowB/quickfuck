#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <vector>

#include "BfProgram.h"
#include "FastProgram.h"
#include "VeryFastProgram.h"

int main(int argc, const char* argv[])
{
	using namespace std;
	vector<string> args;
    for (int i = 1; i < argc; i++) {
		args.push_back(string(argv[i]));
	}

    bool noCompile = false;
    istream* input = &cin;

    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "-i")
            noCompile = true;
        else
            input = new ifstream(args[i].c_str());
	}
	
    std::string code;
    {
        stringstream ss;
        ss << input->rdbuf();
        code = ss.str();
    }
    
    clearerr(stdin);
    
    // cout << code;
    bf::BfBlock block(code);
    bf::fast::Program fastProgram(block);
    
    bf::RuntimeEnvironment re;
    
    if (noCompile) {
        fastProgram.run(re);
    } else {
        bf::fast::VeryFastProgram vfp(fastProgram);
        vfp.run(2 * 1024 * 1024);
    }
	
	return 0;
}

