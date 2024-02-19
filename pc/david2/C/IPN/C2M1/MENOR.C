// 9.Programa que imprime el menor de 3 n£meros introducidos

#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch

void main(void)  {
float a,b,c,menor;

printf("\nIntroduzca 3 n£meros separados por espacios\n");
scanf("%f %f %f",&a,&b,&c);
if (a<b && a<c) menor=a;
else if (b<c && b<a) menor=b;
else menor=c;
printf("El menor es %g\n",menor);
getch();
}