//fitxer historial.c
//crea l'historial a partir del fitxer de dietari
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "historial.h"
#include "servidor.h"
#include "utils.h"
#include "llista_amb_valor.h"





//defines--------------------------------------------------
#define  MAXIM_ELEMENTS_A_MOSTRAR 20 //numéro màxim d'elements a mostrar de les llistes




//variables globals-----------------------------------------------------
int sidebug=0; //especifica si es mostra informacio addicional de depurat
FILE *flog;
struct llista_amb_valor *urls=NULL,*urlsbloq=NULL,*urlsbloqparaula,*planesbloq=NULL,*planes=NULL,*paraulesbloq=NULL;
char *buffer=NULL;





//funcions--------------------------------------------------------------



//funcio que es crida quan es surt de main amb return o quan es crida a exit
//es crida abans de finalitzar el programa
void en_sortir(void) {


  if (sidebug)
    fputs("Sortint de programa\n",stderr);
  //tenquem fitxer
  if (flog!=NULL) fclose(flog);
  //alliberem memòria usada per les llistes
  if (urls!=NULL) destrueix_llista_amb_valor(urls);
  if (planes!=NULL) destrueix_llista_amb_valor(planes);
  if (planesbloq!=NULL) destrueix_llista_amb_valor(planesbloq);
  if (paraulesbloq!=NULL) destrueix_llista_amb_valor(paraulesbloq);
  if (urlsbloq!=NULL) destrueix_llista_amb_valor(urlsbloq);
  //alliberem memòria usada pel buffer
  if (buffer!=NULL) free(buffer);

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







//funcio local, afegeix un element
//paràmetres:
// pllista: apunta a l'adreça del valor de la variable de la llista
// p: element (cadena) a afegir
//valors de retorn:
//  0:ha anat tot be
// -1:error de memòria
static int afegeixelement(struct llista_amb_valor *pllista,char *p) {

  struct element_amb_valor *pelement;
  unsigned long pvegades;
  char tbuffer[32];



  //mirem si ja teniem l'element a la llista
  pelement=cerca_element_amb_valor(pllista,p);
  if (pelement==NULL)
    //si no estava
    pvegades=1;
  else {
    //si estava
    sscanf(retorna_valor(pelement),"%lu",&pvegades);
    ++pvegades;
  }
  //afegim l'element
  tbuffer[sizeof(tbuffer)-1]=0;
  snprintf(tbuffer,sizeof(buffer)-1,"%lu",pvegades);
  if (afegeix_element_amb_valor(pllista,p,tbuffer)!=0) {
    //n'hi ha hagut error afegint element
    return -1;
  }
//si arribem aqui es que tot ha anat be
return 0;
}





//funció per poder ordenar la llista pel valor
//el valor es converteix a enter i es compara
int compara_pel_qsort(const void *pa,const void *pb) {
  struct element_amb_valor *a,*b;
  unsigned long va,vb;

  a=(struct element_amb_valor *)pa;
  b=(struct element_amb_valor *)pb;
  //converteix els valors a enters
  sscanf(retorna_valor(a),"%lu",&va);
  sscanf(retorna_valor(b),"%lu",&vb);
  return (va<vb) - (va>vb);
}






//mostra els n elements de la llista
//paràmetres:
//    pmissatgeinicial:missatge mostrar inicialment
//    pllista: llista a mostrar elements amb valor
//    pnumelements: numero d'elements com a màxim a mostrar de la llista
static void mostraelements(char *pmissatgeinicial,struct llista_amb_valor *pllista,unsigned int pmaxelemsamostrar) {

  struct element_amb_valor *pelact;
  unsigned int i,ii,ttlong,tespais,iii;
  char *tvalor,*telement;
  unsigned long ttvalor;
  char *tseparadors=".-*";
  int tnumseparadors=strlen(tseparadors);

  fprintf(stdout,"%s\n",pmissatgeinicial);
  pelact=recorre_elements_amb_valor(pllista);
  i=0;
  iii=0;
  while (esfinal_amb_valor(pelact)!=1) {
    //mentres n'hi hagi elements per mostrar i no haguem
    //mostrat el màxim d'elements permesos
    tvalor=retorna_valor(pelact);
    sscanf(tvalor,"%lu",&ttvalor);
    telement=retorna_element_del_valor(pelact);
    fputs(telement,stderr);
    //calculem espais a posar
    ttlong=strlen(telement);
    if (ttlong>60) {
      tespais=(ttlong % 5);
      if (tespais==0)
	tespais=5;
    }
    else {
      tespais=60-ttlong;
    }
    //primer possem un espai
    fputc(' ',stdout);
    //possem separador diferent en cada linia
    for (ii=0;ii<tespais;++ii)
      fputc(tseparadors[iii],stdout);
    //possem un altre espai
    fputc(' ',stdout);
    //possem numero de vegades
    fprintf(stdout,"%s vegad%s\n",tvalor,(ttvalor)>1 ? "es" : "a");
    ++i;
    if (i==pmaxelemsamostrar)
      //si ja hem mostrar més elements que el màxim
      break;
    pelact++;
    iii=(iii+1) % tnumseparadors; //actualitzem variable per posar diferents separadors en cada linia
  }
  fputs("\n",stdout);
}








//funcio principal
int main(int argc,char **argv) {
  int nomesparametres=0;
  int especificanomfitxer=0;
  int anteriorllistat;
  int sipa,i,ii;
  char nomfitxer[512];
  char *p;
  char *pplana;
  size_t longbuffer=0;
  char tesperar[]=".-\\|/-";
  char delimitador[]=" ";


 //possa capçalera
  fputs(NOM_HISTORIAL" Versió "VERSIO_HISTORIAL
" 23/5/2005    Autor:David Hernandez Baño\n",stderr);
  //llegeix paràmetres
do
  switch(getopt(argc,argv,"hvf:")) {
  case '?':
    //s'ha possat una opció no suportada
    fprintf(stderr,"Opció %c no reconeguda\n",optopt);
  case 'h':
    //-h
    fputs("Paràmetres possibles:\n",stderr);
    fputs("  -h: Mostra aquest missatge d'ajuda\n",stderr);
    fputs("  -v: Mostra missatges addicionals\n",stderr);
    fputs("  -f [nomfitxer]: Especifica el nom del de dietari \n",stderr);
    fputs("Si no s'especifica el nom del fitxer de dietari s'agafarà el "
    "nom del fitxer de dietari per defecte: "NOM_FITXER_CFG"\n",stderr);
    return 0;
  case 'f':
    // -f
    nomfitxer[sizeof(nomfitxer)-1]=0;
    //copiem el nom del fitxer
    strncpy(nomfitxer,optarg,sizeof(nomfitxer)-1);
    especificanomfitxer=1;
    break;
  case 'v':
    //-v
    sidebug=1;
    break;
  case -1:
    nomesparametres=1;
  }while (!nomesparametres);

 if (especificanomfitxer==0) {
   //no s'ha especificat el nom del fitxer
   //agafarem el nom del fitxer per defecte
   nomfitxer[sizeof(nomfitxer)-1]=0;
   strncpy(nomfitxer,NOM_FITXER_LOG,sizeof(nomfitxer)-1);
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
 if (sidebug) {
   fputs("Obrint fitxer ",stderr);
   fputs(nomfitxer,stderr);
   fputs("\n",stderr);
 }
 if ((flog=fopen(nomfitxer,"r"))==NULL) {
   fputs("No s'ha pogut obrir el fitxer:",stderr);
   posa_error(errno);
   return 1;
 }
 if (sidebug)
   fputs("Obert\n",stderr);
 if(sidebug)
   fputs("Inicialitzant llistes...",stderr);
 if ((urls=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if ((planes=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if ((urlsbloq=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if ((urlsbloqparaula=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if ((planesbloq=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if ((paraulesbloq=inicialitza_llista_amb_valor())==NULL) {
   fputs("Error de memoria",stderr);
   return -1;
 }
 if (sidebug)
   fputs("Inicialitzades\n",stderr);
 fputs("Llegint fitxer de dietari ...",stderr);
 i=0;
 ii=0;
 fputc(tesperar[0],stderr);
 fflush(stderr);
 while (!feof(flog)) {
   //mentres quedi fitxer per llegir
   //llegim linia
   ++ii;
   if (ii==10) {
     ++i;
     if (i==sizeof(tesperar))
       i=0;
     ii=0;
     fputc(tesperar[i],stderr);
     fflush(stderr);
   }
   if (getline(&buffer,&longbuffer,flog)==-1 && !feof(flog)) {
     //error llegint linia
     fputs("Error llegint linia:",stderr);
     posa_error(errno);
     //sortim del bucle
     break;
   }
   //ja hem llegit la linia
   //separem els camps
   p=strtok(buffer,delimitador);
   if (p==NULL)
     //entrada incorrecta, la ignorem
     continue;
   //p=tipus entrada
   sipa=0;
   if (!strcmp(p,URLBLOQ) || (sipa=!strcmp(p,PARAULABLOQ))) {
     //url bloquejada o paraula bloquejada
     //en cas que sigui paraula bloquejada, la url a on esta
     //la paraula bloquejada l'afegim a la llista de urls 
     //bloquejades ja que quan detecta una paraula a la llista
     //de paraules prohibides es com si bloquejes l'accés a la
     //URL
     if (sidebug) {
       fputs(p,stderr);
       fputs("\n",stderr);
     }
     p=strtok(NULL,delimitador);
     //p=URL
     if (p==NULL)
       //entrada incorrecta, la ignorem
       continue;
     if (!sipa) {
       //si no era una paraula bloquejada
       if (afegeixelement(urlsbloq,p)!=0) {
	 //error afegint element
	 fputs("Error afegint element, sense memòria\n",stderr);
	 //sortim
	 return -1;
       }
     }
     else {
       //si era una paraula bloquejada
       if (afegeixelement(urlsbloqparaula,p)!=0) {
	 //error afegint element
	 fputs("Error afegint element, sense memòria\n",stderr);
	 //sortim
	 return -1;
       }
       p=strtok(NULL,delimitador);
       //p=paraula
       if (p==NULL)
	 //entrada incorrecta
	 continue;
       if (afegeixelement(paraulesbloq,p)!=0) {
	 //error afegint element
	 fputs("Error afegint element, sense memòria\n",stderr);
	 //sortim
	 return -1;
       }
     }
     //següent linia
     continue;
   }
   if (!strcmp(p,WEBBLOQ)) {
     //plana bloquejada
     if (sidebug) {
       fputs(URLBLOQ,stderr);
       fputs("\n",stderr);
     }
     p=strtok(NULL,delimitador);
     //p=URL
     if (p==NULL)
       //entrada incorrecta, la ignorem
       continue;
     //ara afegim només la part de la plana web
     pplana=strdup(p);
     //cerquem si n'hi ha /
     p=strchr(pplana,'/');
     if (p!=NULL)
       //si n'hi ha
       *p=0;
     //afegim la plana
     if (afegeixelement(planesbloq,pplana)!=0) {
       //error afegint element
       fputs("Error afegint element, sense memòria\n",stderr);
       //sortim
       return -1;
     }
     //alliberem memòria
     free(pplana);
     //seguent linia
     continue;
   }
   if (!strcmp(p,URLOK)) {
     //url ok
     if (sidebug) {
       fputs(URLOK,stderr);
       fputs("\n",stderr);
     }
     p=strtok(NULL,delimitador);
     //p=URL
     if (p==NULL)
       //entrada incorrecta, la ignorem
       continue;
     if (afegeixelement(urls,p)!=0) {
       //error afegint element
       fputs("Error afegint element, sense memòria\n",stderr);
       //sortim
       return -1;
     }
     //ara afegim només la part de la plana web
     pplana=strdup(p);
     //cerquem si n'hi ha /
     p=strchr(pplana,'/');
     if (p!=NULL)
       //si n'hi ha
       *p=0;
     //afegim la plana
     if (afegeixelement(planes,pplana)!=0) {
       //error afegint element
       fputs("Error afegint element, sense memòria\n",stderr);
       //sortim
       return -1;
     }
     //alliberem memòria
     free(pplana);
     //seguent linia
     continue;
   }
 }
 //ja hem llegit els elements i afegits a les corresponents llistes
 fputs("\n\n",stderr);
 fflush(stderr);
 fputs("Fet\n",stderr);
 fputs("Ordenant llistes...\n",stderr);
 //ara ordenem, però ordenarem per valor, el valor es considera un enter
 //aquest enter s'aconsegueix convertint el valor , que es una cadena, a enter
 //ordenem la llista de paraules bloquejades
 if (paraulesbloq!=NULL)
   qsort(recorre_elements_amb_valor(paraulesbloq),retorna_numelements_amb_valor(paraulesbloq),sizeof (struct element_amb_valor),compara_pel_qsort);
 //ordenem la llista de urls bloquejades
 if (urlsbloq!=NULL)
   qsort(recorre_elements_amb_valor(urlsbloq),retorna_numelements_amb_valor(urlsbloq),sizeof (struct element_amb_valor),compara_pel_qsort);
 //ordenem la llista de urls bloquejades a causa de paraula prohibida
 if (urlsbloqparaula!=NULL)
   qsort(recorre_elements_amb_valor(urlsbloqparaula),retorna_numelements_amb_valor(urlsbloqparaula),sizeof (struct element_amb_valor),compara_pel_qsort);
 //ordenem la llista de planes bloquejades
 if (planesbloq!=NULL)
   qsort(recorre_elements_amb_valor(planesbloq),retorna_numelements_amb_valor(planesbloq),sizeof (struct element_amb_valor),compara_pel_qsort);
 if (urls!=NULL)
   qsort(recorre_elements_amb_valor(urls),retorna_numelements_amb_valor(urls),sizeof (struct element_amb_valor),compara_pel_qsort);
 //ordenem la llista de planes bloquejades
 if (planes!=NULL)
   qsort(recorre_elements_amb_valor(planes),retorna_numelements_amb_valor(planes),sizeof (struct element_amb_valor),compara_pel_qsort);
 fputs("Fet\n\n",stderr);
 //ara mostrem les llistes
 anteriorllistat=0;
 if (urls!=NULL) {
   anteriorllistat=1;
   mostraelements("Mostrant URLs més accedides",urls,MAXIM_ELEMENTS_A_MOSTRAR);
 }
 if (planes!=NULL) {
 if (anteriorllistat==1) {
     fputs("Prem enter per continuar...",stderr);
     getc(stdin);
     fputc('\n',stderr);
   }

   mostraelements("Mostrant planes més accedides",planes,MAXIM_ELEMENTS_A_MOSTRAR);
   anteriorllistat=1;
 }
 if (urlsbloq!=NULL) {
   if (anteriorllistat==1) {
     fputs("Prem enter per continuar...",stderr);
     getc(stdin);
     fputc('\n',stderr);
   }
   mostraelements("Mostrant urls bloquejades més accedides",urlsbloq,MAXIM_ELEMENTS_A_MOSTRAR);
   anteriorllistat=1;
 }
 if (urlsbloqparaula!=NULL) {
   if (anteriorllistat==1) {
     fputs("Prem enter per continuar...",stderr);
     getc(stdin);
     fputc('\n',stderr);
   }
   mostraelements("Mostrant urls bloquejades més accedides a causa de paraula prohibida",urlsbloqparaula,MAXIM_ELEMENTS_A_MOSTRAR);
   anteriorllistat=1;
 }
 if (planesbloq!=NULL) {
   if (anteriorllistat==1) {
     fputs("Prem enter per continuar...",stderr);
     getc(stdin);
     fputc('\n',stderr);
   }
   mostraelements("Mostrant les planes més bloquejades",planesbloq,MAXIM_ELEMENTS_A_MOSTRAR);
   anteriorllistat=1;
 }
 if (paraulesbloq!=NULL) {
   if (anteriorllistat==1) {
     fputs("Prem enter per continuar...",stderr);
     getc(stdin);
     fputc('\n',stderr);
   }
   mostraelements("Mostrant les paraules prohibides que han sortit més",paraulesbloq,MAXIM_ELEMENTS_A_MOSTRAR);
   anteriorllistat=1;
 }

return 0;
}
