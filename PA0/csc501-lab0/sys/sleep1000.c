/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep1000(int n)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[21]=ctr1000;
		proc->count[21] += 1;
        }

	STATWORD ps;    

	if (n < 0  || clkruns==0)
	{
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[21]= (proc->exectime[21]* (proc->count[21]-1) + (ctr1000-proc->t1[21]))/(proc->count[21]);
        }	
         return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
                proc->exectime[21]= (proc->exectime[21]* (proc->count[21]-1) + (ctr1000-proc->t1[21]))/(proc->count[21]);
        }
	return(OK);
}
