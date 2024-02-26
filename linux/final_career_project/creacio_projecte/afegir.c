//fitxer afegir.c
//permet d'afegir paraules o urls al servidor proxy.
//simplement el que fa es connectar-se al socket local (UNIX socket) del
//proxy i li passa les paraules o urls que es volen afegir.

#include "servidor.h"
#include "afegir.h"
#include "utils.h"
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>



//variables globals--------------------------------------------------------
int sidebug=0;
FILE *ffitxer=NULL;
int socket_local=0;
FILE *fsocket_local=NULL;


//funcions----------------------------------------------------------------











//funcio local
//envia una cadena a un handle tipus FILE * afegint \n al final
//paràmetres:
//   ffile: FILE * al qual enviar la cadena
//   pcadena: cadena a enviar
static int envia_cadena(FILE *ffile,char *pcadena) {

  fprintf(ffile,"%s\n",pcadena);
  return ferror(ffile);

}





//funcio que es crida quan es surt de main amb return o quan es crida a exit
//es crida abans de finalitzar el programa
void en_sortir(void) {


  if (sidebug)
    fputs("Sortint de programa\n",stderr);
  //tenquem fitxer
  if (ffitxer!=NULL) fclose(ffitxer);
  //tenquem socket
  if (fsocket_local!=NULL) {
    fclose(fsocket_local);
  }
  else {
    if (socket_local)
      close(socket_local);
  }
}







//funció que es crida quan es rep SIGINT(CTRL-C) o SIGQUIT
void ctrl_break(int psignal) {


if (psignal==SIGINT) {
    fputs("Rebut senyal INT\n",stderr);
}
else {
    if (psignal==SIGQUIT) {
        fputs("Rebut senyal QUIT\n",stderr);
    }
    else {
        if (psignal==SIGTERM) {
            fputs("Rebut senyal TERM\n",stderr);
        }
    }
}

 exit(1);
}







//funció principal

int main(int argc,char **argv) {

  int nomesparametres=0;
  int afegirparaules=0;
  int afegirurls=0;
  int especificafitxer=0;
  char nomfitxer[512];
  char buffer[LONG_MAX_PETICIO_HTTP];
  struct sockaddr_un pnom;

  //possa capçalera
  fputs(NOM_AFEGIR" Versió "VERSIO_AFEGIR" 17/5/2005    Autor:David Hernandez "
	"Baño\n",stderr);
  //llegeix paràmetres
do
  switch(getopt(argc,argv,"hvpuf:")) {
  case '?':
    //s'ha possat una opció no suportada
    fprintf(stderr,"Opció %c no reconeguda\n",optopt);
  case 'h':
    //-h
    fputs("Paràmetres possibles:\n",stderr);
    fputs("  -h: Mostra aquest missatge d'ajuda\n",stderr);
    fputs("  -v: Mostra missatges opcionals de depurat\n",stderr);
    fputs("  -p: Especifica que s'afegeixen paraules\n",stderr);
    fputs("  -u: Especifica que s'afegeixen urls\n",stderr);
    fputs("  -f [nomfitxer]: Especifica el nom del fitxer que té les paraules "
          "o urls a afegir\n",stderr);
    fputs("Si no s'especifica el nom del fitxer s'agafarà l'entrada estandard "
          "per llegir les paraules o urls a afegir\n",stderr);
    return 0;
  case 'f':
    // -f
    nomfitxer[sizeof(nomfitxer)-1]=0;
    //copiem el nom del fitxer
    strncpy(nomfitxer,optarg,sizeof(nomfitxer)-1);
    especificafitxer=1;
    break;
  case 'p':
    //-p
    if (afegirurls==1) {
      //s'ha especificat també afegir urls
      fputs("\nNomés pots especificar o afegir paraules o urls\n",stderr);
      return 1;
    }
    afegirparaules=1;
    break;
  case 'u':
    //-u
    if (afegirparaules==1) {
      //s'ha especificat també afegir paraules
      fputs("\nNomés pots especificar o afegir paraules o urls\n",stderr);
      return 1;
    }
    afegirurls=1;
    break;
  case 'v':
    //-v
    sidebug=1;
    break;
  case -1:
    nomesparametres=1;
  }while (!nomesparametres);

 if (!afegirparaules && !afegirurls) {
   //no s'ha especificat si s'afegeixen urls o paraules
   fputs("S'ha d'especificar si s'afegeixen paraules o urls\n",stderr);
   return 1;
 }
 //definim signal traps, si es rep SIGINT , SIGQUIT o SIGTERM es crida a ctrl_break
 signal(SIGINT,ctrl_break);
 signal(SIGQUIT,ctrl_break);
 signal(SIGTERM,ctrl_break);
 signal(SIGPIPE,SIG_IGN); //s'ignora senyal SIGPIPE
 //definim handler de sortida
 if (atexit(en_sortir)) {
  fputs("No es pot registrar handler de sortida\n",stderr);
  return 1;
 }
 if (sidebug)
   fputs("Activats missatges de depurat\n",stderr);
 if (especificafitxer==0) {
   //si no s'ha especificat el fitxer a usar
   //s'usa l'entrada estandard
   ffitxer=stdin;
   if (sidebug)
     fputs("Usant entrada estandard\n",stderr);
 }
 else {
   //obrim fitxer
   if (sidebug)
     fprintf(stderr,"Obrint %s\n",nomfitxer);
   if ((ffitxer=fopen(nomfitxer,"r"))==NULL) {
     //error obrint fitxer
     fputs("Error obrint fitxer :",stderr);
     posa_error(errno);
     return -errno;
   }
 }
 //ja hem obert el fitxer
 if (sidebug)
   fputs("Creant socket local\n",stderr);
 if ((socket_local=socket(PF_LOCAL,SOCK_STREAM,0))==-1) {
   //error creant el socket local
   fputs("Error creant el socket\n",stderr);
   return -1;
 }
 if (sidebug)
   fputs("Creat\n",stderr);
 //preparem nom del socket    
 pnom.sun_family=AF_LOCAL;
 strcpy(pnom.sun_path,NOM_SOCKET_LOCAL);
 if (sidebug)
   fprintf(stderr,"Socket local amb nom de fitxer:%s\n",NOM_SOCKET_LOCAL);
 //ens connectem
 if (connect(socket_local,(struct sockaddr *)&pnom,sizeof pnom)!=0) {
   //n'hi ha hagut error de connexió
   fputs("Error de connexió al socket local del servidor:",stderr);
   posa_error(errno);
   return -errno;
 }
 if (sidebug)
   fputs("Connectat amb el socket local del servidor\n",stderr);
 //ara creem fsocket_local a partir de socket_local
 if ((fsocket_local=fdopen(socket_local,"w"))==NULL) {
   //n'hi ha hagut error en fdopen
   fputs("Error en fdopen:",stderr);
   posa_error(errno);
   return -errno;
 }
 //ja ens hem connectat amb el socket local del servidor
 //enviem primera linia
 if (afegirparaules==1) {
   fputs("Enviant paraules\n",stderr);
   if (envia_cadena(fsocket_local,CONTINGUT_PARAULES)) {
     fputs("Error enviant linia :",stderr);
     posa_error(errno);
     return -errno;
   }
 }
 if (afegirurls==1) {
   fputs("enviant urls\n",stderr);
   if (envia_cadena(fsocket_local,CONTINGUT_URLS)) {
     fputs("Error enviant linia :",stderr);
     posa_error(errno);
     return -errno;
   }
 }
 //mentres quedin linies o urls en el fitxer
 while(!feof(ffitxer)) {
   buffer[sizeof(buffer)-1]=0;
   if (fgets(buffer,sizeof(buffer)-1,ffitxer)==NULL) {
     int numerr=errno;
     //n'hi ha hagut error llegint fitxer
     if (numerr==ESPIPE) 
       //s'ha tencat la connexió
       //això pot ser eof en entrada estandard
       //sortim del bucle
       break;
     fputs("Error llegint linia :",stderr);
     posa_error(errno);
     return -errno;
  }
   //treiem crlf
   treu_crlf(buffer);
   if (sidebug)
     fprintf(stderr,"%s\n",buffer);
   //enviem linia al socket local
   if (envia_cadena(fsocket_local,buffer)) {
     //n'hi ha hagut error
     fputs("Error enviant linia :",stderr);
     posa_error(errno);
     return -errno;
   }
 }
 fflush(fsocket_local); //flushem buffer
 fputs("Finalitzat l'enviament\n",stderr);
 return 0;
}
