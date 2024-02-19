//Programa 4:Busqueda secuencial
#include <stdio.h> //para printf,scanf
#include <conio.h> //para getch


#define MAX 50

void main(void) {
int datos[MAX],lug,dato;
unsigned int n;

printf("\nArreglo de cuantos n£meros(M ximo %d):",MAX);
do scanf("%u",&n); while (n>MAX);
for (lug=0;lug<n;++lug) {
  printf("Dato n£mero %d:",lug);
  scanf("%d",&datos[lug]);
  }
printf("\nDato a buscar :");
scanf("%d",&dato);
lug=0;
while(datos[lug++]!=dato && lug<n);
--lug;
if (lug>=n) printf("\nNo he encontrado el dato\a\n");
else printf("\nHe encontrado el dato en la posici¢n %d\n",lug);
getch();
}