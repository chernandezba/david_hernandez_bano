// 10. Programa que a trav‚s de un menu suma,resta,divide y multiplica
// con 2 n£meros

#include <stdio.h> // para scanf,printf
#include <conio.h> // para gotoxy,clrscr,getch

void main(void)  {
int o;
float a,b;

do  {
clrscr();
gotoxy(35,3);
printf("MENU");
gotoxy(25,6);
printf("1 - SUMAR");
gotoxy(25,7);
printf("2 - RESTAR");
gotoxy(25,8);
printf("3 - MULTIPLICAR");
gotoxy(25,9);
printf("4 - DIVIDIR");
gotoxy(25,10);
printf("5 - SALIR");
gotoxy(25,12);
printf("ELIJA OPCION:");
scanf("%d",&o);
switch (o)  {
  case 1:clrscr();
			printf("Introduzca los dos n£meros a sumar separados por espacios\n");
			scanf("%f %f",&a,&b);
			printf("%g + %g = %g",a,b,a+b);
			getch();
			break;
  case 2:clrscr();
			printf("Introduzca los dos n£meros a restar separados por espacios\n");
			scanf("%f %f",&a,&b);
			printf("%g - %g = %g",a,b,a-b);
			getch();
			break;
  case 3:clrscr();
			printf("Introduzca los dos n£meros a multiplicar separados por espacios\n");
			scanf("%f %f",&a,&b);
			printf("%g * %g = %g",a,b,a*b);
			getch();
			break;
  case 4:clrscr();
			printf("Introduzca los dos n£meros a dividir separados por espacios\n");
			scanf("%f %f",&a,&b);
			if (b==0.0) printf("No se puede dividir un n£mero entre 0\a\n");
			else
			printf("%g / %g = %g",a,b,a/b);
			getch();
			break;
  }
}while(o!=5);
clrscr();
}