/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[10]=ctr1000;
       		proc->count[10] += 1;
	 }

	extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
	{
	  if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[10]= (proc->exectime[10]* (proc->count[10]-1) + (ctr1000-proc->t1[10]))/(proc->count[10]);
	}

	return(SYSERR);
	}
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[10]= (proc->exectime[10]* (proc->count[10]-1) + (ctr1000-proc->t1[10]))/(proc->count[10]);
	}

	return(semaph[sem].semcnt);
}
