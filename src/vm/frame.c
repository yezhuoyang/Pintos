#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "vm/spt.h"
#include "vm/swap.h"
#include <stdio.h>

/*
unsigned 
fte_addr (const struct hash_elem *e, void *aux UNUSED)
{
  struct frame_entry *x = hash_entry (e, struct frame_entry, elem);
  return hash_int ((int) x->frame_addr);
}

bool
fte_addr_less (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  struct frame_entry *x = hash_entry (a, struct frame_entry, elem);
  struct frame_entry *y = hash_entry (b, struct frame_entry, elem);
  return x->frame_addr < y->frame_addr;
}
*/

void 
frame_table_init (void)
{
  lock_init (&frame_table_lock);
  //hash_init (&frame_table, fte_addr, fte_addr_less, NULL);
  list_init (&frame_table);
}

void *
frame_get (struct spt_entry *spte, bool ZERO)
{
  lock_acquire (&frame_table_lock);

  void *addr = palloc_get_page (ZERO ? PAL_USER | PAL_ZERO : PAL_USER);

  while (addr == NULL)
    addr = frame_evict (ZERO ? PAL_USER | PAL_ZERO : PAL_USER); 
  
  if (addr != NULL)
  {
    struct frame_entry *fe = malloc (sizeof (struct frame_entry));
    fe->frame_addr = addr;
    fe->owner_thread = thread_current ();
    fe->spte = spte;
    list_push_back (&frame_table, &fe->elem);
    lock_release (&frame_table_lock);
  } 
  else
  {
    PANIC ("frame not enough");   
  }
  return addr;
}

void
frame_free (void *frame)
{
  if (frame == NULL) return;
  lock_acquire (&frame_table_lock);

  struct list_elem *e;
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
  {
    struct frame_entry *fe = list_entry (e, struct frame_entry, elem);
    if (fe->frame_addr == frame)
    {
      list_remove (e);
      palloc_free_page (frame);
      free (fe);
      break;
    }
  }
  lock_release (&frame_table_lock);
}

//second chance algorithm
void *
frame_evict (enum palloc_flags flags)
{
  //struct hash_iterator it;

  for (; ;)
  {
    struct list_elem *e = list_begin (&frame_table);
    for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame_entry *fe = list_entry (e, struct frame_entry, elem); 
    
      if (!fe->spte->pinned)
      {
        uint32_t *pd = fe->owner_thread->pagedir;
        uint8_t *upage = fe->spte->addr;
        if (pagedir_is_accessed (pd, upage)) 
          pagedir_set_accessed (pd, upage, false);
        else
        {
          if (fe->spte->type == PAGE_MMAP)
          {
            if (pagedir_is_dirty (pd, upage))
            {
              lock_acquire (&filesystem_lock);
              file_write_at (fe->spte->file, fe->frame_addr, fe->spte->read_bytes, fe->spte->ofs);
              lock_release (&filesystem_lock);
            }
          }
          else if (fe->spte->type == PAGE_SWAP)
          {
            fe->spte->swap_index = swap_dump (fe->frame_addr);
          }
          else if (fe->spte->type == PAGE_ELF)
          {
            if (pagedir_is_dirty (pd, upage))
            {
              fe->spte->type = PAGE_SWAP;
              fe->spte->swap_index = swap_dump (fe->frame_addr);
            }
          } 
          fe->spte->is_present = false;
          pagedir_clear_page (pd, upage);
          palloc_free_page (fe->frame_addr);
          list_remove (&fe->elem);
          free (fe);
          
          return palloc_get_page (flags);
        }
      }
    }
  }

}