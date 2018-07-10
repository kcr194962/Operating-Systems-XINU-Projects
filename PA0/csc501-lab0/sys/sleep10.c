/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep10(int n)
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[19]=ctr1000;
		proc->count[19] += 1;
        }

	STATWORD ps;    
	if (n < 0  || clkruns==0)
	{
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[19]= (proc->exectime[19]* (proc->count[19]-1) + (ctr1000-proc->t1[19]))/(proc->count[19]);
        }
        return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
	restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[19]= (proc->exectime[19]* (proc->count[19]-1) + (ctr1000-proc->t1[19]))/(proc->count[19]);
        }
	return(OK);
}
