//Programa 3: ordena datos por insercion
#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch
#include <limits.h> //para INT_MIN


#define MAXDATOS 50

void main(void) {
int datos[MAXDATOS+1],k,ptr,temp;
unsigned int n;

printf("\nN£mero de datos a ordenar(M ximo %d):",MAXDATOS);
do scanf("%u",&n); while (n>MAXDATOS);
for (ptr=1;ptr<=n;++ptr) {
  printf("Dato n£mero %d:",ptr);
  scanf("%d",&datos[ptr]);
  }
datos[0]=INT_MIN; //m¡nimo valor de un int
for (k=2;k<=n;++k)  {
  temp=datos[k];
  ptr=k-1;
  while(temp<datos[ptr])  {
    datos[ptr+1]=datos[ptr];
    ptr--;
    }
  datos[ptr+1]=temp;
  }
printf("\nDatos ordenados (por insercion):\n");
for (ptr=1;ptr<=n;++ptr)
  printf("%d, ",datos[ptr]);
printf("\b\b \n");
getch();
}