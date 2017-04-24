
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



address_t sp, pc;


// --------------------------------------------------------------------------------------
// This file contains the implementation of the class Thread.
// --------------------------------------------------------------------------------------

//Default Constructor
Thread::Thread()
{
    _id = DEFAULT_ID;
    _state = READY_STATE;
    _thread_func = NULL;
    _number_of_quantumes = 0;

    sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
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
    _thread_func = func;
    _state = READY_STATE;
    _number_of_quantumes = 0;

    sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)_thread_func;
    sigsetjmp(_env, 0);
    (_env->__jmpbuf)[JB_SP] = translate_address(sp);
    (_env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_env->__saved_mask);
}

//Destructor
Thread::~Thread()
{}


// ------------------ Access methods ------------------------

//Defined in Thread.h


// ------------------ Modify methods ------------------------

void Thread::setState(int state)
{
    _state = state;
}

void Thread::addToSyncedThreads(int tid)
{
    _synced_threads.push_back(tid);
}

void Thread::addQuantum()
{
    _number_of_quantumes++;
}

void Thread::setSynced()
{
    _is_synced = true;
}

void Thread::setUnsynced()
{
    _is_synced = false;
}
