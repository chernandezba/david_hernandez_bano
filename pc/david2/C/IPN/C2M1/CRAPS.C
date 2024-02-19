// 12 . Programa del juego de CRAPS

#include <stdio.h> // para printf,scanf
#include <stdlib.h> // para srand,random
#include <ctype.h> // para randomize
#include <time.h> // para randomize
#include <conio.h> // para getch
#define SEMILLA 12345

int tirada(void);
void juego(void);

void main() {
char respuesta='s';

printf("\nBienvenido al juego de Craps\n\n");
printf("para lanzar los dados oprima RETURN\n\n");
randomize();
/* bucle principal*/
while(toupper(respuesta)!='N') {
  juego();
  printf("\n¨Deseas jugar de nuevo (S/N) ?");
  scanf("%c",&respuesta);
  printf("\n");
  }
printf("Adi¢s , que la pases bien ...\n");
getch();
}



void juego() /* simula jugada completa */
  {
  int puntos1,puntos2;
  char nada;

printf("\npor favor , lanza los dados ...");
scanf("%c",&nada);
printf("\n");
puntos1=tirada();
switch(puntos1)  {
  case 7: /*se ha ganado en la 1¦ tirada */
  case 11:  printf("-Felicidades- ganaste la 1¦ tirada\n");
  break;
  case 2: /* se ha perdido en la 1¦ tirada */
  case 3:
  case 12:printf ("-Lo siento- perdiste en la 1¦ tirada\n");
			 break;
  case 4: case 5: case 6: case 8: case 9:
  case 10: do {
				printf("Lanza los dados de nuevo ...");
				scanf("%c",&nada);
				printf("\n");
				puntos2=tirada();
				}while (puntos2!=puntos1 && puntos2!=7);
				if (puntos2==puntos1)
				  printf("Ganas por igualar tu 1¦ puntuaci¢n\n");
				else printf("pierdes por no igualar tu 1¦ puntuaci¢n\n");
				break;
  }
}


int tirada() /* simula lanzamiento de un par de dados */
  {
  int n1,n2,n3;   /* n£meros aleatorios entre 1 y 6 */

n3=random(5);
while (n3-->=0)  n1=random(6)+1;
n3=random(5);
while (n3-->=0)  n2=random(6)+1;
printf ("Dado 1:%2d Dado 2:%2d , puntuaci¢n total %d\n",n1,n2,n1+n2);
return (n1+n2); /* puntuaci¢n total */
}
/* fin programa */