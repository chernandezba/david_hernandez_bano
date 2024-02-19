//Programa 5:busqueda binaria
#include <stdio.h> //para printf,scanf,puts
#include <conio.h> //para getch
#define MAX 50

int busquedabin(int *datos,int li,int ls, int elemento) {
//los busca ordenados de mayor a menor
int mit;

mit=(li+ls)/2;
while(li<=ls && datos[mit]!=elemento)  {
  if (elemento>datos[mit]) ls=mit-1; else li=mit+1;
  mit=(li+ls)/2;
  }
if (li<=ls) return mit; else return -1;
}

void main(void) {
int datos[MAX],lug,dato,i,j,aux;
unsigned int n;

printf("\nArreglo de cuantos n£meros(M ximo %d):",MAX);
do scanf("%u",&n); while (n>MAX);
for (lug=0;lug<n;++lug) {
  printf("Dato n£mero %d:",lug);
  scanf("%d",&datos[lug]);
  }
puts("Ordenando...");
//los ordena de mayor a menor
for (i=0;i<n;i++)
  for (j=0;j<n-i-1;j++) {
    if (datos[j]<datos[j+1]) {
		aux=datos[j];
      datos[j]=datos[j+1];
		datos[j+1]=aux;
		}
    }
puts("Ordenado");
puts("Arreglo ordenado:");
for (lug=0;lug<n;++lug)
  printf("%5d, ",datos[lug]);
printf("\nDato a buscar :");
scanf("%d",&dato);
lug=busquedabin(datos,0,n-1,dato);
if (lug==-1) printf("\nNo he encontrado el dato\a\n");
else printf("\nHe encontrado el dato en la posici¢n %d\n",lug);
getch();
}