// 3.Programa que resuelve x=axý+bx+c
// para valores reales y valores imaginarios

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch
#include <math.h> // para sqrt , abs


void main(void) {
float a,b,c,d;

printf("\nDe la f¢rmula X=axý+bx+c , introduzca los valores de \na,b,c"
" separados por espacios:\n");
scanf("%f %f %f",&a,&b,&c);
d=(b*b)-(4*a*c);
if (a==0 && (b!=0 || c!=0))
  printf("\nEl resultado es X=%g",-c/b);
else {
  if (d>=0)
	 printf("\nLa primera soluci¢n es X=%g\nLa segunda soluci¢n es X=%g\n",
	 (-b+sqrt(d))/(2*a),(-b-sqrt(d))/(2*a));
  else
	 printf("\nLa primera soluci¢n es X=%g + %g i\n"
	 "La segunda soluci¢n es X=%g - %g i\n",-b/(2.0*a),sqrt(abs(d))/(2.0*a),
	 -b/(2.0*a),sqrt(abs(d))/(2.0*a));
	 }
getch();
}
