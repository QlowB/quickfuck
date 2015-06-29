//
//  BfProgram.cpp
//  Quickfuck
//
//  Created by Nicolas Winkler on 29.10.14.
//  Copyright (c) 2014 Winfor. All rights reserved.
//

#include "BfProgram.h"


using namespace bf;


BfInstruction::~BfInstruction(void)
{
}


BfChar::BfChar(char c)
{
    this->c = c;
}


void BfChar::run(RuntimeEnvironment& re)
{
    re.run(c);
}


char BfChar::getChar(void) const
{
    return c;
}


BfBlock::BfBlock(const std::string& code)
{
    for (std::string::const_iterator i = code.begin(); i != code.end(); i++) {
        switch ((*i)) {
            case '+':
            case '-':
            case '<':
            case '>':
            case '.':
            case ',':
                this->code.push_back(new BfChar(*i));
                break;
            case '[':
                int level = 1;
                i++;
                std::string::const_iterator blockBegin = i;
                while (i != code.end() && level > 0) {
                    if (*i == '[')
                        level ++;
                    else if
                        (*i == ']')
                        level --;
                    i++;
                }
                i--;
                std::string loop = code.substr(blockBegin - code.begin(), i - blockBegin);
                this->code.push_back(new BfLoop(loop));
        }
    }
}


BfBlock::~BfBlock(void)
{
    for (std::vector<BfInstruction*>::const_iterator i = code.begin(); i != code.end(); i++) {
        delete *i;
    }
}


void BfBlock::run(RuntimeEnvironment& re)
{
    for (std::vector<BfInstruction*>::const_iterator i = code.begin(); i != code.end(); i++) {
        (*i)->run(re);
    }
}


char BfBlock::getChar(void) const
{
    return 0;
}


const BfInstruction* BfBlock::getInstruction(int index) const
{
    return code[index];
}


long BfBlock::getNInstructions(void) const
{
    return code.size();
}


BfLoop::BfLoop(const std::string& code) :
BfBlock(code)
{
}


void BfLoop::run(RuntimeEnvironment& re)
{
    while (!re.isZero()) {
        BfBlock::run(re);
    }
}
