/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

int prx;

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
void prch(char c)
{
	int i;
	sleep(1);
}
int main()
{	
	kprintf("\n 0xaabbccdd = %08x \n",zfunction(0xabcdabcd)); 	
	printsegaddress();
	printtos();
	printprocstks(0);
        syscallsummary_start();
        resume(prx = create(prch,2000,20,"proc X",1,'A'));
        sleep(2);
        syscallsummary_stop(); 
        printsyscallsummary();
	return 0;	
}
