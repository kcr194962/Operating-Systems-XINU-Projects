
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define DEFAULT_LOCK_PRIO 20
extern unsigned long ctr1000;
/*--------------------------------Test 1--------------------------------*/

void usinglocks_write (char *msg, int lck)
{ 
	kprintf ("%s: Requesting the lock, sleep 2s\n", msg);
	lock (lck, WRITE, DEFAULT_LOCK_PRIO);
	kprintf ("%s: acquired the lock, sleep 2s\n", msg);
	sleep(1);
	kprintf ("%s: to release lock\n", msg);
	releaseall (1, lck);
}

void usingsem_write (char *msg, int sid)
{
	kprintf ("%s: Requesting the lock, sleep 2s\n", msg);
	wait(sid);
	kprintf ("%s: acquired the lock (semaphore), sleep 2s\n", msg);
	sleep(1);
	kprintf ("%s: to release lock\n", msg);
	signal(sid);
}
void processmid( char *msg)
{
	kprintf ("B is running with midlevel priority\n", msg);
		sleep(2);
	kprintf("B done.\n");
}
void test1 ()
{
	int lck;
	int pid1;
	int pid2;
	int pid3;
	int sid;

	kprintf("\nTest 1\n");
	lck  = lcreate ();

	pid1 = create(usinglocks_write, 2000, 20, "Process a", 2, "A", lck);
	pid2 = create(processmid, 2000, 30, "Process b", 1, "Process B");
	pid3 = create(usinglocks_write, 2000, 40, "Process c", 2, "C", lck);

	resume(pid1);
	resume(pid3);
	resume(pid2);
	sleep (5);
	ldelete (lck);

	kill(pid1);
	kill(pid3);
	kill(pid2);

	kprintf("Locks test completed \n");
	kprintf("\n Using semaphores now \n");

	/* Using Semaphores */

	sid= screate(1);

	pid1 = create(usingsem_write, 2000, 20, "Process a", 2, "A", sid);
	pid3 = create(usingsem_write, 2000, 40, "Process c", 2, "C", sid);
	pid2 = create(processmid, 2000, 30, "Process b", 1, "Process B");

	
	resume(pid1);
	resume(pid3);
	resume(pid2);	
	sleep (5);
	sdelete (sid);

	kill(pid1);
	kill(pid2);
	kill(pid3);


	kprintf(" All tests complete \n");

}

int main( )
{

        test1();

shutdown();
}


