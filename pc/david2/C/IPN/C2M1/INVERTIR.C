// 13.Programa que invierte una cadena

#include <stdio.h> // para gets,printf
#include <conio.h> // para getch

void main(void)  {
char cadena[80];
int n;

printf("\nIntroduce una cadena\n");
gets(cadena);
n=0;
while(cadena[n++]!=0);
--n;
printf("La cadena invertida es\n");
while(n-->0)  printf("%c",cadena[n]);
printf("\n");
getch();
}