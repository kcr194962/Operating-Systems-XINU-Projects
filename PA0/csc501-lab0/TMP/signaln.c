/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[17]=ctr1000;
		proc->count[17] += 1;
        }

	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[17]= (proc->exectime[17]* (proc->count[17]-1) + (ctr1000-proc->t1[17]))/(proc->count[17]);
        }
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        	proc->exectime[17]= (proc->exectime[17]* (proc->count[17]-1) + (ctr1000-proc->t1[17]))/(proc->count[17]);
	}

	return(OK);
}
