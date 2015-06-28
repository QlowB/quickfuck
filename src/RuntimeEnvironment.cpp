//
//  Runtime.cpp
//  Brainfucker
//
//  Created by Nicolas Winkler on 29.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#include "RuntimeEnvironment.h"
#include <cstring>
#include <iostream>

using namespace bf;

RuntimeEnvironment::RuntimeEnvironment(void)
{
    memorySize = 4096;
    memoryPointerMask = 0x7FF;
    memory = new char[memorySize];
    memset(memory, 0, memorySize);
    memoryPointer = 0;
}


RuntimeEnvironment::~RuntimeEnvironment(void)
{
    if (memory != 0) {
        delete memory;
        memory = 0;
    }
}


void RuntimeEnvironment::run(char c)
{
    switch (c) {
        case '+':
            memory[memoryPointer]++;
            break;
        
        case '-':
            memory[memoryPointer]--;
            break;
            
        case '>':
            memoryPointer ++;
            memoryPointer &= memoryPointerMask;
            break;
            
        case '<':
            memoryPointer --;
            memoryPointer &= memoryPointerMask;
            break;
            
        case '.':
            std::cout << memory[memoryPointer];
            break;
            
        case ',':
            std::cin >> memory[memoryPointer];
            break;
    }
}


bool RuntimeEnvironment::isZero(void) const
{
    return memory[memoryPointer] == 0;
}
