/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[9]=ctr1000;
       		proc->count[9]+=1;
	 }

	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
	if(flag ==1 )

        {
         struct pentry* proc= &proctab[currpid];
        proc->exectime[9]= (proc->exectime[9]* (proc->count[9]-1) + (ctr1000-proc->t1[9]))/(proc->count[9]);

        }
	
	return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	 if(flag ==1 && currpid!=0 )

        {
	 struct pentry* proc= &proctab[currpid];
        proc->exectime[9]= (proc->exectime[9]* (proc->count[9]-1) + (ctr1000-proc->t1[9]))/(proc->count[9]);

	}

	return(prio);
}
