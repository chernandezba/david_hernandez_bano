//Programa 2.Ordena una lista de nombres alfabeticamente
#include <stdio.h> //para printf,scanf
#include <conio.h> //para cgets,getch
#include <string.h> //para strcmp,strcpy


#define MAXNUM 50
#define MAXLEN 30

void main(void) {

char noms[MAXNUM][MAXLEN],aux[MAXLEN+2];
int i,j,n,comps;

printf ("\nCuantos nombres(M ximo %d):",MAXNUM);
do scanf("%u",&n);  while(n>50);
comps=0;
for (i=0;i<n;i++)  {
  printf ("\nNombre N.%d(Longitud m xima %d):",i+1,MAXLEN);
  aux[0]=MAXLEN;
  cgets(aux);
  strcpy(&noms[i][0],&aux[2]);
  }
for (i=0;i<n;i++)
  for (j=0;j<n-i-1;j++) {
    comps++;
    if (strcmp(&noms[j][0],&noms[j+1][0])>0) {
		strcpy(aux,&noms[j][0]);
		strcpy(&noms[j][0],&noms[j+1][0]);
		strcpy(&noms[j+1][0],aux);
      }
    }
printf("\nNumero de comparaciones : %d\n",comps);
for (i=0;i<n;i++)
  printf ("%s, ",noms[i]);
printf("\b\b \n");
getch();
 }