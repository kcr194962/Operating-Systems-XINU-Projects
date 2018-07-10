/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
extern int getutim(unsigned long *);

unsigned long ctr1000;
extern int flag;
/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */
if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[4]=ctr1000;
		proc->count[4]+=1;
        }
 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
   	proc->exectime[4]= (proc->exectime[4]* (proc->count[4]-1) + (ctr1000-proc->t1[4]))/(proc->count[4]);
	}
	 return OK;
}
