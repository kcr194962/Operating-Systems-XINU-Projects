#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {
	STATWORD ps;
	disable(ps);

	int i = 0;
	int shared = 0;
	if( bs_id > 16)
	{
		restore(ps);
		return SYSERR;
	}
	
	if(bsm_tab[bs_id].isPriv == 1){
		init_bsm_entry(bs_id);
		restore(ps);
		return OK;
	}
	for( i =0 ; i< 50 ; i++){
		if(bsm_tab[bs_id].bs_pid[i] > 0){
			shared = 1;
		}
	}

	if(shared == 1){	
		bsm_tab[bs_id].bs_pid[currpid] = -1;
		bsm_tab[bs_id].bs_vpno[currpid] = BACKING_STORE_VIRTUAL_PAGE_NUM;
		bsm_tab[bs_id].bs_npages[currpid] = -1;
		bsm_tab[bs_id].bs_sem = 0;
		restore(ps);
		return OK;
	}
		init_bsm_entry(bs_id);
	restore(ps);
	return OK;
}

