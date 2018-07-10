#ifndef _LOCK_H_
#define _LOCK_H_

#include <proc.h>

#ifndef NLOCK
#define NLOCK 50
#endif

#define LFREE	'\01'
#define LUSED	'\02'
#define READ	'\03'
#define WRITE	'\04'
#define INHOLD	1

struct lentry{
	char lstate;		/* LFREE, LUSED*/
	int lqhead;	
	int lqtail;
	char ltype;		/* READ, WRITE, DELETED, NONE */
	int holders[NPROC];	/* INHOLD, NONE */	
	int nr;
	int maxprio;
};

extern struct lentry ltab[];
extern int nextlock;
extern int KILLRESET;
#define isbadlock(l) ( l<0  || l>=NLOCK )
#endif

