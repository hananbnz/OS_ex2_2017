


// ------------------------------ Includes ----------------------------
#include <stdio.h>
#include "Thread.h"
#include "uthreads.h"
#include <signal.h>
#include <sys/time.h>
#include <vector>
#include <queue>
#include <list>
#include <setjmp.h>
#include<string>
#define FUNC_FAIL -1
#define FUNC_SUCCESS 0

std::string thread_termination_fail = "thread termination fail - invalid thread id";
std::string thread_spawn_fail = "thread spawn fail - id exceeds thread limit";
std::string thread_block_fail_1 = "thread block fail - thread with given id doesn't exist";
std::string thread_block_fail_2 = "thread block fail - can't block main thread";

/**
 * @brief  thread_vec vector that holds the threads created
 */
std::vector<Thread*> thread_vec(MAX_THREAD_NUM, NULL);

/**
 * @brief  thread_counter an integer number represents the number of threads
 * currently available
 */
int thread_counter  = 0;

/**
 * @brief  ready_queue a queue that represents ready state threads
 * currently available
 */
std::queue<Thread*> ready_queue;

/**
 * @brief  blocked_queue a queue that represents blocked state threads
 * currently available
 */
std::queue<Thread*> blocked_queue;

/**
 * @brief  current_running pointer to the running thread
 */
Thread* current_running = NULL;

/**
 *
 */
struct sigaction sa;

/**
 *
 */
struct itimerval timer;


int gotit = 0;


sigset_t blocked_set;


// ------------------------------ Library functions ----------------------------


void thread_library_function_fail(std::string text)
{
    fprintf(stderr, "thread library error: %s\n", text);
}

void system_call_fails(std::string text)
{
    fprintf(stderr, "system error: %s\n", text);
}

void block_vclock()
{
    sigemptyset(&blocked_set);
    sigaddset(&blocked_set, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &blocked_set, NULL);
}

void unblock_vclock()
{
    sigprocmask(SIG_UNBLOCK, &blocked_set, NULL);
}


void switchThreads(void)
{
    /**
    1. stop running current thread
    2. save current state of the current running thread - if not NULL
    3. move running thread if not NULL to end of ready queue
    4. jump to the first thread in the ready queue
    5. set timer
     */
    block_vclock();
    int ret_val = 0;
    Thread* cur_running_thread = current_running;
    // current_running is NULL when the current running thread is terminated
    if(cur_running_thread != NULL && cur_running_thread->getState() == 2) //RUNNING_STATE
    {
        cur_running_thread->setState(1); // 1 - READY_STATE
        ret_val = sigsetjmp(cur_running_thread->_env,1);
        if (ret_val != 0)
        {
            unblock_vclock();
            return;
        }
        ready_queue.push(cur_running_thread);
    }
    if(cur_running_thread != NULL && cur_running_thread->getState() == 3) // BLOCKED_STATE
    {
        ret_val = sigsetjmp(cur_running_thread->_env,1);

        if (ret_val != 0)
        {
            unblock_vclock();
            return;
        }
    }
    // good to all situations
    // TODO , check if front and pop is OK!!!
    while(ready_queue.front()->getState() == 3)
    {
        ready_queue.pop();
    }
    current_running = ready_queue.front();
    current_running->setState(2);
    current_running->addQuantum();
    ready_queue.pop();
    unblock_vclock();
    siglongjmp(current_running->_env, 1);

}


void timer_handler(int sig)
{
    //switch thread
    switchThreads();
    // set timer - will restart the quantes timer
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL))
    {
        printf("setitimer error.");
    }

//    printf("Timer expired\n");
}


int start_timer(int usecs)
{
    // Install timer_handler as the signal handler for SIGVTALRM.
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa,NULL) < 0)
    {
        printf("sigaction error.");
    }

    // Configure the timer to expire after 1 sec... */
    timer.it_value.tv_sec = 0;		// first time interval, seconds part
    timer.it_value.tv_usec = usecs;		// first time interval, microseconds part

    // configure the timer to expire every 3 sec after that.
    timer.it_interval.tv_sec = 0;	// following time intervals, seconds part
    timer.it_interval.tv_usec = usecs;	// following time intervals, microseconds part

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL))
    {
        printf("setitimer error.");
    }
    return FUNC_SUCCESS;
}


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    if(quantum_usecs <= 0)
    {
        return FUNC_FAIL;
    }
    thread_vec.push_back(new Thread());
    thread_counter++;
    current_running = thread_vec.back();
    current_running->addQuantum();
    start_timer(quantum_usecs);
    return FUNC_SUCCESS;
}

/**
 *
 * @return
 */
int get_minimum_id()
{
    for (int i = 1; i<MAX_THREAD_NUM;i++)
    {
        if (thread_vec[i] == NULL) {
            return i;
        }
    }
    return thread_counter;
}


/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (start_point_func)(void)) {
    block_vclock();
    //check if adding thread exceeds thread limit
    if (thread_counter < MAX_THREAD_NUM)
    {
        //get minimum thread id
        int min = get_minimum_id();
        thread_vec[min] = new Thread(min, STACK_SIZE, start_point_func);
        ready_queue.push(thread_vec[min]);
        thread_counter++;
        unblock_vclock();
        return min;
    }
    unblock_vclock();
    thread_library_function_fail(thread_spawn_fail);
    return FUNC_FAIL;
}


/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released.
 * If no thread with ID tid exists it is considered as an error.
 * Terminating the main thread (tid == 0) will result in the termination of the
 * entire process using exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    block_vclock();
    // thread id invalid
    if(tid >= MAX_THREAD_NUM || thread_vec[tid] == NULL)
    {
        thread_library_function_fail(thread_termination_fail);
        unblock_vclock();
        return FUNC_FAIL;
    }
    // thread self termination
    if (current_running->getId() == tid)
    {
        //thread 0 termination
        if(tid == 0)
        {
            for(int i = 1;i<MAX_THREAD_NUM; i++)
            {
                delete thread_vec[tid];
                thread_vec[tid] = NULL;
            }
            thread_counter = 1;
        }
        delete thread_vec[tid];
        thread_vec[tid] = NULL;
        timer_handler(1);
        if(thread_counter == 1)
        {
            unblock_vclock();
            //TODO FIX EXIT
            exit(0);
        }
    }
    thread_counter--;
    unblock_vclock();
    return  FUNC_SUCCESS;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    block_vclock();
    //invalid tid
    if(tid >= MAX_THREAD_NUM || thread_vec[tid] == NULL)
    {
        thread_library_function_fail(thread_block_fail_1);
        unblock_vclock();
        return FUNC_FAIL;
    }
    //tid = 0 - block main thread
    if(tid == 0)
    {
        thread_library_function_fail(thread_block_fail_2);
        unblock_vclock();
        return FUNC_FAIL;
    }
    thread_vec[tid]->setState(3);
    //self thread block
    if(tid == current_running->getId())
    {
        timer_handler(1);
        block_vclock();//TODO check block/unblock
    }
    unblock_vclock();
    return 0;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid);


/*
 * Description: This function blocks the RUNNING thread until thread with
 * ID tid will move to RUNNING state (i.e.right after the next time that
 * thread tid will stop running, the calling thread will be resumed
 * automatically). If thread with ID tid will be terminated before RUNNING
 * again, the calling thread should move to READY state right after thread
 * tid is terminated (i.e. it won’t be blocked forever). It is considered
 * as an error if no thread with ID tid exists or if the main thread (tid==0)
 * calls this function. Immediately after the RUNNING thread transitions to
 * the BLOCKED state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid);

///////////////////////////////////////////////////////////////////////////////

/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    return current_running->getId();
}


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    block_vclock();
    int sum = 0;
    for(int i=0; i< MAX_THREAD_NUM; i++)
    {
        if(thread_vec[i] != NULL)
        {
            sum += thread_vec[i]->getQuantum();
        }
    }
    unblock_vclock();
    return sum;
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid)
{
    return thread_vec[tid]->getQuantum();
}

//int main() {
//    printf("start timer");
//    start_timer(1000);
//    return 0;
//}
