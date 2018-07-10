#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

struct lentry ltab[NLOCK];
extern int nextlock=NLOCK-1;
extern int KILLRESET = 0;
void linit()
{
	int i,j;
	struct lentry *lptr;
	for (i=0 ; i<NLOCK ; i++) {      /* initialize locks*/
		ltab[i].lstate = LFREE;	
		lptr = &ltab[i];
		lptr->ltype = NONE;
		lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        	lptr->maxprio = 0;
		/* Set holders to 0 */
		for( j=0; j< NPROC; j++){
			lptr->holders[i] = NONE;
		}
	}
}
