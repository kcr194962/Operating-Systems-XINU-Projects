#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int lcreate(void){

	STATWORD ps;
	int lock=0;
	int i;

	disable(ps);

	for(i=0; i<NLOCK; i++){
		lock = i;
		if (ltab[lock].lstate == LFREE){
			ltab[lock].lstate = LUSED;
			ltab[lock].ltype = NONE;
			/* process that calls this should have locktype NONE */
			proctab[currpid].locktype[lock] = NONE;
			restore(ps);
			return lock;
		}
	}
	restore(ps);
}
