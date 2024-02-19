// 4. Programa que hace la media de n n£meros

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch


void main(void) {
int numdatos,i;
float total=0.0,temp;

printf("\nCuantos n£meros para hacer la media:\n");
scanf("%d",&numdatos);
for (i=numdatos-1;numdatos>0;numdatos--)  {
  printf("Quedan %d datos para introducir\n",numdatos);
  scanf("%f",&temp);
  total+=temp;
  }
i++;
printf("La media de los %d n£meros es %g\n",i,total/i);
getch();
}