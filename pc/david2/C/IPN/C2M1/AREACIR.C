/* 6.Programa que calcula el area de 1 c¡rculo*/

#include <stdio.h> // para printf,scanf
#include <math.h> // para M_PI=pi
#include <conio.h> // para getch()



void main(void) {

float radio,area;

printf("\nRadio = ");
scanf("%f",&radio);
area=M_PI * radio * radio;
printf("Area = %g\n",area);
getch();
}