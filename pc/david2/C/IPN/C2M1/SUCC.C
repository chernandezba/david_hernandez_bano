// 21.Programa que calcula la succesi¢n de 1+2+...+n , dado n

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch


void main(void)  {
unsigned int m,n;
unsigned long res;

printf("\nIntroduzca el valor de n para la suma 1+2+..+n\n");
scanf("%u",&n);
res=0;
if (n)
  for (m=1;m<=n;++m)
	 res+=m;
printf("\nEl resultado es %lu",res);
getch();
}