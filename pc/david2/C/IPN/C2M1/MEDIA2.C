// 5. Programa que hace la media de n listas y la
// media de todas las listas en conjunto

#include <stdio.h> // para printf,scanf
#include <conio.h> // para getch

void main(void) {
int a,b,c,d;
float total,temp,total2;

printf("¨Cuantas listas para hacer las medias?\n");
scanf("%d",&a);
total2=0.0;
for (c=1;c<=a;c++) {
  total=0.0;
  printf("La lista %d , ¨de cuantos datos?\n",c);
  scanf("%d",&b);
  for (d=1;d<=b;d++)  {
	 printf("Lista %d , dato %d\n",c,d);
	 scanf("%f",&temp);
	 total+=temp;
	 }
  printf("La media de la lista %d es %g\n",c,total/b);
  total2+=total/b;
}
printf("\n\nLa media de todas las listas es : %g\n",total2/a);
getch();
}