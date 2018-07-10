/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	STATWORD ps;
	disable(ps);
	
	if (virtpage < 4096 || source < 0 || source > MAX_BS || npages < 1 || npages > 128){
		restore(ps);
		return SYSERR;
	}
	
	if(bsm_tab[source].bs_status == BSM_UNMAPPED){
		restore(ps);
		return SYSERR;
	}
	else{
		if(bsm_tab[source].maxpages < npages){
			restore(ps);
			return SYSERR;
		}
		else{
			bsm_map(currpid, virtpage, source, npages);
			restore(ps);
			return OK;
		}
	}
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	STATWORD ps;
	disable(ps);
	if (virtpage < 4096){

		return SYSERR;
	}
	bsm_unmap(currpid, virtpage, 0);

	restore(ps);
 	return OK;
}
