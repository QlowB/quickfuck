#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <vector>

#include "BfProgram.h"
#include "FastProgram.h"
#include "SystemInfo.h"
#include "VeryFastProgram.h"

void printHelp(void);
const char* progName;



int main(int argc, const char* argv[])
{
    using namespace std;
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(string(argv[i]));
    }

    progName = argv[0];

    bool noCompile = false;
    bool help = false;
    bool circular = false;
    istream* input = &cin;

    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "-n" || args[i] == "--no-jit")
            noCompile = true;
        else if (args[i] == "-h" || args[i] == "--help")
            help = true;
        else if (args[i] == "-c" || args[i] == "--circular")
            circular = true;
        else
            input = new ifstream(args[i].c_str());
    }

    if (help) {
        printHelp();
        return 0;
    }

    if (input == &cin && qf::stdoutIsTerminal()) {
        const std::string programInfo =
            std::string("Welcome to Quickfuck!\n\nPlease enter your code ") +
            "and submit by pressing Ctrl+D/Ctrl+Z\n"
            "For more information, please run \"" + progName + " --help\"\n\n";
 
        std::cout << programInfo;
    }

// just in time compilation is not yet implemented on windows
// or any non x64 systems
#ifndef USE_JUST_IN_TIME_COMPILER
    noCompile = true;
#endif

    
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
        block.run(re);
        //fastProgram.run(re);
    }
#ifdef USE_JUST_IN_TIME_COMPILER
    else {
        bf::fast::VeryFastProgram vfp(fastProgram);
        vfp.run(8 * 1024 * 1024);
    }
#endif
    
    return 0;
}


void printHelp(void)
{
    std::cout << "Quickfuck is an optimizing interpreter for the esoteric "
    << "programming language brainfuck.\n\n"
    << "\tUsage: " << progName << " filename [-i] [-h]\n"
    << "\tArguments:\n\t\t"
        << "\t\t-h/--help displays this message\n"

        << "-n/--no-jit runs the brainfuck program without "
        << "just in time compilation (On non-POSIX systems (mainly Windows), "
        << "this is always the case).\n"
    
        << "\t\t-c/--circular enforces a circular data tape";
}



