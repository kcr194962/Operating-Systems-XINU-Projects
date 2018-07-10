#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

extern unsigned long ctr1000;
int release(int ldes);
void updatemaxprio( int ldes);
void updateprocprio( int ldes);

int releaseall (int numlocks, int ldes1, ...)
{
	STATWORD ps;
	int ldes; 
	int retval = OK;
	int i =0;
	disable(ps);
	
	if( numlocks == 0)
		return SYSERR;
	for(i=0;i<numlocks;i++)
	{
		/* &ldes1 +1 gives 1st argument */
		ldes = (int)*( (&ldes1) + i );
		retval = release(ldes);
	}
	resched();
	restore(ps);
	return retval;
}

void updatemaxprio(int ldes){
	int i;
	int maxp=-1;
	for( i=0; i<NPROC; i++){
		/* for all procs in watilist of ldes */
		if( proctab[i].waitingfor == ldes)
			if( maxp < proctab[i].pprio )
				maxp = proctab[i].pprio;

	}
	ltab[ldes].maxprio = maxp;
}

void updateprocprio( int ldes) 
{
	int i, highest = -1;
	int t;
	struct pentry *ptr;
	for( i=0; i< NLOCK; i++){
		if( (t= proctab[currpid].locktype[i]) != NONE && t != DELETED && i != ldes)
			if(highest < ltab[i].maxprio )
				highest = ltab[i].maxprio;
	}
	ptr = &proctab[currpid];
	if ( highest == -1 || highest < ptr->oldprio)
		ptr->pprio = ptr->oldprio;
	else 
		ptr->pprio = highest;
}

int release(int ldes)
{
	struct lentry *lptr;
	struct pentry *ptr;
	int writeid;
	int pid;	/* Last proc pid or WRITER pid*/
	long timediff;
	int next; 
	ptr = &proctab[currpid]; 
	lptr = &ltab[ldes];
	/* check if lock can be released */
	if( isbadlock(ldes) || proctab[currpid].locktype[ldes] == DELETED
		|| ptr->pstate == PRFREE || lptr->lstate == LFREE){
		return SYSERR;
	}
	/* check if proc owns the lock ldes */
	if( lptr->holders[currpid] == NONE )
		return SYSERR;

	/* IF curr PROC ic a READER */
	if( ptr->locktype[ldes] == READER)
		lptr->nr--;
	/* Last proc in wait q */
	pid = q[lptr->lqtail].qprev;
	/* IF wait q is empty */
	if( pid == lptr->lqhead){
		ptr->waitingtime = 0;
		ptr->locktype[ldes] = NONE;

		lptr->ltype = NONE;
		
		lptr->maxprio = ptr->pprio;
		updateprocprio(ldes);
		lptr->holders[currpid] = NONE;
		resched();
		return OK;
	}
	/* If last proc type is READER */
	if( proctab[pid].locktype[ldes] == READER){
		next = pid;
		/* If last reader, Schedule readers until a writer is found or till the end */

		while( pid != lptr->lqhead ){
			if(  proctab[pid].locktype[ldes] == WRITER)
				break;
			lptr->nr++;
			proctab[pid].waitingtime = 0;
			next = q[pid].qprev;
				
			dequeue(pid);
			ready(pid, RESCHNO);

			lptr->holders[pid] = INHOLD;
			
			pid = next;
			
		}
		/* if no WRITER was found =>q is empty now and lock state = READ*/
		if( pid == lptr->lqhead){
			lptr->ltype = READ;
			/*RELEASE LOCK */
			lptr->maxprio = -1;
			updateprocprio(ldes);
			lptr->holders[currpid] = NONE;
			resched();
		}
		/* if WRITER was found = pid */ 
		else{
			writeid = pid;
			pid = q[pid].qprev;
			struct pentry *p;
			/* Find next reader with same priority */ 
			next = pid;
			while( pid != lptr->lqhead && q[pid].qkey == q[writeid].qkey ){
				p = &proctab[pid];
				/* Skip WRITER procs, only calc timediff for READERS */
				if(p->locktype[ldes] == READER ){
					/*calculate time diff */
					timediff = (ctr1000-proctab[writeid].waitingtime) - (ctr1000-p->waitingtime);
					if( timediff < 500){
					/*READER gets lock, Schedule reader process pid*/
						lptr->nr++;
						next = q[pid].qprev;
						proctab[pid].waitingtime = 0;
						lptr->holders[pid] = INHOLD;
						dequeue(pid);
						ready(pid, RESCHNO);

					}
					else if( timediff >= 500){
						/*WRITER gets lock, SKIP */
						next = q[pid].qprev;
						;
					}
				}
				else
					next = q[pid].qprev;
				pid = next;
			}
			/*Schedule WRITER/READER now */
			if(lptr->nr ==0 ){
				ltab[ldes].ltype = WRITE;
				proctab[writeid].waitingtime = 0;
				dequeue( writeid);
				ready(writeid, RESCHNO);

				lptr->holders[writeid] = INHOLD;				
			}
			else{
				ltab[ldes].ltype = READ;
			}
			/*RELEASE LOCK */
			updatemaxprio(ldes);
			updateprocprio(ldes);
					
			resched();
		}
	}
	/* If last proc type is  WRITER*/
	else if( proctab[pid].locktype[ldes] == WRITER){
		/* Last proc in wait q */
		pid = q[lptr->lqtail].qprev;
		/* Find Readers */
		writeid = pid;
		pid = q[pid].qprev;
		struct pentry *p;
		/* Find next reader with same priority */ 
		next = pid;
		/* only when the last reader gives up lock */
		if( lptr->nr == 0) 
		while( pid != lptr->lqhead && q[pid].qkey == q[writeid].qkey ){
			p = &proctab[pid];
			/* Skip WRITER procs, only calc timediff for READERS */
			if(p->locktype[ldes] == READER ){
				/*calculate time diff */
				timediff = (ctr1000-proctab[writeid].waitingtime) - (ctr1000-p->waitingtime);
				if( timediff <  500)
				{
				/* READER gets lock */
					lptr->nr++;
					proctab[pid].waitingtime = 0;
					next = q[pid].qprev;
					dequeue(pid);
					ready(pid, RESCHNO);
					lptr->holders[pid] = INHOLD;
				}
				else if( timediff >= 500){
					/*SKIP, WRITER gets lock */
					next = q[pid].qprev;
					;
				}
			}
			else
				next = q[pid].qprev;
			pid = next;
		}
		/*Schedule WRITER/READER now */
		if(lptr->nr ==0 ){
			ltab[ldes].ltype = WRITE;
			proctab[writeid].waitingtime = 0;
			dequeue( writeid);
			ready(writeid, RESCHNO);	
			lptr->holders[writeid] = INHOLD;			
		}
		else{
			ltab[ldes].ltype = READ;
		}
		/*RELEASE LOCK */
		updatemaxprio(ldes);
                updateprocprio(ldes);
		resched();
	}
	else
		return SYSERR;	
	return OK;
}
