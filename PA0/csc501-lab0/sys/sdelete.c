/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 * sdelete  --  delete a semaphore by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL sdelete(int sem)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[11]=ctr1000;
       		proc->count[11] += 1;
	 }

	STATWORD ps;    
	int	pid;
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE) {
		restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[11]= (proc->exectime[11]* (proc->count[11]-1) + (ctr1000-proc->t1[11]))/(proc->count[11]);
	}

	
	return(SYSERR);
	}
	sptr = &semaph[sem];
	sptr->sstate = SFREE;
	if (nonempty(sptr->sqhead)) {
		while( (pid=getfirst(sptr->sqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[11]= (proc->exectime[11]* (proc->count[11]-1) + (ctr1000-proc->t1[11]))/(proc->count[11]);
	}

	return(OK);
}
