/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL kill(int pid)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[5]=ctr1000;
        	proc->count[5]+=1;
	}

	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[5]= (proc->exectime[5]* (proc->count[5]-1) + (ctr1000-proc->t1[5]))/(proc->count[5]);

        }
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);
	freestk(pptr->pbase, pptr->pstklen);
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[5]= (proc->exectime[5]* (proc->count[5]-1) + (ctr1000-proc->t1[5]))/(proc->count[5]);
	
        }
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[5]= (proc->exectime[5]* (proc->count[5]-1) + (ctr1000-proc->t1[5]))/(proc->count[5]);

        }	
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[5]= (proc->exectime[5]* (proc->count[5]-1) + (ctr1000-proc->t1[5]))/(proc->count[5]);
        
        }

	return(OK);
}
