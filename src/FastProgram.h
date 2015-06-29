//
//  FastProgram.h
//  Quickfuck
//
//  Created by Nicolas Winkler on 30.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#ifndef _BRAINFUCKER_FASTPROGRAM_H
#define _BRAINFUCKER_FASTPROGRAM_H

#include <stdio.h>
#include <vector>
#include "RuntimeEnvironment.h"
#include "BfProgram.h"

namespace bf
{
    namespace fast
    {
        class Instruction;
        class AddConstant;
        class SetConstant;
        class AddVariable;
        class MovePointer;
        class AddMultipliedVariable;
        class AddMinusMultipliedVariable;
        class Print;
        class Read;
        
        class Loop;
        
        
        class Program;
    }
}


class bf::fast::Instruction
{
public:
    virtual ~Instruction(void);
    virtual void run(RuntimeEnvironment& re) = 0;
    virtual std::string getRawCCode(int ident) const = 0;
};


class bf::fast::Program :
public Instruction
{
    std::vector<Instruction*> instructions;
public:
    Program(void);
    Program(const bf::BfBlock& block);
    ~Program(void);
    virtual void run(RuntimeEnvironment& re);
    
    virtual void addInstruction(Instruction* i);
    virtual long getNInstructions(void) const;
    virtual const Instruction* getInstruction(long i) const;
    
    std::string createCCode(void) const;
protected:
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::AddConstant :
public Instruction
{
    char addValue;
    int relPos;
public:
    AddConstant(char value, int relPos);
    virtual void run(RuntimeEnvironment& re);
    char getValue(void) const;
    inline int getRelPos(void) const { return relPos; }
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::SetConstant :
public Instruction
{
    char value;
    int relPos;
public:
    SetConstant(char value, int relPos);
    virtual void run(RuntimeEnvironment& re);
    char getValue(void) const;
    inline int getRelPos(void) const { return relPos; }
    virtual std::string getRawCCode(int ident) const;
};


/// \brief another variable is added to the current variable
class bf::fast::AddVariable :
public Instruction
{
    /// \brief relative source position
    int sourcePos;
    
    /// \brief relative destination position
    int destPos;
public:
    AddVariable(int sourcePos, int destPos);
    virtual void run(RuntimeEnvironment& re);
    int getSourcePos(void) const;
    int getDestinationPos(void) const;
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::MovePointer :
public Instruction
{
    int addValue;
public:
    MovePointer(int relativePos);
    virtual void run(RuntimeEnvironment& re);
    virtual int getValue(void) const;
    virtual std::string getRawCCode(int ident) const;
};


/// \brief adds the multiple of a variable to another one
class bf::fast::AddMultipliedVariable :
public Instruction
{
    /// \brief relative position of the source variable
    int sourcePos;
    /// \brief factor
    int multiplier;
    /// \brief relative position of the target variable
    int targetPos;
public:
    AddMultipliedVariable(int sourcePos, int multiplier, int targetPos);
    virtual void run(RuntimeEnvironment& re);
    int getMultiplier(void) const;
    int getSourcePos(void) const;
    int getTargetPos(void) const;
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::Print :
public Instruction
{
    int relativePos;
    int repetitions;
public:
    Print(int repetitions, int relativePos);
    virtual void run(RuntimeEnvironment& re);
    virtual int getRepetitions(void) const;
    virtual int getRelativePosition(void) const;
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::Read :
public Instruction
{
    int relativePos;
    int repetitions;
public:
    Read(int repetitions, int relativePos);
    virtual void run(RuntimeEnvironment& re);
    virtual int getRepetitions(void) const;
    virtual int getRelativePosition(void) const;
    virtual std::string getRawCCode(int ident) const;
};


class bf::fast::Loop :
public Program
{
public:
    Loop(const BfBlock& block);
    virtual void run(RuntimeEnvironment& re);
    virtual std::string getRawCCode(int ident) const;
};


#endif // _BRAINFUCKER_FASTPROGRAM_H
