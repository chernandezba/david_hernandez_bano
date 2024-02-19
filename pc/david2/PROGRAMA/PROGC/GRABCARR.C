#include <process.h>
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <stdlib.h>

unsigned segmento,oldds,i16a,i16b,sp2,ss2,es2;
char far *puntero;
int longi;
unsigned long longitud;
FILE *ficheros,*ficheros2;
char *p;
char far *dire,far *dire2,far *dire3;
unsigned ds2;
char bufferfile[32768];

void interrupt rutina() {

disable();
oldds=_DS;
sp2=_SP;
ss2=_SS;
es2=_ES;
dire=MK_FP(peek(_SS,_BP+6),peek(_SS,_BP+4));
dire2=MK_FP(peek(_SS,_BP+2),peek(_SS,_BP+4));
dire3=MK_FP(peek(_SS,_BP+8),peek(_SS,_BP+10));
puntero=MK_FP(segmento++,2);
longi=peek(segmento,2);
longitud=250000L;/*longitud=((long)longi*16L)-256L;*/
enable();
setdisk(1);
fprintf(ficheros,"PSP %p CARRIER %p puntero2 %p puntero3 %p",
puntero,dire,dire2,dire3);
fprintf(ficheros,"\nLongitud %lu SP:%X SS:%X DS:%X ES:%X",longitud,sp2,ss2,oldds,es2);
fflush(ficheros);
segmento++;
fwrite(MK_FP(segmento,0),32768,1,ficheros2);
fflush(ficheros2);
fwrite(MK_FP(segmento,32768),32768,1,ficheros2);
fflush(ficheros2);
segmento++;
fwrite(MK_FP(segmento,0),32768,1,ficheros2);
fflush(ficheros2);
fwrite(MK_FP(segmento,32768),32768,1,ficheros2);
fflush(ficheros2);
segmento++;
fwrite(MK_FP(segmento,0),32768,1,ficheros2);
fflush(ficheros2);
fwrite(MK_FP(segmento,32768),32768,1,ficheros2);
fflush(ficheros2);
segmento++;
fwrite(MK_FP(segmento,0),32768,1,ficheros2);
fflush(ficheros2);
fwrite(MK_FP(segmento,32768),32768,1,ficheros2);
fflush(ficheros2);
fcloseall();
disable();
poke(0,64,i16a);
poke(0,66,i16b);              
enable();
sound(7);
delay(10000);
nosound();
abort();
}

void main (void) {

ds2=_DS;
disable();
i16a=peek(0,64);
i16b=peek(0,66);
poke(0,64,FP_OFF((void *) rutina));
poke(0,66,FP_SEG((void *) rutina));
enable();
setdisk(0);
segmento=_psp;
ficheros=fopen("B:\\CARRIERD.PUN","wt");
ficheros2=fopen("B:\\CARRIERD.EXE","wb");
setvbuf(ficheros2,bufferfile,_IOLBF,32768);
spawnv(P_WAIT,"carrier.exe",NULL);
}