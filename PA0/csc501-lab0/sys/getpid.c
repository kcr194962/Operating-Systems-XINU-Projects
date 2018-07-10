/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

unsigned long ctr1000;
extern int flag;
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[2]=ctr1000;
        	proc->count[2] += 1;
	}
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[2]= (proc->exectime[2]* (proc->count[2]-1) + (ctr1000-proc->t1[2]))/(proc->count[2]);
	}

	return(currpid);
}
