//
//  BfProgram.h
//  Brainfucker
//
//  Created by Nicolas Winkler on 29.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#ifndef _BRAINFUCKER_BFPROGRAM_H
#define _BRAINFUCKER_BFPROGRAM_H

#include "RuntimeEnvironment.h"

#include <vector>
#include <string>

namespace bf
{
    class BfInstruction;
    class BfChar;
    class BfBlock;
    class BfLoop;
}


class bf::BfInstruction
{
public:
    virtual ~BfInstruction(void);
    virtual void run(RuntimeEnvironment&) = 0;
    virtual char getChar(void) const = 0;
};


class bf::BfChar :
public BfInstruction
{
    char c;
public:
    BfChar(char c);
    virtual void run(RuntimeEnvironment& re);
    virtual char getChar(void) const;
};


class bf::BfBlock :
public BfInstruction
{
    std::vector<BfInstruction*> code;
public:
    BfBlock(const std::string& code);
    ~BfBlock(void);
    virtual void run(RuntimeEnvironment& re);
    virtual char getChar(void) const;
    
    virtual const BfInstruction* getInstruction(int index) const;
    virtual long getNInstructions(void) const;
};


class bf::BfLoop :
public BfBlock
{
public:
    BfLoop(const std::string& code);
    virtual void run(RuntimeEnvironment& re);
};

#endif // _BRAINFUCKER_BFPROGRAM_H

