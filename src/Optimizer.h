//
//  Optimizer.h
//  Brainfucker
//
//  Created by Nicolas Winkler on 30.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#ifndef _BRAINFUCKER_OPTIMIZER_H
#define _BRAINFUCKER_OPTIMIZER_H

#include "BfProgram.h"
#include "FastProgram.h"

#include <queue>

namespace bf
{
    namespace fast
    {
        class Optimizer;
        class LoopInfo;
    }
}


class bf::fast::LoopInfo
{
public:
    struct Change
    {
        int position;
        char amount;
    };
    
private:
    friend class Optimizer;
    
    int positionShift;
    bool children;
    bool printOrRead;
    std::vector <Change> changes;
public:
    LoopInfo(void);
    
    bool isPositionConstant(void) const;
    bool hasChildren(void) const;
    bool hasPrintOrRead(void) const;
    
    void increment(int pos, char amount);
    char getIncrement(int pos) const;
};


class bf::fast::Optimizer
{
    const BfBlock* block;
    int instruction;
    int relativePosition;
    
    std::queue<Instruction*> nextInstructions;
public:
    Optimizer(const BfBlock* block);
    
    Instruction* getInstruction(void);
    
private:
    void enqueueInstruction(void);
    void checkPointerMovements(void);
    Instruction* checkAddSub(void);
    Instruction* checkPrintAndReads(void);
    Instruction* checkLoop(void);
    
    LoopInfo createLoopInfo(const BfLoop*);
};

#endif // _BRAINFUCKER_OPTIMIZER_H
