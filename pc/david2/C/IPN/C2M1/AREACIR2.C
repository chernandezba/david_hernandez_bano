/* 7.Programa para calcular el area de 1 c¡rculo mediante
una funci¢n*/

#include <stdio.h> // printf,scanf
#include <conio.h> // para getch
#include <math.h> // para M_PI



void main(void) {

float radio,area;
float procesar (float radio);

printf("\nRadio =");
scanf("%f",&radio);
area=procesar(radio);
printf("Area = %g\n",area);
getch();
}

float procesar (float radio) {

return M_PI * radio * radio;
}