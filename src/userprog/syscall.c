#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "pagedir.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
static void syscall_handler (struct intr_frame *);

inline static bool  
valid_uaddr (const void * uaddr)
{
  return (uaddr) && (is_user_vaddr (uaddr)) && (pagedir_get_page (thread_current()->pagedir, uaddr));
}

static uint32_t syscall_args[4];

static int  get_syscall_type (struct intr_frame *);
static void get_syscall_arg (struct intr_frame *, uint32_t *, int);

static void sys_exit(int );
static int sys_write (int fd, const void *buffer, unsigned size);

static struct lock write_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init (&write_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int type = get_syscall_type(f);
  switch (type)
   {
   case SYS_EXIT:
    get_syscall_arg (f, syscall_args, 1);
    sys_exit (syscall_args[0]);
    break;
   case SYS_WRITE:
    get_syscall_arg (f, syscall_args, 3);
    sys_write (syscall_args[0], syscall_args[1], syscall_args[2]);
    break;  
  }
  
}

/* Get the type of system call */
static int 
get_syscall_type (struct intr_frame * f)
{
  ASSERT (valid_uaddr (f -> esp));
  return *((uint32_t *)f->esp);
}


/* Get arguments which have been pushed into stack in lib/user/syscall.c */
static void
get_syscall_arg (struct intr_frame *f, uint32_t * buffer, int argc)
{
  

  uint32_t * ptr;
  for (ptr = (uint32_t *)f->esp + 1; argc > 0 ; ++buffer, --argc, ++ptr)
   {
    ASSERT (is_user_vaddr (ptr));
    *buffer = *ptr;
   }
}

/* Implements of syscalls */
static void 
sys_exit (int status)
{
  //thread_current()->exit_status = status;
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

static int 
sys_write (int fd, const void *buffer, unsigned size)
{
  //lock_acquire (&write_lock);
  if (fd == 1) 
   {
    /* Write to the Console*/
    putbuf ((const char *)buffer, size);
    //lock_release (&write_lock);

    return size;
   }
  //lock_release (&write_lock);
  //return thread_current()->eax;
  return 1;
}
