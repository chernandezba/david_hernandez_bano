// Programa que suma y multiplica dos matrices dadas con punteros

#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch
#include <stdlib.h> //para calloc,exit

#define MAXFIL 20
double *a,*b,*c;
int nfilas,ncols,nmatriz;


void nohaymemoria(void)  {
puts("No hay memoria\a");
exit(1); //sale del programa
}


void leerentrada (double *A,int m,int n)  {

  int fila,col,i;

for (fila=0;fila<m;++fila)  {
  for (col=0;col<n;++col) {
	 printf("Matriz %d,Dato (%d,%d):",nmatriz,fila+1,col+1);
	 i=(fila*n)+col;
	 scanf("%lf",&A[i]);
  //double a[10][20]  a[3][4]=a[(3*20)+4]
  }
}
}

void calcularsuma(double *A,double *B,double *C,int m,int n)  {

  int fila,col,i;

for (fila=0;fila<m;++fila)  {
  for (col=0;col<n;++col)  {
	 i=(fila*n)+col;
	 C[i]=A[i]+B[i];
	 }
}
}

void sacarsalida(double *A,int m,int n)  {

  int fila,col,i;
  struct text_info info;

gettextinfo(&info); //da informaci¢n sobre el ancho,alto,etc. de la pantalla
for (fila=0;fila<m;++fila)  {
  for (col=0;col<n;++col) {
	 i=(fila*m)+col;
	 printf("Elemento (%d,%d):%lg\n",fila+1,col+1,A[i]);
	 if (wherey()==info.screenheight)  {
		printf("Pulse tecla ...");
		getch();
		clrscr();
		}
  }
}
}

void calcularmult(double *a2,double *b2,double *c2,int m,int n)  {

  int fila,col,col2,i,j;
  double sumparc;

//el m‚todo es el siguiente , se multiplica cada fila de a2 por cada columna
//de b2
/*
Ú            ¿     Ú           ¿    Ú            ¿
³ 1  2  4  5 ³  *  ³ 1   0   4 ³    ³ 31  -2  43 ³
³ 0 -1  3  2 ³     ³ 4   1   1 ³  = ³  9  -4  18 ³
À		    	 Ù 	 ³ 3  -1   3 ³    À		    	 Ù
						 ³ 2   0   5 ³
						 À           Ù
(fila,columna)
31=1*1+2*4+4*3+5*2    ((1,1)*(1,1)) + ((1,2)*(2,1)) + ...
-2=1*0+2*1+4*(-1)+5*0 ((1,1)*(1,2)) + ((1,2)*(2,2)) + ...
43=1*4+2*1+4*3+5*5
9=0*1+(-1)*4+3*3+2*2
-4=0*0+(-1)*1+3*(-1)+2*0
18=0*4+(-1)*1+3*3+2*5
*/

for (fila=0;fila<m;++fila)
  for (col=0;col<n;++col) {
	 for (sumparc=0,col2=0;col2<n;++col2)  {
		  i=(fila*m)+col2;
		  j=(col2*n)+col;
		  sumparc+=a2[i]*b2[j];
		  }
	 i=(fila*n)+col;
	 c2[i]=sumparc;
  }
}





void main(void)  {

puts("Programa que suma y multiplica dos matrices nxn dadas\n");
puts("Nota:Por ser matriz cuadrada el n£mero de filas ser  igual al n£mero de columnas");
do {
  printf("¨Cuantas filas (m ximo %d)?\n",MAXFIL);
  scanf("%d",&nfilas);
  }while(nfilas>MAXFIL);
ncols=nfilas;
//le asigna memoria a las 3 matrices que se utilizar n
if ((a=(double *)calloc(nfilas*ncols,sizeof(double)))==NULL) nohaymemoria();
//calloc(n£mero de elementos,longitud de elemento
if ((b=(double *)calloc(nfilas*ncols,sizeof(double)))==NULL) nohaymemoria();
if ((c=(double *)calloc(nfilas*ncols,sizeof(double)))==NULL) nohaymemoria();
nmatriz=1;
printf("\n\nprimera tabla:\n");
leerentrada(a,nfilas,ncols);
nmatriz=2;
printf("\n\nsegunda tabla:\n");
leerentrada(b,nfilas,ncols);
calcularsuma(a,b,c,nfilas,ncols);
puts("\n\nSumas de los elementos:\n");
sacarsalida(c,nfilas,ncols);
getch();
puts("\n\nMultiplicaci¢n de los elementos:\n");
calcularmult(a,b,c,nfilas,ncols);
sacarsalida(c,nfilas,ncols);
getch();
}

