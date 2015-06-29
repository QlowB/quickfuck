//
//  FastProgram.cpp
//  Quickfuck
//
//  Created by Nicolas Winkler on 30.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#include "FastProgram.h"
#include "Optimizer.h"

#include <iostream>
#include <sstream>

using namespace bf;
using namespace fast;


// ===============================
// ========= Instruction =========
// ===============================
Instruction::~Instruction(void)
{
}


// ===============================
// =========== Program ===========
// ===============================
Program::Program(void)
{
}


Program::Program(const BfBlock& block)
{
    Optimizer optimizer(&block);
    
    Instruction* inst = optimizer.getInstruction();
    
    while (inst != 0) {
        instructions.push_back(inst);
        inst = optimizer.getInstruction();
    }
}


Program::~Program(void)
{
    for (int i = 0; i < instructions.size(); i++) {
        if (instructions[i] != 0)
            delete instructions[i];
        instructions[i] = 0;
    }
}


void Program::run(RuntimeEnvironment& re)
{
    for (int i = 0; i < instructions.size(); i++) {
        instructions[i]->run(re);
    }
}


void Program::addInstruction(Instruction* i)
{
    instructions.push_back(i);
}


long Program::getNInstructions(void) const
{
    return instructions.size();
}


const Instruction* Program::getInstruction(long i) const
{
    return instructions[i];
}


std::string Program::createCCode(void) const
{
    using std::string;
    
    string includes = "#include <stdio.h>\n#include <limits.h>\n#include <stdlib.h>\n#include <string.h>\n\n";
    string int16def = "#if (UINT_MAX == 0xFFFF)\ntypedef unsigned int UInt16;\n#else\ntypedef unsigned short UInt16;\n#endif\n\n";
    string defs = "void print(char);\nvoid read(char*);\n\n";
    string pre = "int main(void)\n{\n\tregister UInt16 pointer = 0;\n\tchar memory[0xFFFF];\n\tmemset(memory, 0, 0xFFFF);\n";
    
    string code = getRawCCode(1);
    
    string post = "}\n\n";
    
    string fulldefs = string("void print(char c)\n{\n") +
                        "\tputchar(c);\n}\n\n" +
                        "void read(char* c)\n{" +
                        "\tstatic int inputZero = 0;\n" +
                        "\tif (inputZero)\n\t\t*c = 0;\n" +
                        "\telse {\n\t\t*c = getchar();\n" +
                        "\t\tif (*c == EOF || *c == '\\n') {\n" +
                        "\t\t\t*c = 0;\n" +
                        "\t\t\tinputZero = 1;\n" +
                        "\t\t}\n\t}\n}\n";
    
    
    return includes + int16def + defs + pre + code + post + fulldefs;
}


std::string Program::getRawCCode(int ident) const
{
    std::string ret = "";
    
    for (std::vector<Instruction*>::const_iterator i = instructions.begin(); i != instructions.end(); i++) {
        ret += (*i)->getRawCCode(ident);
    }
    
    return ret;
}


// ===============================
// ========= AddConstant =========
// ===============================
AddConstant::AddConstant(char val, int destPos)
{
    this->addValue = val;
    this->relPos = destPos;
}


void AddConstant::run(RuntimeEnvironment& re)
{
    re.memory[re.memoryPointer + relPos] += addValue;
}


char AddConstant::getValue(void) const
{
    return addValue;
}


std::string AddConstant::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    ret << "memory[pointer + " << relPos << "] += " << ((int) this->addValue) << ";\n";
    return ret.str();
}


// ===============================
// ========= SetConstant =========
// ===============================
SetConstant::SetConstant(char val, int destPos)
{
    this->value = val;
    this->relPos = destPos;
}


void SetConstant::run(RuntimeEnvironment& re)
{
    re.memory[re.memoryPointer + relPos] = value;
}


char SetConstant::getValue(void) const
{
    return value;
}


std::string SetConstant::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    ret << "memory[pointer + " << relPos << "] = " << ((int) this->value) << ";\n";
    return ret.str();
}



// ===============================
// ========= AddVariable =========
// ===============================
AddVariable::AddVariable(int srcPos, int destPos)
{
    this->sourcePos = srcPos;
    this->destPos = destPos;
}


void AddVariable::run(RuntimeEnvironment& re)
{
    re.memory[re.memoryPointer + destPos] += re.memory[re.memoryPointer + sourcePos];
}


int AddVariable::getSourcePos(void) const
{
    return sourcePos;
}


int AddVariable::getDestinationPos(void) const
{
    return destPos;
}


std::string AddVariable::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    ret << "memory[pointer + " << destPos << "] += memory[pointer + " << this->sourcePos << "];\n";
    return ret.str();
}


// ===============================
// ========= MovePointer =========
// ===============================
MovePointer::MovePointer(int addValue)
{
    this->addValue = addValue;
}


void MovePointer::run(RuntimeEnvironment& re)
{
    re.memoryPointer += addValue;
}


int MovePointer::getValue(void) const
{
    return addValue;
}


std::string MovePointer::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    ret << "pointer += " << this->addValue << ";\n";
    return ret.str();
}


// ===============================
// ==== AddMultipliedVariable ====
// ===============================
AddMultipliedVariable::AddMultipliedVariable(int sourcePos, int multiplier, int targetPos)
{
    this->sourcePos = sourcePos;
    this->multiplier = multiplier;
    this->targetPos = targetPos;
}


void AddMultipliedVariable::run(RuntimeEnvironment& re)
{
    re.memory[re.memoryPointer + targetPos] += multiplier * re.memory[re.memoryPointer + sourcePos];
}


int AddMultipliedVariable::getMultiplier(void) const
{
    return multiplier;
}


int AddMultipliedVariable::getSourcePos(void) const
{
    return sourcePos;
}


int AddMultipliedVariable::getTargetPos(void) const
{
    return targetPos;
}


std::string AddMultipliedVariable::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    if (multiplier == 1)
        ret << "memory[pointer + " << targetPos << "] += memory[pointer + " << sourcePos << "];\n";
    else if (multiplier == -1)
        ret << "memory[pointer + " << targetPos << "] -= memory[pointer + " << sourcePos << "];\n";
    else
        ret << "memory[pointer + " << targetPos << "] += " << multiplier << " * memory[pointer + " << sourcePos << "];\n";
    return ret.str();
}


// ===============================
// ============ Print ============
// ===============================
Print::Print(int repetitions, int relativePos)
{
    this->repetitions = repetitions;
    this->relativePos = relativePos;
}


void Print::run(RuntimeEnvironment& re)
{
    for (int i = 0; i < repetitions; i++)
        std::cout << re.memory[re.memoryPointer + relativePos];
}


int Print::getRepetitions(void) const
{
    return repetitions;
}


int Print::getRelativePosition(void) const
{
    return relativePos;
}


std::string Print::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < repetitions; i++) {
        for (int i = 0; i < ident; i++)
            ret << "\t";
        ret << "print(memory[pointer + " << relativePos << "]);\n";
    }
    return ret.str();
}


// ================================
// ============= Read =============
// ================================
Read::Read(int repetitions, int relativePos)
{
    this->repetitions = repetitions;
    this->relativePos = relativePos;
}


void Read::run(RuntimeEnvironment& re)
{
    for (int i = 0; i < repetitions; i++)
        std::cin.read(&re.memory[re.memoryPointer + relativePos], 1);
}


int Read::getRepetitions(void) const
{
    return repetitions;
}


int Read::getRelativePosition(void) const
{
    return relativePos;
}


std::string Read::getRawCCode(int ident) const
{
    std::stringstream ret;
    
    for (int i = 0; i < repetitions; i++) {
        for (int i = 0; i < ident; i++)
            ret << "\t";
        ret << "read(&memory[pointer + " << relativePos << "]);\n";
    }
    return ret.str();
}


// ================================
// ============= Loop =============
// ================================
Loop::Loop(const BfBlock& b) :
Program(b)
{
}


void Loop::run(RuntimeEnvironment& re)
{
    while (re.memory[re.memoryPointer]) {
        Program::run(re);
    }
}


std::string Loop::getRawCCode(int ident) const
{
    std::stringstream ret;
    for (int i = 0; i < ident; i++)
        ret << "\t";
    
    ret << "while (memory[pointer]) {\n";
    
    ret << Program::getRawCCode(ident + 1);
    
    for (int i = 0; i < ident; i++)
        ret << "\t";
    ret << "}\n";
    
    return ret.str();
}

