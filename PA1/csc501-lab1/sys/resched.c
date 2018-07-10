/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
extern int schedclass=0;
void setschedclass(int );
int getschedclass();
void randsched();
void linuxsched();
int getmaxgoodness();
int newepoch();
void printlist();
void pswitch( struct pentry *, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
void setschedclass(int type)
{
	schedclass = type;
}
int getschedclass()
{
	return schedclass;
}

void randsched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int node,totalc=0;
	int randc;

	optr = &proctab[currpid];
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}
	/*find total count*/
	node = q[rdyhead].qnext;
	while(node != rdytail)
	{
		totalc += q[node].qkey;
		node = q[node].qnext;
	}
	/*if last process is null ,set node to null */
	if (totalc <= 0)
		{
			node = 0;
		
		}
	else
	{
		randc= rand()% totalc;
		node = q[rdytail].qprev;
		while( node != rdyhead)
		{
			if( randc > q[node].qkey )
			randc = randc- q[node].qkey;
			else
			break;
			node = q[node].qprev;
		}
	}
	nptr = &proctab[node];
	currpid = node;

	dequeue(node);
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	/* The OLD process returns here when resumed. */
	return OK;
}
int getmaxgoodness()
{
	/*only runnable processes in ready q */
	int pid=0,i=0;
	int maxgoodness=0;
	register struct pentry *ptr; 
	i = q[rdytail].qprev;
	while( i != rdyhead)
	{
		ptr = &proctab[i];
		if (maxgoodness < ptr->goodness)
		{	
			maxgoodness = ptr->goodness;
			pid = i;
		}
		i = q[i].qprev;
	}
	return pid;
}
int newepoch()
{
	int i=0,sum=0;
	while( i < NPROC )
	{
	 	if ( proctab[i].pstate != PRFREE ) 
		{
			sum = sum + proctab[i].quantum;
		}
		i++;
	}
	return sum;
}
void printlist()
{
	int node = q[rdytail].qprev;
	register struct pentry *ptr;
	kprintf("\nREADY List is \n" );
	while( node != rdyhead)
	{
	ptr = &proctab[node];
	kprintf(" proc %s[%d] prio %d  goodness %d quantum %d canrun %d \n",ptr->pname,node,ptr->pprio,ptr->goodness,ptr->quantum,ptr->canrun);
	node = q[node].qprev;
	}
}
void pswitch ( struct pentry *optr, int n, int p)
{
	struct pentry * nptr;
	if ( optr->pstate == PRCURR )
	{
        	optr->pstate = PRREADY;
        	insert(currpid, rdyhead, optr->pprio);
        }
      	int next = n;
        nptr = &proctab[next];
        currpid = next;
        nptr->pstate = PRCURR;
        dequeue(next);
        if ( p != -1)
	preempt = p;
	else
	preempt = nptr->counter;
        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
       	return OK;

}
void linuxsched()
{
	register struct pentry  *optr;  /* pointer to old process entry */
        register struct pentry  *nptr;  /* pointer to new process entry */
        register struct pentry	*ptr;
	int i=0;
	int next;
	int oldprio;
	int nextg = 0;

	/* for handling -ve preempts */
	if( preempt < 0 )
        	preempt = 0;
	
	/*set old ptr and counter */
	optr = &proctab[currpid];
	optr->counter = preempt;
	
	/*calculate goodness for all procs except null and exhausted */	
	if ( currpid != 0 && optr->counter != 0 )
	{
		/* handle chrpio() case */
		oldprio = optr->goodness - optr->counter;
        	optr->goodness = oldprio + preempt;
	}
	else{
	optr->counter = 0;
        optr->quantum = 0;
        optr->goodness = 0;
	}

	/* get next proc */
	next = getmaxgoodness();
	nptr = &proctab[next];
	nextg = nptr->goodness;
	
	/* Case 1:  when proc gives up CPU or is terminated */
	if( nextg == 0 && (optr->pstate != PRCURR || optr->counter == 0))
	{
		/* calculate quantums if newepoch */
		if( newepoch( )== 0) 
		{
			for( i=0 ; i<NPROC; i++)
			{	
				ptr = &proctab[i];
			/*if free, ignore */
			if ( ptr->pstate != PRFREE)
			{
				/*for incomplete proc */
				if ( ptr->counter != 0 && ptr->counter != ptr->quantum)
                        ptr->quantum = ptr->counter/2 + ptr->pprio;
                else
                	/* for unrun or exhausted processes */
         				ptr->quantum = ptr->pprio;
			/* re calculate goodness */	
			ptr->counter = ptr->quantum;
			ptr->goodness = ptr->counter + ptr->pprio;
			}
			}
		}
		/* set preempt as switch can happen */
		preempt = optr->counter;
		/* handle last switch */
		if( (optr->pstate == PRFREE && next ==0) ||( currpid ==0 && next ==0 && optr->pstate == PRCURR ))
		{
		pswitch(optr,next,-1);
		}
		else
		{  //handle switching 
		next = getmaxgoodness();
		pswitch( optr, next, -1); 
		}
	}
	/* Case 2: curr proc has highest goodness */
        if ( nextg <= optr->goodness && optr->goodness > 0 )
        {
        if( optr->pstate == PRCURR){
                preempt = optr->counter;
		return OK;
                }
        }
	/* Case 3: maxgoodness is 0 then switch to null*/
	if( nextg == 0) 
	{
		next = 0;
		pswitch( optr, next, 0);
	}
	/* Case 4: if another process exsits with better goodness , switch */
	else {
		if( nextg > optr->goodness || optr->pstate != PRCURR || optr->counter == 0){	
		pswitch(optr, next, -1);
		}
	}
}

int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
	if(getschedclass()==RANDOMSCHED)
	{
		randsched();
	}
	else if(getschedclass()==LINUXSCHED)
	{
		linuxsched();
	}
	else
	{
	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	/* The OLD process returns here when resumed. */
	return OK;
}
}
