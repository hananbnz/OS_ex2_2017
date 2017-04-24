#ifndef EX2_THREAD_H
#define EX2_THREAD_H

#include <stdlib.h>
#include <malloc.h>
#include <setjmp.h>
#include <vector>


#define DEFAULT_ID 0
#define DEFAULT_STACK_SIZE 4096

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

    //  Getter Functions

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
    int getQuantum() {return _number_of_quantumes; };

    int getNumOfSyncedThreads(){ return (int)_synced_threads.size();};

    bool is_synced(){return _is_synced; };

    //  Setter Functions

    /**
     * Gets two ints and changes the x and y cordinates accordingly.
     * @param state An int representing the x value of a point
     */
    void setState(int state);

    /**
     * Adds quantum to this thread
     */
    void addQuantum();

    /**
     *
     * @param tid
     */
    void addToSyncedThreads(int tid);

    void setSynced();

    void setUnsynced();



private:
    int _id; /**< the id of this Thread object*/
    int _state; /**< the state of this Thread object**/
    char* _stack[DEFAULT_STACK_SIZE];
    void (*_thread_func)(void);
    int _number_of_quantumes;
    bool _is_synced = false;

public:
    sigjmp_buf _env;
    std::vector<int> _synced_threads;
};


#endif //EX2_THREAD_H
