#pragma inline
#include "conio.h"
#include "stdio.h"
#include "dos.h"

void main()
{
 FILE *fp;

 unsigned char a,i;
 unsigned char hex[10],hex2[10];

 clrscr();
 fp=fopen("CMOST.DAT","wb");
 for(i=0;i<=63;i++)
  {
   a=i;
   asm {
	mov al,a
	out 70h,al
	in al,71h
	mov a,al
       }

   itoa(a,hex,16);
   if(strlen(hex)==1)
    {strcpy(hex2,"0");strcat(hex2,hex);strcpy(hex,hex2);}
   strupr(hex);
   strcat(hex,"  ");
   fwrite(hex,4,1,fp);
   cprintf("%3i ",a);
  }

 fclose(fp);

}
