# Project 1 Design Document

> SJTU MS110  
> Spring 2019

## GROUP

> Fill in the name and email addresses of your group members.

- Bohan Hou <spectrometer@sjtu.edu.cn>
- Qidong Su <suqd99@sjtu.edu.cn>
- Yaxing Cai <caiyaxing@sjtu.edu.cn>

## PRELIMINARIES

> If you have any preliminary comments on your submission, notes for the
> Instructor, or extra credit, please give them here.

> Please cite any offline or online sources you consulted while
> preparing your submission, other than the Pintos documentation, course
> text, lecture notes, and course staff.

## PROJECT PARTS

> In the documentation that you provide in the following, you need to provide clear, organized, and coherent documentation, not just short, incomplete, or vague "answers to the questions".  The questions are provided to help you structure your thinking around the information you need to convey.

### A. ALARM CLOCK  

#### DATA STRUCTURES

> A1: Copy here the declaration of each new or changed `struct` or
> `struct` member, `global` or `static` variable, `typedef`, or
> enumeration.  Document the purpose of each in 25 words or less.


#### ALGORITHMS

> A2: Briefly describe the algorithmic flow for a call to `timer_sleep()`,
> including the effects of the timer interrupt handler.

> A3: What steps are taken to minimize the amount of time spent in
> the timer interrupt handler?

#### SYNCHRONIZATION

> Think about, in the general case, the possible concurrent threads of execution involved in the pieces of code for the Alarm Clock, and the synchronization needed to provide a correct solution to the problem.

> A4: How are race conditions avoided when multiple threads call
> `timer_sleep()` simultaneously?

> A5: How are race conditions avoided when a timer interrupt occurs
> during a call to `timer_sleep()`?

#### RATIONALE/JUSTIFICATION

> A6: Why did you choose this design?  In what ways is it superior to
> another design you considered?

### PRIORITY SCHEDULING

#### DATA STRUCTURES

> B1: Copy here the declaration of each new or changed `struct` or
> `struct` member, `global` or `static` variable, `typedef`, or
> enumeration.  Document the purpose of each in 25 words or less.

> B2: Explain the data structure used to track priority donation.  Provide a diagram showing nested donation, using Markdown include of a jpeg or png.

#### ALGORITHMS

> B3: How do you ensure that the highest priority thread waiting for
> a lock, semaphore, or condition variable wakes up first?

> B4: Describe the sequence of events when a call to `lock_acquire()`
> causes a priority donation.  How is nested donation handled?

> B5: Describe the sequence of events when `lock_release()` is called
> on a lock that a higher-priority thread is waiting for.

#### SYNCHRONIZATION

> B6: Describe a potential race in `thread_set_priority()` and explain
> how your implementation avoids it.  Can you use a lock to avoid
> this race?

#### RATIONALE

> B7: Why did you choose this design?  In what ways is it superior to
> another design you considered?


### ADVANCED SCHEDULER

#### DATA STRUCTURES

> C1: Copy here the declaration of each new or changed `struct` or
> `struct` member, `global` or `static` variable, `typedef`, or
> enumeration.  Document the purpose of each in 25 words or less.

#### ALGORITHMS

> C2: Suppose threads A, B, and C have "nice" values 0, 1, and 2, respectively.  Each
> has a recent_cpu value of 0.  Fill in the table below showing the
> scheduling decision (thread-to-run) and the priority and recent_cpu values for each
> thread after each given number of timer ticks:

> C3: Did any ambiguities in the scheduler specification make values
> in the table uncertain?  If so, what rule did you use to resolve
> them?  Does this match the behavior of your scheduler?

> C4: How is the way you divided the cost of scheduling between code
> inside and outside interrupt context likely to affect performance?

#### RATIONALE

> C5: Briefly critique your design, pointing out advantages and
> disadvantages in your design choices.  If you were to have extra
> time to work on this part of the project, how might you choose to
> refine or improve your design?

> C6: The assignment explains arithmetic for fixed-point math in
> detail, but it leaves it open to you to implement it.  Why did you
> decide to implement it the way you did?  If you created an
> abstraction layer for fixed-point math, that is, an abstract data
> type and/or a set of functions or macros to manipulate fixed-point
> numbers, why did you do so?  If not, why not?