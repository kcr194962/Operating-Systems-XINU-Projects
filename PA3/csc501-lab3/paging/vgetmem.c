/* getmem.c - getmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include <proc.h>
#include <paging.h>
/*------------------------------------------------------------------------
 * getmem  --  allocate heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD *vgetmem(unsigned nbytes)
{
	STATWORD ps;    
	struct	mblock	*p, *q, *leftover;

	disable(ps);
	if (nbytes==0 || proctab[currpid].vmemlist->mnext== (struct mblock *) NULL) {
		restore(ps);
		return( (WORD *)SYSERR);
	}
	struct mblock *ptr = proctab[currpid].vmemlist;
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= ptr,p= ptr->mnext ;
	     p != (struct mblock *) NULL ;
	     q= p,p= p->mnext)
		if ( q->mlen == nbytes) {
			q->mnext = p->mnext;
			restore(ps);
			return( (WORD *)p );
		} else if ( q->mlen > nbytes ) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			//leftover->mnext = p->mnext;
			q->mlen = q->mlen - nbytes;
			restore(ps);
			return( (WORD *)p );
		}
	restore(ps);
	return( (WORD *)SYSERR );
}
