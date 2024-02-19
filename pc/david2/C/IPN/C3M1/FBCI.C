#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXIMO 200

/*

FICHERO : FBCI.C

El programa realiza la funci¢n de fibonnaci de forma recursiva o iterativa.
Tiene la siguiente sint xis:

  FBCI <R/I> <num>

		R = Forma recursiva
		I = Forma iterativa.

		num = N£mero de iteracciones.

*/

long fibor(int n);
long fiboi(int n);

int main(int argc,char *argv[]) {

  int ite;
  long resul;

  if (argc<3) {
	 puts("(c) FBCI 1.0. Hecho 18/9/96\n\n"
			"Sint xis :\n\n"
			"\tFBCI <R | I> <num>\n\n"
			"\t R   = Forma recursiva\n"
			"\t I   = Forma iterativa\n"
			"\t num = N£mero de iteracciones"
			);
	 return EXIT_SUCCESS;
	 }
  ite=atoi(argv[2]);
  switch (toupper(argv[1][0])) {
	 case 'R': resul=fibor(ite); break;
	 case 'I': resul=fiboi(ite); break;
	 default:
		puts("Argumento 1§ inv lido. Ponga I o R");
		return EXIT_FAILURE;
	 }
  printf("La resultado es %ld\n",resul);

  return EXIT_SUCCESS;

  }

long fibor(int n) {

  if (n<2) return 1;
  printf("%d\n",n);

  return fibor(n-1)+fibor(n-2);

  }

long fiboi(int n) {

  long f[MAXIMO]= { 1,1 };
  int i;

  for (i=2;i<=n;i++) f[i]=f[i-1]+f[i-2];

  return f[n];

  }