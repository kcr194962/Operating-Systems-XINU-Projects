/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include <proc.h>

unsigned long ctr1000;
extern int flag;
/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL	freemem(struct mblock *block, unsigned size)
{	
	if(flag ==1){
                struct pentry* proc= &proctab[currpid];
                proc->t1[0]=ctr1000;
       		proc->count[0]+=1;
	 }

	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;

	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end))
	{
	if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        	proc->exectime[0]= (proc->exectime[0]* (proc->count[0]-1) + (ctr1000-proc->t1[0]))/(proc->count[0]);
	}	
	return(SYSERR);
	}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
		proc->exectime[0]= (proc->exectime[0]* (proc->count[0]-1) + (ctr1000-proc->t1[0]))/(proc->count[0]);
	}
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	 if(flag ==1)
        {
        struct pentry* proc= &proctab[currpid];
        proc->exectime[0]= (proc->exectime[0]* (proc->count[0]-1) + (ctr1000-proc->t1[0]))/(proc->count[0]);
	}

	return(OK);
}
