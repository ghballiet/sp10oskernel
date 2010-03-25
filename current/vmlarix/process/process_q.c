#include <process_q.h>
#include <stddef.h>
#include <process.h>

void pq_init(pq *q)
{
  /* not sure what this one is supposed to do? I'm assuming here that it's
     supposed to be given an unitialized, freshly-declared pq pointer (like "pq
     x; pq_init(&x);"), and just sets up the first/last pointers as null */
  q->first = NULL;
  q->last = NULL;
}

void pq_append(pq *q,proc_rec *proc)
{
  /* add the give proc_rec to the end of the queue */
  proc_rec *z = q->last;
  z->next = proc;
  q->last = proc;
  /* set the given proc_rec's pointers to tie it into the list */
  proc->next = null;
  proc->prev = z;
}

proc_rec* pq_find(pq *q,PID_t PID)
{
  proc_rec *x = q->first;
  while(x && x->PID != PID)
    x = x->next;
  return x; /* will return NULL if proc_rec with given PID is not found */
}

proc_rec*  pq_remove(pq *q,PID_t PID)
{
  proc_rec *x = pq_find(q, PID);
  /* what's the desired behavior if no record with that PID is found? currently
     it will crash after displaying message... */
  kprintf("Could not find proc_rec entry with PID %d\r\n", PID);
  if(x->prev)
    x->prev->next = x->next;
  else
    q->first = x->next; /* is there any way for x->prev to be null when x isn't
			   q->first? i don't think that *should* be possible */
  if(x->next)
    x->next->prev = x->prev;
  else
    q->last = x->prev; /* same question as for the x->prev bit above */
  return x;
}

void pq_push_back(pq *q,proc_rec *proc)
{
  /* what's the difference between push_back and append? My reading is that
     they're identical -- add new element to end (back) of queue... */
  /* TODO: send an email about this */
  pq_append(q, proc);
}

proc_rec* pq_pop(pq *q)
{
  proc_rec *proc = q->first;

  /* update queue pointers */
  q->first = proc->next;
  q->first->prev = NULL;

  return proc;
}

int pq_empty(pq *q)
{
  if(q->first)
    return 1;
  else
    return 0;
}
