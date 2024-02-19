// 8. Programa compuesto.


#include <stdio.h> // para printf
#include <conio.h> // para getch



void main(void) {
int i;
float f;
double d;
char c;

/*muestra cuanto ocupa en bytes dicha determinada
variable */
printf("int : %d\n",sizeof i);
printf("float : %d\n",sizeof f);
printf("double : %d\n",sizeof d);
printf("char : %d\n",sizeof c);
getch();
// prueba el operador ++ antes de variable
i=1;
printf("\ni=%d\n",i);
printf("i=%d\n",++i);
printf("i=%d\n",i);
// prueba el operador ++ despues de variable
getch();
i=1;
printf("\ni=%d\n",i);
printf("i=%d\n",i++);
printf("i=%d\n",i);
getch();
}