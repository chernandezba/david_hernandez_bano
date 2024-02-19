//Programa 1 : Ordena una lista de N n£meros decrecientemente por metodo burbuja
#include <stdio.h>
#include <conio.h>

#define MAXNUM 50

void main(void) {

int nums[MAXNUM],i,j,n,comps,aux;

printf ("\nCuantos numeros(M ximo %d):",MAXNUM);
do scanf("%u",&n);  while(n>50);
comps=0;
for (i=0;i<n;i++)  {
  printf ("Numero N.%d:",i+1);
  scanf("%d",&nums[i]);
  }
for (i=0;i<n;i++)
  for (j=0;j<n-i-1;j++) {
	 comps++;
	 if (nums[j]<nums[j+1]) {
		aux=nums[j];
		nums[j]=nums[j+1];
      nums[j+1]=aux;
      }
    }
for (i=0;i<n;i++)
  printf ("%5d,",nums[i]);
printf("\nNumero de comparaciones : %d",comps);
getch();
 }