/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL	setnok(int nok, int pid)
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[14]=ctr1000;
		proc->count[14] += 1;
        }

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[14]= (proc->exectime[14]* (proc->count[14]-1) + (ctr1000-proc->t1[14]))/(proc->count[14]);
	}
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[14]= (proc->exectime[14]* (proc->count[14]-1) + (ctr1000-proc->t1[14]))/(proc->count[14]);
	}

	return(OK);
}
