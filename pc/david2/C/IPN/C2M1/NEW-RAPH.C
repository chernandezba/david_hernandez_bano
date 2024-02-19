//Programa 8:Programa que resuelve una f¢rmula por Newton-Raphson

#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch
#include <math.h> //para pow


#define MAXGRADO 20

long double coefs[MAXGRADO+1];
unsigned int gradomax;
unsigned int i;

long double f(long double *coef,unsigned int gradmax,
long double x)  {
long double resultado=0.0;
unsigned int grado;

for (grado=0;grado<=gradmax;grado++)  {
  if (!grado) resultado=coef[0];else
  resultado+=coef[grado]*pow(x,grado);
  }
return resultado;
}
long double f_(long double *coef,unsigned int gradmax,
long double x)  {
//resuelve f'(x)
long double resultado=0.0;
unsigned int grado;

for (grado=1;grado<=gradmax;grado++)  {
  if (grado==1) resultado=coef[1];else
  resultado+=coef[grado]*grado*pow(x,(grado-1));
  }
return resultado;
}

void imprimeformula(long double *coef,unsigned int gradmax)  {
unsigned int n;

for (n=0;n<=gradmax;n++)   {
  if (n) printf("%LgX^%d + ",coef[n],n);
  else printf("%Lg + ",coef[0]);
  }
printf("\b\b = 0");
}

long double newton_raphson(long double p0,long double tol,
unsigned int niter) {

long double p;

i=1;
while(i<=niter) {
p=p0-(f(coefs,gradomax,p0)/f_(coefs,gradomax,p0));
if (fabsl(p-p0)<tol) return p;
i++;
p0=p;
}
return _LHUGE_VAL;
}

void main(void) {
long double a,tol,resultado;
unsigned int x,niter;

puts("\nPrograma para resolver una ecuaci¢n mediante algoritmo de Newton-Raphson");
printf("M ximo grado de la ecuaci¢n(M ximo %d):",MAXGRADO);
do scanf("%u",&gradomax); while(gradomax>MAXGRADO);
for (x=0;x<=gradomax;x++) {
  printf((x) ? "Coeficiente de X^%d:" :
  "Coeficiente del t‚rmino independiente:",x);
  scanf("%Lf",&coefs[x]);
  }
puts("");
imprimeformula(coefs,gradomax);
puts("\n"
"Introduzca un valor donde la funci¢n est‚ definida");
scanf("%Lf",&a);
puts("Introduzca la tolerancia");
scanf("%Lf",&tol);
puts("Introduzca el n£mero de iteraciones m ximo que puede hacer el programa\n"
"M ximo 65534");
do scanf("%u",&niter); while(niter>65534);
resultado=newton_raphson(a,tol,niter);
if (resultado==_LHUGE_VAL) puts("No he podido encontrar una soluci¢n\a");
else printf("La soluci¢n es p=%Lg y la encontrado en %u iteracciones",resultado,i);
getch();
}
