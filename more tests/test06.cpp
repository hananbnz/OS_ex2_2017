#include "uthreads.h"
#include <iostream>

#define THREAD_COUNT 20 // Threads Count
#define PRINT_TIMES 1
#define LOOPS 1000
#define QUANTUM 100

using namespace std;

int runner = THREAD_COUNT;

void print_thread_id(void);

void print_thread_id(void){
    int a = 0; // dummy variable for delay
    while (uthread_get_tid() != runner){
        uthread_sync(uthread_get_tid()+1);
    }

    int j = PRINT_TIMES;
    while (j--){
        for (int i = 0; i < LOOPS; ++i) {
            a += 1;
        }
        cout<< "In Thread: " <<uthread_get_tid() << endl;
    }
    cout<< "Terminate Thread: " << runner;
    cout<<" Now Should Run Thread: " << --runner << endl;
    uthread_terminate(runner+1);
}


int main(){
    uthread_init(QUANTUM);
    for (int i = 1 ; i <= THREAD_COUNT;++i){
        uthread_spawn(print_thread_id);
    }
    cout<<"Done Spawn " << THREAD_COUNT << " Threads" << endl;
    while (runner){}
    cout<< "And... We Done" << endl;
    uthread_terminate(0);
}