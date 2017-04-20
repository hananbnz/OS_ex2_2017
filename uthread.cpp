


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



#define FUNC_FAIL -1
#define FUNC_SUCCESS 0


/**
 * @brief  thread_vec vector that holds the threads created
 */
std::vector<Thread*> thread_vec;

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



void switchThreads(void)
{
    /**
    1. stop running current thread
    2. save current state of the current running thread
    3. jump to the first thread in the ready queue
    4. set timer
     */


    static int currentThread = 0;

    int ret_val = sigsetjmp(env[currentThread],1);
    printf("SWITCH: ret_val=%d\n", ret_val);
    if (ret_val == 1) {
        return;
    }
    currentThread = 1 - currentThread;
    siglongjmp(env[currentThread],1);







    // set timer - will restart the quantes timer
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL))
    {
        printf("setitimer error.");
    }
}


void timer_handler(int sig)
{
    //switch thread
    switchThreads();
    gotit = 1;
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
    start_timer(quantum_usecs);
    return FUNC_SUCCESS;
}







//////////////////////////////////////////////////////////////



int get_minimum()
{
    for (int i = 0; i<thread_counter;i++)
    {
        if (thread_vec[i] == NULL) {
            return i;
        }
    }
    return thread_counter;
}


//////////////////////////////////////////////////////////////

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
    if (thread_counter < MAX_THREAD_NUM)
    {
        int min = get_minimum();
        thread_vec[min] = new Thread(min, STACK_SIZE, start_point_func);
        ready_queue.push(thread_vec[min]);
//TODO READY QUEUE
        return min;

    }
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
    if(tid >= thread_counter)
    {
        //TODO error change
        return FUNC_FAIL;
    }
    if(thread_vec[tid] == NULL)
    {
        //TODO error change
        return FUNC_FAIL;
    }
    if (current_running->getId() == tid )
    {

    }
    //TODO DEAL WITH MAIN TERMINATION



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
int uthread_block(int tid);


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


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid();


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums();


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid);

int main() {
    printf("start timer");
    start_timer(1000);
    return 0;
}
