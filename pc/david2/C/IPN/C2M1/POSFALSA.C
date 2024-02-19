//Programa que resuelve formula por algoritmo de la posici¢n falsa
#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch
#include <math.h> //para pow


#define MAXGRADO 20

long double coefs[MAXGRADO+1];
unsigned int gradomax,iteracc;

long double resuelve(long double x,int maxincog,long double *c)  {
int x1;
long double result=0.0;

for (x1=0;x1<=maxincog;++x1) {
  if (x1!=maxincog) result+=c[x1]*powl(x,maxincog-x);
  else result+=c[x1];
  }
return result;
}

void imprimeformula(long double *coef,unsigned int gradmax)  {
unsigned int n;

for (n=0;n<=gradmax;n++)   {
  if (n!=gradmax) printf("%LgX^%d ",fabsl(coef[n]),gradmax-n);
  else printf("%Lg ",fabsl(coef[gradmax]));
  if (n!=gradmax)  {
	  if (coef[n+1]<0.0)
	  printf("- ");
	  else printf("+ ");
  }
  }
printf("= 0");
}

long double posfalsa(long double xi,long double xd,long double tol,
unsigned int niter)  {
long double fi,fd,xm,fm;
//hacer posfalsa


iteracc=1;
while(iteracc<niter)   {
  fi=resuelve(xi,gradomax,coefs);
  fd=resuelve(xd,gradomax,coefs);
  xm=(xi*fd-xd*fi)/(fd-fi);
  fm=resuelve(xm,gradomax,coefs);
  if (fabsl(fm)<=tol || fabsl(xd-xm)<=tol) return xm;
  if (fd*fm>0) xd=xm; else xi=xm;
  iteracc++;
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
  printf((x!=gradomax) ? "Coeficiente de X^%d:" :
  "Coeficiente del t‚rmino independiente:",gradomax-x);
  scanf("%Lf",&coefs[x]);
  }
puts("");
imprimeformula(coefs,gradomax);
puts("\n"
"Introduzca los puntos iniciales , los n£meros pueden estar\n"
"escritos en coma flotante");
scanf("%Lf %Lf",&a,&b);
puts("Introduzca la tolerancia");
scanf("%Lf",&tol);
puts("Introduzca el n£mero de iteraciones m ximo que puede hacer el programa\n"
"M ximo 65534");
do scanf("%u",&niter); while(niter>65534);
resultado=posfalsa(a,b,tol,niter);
if (resultado==_LHUGE_VAL) puts("No he podido encontrar una soluci¢n\a");
else printf("La soluci¢n es p=%Lg\ny la he encontrado en %u iteracciones\n",
resultado,iteracc);
getch();
}