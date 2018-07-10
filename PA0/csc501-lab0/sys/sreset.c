/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(int sem, int count)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[22]=ctr1000;
		proc->count[22] += 1;
        }

	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[22]= (proc->exectime[22]* (proc->count[22]-1) + (ctr1000-proc->t1[22]))/(proc->count[22]);
        }
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);

	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[22]= (proc->exectime[22]* (proc->count[22]-1) + (ctr1000-proc->t1[22]))/(proc->count[22]);
        }
	return(OK);
}
