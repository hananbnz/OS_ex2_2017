

#include "Thread.hpp"
//
// Created by reuveny on 4/9/17.
//

#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include "Thread.h"

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND 1000000
#define STACK_SIZE 4096

char stack[STACK_SIZE];



#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
            "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
            "rol    $0x9,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#endif


address_t sp, pc;


// --------------------------------------------------------------------------------------
// This file contains the implementation of the class Thread.
// --------------------------------------------------------------------------------------

//Default Constructor
Thread::Thread()
{
    _id = DEFAULT_ID;
    _stack = (void*)malloc(DEFAULT_STACK_SIZE);
    _state = READY_STATE;
    _thread_func = NULL;
    _number_of_quantumes = 1;
    sp = (address_t)stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)_thread_func;
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}

//Constructor
Thread::Thread(const int id, int stack_size,void (*func)(void))
{
    _id = id;
    _stack = (void*)malloc(stack_size);
    _thread_func = NULL;
    _state = READY_STATE;
    _number_of_quantumes = 0;
    sp = (address_t)stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)_thread_func;
    sigsetjmp(_env, 1);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}

//Destructor
Thread::~Thread()
{
    free(_stack);
}


// ------------------ Access methods ------------------------

//Defined in Thread.h


// ------------------ Modify methods ------------------------

void Thread::setState(int state)
{
    _state = state;
}

////////
//////
//// reuven is the king
//// hhhhhhhhhhhhhhhhhhhhhhhhhhhhh