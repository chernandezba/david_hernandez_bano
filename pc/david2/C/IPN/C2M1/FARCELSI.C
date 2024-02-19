//1. Programa que pasa de grados fahrenheit a celsius

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch


void main(void) {
float temp,temp2; // variables

printf("\nIntroduzca la temperatura en grados Fahrenheit\n");
scanf("%f",&temp);
temp2=(5.0/9)*(temp-32);
printf("En grados celsius es :%g\n",temp2);
getch();
}