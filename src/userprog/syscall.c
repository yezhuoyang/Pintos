#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "pagedir.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"
#include "threads/input.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);



static uint32_t syscall_args[4];

static int  get_syscall_type (struct intr_frame *);
static void get_syscall_arg (struct intr_frame *, uint32_t *, int);

// static thread_action_func check_filename_open;

static bool sys_create (const char *file, unsigned initial_size);
static bool sys_remove (const char *file);
static int sys_open (const char *file);
static void sys_close (int fd);
void sys_exit(int status);
static int sys_write (int fd, const void *buffer, unsigned size);
static int sys_read (int fd, void *buffer, unsigned size);
static int sys_filesize (int fd);
static pid_t sys_exec (const char *cmd_line);
static int sys_wait (pid_t pid);
static void sys_seek (int fd, unsigned position);
static unsigned sys_tell (int fd);
static void sys_halt (void);

//static void thread_close_files();


static struct lock filesystem_lock;

static void  
valid_uaddr (const void * uaddr, unsigned int len)
{
  for (const void * addr = uaddr; addr < uaddr + len ; ++addr)
    if ((!addr) ||
       !(is_user_vaddr (addr)) ||
       !(pagedir_get_page (thread_current()->pagedir, addr)))
       {
        //printf("invalid addr\n");
        sys_exit(-1);
        return;
       }
}


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init (&filesystem_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int type = get_syscall_type(f);
  switch (type)
   {
   case SYS_CREATE:
    get_syscall_arg (f, syscall_args, 2);
    f->eax = sys_create(syscall_args[0], syscall_args[1]);
    break;
   case SYS_REMOVE:
    get_syscall_arg (f, syscall_args, 1);
    f->eax = sys_remove(syscall_args[0]);
    break;
   case SYS_OPEN:
    get_syscall_arg (f, syscall_args, 1);
    f->eax = sys_open(syscall_args[0]);
    break;
   case SYS_CLOSE:
    get_syscall_arg (f, syscall_args, 1);
    sys_close(syscall_args[0]);
    break;
   case SYS_EXIT:
    get_syscall_arg (f, syscall_args, 1);
    sys_exit (syscall_args[0]);
    break;
   case SYS_WRITE:
    get_syscall_arg (f, syscall_args, 3);
    f->eax = sys_write (syscall_args[0], syscall_args[1], syscall_args[2]);
    //printf("written %d\n", f->eax );
    break;  
   case SYS_READ:
    get_syscall_arg (f, syscall_args, 3);
    f->eax = sys_read(syscall_args[0], syscall_args[1], syscall_args[2]);
    break;
   case SYS_FILESIZE:
    get_syscall_arg(f, syscall_args, 1);
    f->eax = sys_filesize(syscall_args[0]);
    break;
   case SYS_EXEC:
    get_syscall_arg(f, syscall_args, 1);
    f->eax = sys_exec(syscall_args[0]);
    break;
   case SYS_WAIT:
    get_syscall_arg(f, syscall_args, 1);
    f->eax = sys_wait(syscall_args[0]);
    break;
   case SYS_SEEK:
    get_syscall_arg(f, syscall_args, 2);
    sys_seek (syscall_args[0], syscall_args[1]);
    break;
   case SYS_TELL:
    get_syscall_arg(f, syscall_args, 1);
    f->eax = sys_tell (syscall_args[0]);
    
    break;
   case SYS_HALT:
    sys_halt();
    break;
  }
  
}

// /* Close all file descriptors under a thread */
// inline static void 
// thread_close_files()
// {
//   //lock_acquire(&filesystem_lock);
//   struct list_elem *e, *old_e;
//   for (e = list_begin(&thread_current()->file_descriptors);
//        e != list_end(&thread_current()->file_descriptors);
//        )
//    {
//      struct file_descriptor * fd_s = list_entry(e, struct file_descriptor, elem);
//      file_close(fd_s -> file_pointer);
//      old_e = e;
//      e = list_next(e);
//      list_remove(old_e);
//      free (fd_s);
//    }
//   //lock_release(&filesystem_lock);
// }

/* Get the type of system call */
static int 
get_syscall_type (struct intr_frame * f)
{
  valid_uaddr (f -> esp, sizeof(uint32_t));
  return *((uint32_t *)f->esp);
}


/* Get arguments which have been pushed into stack in lib/user/syscall.c */
static void
get_syscall_arg (struct intr_frame *f, uint32_t * buffer, int argc)
{
  
  uint32_t * ptr;
  for (ptr = (uint32_t *)f->esp + 1; argc > 0 ; ++buffer, --argc, ++ptr)
   {
    valid_uaddr (ptr, sizeof(uint32_t));
    *buffer = *ptr;
   }
}

static void
check_string(const char * str)
{
  const char *ptr;
  for (ptr = str ; valid_uaddr(ptr, 1), *ptr ; ++ptr);
  valid_uaddr(ptr, 1);
}


static struct file_descriptor * 
get_fdstruct(int fd)
{
  struct list_elem *e;

  for (e = list_begin (&thread_current()->file_descriptors);
       e != list_end (&thread_current()->file_descriptors);
       e = list_next (e))
   {
    struct file_descriptor *f = list_entry (e, struct file_descriptor, elem);
    if (f->fd == fd) return f;
   }
  //printf("NULL!\n");
  return NULL;
}

/* Implements of syscalls */
static bool
sys_create (const char *file, unsigned initial_size)
{
  
  /* Check the string */
  check_string(file);

  /* Call filesys */
  lock_acquire(&filesystem_lock);
  bool r = filesys_create(file, initial_size);
  lock_release(&filesystem_lock);
  return r;
}

static bool 
sys_remove (const char *file)
{
  
  /* Check file name */
  check_string(file);

  lock_acquire(&filesystem_lock);
  bool r = filesys_remove(file);
  lock_release(&filesystem_lock);
  return r;  
}

static int 
sys_open (const char *file)
{
  check_string(file);

  lock_acquire(&filesystem_lock);
  struct file * r = filesys_open(file);
  if (!r)
   {
    lock_release(&filesystem_lock);
    return -1;
   }
  struct file_descriptor * fd_s = malloc(sizeof(struct file_descriptor));
  
  fd_s->fd = thread_current()->fd_index++;
  fd_s->file_pointer = r;
  strlcpy (fd_s->name, file, strlen (file));
  list_push_back(&thread_current()->file_descriptors, &fd_s->elem);
  lock_release(&filesystem_lock);
  return fd_s->fd;
}

static void 
sys_close (int fd)
{
  if (fd < 2) sys_exit(-1) ;
  struct file_descriptor * fd_s = get_fdstruct(fd);
  if (!fd_s) sys_exit(-1) ;
  lock_acquire (&filesystem_lock);
  file_close (fd_s->file_pointer);
  list_remove (&fd_s->elem);
  free (fd_s);
  lock_release (&filesystem_lock);
}

void 
sys_exit (int status)
{





  thread_current()->exit_status = status;
  printf("%s: exit(%d)\n", thread_current()->name, status);

  thread_exit();
}




static int 
sys_write (int fd, const void *buffer, unsigned size)
{
  valid_uaddr (buffer, size);
  lock_acquire (&filesystem_lock);
  if (fd == 0)
   {
     lock_release (&filesystem_lock);
     sys_exit(-1);
     return -1;
   }
  else if (fd == 1) 
   {
    /* Write to the Console*/
    putbuf ((const char *)buffer, size);
    lock_release (&filesystem_lock);
    return size;
   }
  else {
    struct file_descriptor * fd_s = get_fdstruct(fd);
    if (!fd_s){
      
      lock_release (&filesystem_lock);
      return -1;
    }

    //thread_foreach (check_filename_open, fd_s->name);

    int r = file_write (fd_s->file_pointer, buffer, size);
    
    lock_release (&filesystem_lock);
    return r;
  }
}

static int 
sys_read (int fd, void *buffer, unsigned size)
{
  valid_uaddr(buffer, size);
  lock_acquire (&filesystem_lock);
  if (fd == 0)
   {
     //get_line(buffer);   
     return 0;
   }
  else if (fd == 1)
   {
     lock_release(&filesystem_lock);
     sys_exit(-1);
     return -1;
   }
  else
   {
    struct file_descriptor * fd_s = get_fdstruct(fd);
    if (!fd_s)
     {
       lock_release (&filesystem_lock);
       return -1;
     }
    int r = file_read(fd_s -> file_pointer, buffer, size);
    lock_release (&filesystem_lock);
    return r;
   }
  //lock_release (&filesystem_lock);
}

static int 
sys_filesize (int fd)
{
  struct file_descriptor * fd_s = get_fdstruct(fd);
  if (!fd_s) return -1;
  lock_acquire(&filesystem_lock);
  int r = file_length(fd_s->file_pointer);
  lock_release(&filesystem_lock);
  return r;
}

static pid_t
sys_exec (const char *cmd_line)
{
  check_string(cmd_line);
  lock_acquire (&filesystem_lock);
  int r = process_execute (cmd_line);
  lock_release (&filesystem_lock);

  //printf("exec: %d\n", r);
  return r;
}

static int 
sys_wait (pid_t pid)
{
  //printf("waiting %d:\n", pid);
  return process_wait(pid);
}

static void 
sys_seek (int fd, unsigned position)
{
  struct file_descriptor * fd_s = get_fdstruct (fd);
  if (!fd_s) return ;
  lock_acquire (&filesystem_lock);
  file_seek (fd_s->file_pointer, position);
  lock_release (&filesystem_lock);
}

static unsigned 
sys_tell (int fd)
{
  struct file_descriptor * fd_s = get_fdstruct (fd);
  if (!fd_s) return -1;
  lock_acquire (&filesystem_lock);
  unsigned r = file_tell (fd_s->file_pointer);
  lock_release (&filesystem_lock);
  return r;
}

static void 
sys_halt (void)
{
  shutdown_power_off();
}


// static void 
// check_filename_open (struct thread *t, void *str_) 
// {
//   const char * str = str_;
//   if (strcmp(str, t->name) == 0)
//   {
//     lock_release (&filesystem_lock);
//     sys_exit(-1);
//   }
// }