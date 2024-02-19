//Ordena una lista de nombres alfabeticamente utilizando punteros
#include <stdio.h> //para printf,scanf
#include <conio.h> //para cgets,getch
#include <string.h> //para strcmp,strcpy
#include <stdlib.h> //para malloc


#define MAXNUM 50
#define MAXLEN 30

void nohaymemoria(void) {
//si no hay memoria
puts("\nNo queda memoria\a");
exit(1);//sale del programa
}

void main(void) {

char **nombres,*aux;
int i,j,n,comps;

printf ("\nCuantos nombres(M ximo %d):",MAXNUM);
do scanf("%u",&n);  while(n>50);
if ((nombres=(char *)calloc(sizeof(char *),n);
for (i=0;i<n;++i) {
  //asigna memoria para poner los nombres
  if ((aux=(char *)malloc(MAXLEN+2))==NULL) nohaymemoria();
  nombres[i]=aux;
  }
comps=0;
for (i=0;i<n;i++)  {
  printf ("\nNombre N.%d(Longitud m xima %d):",i+1,MAXLEN);
  nombres[i][0]=MAXLEN;
  cgets(nombres[i]);
  strcpy(nombres[i],&nombres[i][2]);
  }
for (i=0;i<n;i++)
  for (j=0;j<n-i-1;j++) {
	comps++;
	if (strcmp(nombres[j],nombres[j+1])>0) {
		aux=nombres[j];
		nombres[j]=nombres[j+1];
		nombres[j+1]=aux;
	  }
	}
printf("\nNumero de comparaciones : %d\n",comps);
puts("Elementos ordenados:");
for (i=0;i<n;i++)
  printf ("%s, ",nombres[i]);
printf("\b\b \n");
getch();
 }