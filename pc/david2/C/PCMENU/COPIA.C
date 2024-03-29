//copia determinados ficheros creados a partir de la fecha del sistema
//28-3-97
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

int main(int argc,char **argv) {
char buffer[255]; //buffer para obtener informaci�n del pa�s
char commandline[300];//lo que se ejecutar�
struct date datep;
union REGS inregs;
struct SREGS sregs;
int a,b,c,logo;

logo=1;
if (argc>=4) {
  strupr(argv[3]);
  if (!strcmp(argv[3],"/NOLOGO")) logo=0;  //si se pone /nologo no aparece mensaje inicial
  }
if (logo) 
  puts("COPIA (c)28/3/1997 ,copia ficheros usando XCOPY a partir de la fecha del sistema");
if (argc<3) {
  puts("Uso: COPIA [ficheros/s] [destino] /nologo");
  puts("Donde fichero/s puede ser el nombre del fichero o un filtro");
  puts("Como por ejemplo *.EXE que copiaria todos los ejecutables");
  puts("En fichero/s se puede incluir una ruta");
  puts("Si se pone /nologo no aparece el mensaje inicial");
  return 1;
  }
inregs.h.ah=0x38; //obtener/establecer informaci�n sobre el pa�s
inregs.h.al=0x00; //obtener
inregs.x.dx=FP_OFF(&buffer);
sregs.ds=FP_SEG(&buffer);
intdosx(&inregs,&inregs,&sregs);
getdate(&datep);
if (*buffer==1 || *buffer==2) //si el mes va enmedio
        b=datep.da_mon;
        else
        b=datep.da_day;
if (*buffer!=2) //si el a�o va al final
        c=datep.da_year-1900;
        else c=datep.da_day;
if (*buffer==0) a=datep.da_mon;
if (*buffer==1) a=datep.da_day;
if (*buffer==2) a=datep.da_year-1900;
sprintf(commandline,"XCOPY %s %s /D:%02d-%02d-%02d",argv[1],argv[2],a,b,c);
if (system(commandline)==-1) {
  puts("Error al ejecutar XCOPY\a");
  return -1;
  }
return 0;
}
