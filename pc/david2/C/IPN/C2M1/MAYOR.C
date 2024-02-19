// 11. Programa que imprime el mayor de 3 n£meros dados

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch


float mayor(float a,float b,float c)  {
if (a>=b && a>=c) return a;
if (b>=c && b>=a) return b;
return c;
}

void main(void)  {

float a1,b1,c1;

printf("\nIntroduzca 3 n£meros separados por espacios\n");
scanf("%f %f %f",&a1,&b1,&c1);
printf("El mayor de los 3 es %g\n",mayor(a1,b1,c1));
getch();
}