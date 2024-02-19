//Programa 7:Programa que resuelve f¢rmulas determinadas por algoritmo
//de bisecci¢n
#include <math.h> //para cosl,pow,M_E
#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch

long double f(long double x,int func) {
//el par metro func es la funci¢n que toma en cuenta
if (!func) return (x-powl(2,-x));
if (func==1) return (powl(M_E,x)+powl(2,-x)+(2*cosl(x))-6); //M_E es la constante e
if (func==2) return (powl(M_E,x)-powl(x,2)+(3*x)-2);
}

char *funcs[]={
"X - 2^x = 0","e^x + 2^(-x) + 2cos(x)-6=0","e^x - x^2 + 3x - 2 = 0"};
//funcs es el arreglo de strings que contiene las funciones a evaluar
int intervalos[][2]={
0,1,
1,2,
0,1};  // es donde se almacenan los intervalos

#define TOL 1e-5
unsigned int i;

long double biseccio(long double a,long double b,int func) {

long double p,fp;

i=1;
while(i<=65534) {
p=(a+b)/2;
fp=f(p,func);
if (fabsl(fp)<TOL) return p;
printf("i=%u  ",i);
i++;
if (f(a,func)*fp>0) a=p; else b=p;
printf("f(p)=%Lg                      \r",fp,i);
}
if (i==65535) return _LHUGE_VAL;
}

void main(void) {
int n;
long double res;

printf("Programa que resuelve ecuaciones ya dadas por el m‚todo de bisecci¢n\n");
getch();
for (n=0;n<=2;n++) {
  printf("Ecuaci¢n : %s\n",funcs[n]);
  res=biseccio(intervalos[n][0],intervalos[n][1],n);
  if (res==_LHUGE_VAL) puts("\nNo he econtrado soluci¢n\a");
  else printf("\nLa soluci¢n es P=%Lg y la he encontrado en %u iteracciones\n",
  res,i);
  getch();
  }
}