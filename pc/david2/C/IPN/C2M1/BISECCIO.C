//Programa 6:Programa que resuelve formula por algoritmo de bisecci¢n
#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch
#include <math.h> //para pow


#define MAXGRADO 20

long double coefs[MAXGRADO+1];
unsigned int gradomax;

long double resuelveformula(long double *coef,unsigned int gradmax,
long double x)  {
long double resultado=0.0;
unsigned int grado;

for (grado=0;grado<=gradmax;grado++)  {
  if (!grado) resultado=coef[0];else
  resultado+=coef[grado]*pow(x,grado);
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

long double biseccion(long double a,long double b,long double tol,
unsigned int niter)  {
long double resultado,p;
unsigned int i;

i=1;
while(i<niter)   {
  p=(a+b)/2;
  resultado=resuelveformula(coefs,gradomax,p);
  if (fabsl(resultado)<=tol) return p;
  printf("a=%Lg  b=%Lg  p=%Lg  f(%Lg)=%Lg    i=%u\n",a,b,p,p,resultado,i);
  i++;
  if (resuelveformula(coefs,gradomax,a)*resultado>0.0)
	 a=p; else b=p;
  }
return _LHUGE_VAL;
}



void main(void) {
long double a,b,tol,resultado;
unsigned int x,niter;

puts("\nPrograma para resolver una ecuaci¢n mediante algoritmo bisecci¢n");
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
"Introduzca dos extremos en donde en un lado la f¢rmula resulte negativa y\n"
"por el otro lado positiva separados por espacios , los n£meros pueden estar\n"
"escritos en coma flotante");
scanf("%Lf %Lf",&a,&b);
puts("Introduzca la tolerancia");
scanf("%Lf",&tol);
puts("Introduzca el n£mero de iteraciones m ximo que puede hacer el programa\n"
"M ximo 65534");
do scanf("%u",&niter); while(niter>65534);
resultado=biseccion(a,b,tol,niter);
if (resultado==_LHUGE_VAL) puts("No he podido encontrar una soluci¢n\a");
else printf("La soluci¢n es p=%Lg\n",resultado);
getch();
}