#include <conio.h>
#include <bios.h>
#include <alloc.h>
#include <mem.h>
#include <dos.h>

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

unsigned char longsec;
char *buffer,*buffer2,*buffer3;
char cabeceras[]={
"\x0\x0\x9\x2"
"\x0\x0\x8\x2"
"\x0\x0\x7\x2"
"\x0\x0\x6\x2"
"\x0\x0\x5\x2"
"\x0\x0\x4\x2"
"\x0\x0\x3\x2"
"\x0\x0\x2\x2"
"\x0\x0\x1\x2"
"\x0\x0\x0c\x0"
"\x0\x0\x0b\x0"};



;

void main(void)
{

unsigned char cara,pista,sector,error,intentos;

if ((buffer=malloc(8000))==NULL) {cprintf ("error");
											exit(1);}
buffer2=buffer;
buffer3=buffer2;
memset(buffer,0,7999);
cprintf("\r\nIntroduzca el disco original de Carrier Command");
getch();
cprintf("\r\nLeyendo");
for (cara=0;cara!=1;cara++)
  for (pista=0;pista!=1;pista++)
	 for (sector=1;sector!=13;sector++)
	 {error=1;
	  intentos=0;
	  while (error!=0 && intentos!=3){
	  pokeb(0,0x0526,0x0b);
	  error=biosdisk(2,0,cara,pista,sector,1,buffer);
	  if (error!=0) {intentos++; biosdisk(0,0,0,0,0,0,0);}
	  }
	  if (error!=0) cprintf("\r\nEn sector %d Error :%s",
	  sector,erroresbios[errorpun[error]]);
	  buffer+=512;
}
buffer=buffer2;
cprintf("\r\nIntroduzca el disco de copia de Carrier Command y pulse tecla\a");
getch();
getch();
cprintf("\r\nFormateando & poniendo protecci¢n");
intentos=0;
error=1;
while (error!=0 && intentos!=3){
pokeb(0,0x0526,0x0b);
error=biosdisk(5,0,0,0,1,11,cabeceras);
if (error!=0) {intentos++; biosdisk(0,0,0,0,0,0,0);}
};
if (error!=0) cprintf("\r\nError :%s",erroresbios[errorpun[error]]);
for (cara=0;cara!=1;cara++)
  for (pista=0;pista!=1;pista++)
	 for (sector=1;sector!=13;sector++)
	 {error=1;
	  intentos=0;
	  while (error!=0 && intentos!=3){
	  pokeb(0,0x0526,0x0b);
	  error=biosdisk(3,0,cara,pista,sector,1,buffer);
	  if (error!=0) {intentos++; biosdisk(0,0,0,0,0,0,0);}
	  }
	  if (error!=0) cprintf("\r\nEn sector %d Error :%s",
	  sector,erroresbios[errorpun[error]]);
	  buffer+=512;
	 }
cprintf("\r\nOperacion completada");
free(buffer3);
}
