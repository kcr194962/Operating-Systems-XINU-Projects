#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long *esp;
static unsigned long *ebp;

void printtos()
{
	kprintf("\nvoid printtos()\n");
	unsigned long *sp1,*sp2;
	int i=0;
	asm("movl %ebp,ebp");
	sp1= ebp;	

	asm("movl %esp,esp");
	sp2= esp;
	
	kprintf("\nBefore[0x%08x]: 0x%08x\n", sp1+2, *(sp1+2));
	kprintf("After[0x%08x]: 0x%08x\n", sp1, *sp1);	
	while(sp2<sp1 && i<4)
	{
	kprintf("\telement[0x%08x]: 0x%08x\n",sp2+1,*(sp2+1));
	sp2=sp2+1;
	i++;
	}
}
