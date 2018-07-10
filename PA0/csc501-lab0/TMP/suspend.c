/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
SYSCALL	suspend(int pid)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[24]=ctr1000;
      		proc->count[24] += 1;
	  }

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority returned		*/

	disable(ps);
	if (isbadpid(pid) || pid==NULLPROC ||
	 ((pptr= &proctab[pid])->pstate!=PRCURR && pptr->pstate!=PRREADY)) {
		restore(ps);
		if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[24]= (proc->exectime[24]* (proc->count[24]-1) + (ctr1000-proc->t1[24]))/(proc->count[24]);
        }
		return(SYSERR);
	}
	if (pptr->pstate == PRREADY) {
		pptr->pstate = PRSUSP;
		dequeue(pid);
	}
	else {
		pptr->pstate = PRSUSP;
		resched();
	}
	prio = pptr->pprio;
	restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[24]= (proc->exectime[24]* (proc->count[24]-1) + (ctr1000-proc->t1[24]))/(proc->count[24]);
        }
	return(prio);
}
