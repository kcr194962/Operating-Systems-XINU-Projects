/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

circ_queue* cq;

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
void init_bsm_entry(int i){
	
	int j = 0;
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	bsm_tab[i].bs_sem = 0;
	bsm_tab[i].isPriv = 0;
	for( j =0 ; j< 50; j++){
		bsm_tab[i].bs_pid[j] 	= -1;
		bsm_tab[i].bs_npages[j] = -1;
		bsm_tab[i].bs_vpno[j] 	= BACKING_STORE_VIRTUAL_PAGE_NUM;
	}
	bsm_tab[i].maxpages = 0;
}

SYSCALL init_bsm()
{
	STATWORD ps;
	disable(ps);

	int i = 0;
	for(i = 0; i < MAX_BS; i++){
		init_bsm_entry(i);
	}

	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	for(i = 0; i < MAX_BS; i++){
		if(bsm_tab[i].bs_status == BSM_UNMAPPED){
			//kprintf(" Backing store %d acquired \n",i);
			*avail = i;
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	disable(ps);

	init_bsm_entry(i);

	restore(ps);
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD ps;
	disable(ps);
	int i = 0;
	unsigned int vpno = (vaddr/NBPG);
	for(i = 0; i < MAX_BS; i++){
		if(bsm_tab[i].bs_pid[pid] == 1){
			*store = i;
			*pageth = vpno - bsm_tab[i].bs_vpno[pid];
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;
}

void showBSM(int pid){
	int i  = 0;
	for( i = 0; i< MAX_BS; i++){
		kprintf("\tBSM %d pid %d\n", i, bsm_tab[i].bs_pid[pid]);
	}
}
/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	STATWORD ps;
	disable(ps);
	
	int i = source;
	
	bsm_tab[i].bs_status = BSM_MAPPED;
	bsm_tab[i].bs_pid[pid] 	  = 1;
	bsm_tab[i].bs_vpno[pid]	  = vpno;
	bsm_tab[i].bs_npages[pid] = npages;
	bsm_tab[i].isPriv 	= 0;
	
	proctab[currpid].vhpno = vpno;
	proctab[currpid].store = source;

	restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD ps;
	disable(ps);
	int i =0;
	int bs,pageth;
	for(i = 0; i < NFRAMES; i++){
		if(frm_tab[i].fr_pid == pid && frm_tab[i].fr_type == FR_PAGE){
			bsm_lookup(pid,vpno * NBPG,&bs,&pageth);
			write_bs((i+NFRAMES)*NBPG, bs, pageth);
		}
	}
	init_bsm_entry(bs);

	restore(ps);
	return OK;
}


