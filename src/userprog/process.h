#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"
struct proc_init {
    char * name;
    struct semaphore init_sem;
    bool success;
};

extern struct lock filesystem_lock;

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
bool install_page (void *, void *, bool);
void remove_mapid (struct list *, int);

#endif /* userprog/process.h */
