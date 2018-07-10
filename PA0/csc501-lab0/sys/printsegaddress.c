#include <stdio.h>

extern char etext,edata,end;
void printsegaddress()
{
	int* t_ptr = &etext;
	int* d_ptr = &edata;
	int* e_ptr = &end;
 	kprintf("\nvoid printsegaddress()\n");	
	kprintf("\nCurrent: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x \n",t_ptr,*t_ptr,d_ptr,*d_ptr,e_ptr,*e_ptr);

	t_ptr-=1;
        d_ptr-=1;
        e_ptr-=1;
	kprintf("Preceding: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x \n",t_ptr,*t_ptr,d_ptr,*d_ptr,e_ptr,*e_ptr);

	t_ptr=t_ptr+2;
        d_ptr=d_ptr+2;
        e_ptr=e_ptr+2;
	 kprintf("After: etext[0x%08x]=0x%08x, edata[0x%08x]=0x%08x, ebss[0x%08x]=0x%08x \n",t_ptr,*t_ptr,d_ptr,*d_ptr,e_ptr,*e_ptr);	
}
