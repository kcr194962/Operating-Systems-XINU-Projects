#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

extern unsigned long ctr1000;

void rampup(int procid, int ldes, int newprio){
	struct pentry *ptr;
	struct lentry *lptr;
	int i;
	int pid, wlock;
	ptr = &proctab[procid];
	lptr = &ltab[ldes];
	/*When Kill calls to rampup */
	if(KILLRESET){
		/*set proc pprio to oldprio, if inherited*/
		if( ptr->pinh ==1) 
			{
				ptr->pprio = ptr->oldprio;
				ptr->pinh = 0;	/* back to original */
			}
	}
	/* when lock.c calls rampup */
	else{
		/*set pprio to new prio when its prio < newprio*/
		if( ptr->pprio < newprio)
			{	/* ptr prio is inherited now */
				ptr->pinh = 1;
				ptr->pprio = newprio;
			}
	}
	/* BASE c: when procid is not waiting for any lock */
	if( ptr->waitingfor == -1|| ptr->pstate == PRFREE){
		return;
	}
	/*When it is waiting */
	/*for each holder of the lock ldes waiting for wlock, rampup */
	for( i=0; i< NPROC; i++){
		pid=i;
		if(  lptr->holders[pid] == INHOLD){  /*pid is id of proc holding this lock */
			wlock = proctab[pid].waitingfor; /* lockid proc is waiting on */
			rampup( pid, wlock, newprio );	/* have to rampup this process */
			
		}
	}
}

int lock (int ldes1, int type, int priority) 
{

	STATWORD ps;
	struct lentry *lptr;
	struct pentry *ptr;
	disable(ps);
	lptr = &ltab[ldes1];
	if( isbadlock(ldes1) || lptr->lstate == LFREE|| proctab[currpid].locktype[ldes1] == DELETED){
		restore(ps);
		return SYSERR;
	}
	/* Lock is free, First proc to get lock is scheduled*/
	if( lptr->lstate == LUSED && lptr->ltype == NONE){
		lptr->ltype = type;
		if( type == READ){
			proctab[currpid].locktype[ldes1] = READER;
			lptr->nr++;
		}
		else
			proctab[currpid].locktype[ldes1] = WRITER; 
		lptr->holders[currpid] = INHOLD;
		resched();		
	}
	/* Lock is held by a WRITER */
	else if( lptr->ltype == WRITE){
		ptr = &proctab[currpid];
	 /* Both processes have to wait in queue */
		if( type == READ){
			lptr->nr++;
			ptr->locktype[ldes1] = READER;
		}
		if( type == WRITE){
			ptr->locktype[ldes1] = WRITER;
		}
		
		ptr->pstate = PRWAIT;
		ptr->waitingtime = ctr1000;
		insert(currpid, lptr->lqhead, priority);
		/*Adjust priorities */
		ptr->waitingfor = ldes1;
		updatemaxprio(ldes1);
		rampup( currpid, ldes1, proctab[currpid].pprio );
		updatemaxprio(ldes1);
		resched();
	}
	/* Lock is held by a READER */
	else if ( lptr->ltype == READ)
	{
		ptr = &proctab[currpid];
		/*if READER requests lock, no need to wait if it's prio > last writer. */
		if( type == READ){
			ptr->locktype[ldes1] = READER;
			int max = lastkey(lptr->lqtail);
			/* check if no writer or reader has highest prio to be scheduled */
			if( max == q[lptr->lqhead].qkey || max < priority){
				/* Schedule READER */
				lptr->nr++;
				lptr->holders[currpid] = INHOLD;
				resched();
			}
			else if ( max > priority ){
				/*Push reader into wait queue */
				ptr->pstate = PRWAIT;
                                ptr->waitingtime = ctr1000;
                       	        insert(currpid, lptr->lqhead, priority);
 	                        ptr->waitingfor = ldes1;
				updatemaxprio(ldes1);
				rampup( currpid, ldes1, proctab[currpid].pprio );
				updatemaxprio(ldes1);
				resched();
			}
			/* Equal priorities */
			else if( max == priority ){
				/*
				ptr->pstate = PRWAIT;
				ptr->waitingtime = ctr1000;
				insert(currpid, lptr->lqhead, priority);
				ptr->waitingfor = ldes1;
				updatemaxprio(ldes1);
				rampup( currpid, ldes1, proctab[currpid].pprio );
				updatemaxprio(ldes1);
				resched();
				*/
				/* READER gets lock */
				lptr->nr++;
                                lptr->holders[currpid] = INHOLD;
                                resched();
			}
		}
		/* if WRITER requests lock, Insert into wait q */
		if(type == WRITE){
			ptr->pstate = PRWAIT;
			ptr->waitingtime = ctr1000;

			ptr->locktype[ldes1] = WRITER;
			
			insert(currpid, lptr->lqhead, priority);
			ptr->waitingfor = ldes1;
			updatemaxprio(ldes1);
			rampup( currpid, ldes1, proctab[currpid].pprio );
			updatemaxprio(ldes1);
			resched();
		}
	}
	restore(ps);
	return proctab[currpid].pwaitret; 
}
