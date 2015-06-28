//
//  VeryFastProgram.h
//  Quickfuck
//
//  Created by Nicolas Winkler on 23.04.15.
//  Copyright (c) 2015 Winfor. All rights reserved.
//

#ifndef QUICKFUCK_VERYFASTPROGRAM_H_
#define QUICKFUCK_VERYFASTPROGRAM_H_

#ifndef _WIN32

#include "FastProgram.h"

#include <vector>

namespace bf
{
    namespace fast
    {
        class VeryFastProgram;
    }
}


class bf::fast::VeryFastProgram
{
    std::vector<unsigned char> code;
    void* mappedMemory;
    bool mac;
    
    /// relative addreos of the byte value currently stored in the al register.
    /// \warning not yet implemented
    int valueInAl;
public:
    VeryFastProgram(const fast::Program&);
    ~VeryFastProgram(void);
    
    void addProgram(const fast::Program&);
    
    void compile_x86_64(const Instruction* inst);
    
    void run(size_t memory);
    
private:
    void allocate(void);
    void addCode(const unsigned char* code, int length);
};


#endif // _WIN32

#endif // QUICKFUCK_VERYFASTPROGRAM_H_

