#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <stdlib.h>
#include <dos.h>
#include <ctype.h>

char errorpun[]={0,2,3,4,8,0x10,0x20,0x40,0x80};
char *erroresbios[]={
"OK",
"MARCA DE DIRECCION NO ENCONTRADA",
"DISCO PROTEGIDO CONTRA ESCRITURA",
"SECTOR NO ENCONTRADO",
"ERROR DMA",
"ERROR CRC",
"ERROR EN EL CONTROLADOR",
"ERROR DE BUSQUEDA",
"TIEMPO EXCEDIDO"};


unsigned char tecla(void)
{
	int key, modifiers;
	unsigned char res;

   /* function 1 returns 0 until a key is pressed */
   while (bioskey(1) == 0);

   /* function 0 returns the key that is waiting */
   key = bioskey(0);
	if (!(key & 0xff)) return (key / 256);
	else {res=(unsigned char) (key & 0xff);
			return(res); }
}


int sibreak (void) {

printf("\nCTRL-BREAK PULSADO , FIN PROGRAMA");
exit(1);
return(0);
}

void main(void)
{
unsigned char cara,pista,sector,error,intentos;
int pistai,sectori,caras,sectorf,pistaf,dondex,dondey,errorp,unit;
char unidad[2];

ctrlbrk(sibreak);
printf("\nVERIFICADOR DE DISCOS");
printf("\n(c) Bit Soft 15/12/1993");
printf("\nDesde pista:");
scanf("%d",&pistai);
printf("Desde sector:");
scanf("%d",&sectori);
printf("Numero de caras:");
scanf("%d",&caras);
printf("Hasta sector:");
scanf("%d",&sectorf);
printf("Hasta pista:");
scanf("%d",&pistaf);
while (!isalpha(unidad[0]) || unidad[1]!=':') {
  printf("Unidad (letra seguida de dos puntos):");
  scanf("%2s",&unidad);
  if (isalpha(unidad[0])) unidad[0]=_toupper(unidad[0]);
  if (!isalpha(unidad[0]) || unidad[1]!=':')  
	  printf ("Unidad incorrecta\n\a");
  }
unit=unidad[0]-'A';
printf("\nIntroduzca el disco en la unidad %s y pulse tecla\n"
"ESC o BREAK salir",unidad);
if (tecla()==27) {
	printf("\nESC PULSADO , FIN PROGRAMA");
	exit(EXIT_SUCCESS);
	};
printf("\nVerificando\n");
dondex=wherex();
dondey=wherey();
intentos=0;
error=1;
while (error!=0 && intentos!=3){
error=biosdisk(4,unit,0,0,1,1,0);
if (error!=0) {intentos++; biosdisk(0,unit,0,0,0,0,0);}
}
if (error!=0) {
printf("Error , no hay disco en la unidad o disco defectuoso\n\a");
exit(EXIT_FAILURE);
}
for (pista=pistai;pista!=pistaf+1;pista++)
  for (sector=sectori;sector!=sectorf+1;sector++)
	 for (cara=0;cara!=caras;cara++)
		{
		gotoxy(dondex,dondey);
		printf("Pista %02d cara %02d sector %02d",pista,cara,sector);
		intentos=0;
		error=1;
		while (error!=0 && intentos!=3){
		  error=biosdisk(4,unit,cara,pista,sector,1,0);
		  if (error!=0) {intentos++; biosdisk(0,unit,0,0,0,0,0);}
		  }
				if (error!=0) {
				printf("   Error :%s",erroresbios[errorpun[error]]);
				if (wherey()==dondey) dondey=wherey();
				if (tecla()==27) {
				printf("\nESC PULSADO , FIN PROGRAMA");
				exit(EXIT_SUCCESS);
				};
		  gotoxy(28,dondey);
		  clreol();};
		  };
};