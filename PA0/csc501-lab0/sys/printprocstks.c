#include <conf.h>
#include <kernel.h>
#include <proc.h>

extern int currpid;
extern int numproc;
static unsigned long *esp;

void printprocstks(int priority)
{
	kprintf("\nvoid printprocstks(int priority)\n");
	int i=0;
	while(i<NPROC)
	{
		
		struct pentry *proc = &proctab[i];
		if ( proc->pprio > priority)
		{	
			kprintf("\nProcess[%s]\n",proc->pname);
			kprintf("\tpid: %d\n", i);
			kprintf("\tpriority: %d\n", proc->pprio);
			kprintf("\tbase: 0x%08x\n",proc->pbase);
		
			kprintf("\tlimit: 0x%08x\n",proc->plimit);
			kprintf("\tlen: %d\n",proc->pstklen);
			if(i!=currpid)
			{
				kprintf("\tpointer:0x%08x\n\n",i,proc->pesp);
			}
			else				
			{
				asm("movl %esp,esp");
				kprintf("\tpointer:0x%08x\n\n",esp);
			}
		}
		i++;
	}
}
