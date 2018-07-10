/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */

void resetprio(int pid ){
	struct  pentry  *pptr;
	int i,j;
	struct lentry *lptr;
	pptr= &proctab[pid];
	 /* when proc being killed/changed has prio = maxprio of wait q, change to next maxprio */
        if( pptr->pstate == PRWAIT &&  pptr->waitingfor != -1 ){
                lptr = &ltab[pptr->waitingfor];
                
		/* Change to old prio */
                        KILLRESET = 1;
                        rampup(pid, pptr->waitingfor, pptr->pprio );
                        KILLRESET = 0; 
		
		if(  pptr->pprio == lptr->maxprio){
		
			/* find next maxprio =maxpid, if any */
			int id = q[lptr->lqhead].qnext;
                        int maxpid = -1, maxp = -1;
                        while ( id != lptr->lqtail){
                                if( proctab[id].pprio > maxp && id!= pid )
                                {       maxp = proctab[id].pprio;
                                        maxpid = id;
                                }
                                id = q[id].qnext;
                        }
			/* Found ,rampup */
                        if (maxpid != -1)
                                rampup( maxpid, pptr->waitingfor, maxp);
                        /* Not Found , reset all holders of lock to old prio */
			/*	if( maxpid == -1 ){
                                int x;
                                for( i =0; i< NPROC; i++)
                                        if( (x= lptr->holders[i]) == INHOLD )
                                                proctab[x].pprio = proctab[x].oldprio;
                        }*/
                }
        }

}

SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	struct lentry *lptr; 
	int i;
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	/*locks */
	pptr->waitingtime = 0;
	resetprio(pid);
        pptr->waitingfor = -1;
	
	/*end */
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
	switch (pptr->pstate) {
	case PRCURR:
			pptr->pstate = PRFREE;	/* suicide */
			for (i = 0; i < NLOCK; i++) {
                            if (proctab[pid].locktype[i] != NONE )
                                 release(i);}
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:
			dequeue(pid);
			pptr->pstate = PRFREE;
			for (i = 0; i < NLOCK; i++) {
                           if (proctab[pid].locktype[i] != NONE )
                                release(i);}
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:
			pptr->pstate = PRFREE;
			for (i = 0; i < NLOCK; i++) {
                            if (proctab[pid].locktype[i] != NONE )
                                 release(i);}
	}
	restore(ps);
	return(OK);
}
