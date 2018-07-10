/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	STATWORD ps;
	disable(ps);

	if (size == 0 || block < BACKING_STORE_VIRTUAL_PAGE_NUM * NBPG){
		restore(ps);
		return SYSERR;
	}
	size = roundmb(size);
	struct mblock *vmlist, *prev, *next;
	
	vmlist = proctab[currpid].vmemlist;
	prev  = vmlist;
	next  = vmlist->mnext;

	while (next < block && next != NULL) {
		prev = next;
		next = next->mnext;
	}

	block->mnext = next;
	block->mlen = size;
	prev->mnext = block;
	
	restore(ps);
	return(OK);
}
