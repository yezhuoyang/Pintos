#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <hash.h>
#include <list.h>
#include <debug.h>
#include "threads/palloc.h"
#include "threads/synch.h"

struct frame_entry
  {
    void *frame_addr;
    struct thread *owner_thread;
    struct spt_entry *spte;
    //struct hash_elem elem;
    struct list_elem elem;
  };

struct lock frame_table_lock;
struct frame_entry **frame_table_index;
struct list frame_table;

void frame_table_init (void);
void *frame_get (struct spt_entry *, bool);
void frame_free (void *);
void *frame_evict (enum palloc_flags);
//unsigned fte_addr (const struct hash_elem *e, void *aux UNUSED);
//bool fte_addr_less (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED);

#endif 