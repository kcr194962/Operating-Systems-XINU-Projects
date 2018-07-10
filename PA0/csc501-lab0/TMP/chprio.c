/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[1]=ctr1000;
		proc->count[1] += 1;
        }

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[1]= (proc->exectime[1]* (proc->count[1]-1) + (ctr1000-proc->t1[1]))/(proc->count[1]);
	}	
	return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[1]= (proc->exectime[1]* (proc->count[1]-1) + (ctr1000-proc->t1[1]))/(proc->count[1]);
	}

	return(newprio);
}
