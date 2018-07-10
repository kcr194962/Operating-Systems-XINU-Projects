/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL getprio(int pid)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[3]=ctr1000;
		proc->count[3] += 1;	
        }

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		   if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[3]= (proc->exectime[3]* (proc->count[3]-1) + (ctr1000-proc->t1[3]))/(proc->count[3]);
	}
		return(SYSERR);
	}
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[3]= (proc->exectime[3]* (proc->count[3]-1) + (ctr1000-proc->t1[3]))/(proc->count[3]);
	}

	return(pptr->pprio);
}
