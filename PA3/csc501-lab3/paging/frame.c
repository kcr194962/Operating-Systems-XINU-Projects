/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_frm_SC(void);
int get_frm_LFU(void);
extern int page_replace_policy;
circ_queue* cq;
/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
	STATWORD ps;
	disable(ps);

	int i = 0;
	for(i = 0; i < NFRAMES; i++){
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = 0;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE;
		frm_tab[i].fr_dirty = 0;
		frm_tab[i].fr_count = 0;
	}

  restore(ps);
  return OK;
}

SYSCALL init_frm_entry(int i)
{
	STATWORD ps;
	disable(ps);

	frm_tab[i].fr_status = FRM_UNMAPPED;
	frm_tab[i].fr_pid = -1;
	frm_tab[i].fr_vpno = 0;
	frm_tab[i].fr_refcnt = 0;
	frm_tab[i].fr_type = FR_PAGE;
	frm_tab[i].fr_dirty = 0;
	frm_tab[i].fr_count = 0;

  restore(ps);
  return OK;
}
/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	STATWORD ps;
	disable(ps);
	
	int i = 0;
	*avail = -1;
	int frame_no;
	
	for(i = 0; i < NFRAMES; i++)
	{
		if(frm_tab[i].fr_status == FRM_UNMAPPED)
		{
			*avail = i;
			restore(ps);
			return OK;
		}
	}
	/* Get frame by policy */
	if(page_replace_policy == SC){
		frame_no = get_frm_SC();
		kprintf(" Returned frame: %d\n", 1024 + frame_no);
		free_frm(frame_no);
		*avail = frame_no;
		restore(ps);
		return OK;
	}
	if(page_replace_policy == LFU){
		frame_no = get_frm_LFU();
		kprintf(" Returned frame: %d\n", 1024 + frame_no);
		free_frm(frame_no);
		*avail = frame_no;
		restore(ps);
		return OK;
	}

	restore(ps);
	return SYSERR;
}

int get_frm_SC(){

	int i = 0, fpid, flag=0;
	unsigned long va, pdbr;

	virt_addr_t* va_struct;
	pd_t *pd_entry; 
	pt_t *pt_entry;

	while(1)
	{
		i = cq->SC_curr->data;
		if(frm_tab[i].fr_type == FR_PAGE){
			fpid = frm_tab[i].fr_pid;
			pdbr = proctab[fpid].pdbr;
			va 	 = frm_tab[i].fr_vpno * 4096;
			va_struct = (virt_addr_t*) &va;
		
			pd_entry = pdbr + (va_struct->pd_offset *sizeof(pd_t));
			pt_entry = (pd_entry->pd_base) * NBPG  + (va_struct->pt_offset *sizeof(pt_t));

			if(pt_entry->pt_acc == 0){
				//showq(cq);
				circ_delete(cq, cq->SC_curr);
				return i;	
			}
			else{
				pt_entry->pt_acc = 0;
			}
		}
		cq->SC_curr = cq->SC_curr->next;
	}
}
int get_frm_LFU(){
	
	int i = 0;
	int frm = -1;
	int min = 9999;
	while(i < NFRAMES){
		if(frm_tab[i].fr_type == FR_PAGE){
			if( min > frm_tab[i].fr_count ){
				min = frm_tab[i].fr_count;
				frm = i;
			}
			if( min == frm_tab[i].fr_count){
				//Get larger vpno frame 
				if(frm_tab[frm].fr_vpno < frm_tab[i].fr_vpno )
					frm = i;
			}
		}
	i++;
	}
	return frm;
}


/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);

	pd_t *pd_entry;
	pt_t *pt_entry;
	virt_addr_t* va_struct;
	int fpid;
	unsigned long va, pdbr;
	int bs_ptr, pg_no, pt_frame;

	if(frm_tab[i].fr_type==FR_PAGE){
		fpid = frm_tab[i].fr_pid;
		pdbr = proctab[fpid].pdbr;
		va 	 = frm_tab[i].fr_vpno * 4096;
		va_struct = (virt_addr_t*) &va;
		
		pd_entry = pdbr + (va_struct->pd_offset *sizeof(pd_t));
		pt_entry = (pd_entry->pd_base) * NBPG  + (va_struct->pt_offset *sizeof(pt_t));

		bs_ptr = proctab[fpid].store;
		pg_no = frm_tab[i].fr_vpno - proctab[fpid].vhpno;
		
		write_bs((i+FRAME0) * NBPG, bs_ptr, pg_no);
		
		pt_entry->pt_pres = 0;
		pt_frame = pd_entry->pd_base - FRAME0;
		
		frm_tab[pt_frame].fr_refcnt -= 1;
		if(frm_tab[pt_frame].fr_refcnt==0){
			frm_tab[pt_frame].fr_status = FRM_UNMAPPED;
			frm_tab[pt_frame].fr_pid = -1;
			frm_tab[pt_frame].fr_vpno = 4096;
			frm_tab[pt_frame].fr_type = FR_PAGE;
			
			pd_entry->pd_pres = 0;
		}
		restore(ps);
		return OK;
	}
	if(frm_tab[i].fr_type != FR_PAGE){
		if( i > 3){
			init_frm_entry(i);
		}
		restore(ps);
		return OK;
	}

	restore(ps);
 	return SYSERR;
}

void evict_frame(int pid)
{
	int i = 0;
	for(i = 0; i < NFRAMES; i++){
		if(frm_tab[i].fr_pid == pid)
			{
		  	frm_tab[i].fr_status = FRM_UNMAPPED;
		  	frm_tab[i].fr_pid = -1;
		  	frm_tab[i].fr_vpno = BACKING_STORE_VIRTUAL_PAGE_NUM;
		  	frm_tab[i].fr_refcnt = 0;
		  	frm_tab[i].fr_type = FR_PAGE;
		  	frm_tab[i].fr_dirty = 0;
		  	frm_tab[i].fr_count = 0;
		  	//circ_delete(cq, cq->SC_curr );
			}
		}
}

void allocatePageDirectories(int pid)
{
	int frame_no = 0;
	int i;
	pd_t *pd_entry;
	if(get_frm(&frame_no)==SYSERR){
		//kprintf("frame.c : get_frm() failed\n");
		return;
	}
	else{
		proctab[pid].pdbr = (FRAME0 + frame_no) * NBPG;
		frm_tab[frame_no].fr_status = FRM_MAPPED;
		frm_tab[frame_no].fr_pid	= pid;
		frm_tab[frame_no].fr_type	= FR_DIR;

		pd_entry = (FRAME0+ frame_no) * NBPG;
		
		for(i = 0; i < NFRAMES; i++){
			pd_entry[i].pd_write = 1;
			if( i < 4){
				pd_entry[i].pd_pres = 1;
				pd_entry[i].pd_base = FRAME0 + i;
			}
			else{
				pd_entry[i].pd_pres = 0;
			}
		}
	}
}

int allocatePageTables(){
	
	int i, frame_no;
	unsigned int frame_addr;
	pt_t *pt_entry;

	if(get_frm(&frame_no) == SYSERR){
		return SYSERR;
	}
	
	frame_addr = (FRAME0 + frame_no)*NBPG;
	pt_entry = (pt_t*) frame_addr;

	for(i=0;i<NFRAMES;i++){
		pt_entry[i].pt_pres 	= 0;
		pt_entry[i].pt_write 	= 1;
		pt_entry[i].pt_user 	= 0;
		pt_entry[i].pt_pwt 		= 0;
		pt_entry[i].pt_pcd 		= 0;
		pt_entry[i].pt_acc 		= 0;
		pt_entry[i].pt_dirty 	= 0;		
		pt_entry[i].pt_mbz 		= 0;
		pt_entry[i].pt_global 	= 0;
		pt_entry[i].pt_avail 	= 0;
		pt_entry[i].pt_base 	= 0;

	}
	return frame_no;
}


void circ_enqueue(circ_queue *q, int newval){

	node* temp = (node*) getmem(sizeof(node));
	temp->data = newval;

	if(q->front == NULL){
		q->front = temp;
		q->SC_curr = temp;
	}
	else
		q->rear->next = temp;

	q->rear = temp;
	q->rear->next = q->front;
}

void circ_delete(circ_queue* q, node* del){
	if( q->front == q->rear){
		q->SC_curr = NULL;
		q->front = NULL;
		q->rear = NULL;
		return;
	}
	node* prev = q->front;
	while(prev->next != del){
		prev = prev->next;
	}
	prev->next = del->next;
	q->SC_curr = prev->next;
	
	if( del == q->rear){
		q->rear = prev;
	}
	return;
}


void showq(circ_queue *q){
	node *curr = q->front; 
	kprintf(" cq elements are  : ");
	while (curr->next != q->front){
		kprintf(" %d ", curr->data);
		curr = curr->next;
	}
	kprintf("\n");
}




