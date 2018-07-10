
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

SYSCALL ldelete(int ldes)
{
	STATWORD ps;
	int pid;
	struct lentry *lptr;
	struct pentry * ptr;

	disable(ps);
	ptr = &proctab[currpid];
	/*if lock is free or deleted or proc doesn't own this lock */
	if( isbadlock(ldes) || ltab[ldes].lstate == LFREE ||
		ltab[ldes].lstate == DELETED || ptr->locktype[ldes] == NONE ) {
		restore(ps);
		return SYSERR;
	}

	lptr= &ltab[ldes];
	/*set state to free and locktype as deleted */
	lptr->lstate = LFREE;
	lptr->ltype = DELETED;

	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY) 
		  {
		    proctab[pid].pwaitret = DELETED;
		    proctab[pid].locktype[ldes] = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	
	restore(ps);
	return(OK);
}
