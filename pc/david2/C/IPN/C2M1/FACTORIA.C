// 15 . Programa para resolver un factorial mediante recursividad

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch

void main(void) {
  int n;
  unsigned long factorial(int n);

printf("\nPrograma para resolver factoriales\n"
"n!=  , introduzca el valor de n\n");
scanf("%u",&n);
printf("n!=%lu",factorial(n));
getch();
}

unsigned long factorial(int n)  {

if (n<1) return 1l;
else return (n*factorial(n-1));
}