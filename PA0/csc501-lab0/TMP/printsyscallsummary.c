#include <conf.h>

#include <kernel.h>
#include <proc.h>
#include <stdio.h>

extern int flag=0;
struct pentry* proc;
char scalls[27][15]={"freemem","chprio","getpid","getprio","gettime","kill","receive","recvclr","recvtim","resume","scount","sdelete","send","setdev","setnok","screate","signal","signaln","sleep","sleep10","sleep100","sleep1000","sreset","stacktrace","suspend","unsleep","wait"};
void syscallsummary_start()
{
	flag=1;
}

void syscallsummary_stop()
{
	flag=0;
}
void printsyscallsummary()
{
	kprintf("\nvoid printsyscallsummary()\n\n");
	int k=0;
	int j=0;
	for(k=0;k<NPROC;k++)
	{
		proc = &proctab[k];
		for(j=0;j<27;j++)
                {
                	if (proc->count[j]>0)
                	{	
			printf("Process [pid:%d]\n",k);
			break;
			}
		}
		for(j=0;j<27;j++)
		{
			if (proc->count[j]>0)
			{
			printf("\tSyscall: sys_%s, ",scalls[j]);
			printf("count: %d, ",proc->count[j]);
		     printf("average execution time: %d (ms)\n",proc->exectime[j]);
			}
		}	
	}	
}
