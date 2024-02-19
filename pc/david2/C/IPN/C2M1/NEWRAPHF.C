//Programa 9:Programa que resuelve f¢rmulas definidas por Newton-Raphson

#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch
#include <math.h> //para pow

char *funcs[]={
"x - cos(x) = 0","x^3 + 3x^2 - 1 = 0"};
//funcs es el arreglo de strings que contiene las funciones a evaluar
int puntos[2]={
0,-1};  // es donde se almacenan los puntos
unsigned int i;

#define TOL 10e-4


void esperatecla(void) {
//imprime el mensaje de pulse de tecla y espera hasta que se pulse una tecla
puts("Pulse tecla...");
getch();
}

long double f(long double x,int func) {
//el par metro func es la funci¢n que toma en cuenta
if (!func) return (x-cosl(x));
if (func==1) return (pow(x,3)+(3*pow(x,2))-1);
}




long double f_(long double x,int func)  {
//el par metro func es la funci¢n que toma en cuenta
//resuelve f'(x)
if (!func) return (1+sinl(x));
if (func==1) return ((3*pow(x,2))+(6*x));
}

long double newton_raphson(long double p0,int ecuacion) {

long double p;

i=1;
while((long)i<=65534l) {
p=p0-(f(p0,ecuacion)/f_(p0,ecuacion));
if (fabsl(p-p0)<TOL) return p;
i++;
p0=p;
}
return _LHUGE_VAL;
}

void main(void) {
long double res;
int n;

puts("Programa que resuelve ecuaciones ya dadas por el m‚todo de Newton-Raphson");
for (n=0;n<=1;n++) {
  esperatecla();
  printf("Ecuaci¢n : %s",funcs[n]);
  res=newton_raphson(puntos[n],n);
  if (res==_LHUGE_VAL) puts("\nNo he econtrado soluci¢n\a");
  else printf("\nLa soluci¢n es x=%Lg y la he encontrado en %u iteracciones\n",
  res,i);
  }
esperatecla();
}
