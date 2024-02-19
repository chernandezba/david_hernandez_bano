// 22.Programa que calcula n n£meros primos
#include <stdio.h> //para printf,scanf
#include <conio.h> //para clrscr,gettextinfo,getch,wherey
//gettextinfo (para saber el n£mero de lineas de la computadora)

void main(void) {

unsigned int n,m,cont,primo;
struct text_info info;

printf("\nIntroduce hasta que n£mero quieres saber si es primo\n");
scanf("%u",&n);
if(!n) return;
clrscr();
gettextinfo(&info);
printf("N£meros primos :\n");
printf("1,\t");
for (cont=2;cont<=n;++cont)  {
  primo=1;
  for (m=2;m<cont;++m)
	 if (!(cont %m)) {
		primo=0;
		break;
	 }
  if (primo) printf("%u,\t",cont);
  if (wherey()==info.screenheight)  {
	 printf("Pulse tecla ...");
	 getch();
	 clrscr();
	 printf(" ");
	 }
  }
printf("\n");
getch();
}