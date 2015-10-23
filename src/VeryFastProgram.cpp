//
//  VeryFastProgram.cpp
//  Quickfuck
//
//  Created by Nicolas Winkler on 23.04.15.
//  Copyright (c) 2015 Winfor. All rights reserved.
//


#include "VeryFastProgram.h"

#ifdef USE_JUST_IN_TIME_COMPILER


#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <iostream>

using namespace bf::fast;


VeryFastProgram::VeryFastProgram(const bf::fast::Program& prog)
{
    mappedMemory = 0;
    
#ifdef __APPLE__
    mac = true;
#else
    mac = false;
#endif
    
    addProgram(prog);

    
    code.push_back(0xc3);
}


VeryFastProgram::~VeryFastProgram(void)
{
}


void VeryFastProgram::addProgram(const fast::Program& prog)
{
    unsigned const char init[] = { 0x48, 0x89, 0xFB };
    addCode(init, sizeof init);

    for (int i = 0; i < prog.getNInstructions(); i++) {
        const Instruction* inst = prog.getInstruction(i);
        compile_x86_64(inst);
    }
}


void printchar(void) // prints the content of the r8b register
{
    int a = 500;
    char c = a * 2;
    asm volatile("movb %%r8b, %0":"=a"(c));
    putchar(c);
}


void readchar(void) // reads one byte into the r8b register
{
    char c = getchar();
    asm volatile("movb %0, %%r8b": : "a"(c));
    putchar(c);
}


void VeryFastProgram::compile_x86_64(const bf::fast::Instruction* inst)
{
    const AddConstant* addConst = dynamic_cast<const AddConstant*>(inst);
    if (addConst != 0) {
        // add    BYTE PTR [rbx+0], addConst->getValue()
        int rp = addConst->getRelPos();
        unsigned char cmd[] = { 0x80, 0x83,
            (unsigned char) (rp & 0xFF), (unsigned char) ((rp >> 8) & 0xFF),
            (unsigned char) ((rp >> 16) & 0xFF), (unsigned char) ((rp >> 24) & 0xFF),
            (unsigned char) addConst->getValue() };
        addCode(cmd, sizeof cmd);
    }
    
    const SetConstant* setConst = dynamic_cast<const SetConstant*>(inst);
    if (setConst != 0) {
        // mov    BYTE PTR [rbx+0], setConst->getValue();
        int rp = setConst->getRelPos();
        unsigned char cmd[] = { 0xC6, 0x83,
            (unsigned char) (rp & 0xFF), (unsigned char) ((rp >> 8) & 0xFF),
            (unsigned char) ((rp >> 16) & 0xFF), (unsigned char) ((rp >> 24) & 0xFF),(unsigned char) setConst->getValue() };
        addCode(cmd, sizeof cmd);
    }
    
    const AddVariable* addVar = dynamic_cast<const AddVariable*>(inst);
    if (addVar != 0) {
        // mov al, BYTE PTR[rbx+source]
        // add BYTE PTR[rbx+destination], al
        int vp = addVar->getSourcePos();
        int dest = addVar->getDestinationPos();
        unsigned char cmd[] = { 0x8A, 0x83,
            (unsigned char) (vp & 0xFF), (unsigned char) ((vp >> 8) & 0xFF),
            (unsigned char) ((vp >> 16) & 0xFF), (unsigned char) ((vp >> 24) & 0xFF),
            0x00, 0x83,
            (unsigned char) (dest & 0xFF), (unsigned char) ((dest >> 8) & 0xFF),
            (unsigned char) ((dest >> 16) & 0xFF), (unsigned char) ((dest >> 24) & 0xFF)};
        addCode(cmd, sizeof cmd);
    }
    
    const MovePointer* movePtr = dynamic_cast<const MovePointer*>(inst);
    if (movePtr != 0) {
        // add rbx, movePtr->getValue();
        int ap = movePtr->getValue();
        unsigned char cmd[] = { 0x48, 0x81, 0xC3, (unsigned char) (ap & 0xFF), (unsigned char) ((ap >> 8) & 0xFF),
            (unsigned char) ((ap >> 16) & 0xFF), (unsigned char) ((ap >> 24) & 0xFF) };
        addCode(cmd, sizeof cmd);
        
        // unsigned char cmd2[] = { 0x48, 0x81, 0xE3, 0xFF, 0xFF, 0x00, 0x00 }; // and rbx,0xffff
        // addCode(cmd2, sizeof cmd2);
    }
    
    const AddMultipliedVariable* addmult = dynamic_cast<const AddMultipliedVariable*>(inst);
    if (addmult != 0) {
        // mov al, BYTE PTR [rbx + src]
        // imul rax, rax, addmult->getMultiplier() ; only if not 1
        // add BYTE PTR[rbx + dest], al
        
        int sp = addmult->getSourcePos();
        unsigned char mov_al_BYTE_PTR_rbx_src[] = { 0x8A, 0x83,
            (unsigned char) (sp & 0xFF), (unsigned char) ((sp >> 8) & 0xFF),
            (unsigned char) ((sp >> 16) & 0xFF), (unsigned char) ((sp >> 24) & 0xFF)
        };
        
        addCode(mov_al_BYTE_PTR_rbx_src, sizeof mov_al_BYTE_PTR_rbx_src);
        
        int tp = addmult->getTargetPos();
        if ((addmult->getMultiplier() != 1 && addmult->getMultiplier() != -1)) {
            unsigned char imul[] = { 0x48, 0x6B, 0xC0,
                (unsigned char) addmult->getMultiplier() };
            addCode(imul, sizeof imul);
        }
        if (addmult->getMultiplier() == -1) {
            unsigned char sub[] = { 0x28, 0x83,
                (unsigned char) (tp & 0xFF), (unsigned char) ((tp >> 8) & 0xFF),
                (unsigned char) ((tp >> 16) & 0xFF), (unsigned char) ((tp >> 24) & 0xFF)
            };
            addCode(sub, sizeof sub);
        } else {
            unsigned char add[] = { 0x00, 0x83,
                (unsigned char) (tp & 0xFF), (unsigned char) ((tp >> 8) & 0xFF),
                (unsigned char) ((tp >> 16) & 0xFF), (unsigned char) ((tp >> 24) & 0xFF)
            };
            addCode(add, sizeof add);
        }
    }
    
    const Print* print = dynamic_cast<const Print*>(inst);
    if (print != 0) {
        // mov eax, 0x2000004
        // mov rdi, 1
        // mov rsi, rbx
        // sub/add rsi, offset
        // mov rdx, 1
        // syscall
        int sp = print->getRelativePosition();
        bool spGreaterZero = sp >= 0;
        if (sp < 0) {
            sp = -sp;
        }
        unsigned char prepcmd[] = { 0x48, 0xC7, 0xC0, (unsigned char) (mac ? 0x04 : 0x01), 0x00, 0x00, (unsigned char)(mac ? 0x02 : 0x00), 0x48, 0xC7, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x48, 0x89, 0xDE, 0x48, 0x81, (unsigned char) (spGreaterZero ? 0xC6 : 0xEE),
            
            (unsigned char) (sp & 0xFF), (unsigned char) ((sp >> 8) & 0xFF),
            (unsigned char) ((sp >> 16) & 0xFF), (unsigned char) ((sp >> 24) & 0xFF),
            
            0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05 };
        
        for (int i = 0; i < print->getRepetitions(); i++)
            addCode(prepcmd, sizeof prepcmd);
    }
    
    const Read* read = dynamic_cast<const Read*>(inst);
    if (read != 0) {
        // mov eax, 0x2000003
        // mov rdi, 0
        // mov rsi, rbx
        // sub/add rsi, offset
        // mov rdx, 1
        // syscall
        int sp = read->getRelativePosition();
        bool spGreaterZero = sp >= 0;
        if (sp < 0) {
            sp = -sp;
        }
        unsigned char prepcmd[] = { 0x48, 0xC7, 0xC0, (unsigned char) (mac ? 0x03 : 0x00), 0x00, 0x00,
            (unsigned char) (mac ? 0x02 : 0x00), 0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0xDE, 0x48, 0x81, (unsigned char) (spGreaterZero ? 0xC6 : 0xEE),
            
            (unsigned char) (sp & 0xFF), (unsigned char) ((sp >> 8) & 0xFF),
            (unsigned char) ((sp >> 16) & 0xFF), (unsigned char) ((sp >> 24) & 0xFF),
            
            0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x05 };
        
        for (int i = 0; i < read->getRepetitions(); i++)
            addCode(prepcmd, sizeof prepcmd);
    }
    
    const Loop* loop = dynamic_cast<const Loop*>(inst);
    const Program* program = dynamic_cast<const Program*>(inst);
    if (loop != 0) {
        int startPosition = (int) code.size();
        
        for (long i = 0; i < loop->getNInstructions(); i++) {
            const Instruction* ins = loop->getInstruction(i);
            compile_x86_64(ins);
        }
        
        // mov al, BYTE PTR[rbx] ; we don't use al here, we use r8b
        // and al, al
        
        // unsigned char mov_and_al[] = { 0x8A, 0x03, 0x20, 0xC0 };
        
        // jump from beg to end
        int jf = (int) code.size() - (int) startPosition;
        /*unsigned char before[] = { 0x44, 0x8A, 0x03, 0x4D, 0x21, 0xC0,
            0x0F, 0x84,
            (unsigned char) (jf & 0xFF), (unsigned char) ((jf >> 8) & 0xFF),
            (unsigned char) ((jf >> 16) & 0xFF), (unsigned char) ((jf >> 24) & 0xFF)
        };*/
        
        int beforeSize = 0;
        
        if (jf <= 127) {
            //jf -= 1;
            unsigned char before[] = { 0x44, 0x8A, 0x03, 0x45, 0x20, 0xC0,
                0x74,
                (unsigned char) (jf & 0xFF)
            };
            beforeSize = sizeof(before);
            for (int i = 0; i < sizeof(before); i++)
                code.insert(code.begin() + startPosition + i, before[i]);
        } else {
            //std::cout << "long jump\n";
            unsigned char before[] = { 0x44, 0x8A, 0x03, 0x45, 0x20, 0xC0,
                0x0F, 0x84,
                (unsigned char) (jf & 0xFF), (unsigned char) ((jf >> 8) & 0xFF),
                (unsigned char) ((jf >> 16) & 0xFF), (unsigned char) ((jf >> 24) & 0xFF)
            };
            beforeSize = sizeof(before);
            for (int i = 0; i < sizeof(before); i++)
                code.insert(code.begin() + startPosition + i, before[i]);
        }
        
        
        // mov r8b, BYTE PTR[rbx]
        // and r8b, r8b
        // jnz 100
        
        int ja = -6 - (int) code.size() + (int) startPosition;
        
        ja -= 6; // but why?
        ja += beforeSize;
        
        if (ja >= -128 - 4 && ja <= 0) {
            ja += 4;
            unsigned char prepcmd[] = { 0x44, 0x8A, 0x03, 0x45, 0x20, 0xC0,
                0x75,
                (unsigned char) (ja)
            };
            
            // 0f 84 fa ff ff ff
            addCode(prepcmd, sizeof prepcmd);
        } else {
            unsigned char prepcmd[] = { 0x44, 0x8A, 0x03, 0x45, 0x20, 0xC0,
                0x0F, 0x85,
                (unsigned char) (ja & 0xFF), (unsigned char) ((ja >> 8) & 0xFF),
                (unsigned char) ((ja >> 16) & 0xFF), (unsigned char) ((ja >> 24) & 0xFF)
            };
        
            // 0f 84 fa ff ff ff
            addCode(prepcmd, sizeof prepcmd);
        }
        
    } else if (program != 0) {
        for (long i = 0; i < program->getNInstructions(); i++) {
            const Instruction* ins = program->getInstruction(i);
            compile_x86_64(ins);
        }
    }
}


void VeryFastProgram::run(size_t memory)
{
    if (mappedMemory == 0) {
        allocate();
    }
    
    typedef void (*Subroutine)(void*);
    
    Subroutine routine = (Subroutine) mappedMemory;
    
    char* bx_ptr = new char[memory];
    void* printPtr = (void*) &printchar;
    void* readPtr = (void*) &readchar;
    ::memset(bx_ptr, 0, memory);
    bx_ptr += memory / 2;

    // after the call to routine, it is not 
    // save anymore to use "this". So we copy
    // it and store it manually on the stack
    VeryFastProgram* new_this = this;

    // we need to save everything because
    // this call is when shit gets crazy
    asm volatile ("pushq %rbx");
    asm volatile ("pushq %%rax" : : "a"(this));
    asm volatile ("pushq %%rax" : : "a"(bx_ptr));
    asm volatile ("pushq %%rax" : : "a"(memory));
    routine(bx_ptr);
    asm volatile ("popq %%rax" : "=a"(memory));
    asm volatile ("popq %%rax" : "=a"(bx_ptr));
    asm volatile ("popq %%rax" : "=a"(new_this));
    asm volatile ("popq %rbx");


    bx_ptr -= memory / 2;
    delete[] bx_ptr;
    ::munmap(new_this->mappedMemory, new_this->code.size());
}

#include <iostream>
void VeryFastProgram::allocate(void)
{
    size_t length = code.size();
    this->mappedMemory =
        ::mmap(NULL, length, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANON, -1, 0);
    
    for (size_t i = 0; i < code.size(); i++) {
        ((unsigned char*) mappedMemory)[i] = code[i];
        //((unsigned char*) mappedMemory)[i] = 0xC3;
        // std::cout << std::hex << (int) code[i] << std::endl;
    }
}


void VeryFastProgram::addCode(const unsigned char* code, int length)
{
    for (int i = 0; i < length; i++) {
        this->code.push_back(code[i]);
    }
}

#endif // USE_JUST_IN_TIME_COMPILER


