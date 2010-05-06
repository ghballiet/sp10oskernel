#include <process_q.h>
#include <stddef.h>
#include <process.h>

void pq_init(pq *q)
{
  q->first=NULL;
  q->last=NULL;
}

void pq_append(pq *q,proc_rec *proc)
{
  proc->next = NULL;
  proc->prev = q->last;
  if(q->last != NULL)
    q->last->next = proc;
  q->last = proc;
  if(q->first == NULL)
    q->first = proc;
}

proc_rec* pq_find(pq *q,PID_t PID)
{
  proc_rec* curr;
  for(curr=q->first;((curr != NULL) && (curr->PID != PID));curr=curr->next);
  return curr;
}

proc_rec*  pq_remove(pq *q,PID_t PID)
{
  proc_rec* curr;
  for(curr=q->first;((curr != NULL) && (curr->PID != PID));curr=curr->next);
  /* If the item is at the head... */
  if(curr->prev == NULL)
    {
      q->first = curr->next;
      if(q->first != NULL)
	q->first->prev=NULL;
    }
  else
    curr->prev->next = curr->next;
  /* If the item is at the tail... */
  if(curr->next == NULL)
    {
      q->last = curr->prev;
      if(q->last != NULL)
	q->last->next=NULL;
    }
  else
    curr->next->prev = curr->prev;
  return curr;
}

void pq_push_back(pq *q,proc_rec *proc)
{
  if(q->last == NULL)
    {
      q->first = q->last = proc;
      proc->next = proc->prev = NULL;
    }
  else
    {
      q->last->next = proc;
      q->last = proc;
      proc->prev = q->last;
      proc->next = NULL;
    }
}

proc_rec* pq_pop(pq *q)
{
  proc_rec* front;
  if(q->first == NULL)
    front = NULL;
  else
    {
      front = q->first;
      q->first = front->next;
      if(q->first != NULL)
	q->first->prev = NULL;
    }
  return front;
}

int pq_empty(pq *q)
{
  return q->first==NULL;    
}
