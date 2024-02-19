// 16.Programa para calcular la funci¢n de Ackerman
// mediante recursividad

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch


unsigned int ackerman(unsigned int n,unsigned int m) {
if (!n) return (m+1);
if (!m) return ackerman(n-1,1);
return ackerman(n-1,ackerman(n,m-1));
}

void main(void)  {
unsigned int n,m;

printf("\nPrograma para calcular la funci¢n de Ackerman\n"
"A(n,m) , introduzca los valores de n,m separados por espacios\n");
scanf("%u %u",&n,&m);
printf("El resultado es %u",ackerman(n,m));
getch();
}