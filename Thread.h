//
// Created by reuveny on 4/9/17.
//


#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include <stdlib.h>
#include <malloc.h>
#include <setjmp.h>

#define DEFAULT_ID 0
#define DEFAULT_STACK_SIZE 4096

//TODO STATE DEFINITIONS
#define READY_STATE 1
#define RUNNING_STATE 2
#define BLOCKED_STATE 3



class Thread
{
public:
    /**
     * A default Thread constructor.
     */
    Thread();

    /**
     * A Thread destructor.
     */
    ~Thread();

    /**
    * A Thread constructor.
    */
   Thread(const int id, int stack_size,void (*func)(void));

    /**
	 * Access method for the id value.
	 * @return An int representing the id value
	 */
    int getId() const {return _id; };

    /**
	 * Access method for the state value.
	 * @return An int representing the state value
	 */
    int getState() const {return _state; };

    /**
	 * Access method for the quantum value.
	 * @return An int representing the quantum value
	 */
    int getQuantum() const {return _number_of_quantumes; };

    void addQuantum() {_number_of_quantumes++;};

    /**
     * Gets two ints and changes the x and y cordinates accordingly.
     * @param state An int representing the x value of a point
     */
    void setState(int state);


private:
    int _id; /**< the id of this Thread object*/
    int _state; /**< the state of this Thread object**/
    void *_stack;
    void *_thread_func;
    int _number_of_quantumes;

public:
    sigjmp_buf _env;
};


#endif //EX2_THREAD_H
