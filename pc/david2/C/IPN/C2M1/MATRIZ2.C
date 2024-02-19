// 19.Programa que multiplica dos matrices dadas

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch

#define MAXFIL 23
#define MAXCOL (80/10)

float a[MAXFIL][MAXCOL],b[MAXFIL][MAXCOL],c[MAXFIL][MAXCOL],
c[MAXFIL][MAXCOL];

void main(void)  {

  int nfilas,ncols,ncols2;
  void leerentrada (float a[][MAXCOL],int nfilas,int ncols);
  void calcularmult(float a[][MAXCOL],float b[][MAXCOL],float c[][MAXCOL],
  int nfilas,int ncols);
  void sacarsalida(float c[][MAXCOL],int nfilas,int ncols);

printf("\n\nprimera tabla:\n");
printf("\n¨Cuantas filas?");
scanf("%d",&nfilas);
printf("¨Cuantas columnas?");
scanf("%d",&ncols);
leerentrada(a,nfilas,ncols);
printf("\n\nsegunda tabla:\n");
printf("¨Cuantas columnas?");
scanf("%d",&ncols2);
leerentrada(b,ncols,ncols2);
calcularmult(a,b,c,nfilas,ncols);
printf("\n\nMultiplicaci¢n de las 2 matrices:\n\n");
sacarsalida(c,nfilas,ncols2);
getch();
}

void leerentrada (float a2[][MAXCOL],int m,int n)  {

  int fila,col;

for (fila=0;fila<m;++fila)  {
  printf("\nIntroducir datos para la fila n§ %2d\n",fila+1);
  for (col=0;col<n;++col) scanf("%f",&a2[fila][col]);
  }
}

void calcularmult(float a2[][MAXCOL],float b2[][MAXCOL],float c2[][MAXCOL],
int m,int n)  {

  int fila,col,col2,sumparc;

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
	 for (sumparc=0,col2=0;col2<n;++col2)
		  sumparc+=a2[fila][col2]*b2[col2][col];
	 c2[fila][col]=sumparc;
  }
}

void sacarsalida(float a2[][MAXCOL],int m,int n)  {

  int fila,col;

for (fila=0;fila<m;++fila)  {
  for (col=0;col<n;++col)
	 printf("%9g ",a2[fila][col]);
  printf("\n");
}
}