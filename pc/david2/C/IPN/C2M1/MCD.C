// 20.Programa que calcula el M ximo Comun Divisor de 2 n£meros

#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch

void main(void)  {

unsigned int m,n,c,res,res2,mcd;

printf(
"\nIntroduzca los valores de M y N para calcular el m ximo comun divisor de\n"
"los dos separados por espacios\n");
scanf("%u %u",&m,&n);
printf("El M ximo Comun Divisor de %u y %u es ",m,n);
if (m<n)  //intercanvia los valores de m y n
  {
  c=n;
  n=m;
  m=c;
  }
if (m%n)  {
  do  {
	 res2=res;
	 res=m%n;
	 m=n;
	 n=res;
	 }while(res);
	 mcd=res2;
  }
  else mcd=n;
printf("%u",mcd);
getch();
}
