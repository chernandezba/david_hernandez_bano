//fitxer filtre.c
//aquest es el fitxer que far� el filtrat de les paraules
//suporta HTML i texte
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include "filtre.h"
#include "llista.h"
#include "utils.h"



//enums---------------------------------------------------

enum SITUACIONS_HTML {
  HTML_OK=0, //estem fora de qualsevol etiqueta
  HTML_DINS_ETIQUETA, //estem dins d'una etiqueta
  HTML_DINS_COMILLES_ETIQUETA, //estem dins d'un delimitador de cadena '"' en una etiqueta
  HTML_DINS_COMENTARI_ETIQUETA, //estem dins el comentari d'una etiqueta, el comentari es delimitat per '--' , ex: -- aix� es un comentari --
  HTML_PARAULA, //indica paraula
  HTML_DINS_SCRIPT, //indica que estem dins un script
};



//variables locals-----------------------------------------------------

static char *htmletiquetascript="SCRIPT "; //etiqueta que indica un script (javascript,...),incloem espai perque despues de '<SCRIPT' sempre n'hi ha alguna cosa mes
static char *htmlfiscript="</SCRIPT>"; //etiqueta que indica fi de l'script
static char *htmlcomentaridinsetiqueta="--"; //comentari dins una etiqueta
static char *conversionshtml[]={
  //texte en html                 //en ASCII
  "&aacute;"               ,       "�", //accent en a tancat
  "&agrave;"               ,       "�", //accent en a obert
  "&auml;"                 ,       "�", //dieresis en a
  "&eacute;"               ,       "�", //accent en e tancat
  "&egrave;"               ,       "�", //accent en e obert
  "&euml;"                 ,       "�", //dieresis en e
  "&iacute;"               ,       "�", //accent en i tancat
  "&igrave;"               ,       "�", //accent en i obert
  "&iuml;"                 ,       "�", //dieresis en i
  "&oacute;"               ,       "�", //accent en o tancat
  "&ograve;"               ,       "�", //accent en o obert
  "&ouml;"                 ,       "�", //dieresis en o
  "&uacute;"               ,       "�", //accent en u tancat
  "&ugrave;"               ,       "�", //accent en u obert
  "&uuml;"                 ,       "�", //dieresis en u
  "&Aacute;"               ,       "�", //accent en A tancat
  "&Agrave;"               ,       "�", //accent en A obert
  "&Auml;"                 ,       "�", //dieresis en A
  "&Eacute;"               ,       "�", //accent en E tancat
  "&Egrave;"               ,       "�", //accent en E obert
  "&Euml;"                 ,       "�", //dieresis en E
  "&Iacute;"               ,       "�", //accent en I tancat
  "&Igrave;"               ,       "�", //accent en I obert
  "&Iuml;"                 ,       "�", //dieresis en I
  "&Oacute;"               ,       "�", //accent en O tancat
  "&Ograve;"               ,       "�", //accent en O obert
  "&Ouml;"                 ,       "�", //dieresis en O
  "&Uacute;"               ,       "�", //accent en U tancat
  "&Ugrave;"               ,       "�", //accent en U obert
  "&Uuml;"                 ,       "�", //dieresis en U
  "&ntilde;"               ,       "�", //enye
  "&Ntilde;"               ,       "�", //enye maj�scula
  "&ccedil;"               ,       "�", //c trancada
  "&Ccedil;"               ,       "�", //c trancada maj�scula
  "&lt;"                   ,       "<", //signe de menor
  "&gt;"                   ,       ">", //signe de major
  "&amp;"                  ,       "&", //signe de AND en C
  "&quot;"                 ,       """", //signe de comilles dobles
  NULL                     ,       NULL, //final
};



//funcions----------------------------------------------------



//inicialitza el filtre
//usa una variable tipus enter per sapiguer on es en cada moment
//el filtre, si dins un TAG, dins Javascript,....
void inicialitza_filtre(int *pvarfiltre) {

  *pvarfiltre=0;
}






//funcio local
//converteix les enyes, c trancades, accents, dieresis de
//llenguatge html a ascii
//el parametre d'entrada es el punter es a on mira si es troba alguna dieresis,
//enye,accent,...
//par�metre d'entrada:
//  p:punter a on mirar� si es troba alguna enye,accent,... en format html per despres convertir-ho a ASCII
//retorna el numero de bytes de la traducci�, ex: si es tradueix de &acute a
//� , llavors retorna 1
static int converteix_accents_dieresis_de_html_a_ascii(char *p) {
  int i,tlon,tlon2,tlon3;


  i=0;
  while (conversionshtml[i]!=NULL) {
    tlon=strlen(conversionshtml[i]);
    if (!strncmp(p,conversionshtml[i],tlon)) {
      //hem trobat una substituci�
      char *tpp;
      char *tpp2;
      tpp2=conversionshtml[i+1];
      tpp=p;
      //fem la substituci�
      while ((*tpp2)!=0) {
	*tpp=*tpp2;
	++tpp;
	++tpp2;
      }
      tlon2=strlen(conversionshtml[i+1]);
      //movem bytes
      //no podem usar strcpy quan 2 cadenes es solapen
      //ja que segons la documentaci� no se sap com actuar� strcpy
      //usem memmove
      tlon3=strlen(p+tlon)+1; //averig�em longitud origen sumant el caracter
      //de final de cadena
      memmove((p+tlon2),(p+tlon),tlon3);
      //ja hem trobat la conversi� a fer, sortim
      return tlon2;
    }
  //actualitzem punter
    i+=2;
  }
  return 0;
}






//funcio local
//cerca possible paraula
//si troba la paraula a la llista de prohibides retorna la paraula
//si no , retorna NULL
//parametres:
//  pi=punter a inici de paraula
//  pf=punter a final de paraula+1
//  pmutexparaules=punter a mutex de paraules
static struct  element *cerca_paraula(char *pi,char *pf,struct llista *pparaules,pthread_mutex_t *pmutexparaules) {

  char psal;
  struct element *pe;

  //delimitem paraula entre pi i pf
  psal=*pf;
  *pf=0;
  aminuscules(pi);
  //bloquejem mutex
  pthread_mutex_lock(pmutexparaules);
  //fem la cerca
  pe=cerca_element(pparaules,pi);
  //desbloquejem mutex
  pthread_mutex_unlock(pmutexparaules);
  *pf=psal; //restaurem caracter modificat
  return pe; //retornem element, si no el troba retornar� NULL

}



//filtra la linia que li enviem en el buffer per veure si cont� una paraula
//prohibida,si troba una paraula prohibida (que esta en la llista que li passem)
//retorna -1, si no, retorna 0
//par�metres:
//  pbuffer: punter al buffer a examinar
//  ptipuscontingut: tipus de contingut, pot ser CONTINGUT_HTML o CONTINGUT_TEXTE
//  pparaules: llista de les paraules que no podem sortir al buffer
//  pvarfiltre: punter a una variable que usa el filtre, abans de filtrar el fitxer html s'ha d'inicialitzar amb inicialitza_filtre
//  pelement: adre�a a una variable tipus struct element * , que s'actualitzar� amb la paraula que ha fet que no pass�s el filtre
//  pmutexparaules: punter al mutex de les paraules com� de tots els fils 
int filtra(char *pbuffer,int ptipuscontingut,struct llista *pparaules,int *pvarfiltre,struct element **pelement,pthread_mutex_t *pmutexparaules) {


  if (ptipuscontingut==CONTINGUT_TEXT) {
    //si el contingut del fitxer es texte
    char *pi,*pf;
    //si el tipus es texte, no usarem *pvarfiltre
    
    pi=pbuffer;
    while ((*pi)!=0) {
      //ignorem possibles espais inicials
      while (!isalnum(*pi)) {
	if ((*pi)==0) break;
	++pi;
      }
      if ((*pi)==0)
	//fi de cadena, sortim
	break;
      pf=pi;
      while(isalnum(*pf)) {
	//mirem fins que caracter no sigui un numero o lletra
	++pf;
      }
      //cerquem si paraula �s a la llista de prohibides
      if ((*pelement=cerca_paraula(pi,pf,pparaules,pmutexparaules))!=NULL) 
	//hem trobat la paraula a la llista, sortim notificant-ho
	return -1;
      pi=pf;
    }
    //si arribem aqui es que no hem trobat cap paraula a la llista de paraules
    //retornem
    return 0;
  }
  if (ptipuscontingut==CONTINGUT_HTML) {
    //si es un fitxer html, el filtre �s m�s complicat
    //hem d'ignorar els tags, javascript incrustat,...
    //aqui si que usarem *pvarfiltre
    char *pi,*p;

    pi=pbuffer;
    if ((*pvarfiltre)==HTML_DINS_SCRIPT) {
      int tlongit;
      //si estem dins un script
      //mirem si n'hi ha la finalitzaci� de l'script
      tlongit=strlen(htmlfiscript);
      if (!strncasecmp(pi,htmlfiscript,tlongit)) {
	//fi de l'script
	//ho indiquem i avancem punter
	*pvarfiltre=HTML_OK;
	pi+=tlongit;
      }
      else 
	//si estem dins script ignorem linia;
	return 0;
    }
    p=pi; //per poder delimitar cadenes
    while ((*pi)!=0) {
      //mentres no arribem a final de cadena
      if ((*pvarfiltre)==HTML_DINS_ETIQUETA) {
	//si estem dins una etiqueta
	//mirem si trobem un comentari
	//els comentaris estan delimitats per --
	if (!strcmp(pi,htmlcomentaridinsetiqueta)) {
	  *pvarfiltre=HTML_DINS_COMENTARI_ETIQUETA;
	  //incrementem punter
	  pi+=strlen(htmlcomentaridinsetiqueta);
	  continue;
	}
      }
      if ((*pvarfiltre)==HTML_DINS_COMENTARI_ETIQUETA) {
	//si estem dins un comentari
	//mirem si trobem final de comentari
	if (!strcmp(pi,htmlcomentaridinsetiqueta)) {
	  //hem trobat final de comentari dins etiqueta
	  *pvarfiltre=HTML_DINS_ETIQUETA;
	  //incrementem punter
	  pi+=strlen(htmlcomentaridinsetiqueta);
	  continue;
	}
      }
      if ((*pi)=='<' && ((*pvarfiltre)==HTML_OK || 
			 (*pvarfiltre)==HTML_PARAULA)) {
	//hem trobat l'inici d'una etiqueta
	//mirem si hem trobat una paraula fins l'inici de l'script
	if (p!=pi) {
	  //p=inici de cadena
	  //pi apunta a '<'
	  if ((*pelement=cerca_paraula(p,pi,pparaules,pmutexparaules))!=NULL) 
	    //hem trobat la paraula a la llista de prohibides, sortim notificant-ho
	    return -1;
	}
	//mirem si �s l'inici d'algun script
	if (!strncasecmp(pi+1,htmletiquetascript,strlen(htmletiquetascript))) {
	  *pvarfiltre=HTML_DINS_SCRIPT;
	  //retornem ja que un script acaba amb </SCRIPT> a l'inici d'una linia
	  return 0;
	}
	*pvarfiltre=HTML_DINS_ETIQUETA;
	//incrementem punter
	++pi;
	continue;
      }
      if ((*pi)=='>' && (*pvarfiltre)==HTML_DINS_ETIQUETA) {
	//hem trobat final d'etiqueta i no estaven dins d'una cadena
	//en una etiqueta
	*pvarfiltre=HTML_OK;
	++pi;
	p=pi; //apuntem possible inici de paraula a final de etiqueta
	continue;
      }
      if ((*pi)=='"' && (*pvarfiltre)==HTML_DINS_ETIQUETA) {
	//hem trobat l'inici d'una cadena en una etiqueta
	*pvarfiltre=HTML_DINS_COMILLES_ETIQUETA;
	++pi;
	continue;
      }
      if ((*pi)=='"' && (*pvarfiltre)==HTML_DINS_COMILLES_ETIQUETA) {
	//hem trobat el final d'una cadena en una etiqueta
	*pvarfiltre=HTML_DINS_ETIQUETA;
	++pi;
      }
      if ((*pvarfiltre)==HTML_OK) {
	int ttl;
	//mira si n'hi ha alguna conversi� possible (de html a ascii)
	//abans mirem si el punter apunta a '&' ja que �s l'inici de possibles
	//conversions
	if((*p)=='&') {
	  ttl=converteix_accents_dieresis_de_html_a_ascii(p);
	  /*if (ttl) {
	    //actualitzem punters si ha fet alguna conversi�
	    p+=ttl;
	    pi+=ttl;
	    }*/
	}
	//si estem fora de qualsevol etiqueta,script,...
	if (!isalnum(*p)) {
	  //mentres no trobem inici
	  //incrementem punter de inici de paraula
	  ++p;
	  //incrementem "punter de final de paraula"
	  ++pi;
	  continue;
	}
	if (isalnum(*p)) {
	  //hem trobat inici paraula
	  *pvarfiltre=HTML_PARAULA;
	  p=pi;
	  ++pi;
	  continue;
	}
      }
      if ((*pvarfiltre)==HTML_PARAULA) {
	//estem dins una paraula
	int ttl;
	//mira si n'hi ha alguna conversi� possible (de html a ascii)
	//abans mirem si el punter apunta a '&' ja que �s l'inici de possibles
	//conversions
	if((*pi)=='&') {
	  //estem dins una possible conversi�
	  if(*(pi+1)=='#') {
	    //si el punter apunta a #&
	    //aix� �s un caracter ISO 10646,ignorarem aquest car�cter
	    char *tpp;
	    tpp=pi+2;
	    while((*tpp)!=';') {
	      if ((*tpp)==0) 
		//si ha arribat a final de linia
		break;
	      ++tpp;
	    }
	    if ((*tpp)==0) {
	      //si no hi ha res mes despres del caracter ISO 10646
	      //finalitzem linia en &#
	      *pi=0;
	      //com que finalitza linia, tamb� finalitza paraula
	      //canviem estat
	      *pvarfiltre=HTML_OK;
	      //cerquem la paraula a la llista de prohibides
	      if ((*pelement=cerca_paraula(p,pi,pparaules,pmutexparaules))!=NULL) {
		//hem trobat paraula a la llista de prohibides
		//sortim notificant-ho
		return -1;
	      }
	    }
	    else {
	      //si n'hi han mes coses,movem cadena
	      //no podem usar strcpy quan les cadenes d'origen i dest�
	      //es solapen, hem d'usar memmove
	      int ttlongit;
	      ttlongit=strlen(tpp+1)+1; //averiguem longitud cadena de dest�
	      //sumantli caracter final de cadena
	      memmove(pi,tpp+1,ttlongit);
	      continue;
	    }
	  }
	  else {
	    ttl=converteix_accents_dieresis_de_html_a_ascii(pi);
	    /*if (ttl) 
	      //actualitzem punter si ha fet alguna conversi�
	      pi+=ttl;*/
	  }
	}
	if (isalnum(*pi)) {
	  //continuem estant a la paraula
	  ++pi;
	  continue;
	}
	if (!isalnum(*pi)) {
	  //hem trobat final de paraula
	  //canviem estat
	  *pvarfiltre=HTML_OK;
	  if ((*pelement=cerca_paraula(p,pi,pparaules,pmutexparaules))!=NULL) {
	    //hem trobat paraula a la llista de prohibides
	    //sortim notificant-ho
	    return -1;
	  }
	  p=pi;
	  continue;
	}
      }
      //incrementem punter
      ++pi;
      if ((*pi)==0 && (*pvarfiltre)==HTML_PARAULA) {
	//si finalitza la linia i estem en una paraula
	//la paraula tamb� finalitza
	//canviem estat
	*pvarfiltre=HTML_OK;
	//cerquem la paraula a la llista de prohibides
	if ((*pelement=cerca_paraula(p,pi,pparaules,pmutexparaules))!=NULL) {
	    //hem trobat paraula a la llista de prohibides
	    //sortim notificant-ho
	    return -1;
	  }
      }
    }
  }
  return 0;
}
