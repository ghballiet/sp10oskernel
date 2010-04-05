
#ifndef PROCESS_Q_H
#define PROCESS_Q_H

#include <process.h>

typedef struct process_queue{
  proc_rec *first;
  proc_rec *last;
}pq;

void pq_init(pq *q);
void pq_append(pq *q,proc_rec *proc);
proc_rec* pq_find(pq *q,PID_t p);
proc_rec*  pq_remove(pq *q,PID_t PID);
void pq_push_back(pq *q,proc_rec *proc);
proc_rec* pq_pop(pq *q);
int pq_empty(pq *q);

#endif
