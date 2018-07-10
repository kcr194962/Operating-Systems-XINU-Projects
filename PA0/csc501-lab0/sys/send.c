/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(int pid, WORD msg)
{
	if(flag ==1)
	{
                struct pentry* proc= &proctab[currpid];
                proc->t1[12]=ctr1000;
        	proc->count[12]+=1;
	}

	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg != 0) {
		restore(ps);
		if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[12]= (proc->exectime[12]* (proc->count[12]-1) + (ctr1000-proc->t1[12]))/(proc->count[12]);
        }

		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV)	/* if receiver waits, start it	*/
		ready(pid, RESCHYES);
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
	proc->exectime[12]= (proc->exectime[12]* (proc->count[12]-1) + (ctr1000-proc->t1[12]))/(proc->count[12]);        
	}

	return(OK);
}
