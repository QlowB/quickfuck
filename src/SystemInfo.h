#ifndef SYSTEMINFO_H_
#define SYSTEMINFO_H_


#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64)

#ifndef _WIN32

#define USE_JUST_IN_TIME_COMPILER

#endif // _WIN32

#endif // x64

#ifdef _WIN32
#include <io.h>
#include <stdio.h>

namespace qf {
    inline bool stdoutIsTerminal(void)
    {
        return _isatty(_fileno(stdout));
    }
}

#else // probably POSIX

#include <unistd.h>
#include <stdio.h>

namespace qf {
    inline bool stdoutIsTerminal(void)
    {
        return isatty(fileno(stdout));
    }
}


#endif


#endif // SYSTEMINFO_H_




