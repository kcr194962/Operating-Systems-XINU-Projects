#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <sem.h>

void prod2(int, int), cons2(int, int);
int  n = 0; /* Variable n has initial value zero */


main(void)
{
	syscallsummary_start();
	int  produced, consumed;
	consumed = screate(0);
	produced = screate(1);
	resume( create(cons2, 1024, 20, "cons", 2, consumed, produced) );
	 resume( create(prod2, 1024, 20, "prod", 2, consumed, produced) );
}

void prod2( int  consumed, int produced) {
	int  i;
	for( i=1 ; i<=4 ; i++ )
	 {
	  wait(consumed);
		n++; 
		signal(produced);
	}
}

void cons2( int  consumed, int produced)
{
	int  i;
	for( i=1 ; i<=4 ; i++ ) 
	{ 
	wait(produced);
	printf("n is %d \n", n); 
	signal(consumed);
	}
	        syscallsummary_stop();
	printsyscallsummary();

}
