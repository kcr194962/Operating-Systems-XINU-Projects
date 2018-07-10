/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	/* if proc is waiting in a lockq */
	if( pptr->pstate == PRWAIT ){
		/* if new prio is higher than current prio , rampup*/
		if( newprio > pptr->pprio ) {
			rampup( pid, pptr->waitingfor, newprio);
			pptr->pprio = newprio;
			pptr->oldprio = newprio;
		}
		/* if new prio is lesser than current prio, resetprio to oldprio and then to next maxprio */
		else{
			pptr->pprio = newprio;
			pptr->oldprio = newprio;
			resetprio(pid);	 /* find next maxprio */
		}
	}
	/* If proc is running with lock */
	int ishavinglock = 0, i;
	for(i=0;i< NPROC; i++)
		if( ltab[i].holders[pid]== INHOLD ){
			ishavinglock = 1;
			break;
		}

	if( ishavinglock == 1){
	/* only change pprio if newprio is greater */
		if( newprio > pptr->pprio )
		{
			pptr->pprio = newprio;
		}
		/* set oldprio to newprio */
		pptr->oldprio = newprio;
	}	

	restore(ps);
	return(newprio);
}
