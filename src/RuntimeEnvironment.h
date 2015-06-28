//
//  Runtime.h
//  Brainfucker
//
//  Created by Nicolas Winkler on 29.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#ifndef _BRAINFUCKER_RUNTIMEENVIRONMENT_H
#define _BRAINFUCKER_RUNTIMEENVIRONMENT_H

#include "Integers.h"

namespace bf
{
    class RuntimeEnvironment;
    namespace fast {
        class Instruction;
    }
}


class bf::RuntimeEnvironment
{
public: // public because time critical
    char* memory;
    int memorySize;
    int memoryPointerMask;
    int memoryPointer;
public:
    RuntimeEnvironment(void);
    ~RuntimeEnvironment(void);
    
    void run(char c);
    bool isZero(void) const;
};


#endif // _BRAINFUCKER_RUNTIMEENVIRONMENT_H
