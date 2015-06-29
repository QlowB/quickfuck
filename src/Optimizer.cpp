//
//  Optimizer.cpp
//  Quickfuck
//
//  Created by Nicolas Winkler on 30.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#include "Optimizer.h"


using namespace bf;
using namespace fast;


LoopInfo::LoopInfo(void)
{
    children = false;
    positionShift = 0;
    printOrRead = false;
}


bool LoopInfo::isPositionConstant(void) const
{
    return positionShift == 0;
}


bool LoopInfo::hasChildren(void) const
{
    return children;
}


bool LoopInfo::hasPrintOrRead(void) const
{
    return printOrRead;
}


void LoopInfo::increment(int pos, char amount)
{
    bool foundPos = false;
    for (std::vector<Change>::iterator i = changes.begin(); i != changes.end(); i++) {
        Change& c = *i;
        if (c.position == pos) {
            c.amount += amount;
            foundPos = true;
            if (c.amount == 0) {
                i = changes.erase(i);
                i--;
            }
        }
    }
    
    if (!foundPos) {
        Change c;
        c.position = pos;
        c.amount = amount;
        changes.push_back(c);
    }
}


char LoopInfo::getIncrement(int pos) const
{
    for (std::vector<Change>::const_iterator i = changes.begin(); i != changes.end(); i++) {
        const Change& c = *i;
        if (c.position == pos) {
            return c.amount;
        }
    }
    
    return 0;
}


Optimizer::Optimizer(const BfBlock* block)
{
    this->block = block;
    instruction = 0;
    relativePosition = 0;
}


Instruction* Optimizer::getInstruction(void)
{
    if (nextInstructions.empty())
        enqueueInstruction();
    
    if (!nextInstructions.empty()) {
        Instruction* inst = nextInstructions.front();
        nextInstructions.pop();
        return inst;
    }
    
    return 0;
}


void Optimizer::enqueueInstruction(void)
{
    checkPointerMovements();
    
    if (instruction < block->getNInstructions()) {
        
        Instruction* inst = checkAddSub();
        if (inst == 0)
            inst = checkPrintAndReads();
        
        if (inst != 0) {
            nextInstructions.push(inst);
        } else {
            inst = checkLoop();
            if (inst != 0)
                nextInstructions.push(inst);
        }
    } else if (relativePosition != 0) {
        nextInstructions.push(new MovePointer(relativePosition));
        relativePosition = 0;
    }
}


void Optimizer::checkPointerMovements(void)
{
    if (instruction >= block->getNInstructions())
        return;
    
    const BfInstruction* bi = block->getInstruction(instruction);
    
    while (bi->getChar() == '<' || bi->getChar() == '>') {
        if (bi->getChar() == '<')
            relativePosition--;
        else if (bi->getChar() == '>')
            relativePosition++;
        
        
        instruction ++;
        if (instruction >= block->getNInstructions())
            break;
        bi = block->getInstruction(instruction);
    }
}


/*Instruction* Optimizer::checkRepetitions(void)
{
    const BfInstruction* bi = block->getInstruction(instruction);
    
    if (bi->getChar() != '+' && bi->getChar() != '-' && bi->getChar() != '.' && bi->getChar() != ',' )
        return 0;
    
    char firstChar = bi->getChar();
    
    int repetitions = 0;
    while (bi->getChar() == firstChar) {
        repetitions ++;

        instruction ++;
        if(instruction >= block->getNInstructions())
            break;
        bi = block->getInstruction(instruction);
    }
    
    switch (firstChar) {
        case '+':
            return new AddConstant(repetitions, relativePosition);
        case '-':
            return new AddConstant(-repetitions, relativePosition);
        /*case '>':
            // return new MovePointer(repetitions);
            relativePosition += repetitions;
            break;
        case '<':
            // return new MovePointer(-repetitions);
            relativePosition -= repetitions;
            break;*//*
        case '.':
            return new Print(repetitions, relativePosition);
        case ',':
            return new Read(repetitions, relativePosition);
            
        default:
            break;
    }
    
    return 0;
}*/


Instruction* Optimizer::checkAddSub(void)
{
    const BfInstruction* bi = block->getInstruction(instruction);
    
    if (bi->getChar() != '+' && bi->getChar() != '-')
        return 0;
    
    int value = 0;
    
    while (bi->getChar() == '+' || bi->getChar() == '-') {
        if (bi->getChar() == '+')
            value++;
        else
            value--;
        
        instruction ++;
        if(instruction >= block->getNInstructions())
            break;
        bi = block->getInstruction(instruction);
    }
    
    return new AddConstant(value, relativePosition);
}


Instruction* Optimizer::checkPrintAndReads(void)
{
    const BfInstruction* bi = block->getInstruction(instruction);
    
    if (bi->getChar() != '.' && bi->getChar() != ',' )
        return 0;
    
    char firstChar = bi->getChar();
    
    int repetitions = 0;
    while (bi->getChar() == firstChar) {
        repetitions ++;
        
        instruction ++;
        if(instruction >= block->getNInstructions())
            break;
        bi = block->getInstruction(instruction);
    }
    
    switch (firstChar) {
        case '.':
            return new Print(repetitions, relativePosition);
        case ',':
            return new Read(repetitions, relativePosition);
            
        default:
            break;
    }
    
    return 0;
}


Instruction* Optimizer::checkLoop(void)
{
    const BfInstruction* bi = block->getInstruction(instruction);
    
    const BfLoop* bl = dynamic_cast<const BfLoop*>(bi);
    Instruction* optLoop = 0;
    if (bl != 0) {
        instruction++;
        if (bl->getNInstructions() == 1 && bl->getInstruction(0)->getChar() == '-')
            return new SetConstant(0, relativePosition);
        
        LoopInfo li = createLoopInfo(bl);
        
        if (li.isPositionConstant() && !li.hasPrintOrRead()) {
            if (!li.hasChildren()) {
                if (li.getIncrement(0) == -1 || li.getIncrement(0) == 1) {
                    Program* prog = new Program();
                    for (int i = 0; i < li.changes.size(); i++) {
                        const LoopInfo::Change& c = li.changes[i];
                        if (c.position != 0) {
                            Instruction* inst =
                                new AddMultipliedVariable(relativePosition, li.getIncrement(0) * -c.amount, c.position + relativePosition);
                            prog->addInstruction(inst);
                        }
                    }
                    prog->addInstruction(new SetConstant(0, relativePosition));
                    optLoop = prog;
                }
            } else {
                
            }
        }
        
        if (optLoop == 0) {
            if (relativePosition != 0)
                nextInstructions.push(new MovePointer(relativePosition));
            relativePosition = 0;
            optLoop = new Loop(*bl);
        }
    }
    
    return optLoop;
}


LoopInfo Optimizer::createLoopInfo(const BfLoop* loop)
{
    LoopInfo li;
    
    for (int i = 0; i < loop->getNInstructions(); i++) {
        const BfInstruction* inst = loop->getInstruction(i);
        
        if (inst->getChar() == '>')
            li.positionShift ++;
        else if (inst->getChar() == '<')
            li.positionShift --;
        
        else if (inst->getChar() == '+') {
            li.increment(li.positionShift, 1);
        }
        else if (inst->getChar() == '-') {
            li.increment(li.positionShift, -1);
        }
        else if (inst->getChar() == '.' || inst->getChar() == ',') {
            li.printOrRead = true;
        }

        const BfLoop* possibleLoop = dynamic_cast<const BfLoop*>(inst);
        if (possibleLoop != 0) {
            li.children = true;
            LoopInfo childInfo = createLoopInfo(possibleLoop);
            li.positionShift += childInfo.positionShift;
            if (li.hasPrintOrRead())
                li.printOrRead = true;
        }
    }
    
    return li;
}
