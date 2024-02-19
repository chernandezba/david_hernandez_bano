#include "stdlib.h"
#include "stdio.h"
#include "conio.h"

void main()
{
 long d,m,a;
 long g;
 long n,r;
 char str[10];

 cprintf("Dia: ");gets(str);d=atol(str);
 cprintf("Mes: ");gets(str);m=atol(str);
 cprintf("Any: ");gets(str);a=atol(str);

 if(a==1582 && m==10 && d<5)
  {g=0;}
 if(a==1582 && m<10)
  {g=0;}
 if(a==1582 && (m>10 || (m==10 && d>14)))
  {g=(a/100)-(a/400)-2;}
 if(a>1582)
  {g=(a/100)-(a/400)-2;}

 if(m<3)
  {m=m+12;a=a-1;}

 n=d+2*m+(3*(m+1)/5)+a+(a/4)-g;
 r=n-7*(n/7)+1;

 if(r==1) {cprintf("Disabte\r\n");}
 if(r==2) {cprintf("Diumenge\r\n");}
 if(r==3) {cprintf("Dilluns\r\n");}
 if(r==4) {cprintf("Dimarts\r\n");}
 if(r==5) {cprintf("Dimecres\r\n");}
 if(r==6) {cprintf("Digous\r\n");}
 if(r==7) {cprintf("Divendres\r\n");}

}