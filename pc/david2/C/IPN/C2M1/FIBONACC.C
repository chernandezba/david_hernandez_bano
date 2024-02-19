// 17.Programa que resuelve fibonacci por recursi¢n

#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch


unsigned long fib(unsigned int n) {

if (n>=0 && n<=1) return 1l;
return (fib(n-1)+fib(n-2));
}

void main(void)  {
unsigned int n;

printf("\nPrograma que calcula la succesi¢n de Fibonacci\n"
"Fib(n) , introduzca el valor de n\n");
scanf("%u",&n);
printf("El resultado es %lu\n",fib(n));
getch();
}