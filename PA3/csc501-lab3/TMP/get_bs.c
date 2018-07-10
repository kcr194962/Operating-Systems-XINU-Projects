#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
  STATWORD ps;
  disable(ps);

  if( npages < 0 || npages > 128 || bs_id < 0 || bs_id > MAX_BS){
  	restore(ps);
  	return SYSERR;
  }
  if( bsm_tab[bs_id].bs_status == BSM_UNMAPPED ){
      bsm_tab[bs_id].bs_pid[currpid] = 1;
      bsm_tab[bs_id].bs_npages[currpid] = npages;
      bsm_tab[bs_id].bs_status = BSM_MAPPED;
      bsm_tab[bs_id].maxpages  = npages;
      restore(ps);
      return npages;
  }
	if(bsm_tab[bs_id].isPriv == 1 && bsm_tab[bs_id].bs_status == BSM_MAPPED){
    restore(ps);
		return SYSERR;
	}
	else if(bsm_tab[bs_id].bs_status == BSM_MAPPED){
    if( npages > bsm_tab[bs_id].maxpages){
      restore(ps);
      return bsm_tab[bs_id].maxpages;
    }
    else{
      bsm_tab[bs_id].bs_pid[currpid] = 1;
      bsm_tab[bs_id].bs_npages[currpid] = npages;
      restore(ps);
		  return bsm_tab[bs_id].maxpages;
    }
	}
}


