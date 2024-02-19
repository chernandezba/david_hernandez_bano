// 18.Programa que suma dos matrices dadas

#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch

#define MAXFIL 20
#define MAXCOL 30


void main(void)  {

  int nfilas,ncols;
  int a[MAXFIL][MAXCOL],b[MAXFIL][MAXCOL],c[MAXFIL][MAXCOL];
  void leerentrada (int a[][MAXCOL],int nfilas,int ncols);
  void calcularsuma(int a[][MAXCOL],int b[][MAXCOL],int c[][MAXCOL],
  int nfilas,int ncols);
  void sacarsalida(int c[][MAXCOL],int nfilas,int ncols);


printf("\n¨Cuantas filas?");
scanf("%d",&nfilas);
printf("¨Cuantas columnas?");
scanf("%d",&ncols);
printf("\n\nprimera tabla:\n");
leerentrada(a,nfilas,ncols);
printf("\n\nsegunda tabla:\n");
leerentrada(b,nfilas,ncols);
calcularsuma(a,b,c,nfilas,ncols);
printf("\n\nSumas de los elementos:\n\n");
sacarsalida(c,nfilas,ncols);
getch();
}

void leerentrada (int a[][MAXCOL],int m,int n)  {

  int fila,col;

for (fila=0;fila<m;++fila)  {
  printf("\nIntroducir datos para la fila n§ %2d\n",fila+1);
  for (col=0;col<n;++col) scanf("%d",&a[fila][col]);
  }
}

void calcularsuma(int a[][MAXCOL],int b[][MAXCOL],int c[][MAXCOL],
int m,int n)  {

  int fila,col;

for (fila=0;fila<m;++fila)
  for (col=0;col<n;++col)
	 c[fila][col]=a[fila][col]+b[fila][col];

}

void sacarsalida(int a[][MAXCOL],int m,int n)  {

  int fila,col;

for (fila=0;fila<m;++fila)  {
  for (col=0;col<n;++col)
	 printf("%4d",a[fila][col]);
  printf("\n");
}
}