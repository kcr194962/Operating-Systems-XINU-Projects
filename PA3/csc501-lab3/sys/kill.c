/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

circ_queue* cq;
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int i;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);

	/* Demand Paging */
	release_bs(proctab[pid].store);


	node* temp = cq->front;
	//kprintf(" Deleting now \n");
	for(i=4; i<NFRAMES; i++){	
		if( (frm_tab[i].fr_type != FR_DIR ) && frm_tab[i].fr_pid == pid 
			&& frm_tab[i].fr_status == FRM_MAPPED ){
			if( frm_tab[i].fr_type == FR_PAGE){
				//only one frame in q
				if( cq->front == cq->rear && temp->data == i)
					circ_delete(cq, temp);

				else{
					//many frames in q. 
					while( temp->next != cq->front){
						if( temp->data == i)
							break;
					temp = temp->next;
					}
					circ_delete(cq, temp);
				}
				//kprintf("Freeing [page\n");
				free_frm(i);
			}
			else{
			//Free Table
				free_frm(i);
			}
		}
	}
	free_frm((proctab[pid].pdbr >> 12)-FRAME0);

	proctab[pid].pdbr=0;
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}

	restore(ps);
	return(OK);
}
