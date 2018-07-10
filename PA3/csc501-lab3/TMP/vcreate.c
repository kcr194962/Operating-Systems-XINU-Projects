/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD ps;
	disable(ps);

	int pid; 
	int bs_ptr;
	struct mblock *base;

	if(get_bsm(&bs_ptr) == SYSERR){
		return SYSERR;
	}

	bsm_tab[bs_ptr].bs_status 	= BSM_MAPPED;

	bsm_tab[bs_ptr].bs_vpno[pid] 	= BACKING_STORE_VIRTUAL_PAGE_NUM;
	bsm_tab[bs_ptr].bs_npages[pid] 	= hsize;
	bsm_tab[bs_ptr].bs_sem 			= 0;
	bsm_tab[bs_ptr].isPriv 			= 1;

	pid = create(procaddr,ssize,priority,name,nargs,args);
	bsm_tab[bs_ptr].bs_pid[pid] = 1;

	proctab[pid].store 		= bs_ptr;
	proctab[pid].vhpno 		= 4096;
	proctab[pid].vhpnpages 	= hsize;
	proctab[pid].vmemlist->mlen = hsize*4096;
	proctab[pid].vmemlist->mnext = 4096 * NBPG;

	restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
