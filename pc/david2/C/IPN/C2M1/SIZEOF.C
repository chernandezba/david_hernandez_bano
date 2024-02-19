/* programa que muestra cuanto ocupa en bytes dicha determinada
variable */


#include <stdio.h> // para printf
#include <conio.h> // para getch



void main(void) {
int i;
float f;
double d;
char c;

printf("int : %d\n",sizeof i);
printf("float : %d\n",sizeof f);
printf("double : %d\n",sizeof d);
printf("char : %d\n",sizeof c);
getch();
}