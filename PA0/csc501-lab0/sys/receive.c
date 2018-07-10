/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[6]=ctr1000;
		proc->count[6]+=1;
        }

	STATWORD ps;    
	struct	pentry	*pptr;
	WORD	msg;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
	proc->exectime[6]= (proc->exectime[6]* (proc->count[6]-1) + (ctr1000-proc->t1[6]))/(proc->count[6]);
	}
	return(msg);
}
