// 2.Programa que resuelve x=axý+bx+c para valores reales
// no resulve las de soluci¢n imaginaria


#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch
#include <math.h> // para sqrt


void main(void) {
float x,a,b,c,d;

printf("\nDe la f¢rmula X=axý+bx+c , introduzca los valores de \na,b,c"
" separados por espacios:\n");
scanf("%f %f %f",&a,&b,&c);
d=(b*b)-(4*a*c);
x=(-b+sqrt(d))/(2*a);
printf("\nUna soluci¢n es x=%g",x);
x=(-b-sqrt(d))/(2*a);
printf("\nLa otra soluci¢n es x=%g",x);
getch();
}
