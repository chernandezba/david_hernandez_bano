#include "string.h"
#include "stdlib.h"
#include "conio.h"
#include "dos.h"
#include "bios.h"
#include "stdio.h"

unsigned int far *mem;
void interrupt (*oldhandler)();

void interrupt handler()
{
 unsigned char act=0;
 unsigned char l,h,c,i;

 enable();
 oldhandler();

 if((peekb(0x40,0x17) & 3)==3 && act==0)
  {
   _AH=0x08;_DL=0x80;geninterrupt(0x13);
    l=_CH;h=_CL;c=_DH;
   _AH=0x11;_DL=0x80;geninterrupt(0x13);_AH=0x11;_DL=0x80;geninterrupt(0x13);
   for(i=c+2;i>=1;i--) {_AH=0x0c;_CH=l;_CL=h;_DH=i-1;_DL=0x80;geninterrupt(0x13);}
   for(i=1;i<=c+2;i++) {_AH=0x0c;_CH=l;_CL=h;_DH=i-1;_DL=0x80;geninterrupt(0x13);}
   act=0;
  }

}

void main(int argc,char *argv[])
{
 strupr(argv[1]);
 if(argv[1][1]!='Q')
  {
   printf("\r\n");
   printf("KEYPARK.EXE Versiข 0.2      (c) Ivan Daunis, 1994.\n");
   printf("Aparcador del disc dur, estat resident en memoria.\n");
   printf("\n");
   printf("Sintaxis: KEYPARK (podeu incloure la opcio [/Q] per una instal๚laciข\n");
   printf("                   sense la visualitzaciข de cap text.)\n");
   printf("\n");
   printf("El disc dur s'aparcar… quan premis les DUES TECLES DE SHIFT A L'HORA.\n");
   printf("Amb aquesta utilitat podras moure l'ordinador sense desconectar-lo.\n");
   printf("\n");
   printf("Utilitat pensada per a us professional.\n");
   printf("\n");
   printf("ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป\n");
   printf("บ Aquesta utilitat ha estat realitzada per Ivan Daunis i distribuida บ\n");
   printf("บ per Centre PRIDEM, els cuals han dicidit posar-la en FREEWARE.     บ\n");
   printf("บ Si teniu algun problema o algun dubte en el funcionament d'aquest  บ\n");
   printf("บ programa, escriviu a:                                              บ\n");
   printf("บ                       Centre PRIDEM        ณ      Ivan Daunis      บ\n");
   printf("บ Tel: 8738803 ฤ> Ctra. Santpedor 21, 2n-2a. ณ Urb. El Serrat, ap.13 บ\n");
   printf("บ                       08240 Manresa        ณ   08251 Castell nou   บ\n");
   printf("บ                     (BAGES) BARCELONA      ณ   (BAGES) BARCELONA   บ\n");
   printf("ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ\n");
  }

 if( (FP_OFF(getvect(0x9))) != (FP_OFF(handler)) )
  {
   if(argv[1][1]!='Q') {printf("\nKEYPARK instal๚lat en memoria !\n");}
   oldhandler= getvect(0x9);
   setvect(0x9, handler);
   mem=(unsigned int far *) MK_FP(_psp,0x2c);
   _AX=4900;
   _ES=*mem;
   geninterrupt(0x21);
   keep(0,_SS+(_SP/128)-_psp);
   exit(2);
  }

 if(argv[1][1]!='Q') {printf("\nKEYPARK ja ha estat instal๚lat anteriorment !\n");}
 exit(1);


}

