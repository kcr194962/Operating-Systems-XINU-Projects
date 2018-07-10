/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[7]=ctr1000;
		proc->count[7] += 1; 
       }

	STATWORD ps;    
	WORD	msg;

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
       proc->exectime[7]= (proc->exectime[7]* (proc->count[7]-1) + (ctr1000-proc->t1[7]))/(proc->count[7]);
	 }

	return(msg);
}
