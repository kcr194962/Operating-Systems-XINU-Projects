/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

LOCAL int newsem();
unsigned long ctr1000;
extern int flag;

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL screate(int count)
{	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[15]=ctr1000;
		proc->count[15]+=1;
        }

	STATWORD ps;    
	int	sem;

	disable(ps);
	if ( count<0 || (sem=newsem())==SYSERR ) {
		restore(ps);
		if(flag ==1)
		{
	         struct pentry* proc= &proctab[currpid];
		proc->exectime[15]= (proc->exectime[15]* (proc->count[15]-1) + (ctr1000-proc->t1[15]))/(proc->count[15]);			
		}	
	return(SYSERR);
	}
	semaph[sem].semcnt = count;
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);
	if(flag ==1)
	return(sem);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newsem()
{
		if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[15]=ctr1000;
       		 }

	int	sem;
	int	i;

	for (i=0 ; i<NSEM ; i++) {
		sem=nextsem--;
		if (nextsem < 0)
			nextsem = NSEM-1;
		if (semaph[sem].sstate==SFREE) {
			semaph[sem].sstate = SUSED;
		if(flag ==1)
                {
                 struct pentry* proc= &proctab[currpid];
                proc->exectime[15]= (proc->exectime[15]* (proc->count[15]-1) + (ctr1000-proc->t1[15]))/(proc->count[15]);
                }
			return(sem);
		}
	}

	return(SYSERR);
}
