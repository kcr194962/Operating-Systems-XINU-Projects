/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

extern int page_replace_policy;
extern int allocatePageTables();
circ_queue* cq;

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);

	int store, pageth;
	int new_pt, new_frm;
	unsigned long va, pdbr;

	pd_t* pd_entry;
	pt_t* pt_entry;

	va  = read_cr2();

	virt_addr_t* va_struct;
	va_struct = (virt_addr_t*) &va;
	pdbr = proctab[currpid].pdbr;

	pd_entry = pdbr + (va_struct->pd_offset *sizeof(pd_t));
	if(bsm_lookup(currpid, va, &store, &pageth) == SYSERR){
		kill(currpid);
		restore(ps);
		return (SYSERR);
	}

	if(pd_entry->pd_pres ==0){
		new_pt = allocatePageTables();
		//kprintf(" New ptr : %d\n", new_pt);
		pd_entry->pd_pres 	= 1;
		pd_entry->pd_write 	= 1;
		pd_entry->pd_user 	= 1;
		pd_entry->pd_pwt 	= 0;
		pd_entry->pd_pcd 	= 0;
		pd_entry->pd_acc 	= 0;
		pd_entry->pd_mbz 	= 0;
		pd_entry->pd_global = 0;
		pd_entry->pd_avail 	= 0;
		pd_entry->pd_base 	= new_pt + FRAME0;

		frm_tab[new_pt].fr_status 	= FRM_MAPPED;
		frm_tab[new_pt].fr_pid 		= currpid;
		frm_tab[new_pt].fr_type 	= FR_TBL;
	}
	pt_entry = (pd_entry->pd_base) * NBPG  + (va_struct->pt_offset *sizeof(pt_t));

	if(pt_entry->pt_pres == 0)
	{
		get_frm(&new_frm);
		//kprintf(" pt new frame %d\n", new_frm);
		circ_enqueue(cq, new_frm);
		pt_entry->pt_pres 	= 1;
		pt_entry->pt_write 	= 1;
		pt_entry->pt_user 	= 1;
		pt_entry->pt_pwt 	= 0;
		pt_entry->pt_pcd 	= 0;
		pt_entry->pt_acc 	= 1;
		pt_entry->pt_dirty 	= 0;		
		pt_entry->pt_mbz 	= 0;
		pt_entry->pt_global = 0;
		pt_entry->pt_avail 	= 0;
		pt_entry->pt_base 	= new_frm + FRAME0;

		frm_tab[new_frm].fr_status 	= FRM_MAPPED;
		frm_tab[new_frm].fr_pid 	= currpid;
		frm_tab[new_frm].fr_vpno 	= va / NBPG;
		frm_tab[new_frm].fr_type 	= FR_PAGE;
		frm_tab[new_frm].fr_dirty	= 0;
		frm_tab[new_frm].fr_count 		+= 1;
		frm_tab[pd_entry->pd_base - FRAME0].fr_refcnt += 1;
		
		read_bs((char*)( (FRAME0+new_frm) * NBPG ), store, pageth);
	}

	restore(ps);
	return OK;
}


