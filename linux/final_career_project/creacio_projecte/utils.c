//fitxer utils.c

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"


//variables locals-------------------------------------------------------------

static char *errors[]={
  "descriptor invàlid",
  "descriptor no és un socket",
  "no es pot assignar adreça",
  "descriptor no orientat a connexió",
  "Socket reservat o longitud d'adreça invàlida",
  "Connexió tancada",};






//funcions-------------------------------------------------------------------

//converteix l'adreça x.x.x.x a un enter tipus long, compactant-ho tot
unsigned long int iptolong(unsigned char ip0,unsigned char ip1,unsigned char ip2,
unsigned char ip3) {
    //converteix l'adreça x.x.x.x a un enter tipus long, compactant-ho tot

return ((unsigned long int)ip0<<24) +
((unsigned long int)ip1<<16)+
((unsigned long int)ip2<<8)+
((unsigned long int)ip3);
}




//separa adreça IP continguda en un long en els 4 bytes
void longtoip(unsigned long int ip,char *p0,char *p1,char *p2,char *p3) {


*p0=(char)((ip >> 24) & 0x000000ff);
*p1=(char)((ip >> 16) & 0x000000ff);
*p2=(char)((ip >> 8)  & 0x000000ff);
*p3=(char)(ip & 0x000000ff);
}





//posa error corresponent a numerr
void posa_error(int numerr) {
  //posa error depenent de la variable numerr, relacionada con errno
  int numer;

  numer=-1;
  if (numerr==EBADR) numer=0;
  if (numerr==ENOTSOCK) numer=1;
  if (numerr==EADDRNOTAVAIL) numer=2;
  if (numerr==EOPNOTSUPP) numer=3;
  if (numerr==EINVAL) numer=4;
  if (numerr==ESPIPE) numer=5;

  if (numer==-1) {
    //mostra error descriptiu de la llibreria perror
    flockfile(stderr);
    fprintf(stderr,"%s\n",strerror(numerr));
    funlockfile(stderr);
    }
else {
  flockfile(stderr);
  fprintf(stderr,"%s\n",errors[numer]);
  funlockfile(stderr);
  }
}





//afegeix a url especificació del port si aquest no és el 80
//ex:si port=88, de www.miweb.com/algo.htm es pasa a
//www.miweb.com:88/algo.htm
//
//parámetres:
//  purl:url a afegir el port si fa falta, no s'ha de posar l'http://
//  pport:port de connexió
//  purlambport:cadena de sortida amb el numero de port posat si aquest
//  no és el 80
void afegeixportaurl(char *purl,unsigned int pport,char *purlambport) {

  char *pt,psal;

if (pport==80) {
  //si es el port 80, no s'afegeix a url
  strcpy(purlambport,purl);
  return;
}
pt=strchr(purl,'/');
if (pt==NULL) {
  //si no ha trobat el / es que no hi es
  sprintf(purlambport,"%s:%u",purl,pport);
}
else {
  psal=*pt;
  *pt=0;
  sprintf(purlambport,"%s:%u%s",purl,pport,(pt+1));
  *pt=psal;
}
}








//converteix una cadena a minúscules
//paràmetre d'entrada:
//  pcad:cadena a convertir a minúscules
void aminuscules(char *pcad) {
  char *pt;

  pt=pcad;
  while ((*pt)!=0) {
    //si es un caracter numéric
    *pt=tolower((*pt));
    ++pt;
  }
}




//treu els possibles crlf del final de pcadena
//el paràmetre d'entrada es la cadena de la cual treure el possible crlf del final
void treu_crlf(char *pcadena) {
char *p;

p=strchr(pcadena,0);
if (p!=pcadena) {
    --p;
    if ((*p)=='\xd' || (*p)=='\xa') *p=0;
    if (p!=pcadena) {
        --p;
        if ((*p)=='\xd' || (*p)=='\xa') *p=0;
    }
}
}

