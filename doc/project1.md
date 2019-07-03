# Project 1 Design Document

> SJTU MS110  
> Spring 2019

## GROUP

- Bohan Hou <spectrometer@sjtu.edu.cn>
- Qidong Su <suqd99@sjtu.edu.cn>
- Yaxing Cai <caiyaxing@sjtu.edu.cn>

## PROJECT PARTS

#### DATA STRUCTURES

```c
// record all the threads that are sleeping
static struct list sleeping_threads;

struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority, which may be different from original priority because of donation */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */

    /* Owned by timer.c */
    int64_t wake_up_ticks;
    struct list_elem sleepelem;         /* List element for sleeping threads list. */
    struct semaphore sleep_semaphore;   /* semaphore to control thread sleeping*/

    /* Priority donation use */
    int original_priority;       
    struct list locks;                  /* The locks current thread hold */      
    struct lock *waiting_lock;      /* The thread current thread is waiting for */
  };

/* Lock. */
struct lock 
  {
    struct thread *holder;      /* Thread holding lock (for debugging). */
    struct semaphore semaphore; /* Binary semaphore controlling access. */
    struct list_elem elem;      /* CHANGED : Owned by threads that hold this lock */
    int max_priority;           /* CHANGED : Max priority among threads wait for this lock*/
  };
```

### A. ALARM CLOCK  

#### ALGORITHMS

> A1: Briefly describe the algorithmic flow for a call to `timer_sleep()`,
> including the effects of the timer interrupt handler.

1.  Get current thread and record its wake up ticks.
2.  Turn off interrupts, put current thread into sleeping thread queue which is ordered by wake up ticks, and then turn on the interrupts.
3.  Let current thread wait for a specific semaphore and go to sleep.
4. Timer interrupt handler checks the sleeping thread queue and wakes up all threads which are ready by signaling the  semaphore.

> A2: What steps are taken to minimize the amount of time spent in
> the timer interrupt handler?

When putting one thread into sleeping thread queue, we carefully keep its order according to the wake up ticks. Hence, we only need to check from left to right to see whether the sleeping thread needs to wake up when handling timer interrupt. This minimizes the amount of time spent in the handler.

#### SYNCHRONIZATION

> Think about, in the general case, the possible concurrent threads of execution involved in the pieces of code for the Alarm Clock, and the synchronization needed to provide a correct solution to the problem.

> A3: How are race conditions avoided when multiple threads call
> `timer_sleep()` simultaneously? How are race conditions avoided when a timer interrupt occurs
> during a call to `timer_sleep()`?

We turn interrupts off and later on while inserting a thread into sleeping thread queue. Hence only one thread will be operating on the queue.

### PRIORITY SCHEDULING

#### ALGORITHMS

> B1: How do you ensure that the highest priority thread waiting for
> a lock, semaphore, or condition variable wakes up first?

When a thread has to wait for a semaphore, we need to put the thread into its waiting queue. We keep the order according to the priority of threads, and when the semaphore is ready we only need to wake up the head of the queue.

> B2: Describe the sequence of events when a call to `lock_acquire()`
> causes a priority donation.  How is nested donation handled?

1.  Turn off interrupts.
2.  If the lock is held by another thread, we update the priority of all the threads on the waiting chain with the priority of current thread.
3.  Turn on interrupts.
4.  Down the semaphore. If the lock is held by another thread, current thread will go to sleep. Otherwise it will successfully down the semaphore.
5.  Turn off interrupts.
6.  Modify the holder of the lock, and the lock into current threads' holding list, and update the priority of current thread.

> B3: Describe the sequence of events when `lock_release()` is called
> on a lock that a higher-priority thread is waiting for.

1.  Turn off interrupts.
2.  Remove the lock from the holding list of current thread, and update the priority of current thread.
3.  Up the semaphore.
4.  Turn on interrupts.
