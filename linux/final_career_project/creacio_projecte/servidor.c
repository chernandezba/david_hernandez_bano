//fitxer servidor.c
//fitxer principal del servidor, es el que porta el main()
//es la part que inicialitza el servidor, crea els fils(threads),...


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include "configuracio.h"
#include "servidor.h"
#include "llista.h"
#include "llista_amb_valor.h"
#include "utils.h"
#include "linia.h"
#include "filtre.h"


//defines-----------------------------------------------------------------------
#define LONGITUD_CUA_SOCKET_LOCAL   1  //longitud de la cua del socket local
#define LONGITUD_CUA_PETICIONS      50 //longitud de la cua per al proxy http
#define VERSIO_SERVIDOR             "0.1" //versió del servidor
#define NOM_SERVIDOR                "Proxy_filtre" //nom del programa proxy http
#define LONG_BUFFER_BINARI          4096



//enums-------------------------------------------------------------------------



enum TIPUS_COMANDA {ID_GET=0, ID_POST, ID_HEAD}; //tipus de la comanda de la petició




//estructures-------------------------------------------------------------




struct fil { //estructura per als fils (threads)
    pthread_t pid; //id del fil
    int sockclient; //socket del client
    struct sockaddr_in infocl; //informació del client
    FILE *fsockclient; //socket del client quan s'ha usat fdopen per poder usar fgets,fputs,...
    int sockweb; //socket de connexió a la plana web
    FILE *fsockweb; //socket de connexió a la plana web quan s'ha usat fdopen
    struct linia *linies; //buffer per anar llegint les linies de la petició i del fitxer html
    struct linia *linreemp; //linia que reemplaçarà la primera linia a enviar al ervidor web ja que ha de ser diferent
    char *bufferbin; //buffer per llegir en binari
    char *buffer2; //un altre buffer
    int ocupat; //si el l'entrada del fil en cuestió està ocupat, si =0, no, si =1 , si
    struct fil *segfil; //fil seguent, si NULL -> ultim fil
    struct fil *antfil; //fil anterior, si NULL -> primer fil
}; 


//variables---------------------------------------------------------------
int numport; //port per a on llegeix les peticions HTTP PROXY
char *bind_servidor=NULL; //adreça per on la qual llegirà el servidor les peticions HTTP PROXY
int sidebug=0; //si =1 mostra missatges adicionals per fer depurat, si =0 no els mostra
struct configuracio *configuracio; //configuracio del servidor, es troba la configuracio
//llista de paraules i llista de urls
int socket_s=0; //socket del servidor PROXY HTTP
int socket_local=0; //socket local, serveix per poder afegir dinàmicament paraules i urls
//al filtre
struct fil *fils=NULL;
pthread_attr_t atribut_fil; //atribut per als fils, ens serveix per a especificar que son
//DETACHED
//possibles respostes d'error, en format html, son variables tipus *[], l'ultima linia es NULL


char *miss_err_comu_inici[]={
//primeres linies comunes a tots els missatges
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" " "\"http://www.w3.org/TR/html4/loose.dtd\">",
"<HTML><HEAD><META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-1\">",
NULL
};
char *miss_err_comu_fi[]={
//linies finals comunes a tots els missatges
"</HTML>",
NULL
};

//mutexes
pthread_mutex_t mutex_url= PTHREAD_MUTEX_INITIALIZER; //mutex per a les cerques o per afegir 
//urls
pthread_mutex_t mutex_paraules= PTHREAD_MUTEX_INITIALIZER; //mutex per a les cerques o per
//afegir paraules
//handle per al fitxer log
FILE *flog=NULL;



//variables per al fil del socket local------------------------------------
pthread_t pid_fil_socket_local;
int sockclient_socket_local; //connexió amb el client
FILE *fsockcli_socket_local=NULL; //FILE * de connexió amb el client
char *buffer_local=NULL; //buffer per anar llegint les paraules
size_t long_buffer_local=0; //longitud del buffer
//fi de les variables per al fil del socket local-------------------------



//funcions-------------------------------------------------------------------------




//funció que es crida quan es rep SIGINT(CTRL-C) o SIGQUIT
void ctrl_break(int psignal) {
struct fil *pfil,*pfilant;

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
//matem possible fil del socket local
 if (pid_fil_socket_local!=0) {
   pthread_cancel(pid_fil_socket_local);
   if (sidebug) {
     flockfile(stderr);
     fprintf(stderr,"Cancel.lant fil %lu del socket local\n",(unsigned long int)pid_fil_socket_local);
   }
 }
//alliberem memòria
free(bind_servidor);
//tenquem sockets
if (socket_s) close(socket_s);
if (socket_local) close(socket_local);
//esborrem nom del socket local, o sigui , el fitxer del socket local
unlink(NOM_SOCKET_LOCAL);
//tenquem handle fitxer log
if (flog!=NULL) fclose(flog);
//finalitzem fils en execució
pfil=fils;
while (pfil!=NULL) {
    if (pfil->ocupat) {
        if (sidebug) {
            flockfile(stderr);
            fprintf(stderr,"Cancel.lant fil amb pid %lu\n",(unsigned long int)pfil->pid);
            funlockfile(stderr);
        }
        pthread_cancel(pfil->pid);
    }
    pfil=pfil->segfil;
}
//alliberem memòria assignada per la estructura dels fils
pfil=fils;
while (pfil!=NULL) {
    pfilant=pfil;
    pfil=pfil->segfil;
    if (!pfilant->ocupat)
        //si realment el fil ha finalitzat
        free(pfilant);
}
//els fils que no han finalitzat, ja finalitzaran ja que el procés principal finalitza
//i la memòria que no hem dessassignat de la estructura dels fils ja es dessassignarà
//(la dessassignarà la rutina que inclou el compilador al final del programa)
//destruim atribut per als fils
pthread_attr_destroy(&atribut_fil);
//finalment,sortim
exit(0);
}















//si tot va be, retorna 0
static int llegeix_configuracio(void) {
//llegeix el fitxer de configuració i actualitza variables numport i bind_servidor
struct element_amb_valor    *psg;

if (sidebug) {
    fprintf(stderr,"Llegint fitxer de configuracio %s\n",NOM_FITXER_CFG);
}
configuracio=obre_configuracio(NOM_FITXER_CFG);
if (config_error()) {
    posa_error_config();
    return -1;
}
if (sidebug) {
    fputs("Llegit\nParsejant fitxer\n",stderr);
}
//ara llegim seccio global
//llegim numero de port
psg=cerca_element_amb_valor(configuracio->global_config,NOM_OPCIO_PORT);
if (psg==NULL) {
    fputs("Error greu de programació, no s'ha trobat variable "NOM_OPCIO_PORT,stderr);
    return -1;
}
numport=atoi(psg->valor);
//llegim bind
psg=cerca_element_amb_valor(configuracio->global_config,NOM_OPCIO_BIND);
if (psg==NULL) {
    fputs("Error greu de programació, no s'ha trobat variable "NOM_OPCIO_BIND,stderr);
    return -1;
}
bind_servidor=strdup(psg->valor);
if (sidebug)
    fputs("Fet\n",stderr);
return 0;
}





//inicialitza el socket servidor del proxy
//si tot va be, retorna 0
static int inicialitza_socket_servidor(void) {
struct sockaddr_in padreca;

if (sidebug)
    fputs("Creant socket del servidor\n",stderr);
if ((socket_s=socket(PF_INET,SOCK_STREAM,0))==-1) {
    fputs("Error creant el socket\n",stderr);
    return -1;
}
//preparem adreça i port del socket    
padreca.sin_family=AF_INET;
padreca.sin_port=htons(numport);
if (!strcmp(bind_servidor,"127.0.0.1")) {
    padreca.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    if (sidebug)
        fprintf(stderr,"Assignant adreça 127.0.0.1, port %d\n",numport);
    }
else {
    unsigned int p0,p1,p2,p3;
    sscanf(bind_servidor,"%d.%d.%d.%d",&p0,&p1,&p2,&p3);
    padreca.sin_addr.s_addr=htonl(iptolong(p0,p1,p2,p3));
    if (sidebug)
        fprintf(stderr,"Assignant adreça %d.%d.%d.%d, port %d\n",p0,p1,p2,p3,numport);
}
//assignem adreça
if (bind(socket_s,(struct sockaddr *)&padreca,sizeof padreca)==-1) {
    fputs("Em assignar l'adreça al servidor:",stderr);
    if (errno==EACCES)
        fprintf(stderr,"no es tenen privilegis per assignar el port %d\n",numport);
      else
        posa_error(errno);
    return -1;
}
if (sidebug) {
    fputs("Assignada\n",stderr);
    fputs("Possant socket servidor proxy http en mode LISTEN\n",stderr);
}
if (listen(socket_s,LONGITUD_CUA_PETICIONS)==-1) {
    fputs("Error possant servidor proxy http en mode LISTEN:",stderr);
    posa_error(errno);
    return -1;
}
if (sidebug) 
    fputs("Possat\nCreacio socket servidor  proxy http correcte\n",stderr);
return 0;
}











//inicialitza el socket local,el que serveix per poder afegir dinàmicament paraules i urls
//al filtre
//si tot va be, retorna 0
static int inicialitza_socket_local(void) {
struct sockaddr_un pnom;

if (sidebug)
    fputs("Creant socket local\n",stderr);
if ((socket_local=socket(PF_LOCAL,SOCK_STREAM,0))==-1) {
    fputs("Error creant el socket\n",stderr);
    return -1;
}
//primer esborrem socket local, o sigui, el fitxer
if (unlink(NOM_SOCKET_LOCAL)) {
    if (errno!=ENOENT && errno!=ENOTDIR) {
        //si l'error que ha donat no te a veure en que el fitxer
        //no existeix
        fprintf(stderr,"Error esborrant fitxer %s del socket local:",NOM_SOCKET_LOCAL);
        posa_error(errno);
        fprintf(stderr,"Esborri el fitxer %s i reinicii el servidor\n",NOM_SOCKET_LOCAL);
        return -1;
    }
};
//preparem nom del socket    
pnom.sun_family=AF_LOCAL;
strcpy(pnom.sun_path,NOM_SOCKET_LOCAL);
if (sidebug)
    fprintf(stderr,"Socket local amb nom de fitxer:%s\n",NOM_SOCKET_LOCAL);
//assignem el nom al socket local
if (bind(socket_local,(struct sockaddr *)&pnom,SUN_LEN(&pnom))==-1) {
    fputs("Error assignant el fitxer al socket local:",stderr);
    posa_error(errno);
    return -1;
}
if (sidebug)
    fputs("Possant socket local en estat LISTEN\n",stderr);
if (listen(socket_local,LONGITUD_CUA_SOCKET_LOCAL)==-1) {
    fputs("Error possant socket local en estat LISTEN:",stderr);
    posa_error(errno);
    return -1;
}
if (sidebug) {
    fputs("Possat\n",stderr);
    fputs("Creacio socket local correcte\n",stderr);
}
return 0;
}










//possa error del fil
//sense salt de linia
static void posa_missatge_fil_nocrlf(pthread_t pid,char *pmissatge) {

flockfile(stderr);
fprintf(stderr,"%lu:%s",(unsigned long int)pid,pmissatge);
funlockfile(stderr);
}



//possa error del fil
//amb salt de linia
static void posa_missatge_fil(pthread_t pid,char *pmissatge) {

flockfile(stderr);
fprintf(stderr,"%lu:%s\n",(unsigned long int)pid,pmissatge);
funlockfile(stderr);
}




//envia una linia tal qual sense afegir CRLF
//paràmetres:
//  pbuffer: buffer a a enviar
//  pfsocket: a on enviar
//  pfil: estructura del fil actual
//  mostrarenviat:si =1 mostra el que s'envia si s'ha possat com a paràmetre -v en arrencar el servidor(sidebug=1), si =0 no ho mostra
//si no hi ha hagut error, retorna 0, si no, retorna el codi d'error segons ferror
static int envia_linia_nocrlf(char *pbuffer,FILE *pfsocket,struct fil *pfil,int mostrarenviat) {


if (sidebug && mostrarenviat) {
    flockfile(stderr);
    posa_missatge_fil_nocrlf(pfil->pid,">>");
    fprintf(stderr,"%s",pbuffer);
    funlockfile(stderr);
    
};
//envia la linia sense afegir CRLF
fprintf(pfsocket,"%s",pbuffer);
return ferror(pfsocket);
}





//envia una linia. A cada linia se li afegeix CR i LF
//paràmetres:
//  pbuffer: buffer a a enviar
//  pfsocket: a on enviar
//  pfil: estructura del fil actual
//  mostrarenviat:si =1 mostra el que s'envia si s'ha possat com a paràmetre -v en arrencar el servidor(sidebug=1), si =0 no ho mostra
//si no hi ha hagut error, retorna 0, si no, retorna el codi d'error segons ferror
static int envia_linia(char *pbuffer,FILE *pfsocket,struct fil *pfil,int mostrarenviat) {


if (sidebug && mostrarenviat) {
    flockfile(stderr);
    posa_missatge_fil_nocrlf(pfil->pid,">>");
    fprintf(stderr,"%s\n",pbuffer);
    funlockfile(stderr);
    
};
//envia la linia afegint CRLF
fprintf(pfsocket,"%s\xd\xa",pbuffer);
return ferror(pfsocket);
}








//funció local
//envia més d'una linia
//parametres:
//  plinies:linies a enviar
//  pfsocket:a on enviar-ho
//  pfil:fil
//  psiafegirsaltlinia:si =0 no s'afegeix salt de linia,si =1 s'afegeix salt de linia
//  mostrarenviat:si =1 mostra el que s'envia si s'ha possat com a paràmetre -v en arrencar el servidor(sidebug=1), si =0 no ho mostra
//si tot va be,retorna 0
static int envia_linies(struct linia *plinies,FILE *pfsocket,struct fil *pfil,
int psiafegirsaltlinia,int mostrarenviat) {
int ti;
struct linia *plin;

plin=plinies;
while (plin!=NULL) {
  if (psiafegirsaltlinia==1) {
    if ((ti=envia_linia(retorna_cadena_linia(plin),pfsocket,pfil,mostrarenviat))!=0) return ti;
  }
if (psiafegirsaltlinia==0) {
    if ((ti=envia_linia_nocrlf(retorna_cadena_linia(plin),pfsocket,pfil,mostrarenviat))!=0) return ti;
  }

    pthread_testcancel();
    plin=seguent_linia(plin);
}
return 0;
}







//compta totes el numero de bytes de totes les linies(acaba en NULL) com
//si s'haguessin d'enviar(o sigui, afegint a la longitud CRLF)
static int longitud_linies(struct linia *plinies) {
int tlong;
struct linia *plin;




tlong=0;
plin=plinies;
while (plin!=NULL) {
    tlong+=strlen(retorna_cadena_linia(plin))+2; //+2 de CRLF
    plin=seguent_linia(plin);
}
return tlong;
}









//envia el missatge d'error
//paràmetres:
//  pfsocket: FILE * del socket del client(que s'ha creat quan s'ha cridat a accept
//  pnumerror: error corresponent segons HTTP/1.0 (200=OK,...
//  pmissatgeerrorcurt: breu descripció del missatge d'error
//  pmissatgerrorllarg: descripcio mes detallada del missatge d'error
//  ppeticio: peticio que ha fet el client (es la primera linia amb el GET http://......
static void envia_missatge_error(FILE *pfsocket,int pnumerror,char *pmissatgeerrorcurt,
char *pmissatgerrorllarg,char *ppeticio,struct fil *pfil,int ptipuscomanda) {
char ttbuffer[LONG_MAX_PETICIO_HTTP+140];
time_t tdataact;
char ttbuffer2[30],*tp;
int tlongithtml,i;
struct linia *plinies=NULL,*plinact;

    
tdataact=time(NULL);
asctime_r(localtime(&tdataact),ttbuffer2);
treu_crlf(ttbuffer2);
sprintf(ttbuffer,"HTTP/1.0 %03d %s",pnumerror,pmissatgeerrorcurt);
//enviem missatge
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) return;
//enviem nom servidor i versió        
sprintf(ttbuffer,"Server: %s/%s",NOM_SERVIDOR,VERSIO_SERVIDOR);
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) return;
tdataact=time(NULL);
asctime_r(localtime(&tdataact),ttbuffer2);
tp=strchr(ttbuffer2,'\n');
if (tp!=NULL) *tp=0;
//enviem data actual
sprintf(ttbuffer,"Date: %s",ttbuffer2);
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) return;
//les 2 linies següents son perque la plana no s'emmagatzemi a memoria cau del navegador
sprintf(ttbuffer,"Last-Modified: %s",ttbuffer2);
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) return;
sprintf(ttbuffer,"Expires: %s",ttbuffer2);
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) return; 
//afegim les linies inicials
i=0;
plinact=plinies;
while (miss_err_comu_inici[i]!=NULL) {
    if ((plinact=afegeix_linia(plinact,miss_err_comu_inici[i]))==NULL)
        //n'hi ha error de memòria
        return;
    if (plinies==NULL) {
        //si n'era la primera linia
        plinies=plinact;
    }
    ++i;
}
//afegim linies customitzades
sprintf(ttbuffer,"<TITLE>ERROR: %s</TITLE></HEAD>",pmissatgeerrorcurt);
if ((plinact=afegeix_linia(plinact,ttbuffer))==NULL)
    //n'hi ha error de memòria
    return;
sprintf(ttbuffer,"<BODY><H1>%s</H1>",pmissatgerrorllarg);
if ((plinact=afegeix_linia(plinact,ttbuffer))==NULL)
    //n'hi ha error de memòria
    return;
sprintf(ttbuffer,"<H2>%s</H2>",ppeticio);
if ((plinact=afegeix_linia(plinact,ttbuffer))==NULL)
    //n'hi ha error de memòria
    return;
sprintf(ttbuffer,"<H2>Generat per %s a data %s</H2>",NOM_SERVIDOR
" "VERSIO_SERVIDOR,ttbuffer2);
if ((plinact=afegeix_linia(plinact,ttbuffer))==NULL)
    //n'hi ha error de memòria
    return;
strcpy(ttbuffer,"</BODY>");
if ((plinact=afegeix_linia(plinact,ttbuffer))==NULL)
    //n'hi ha error de memòria
    return;
//afegim linies finals
i=0;
while (miss_err_comu_fi[i]!=NULL) {
    if ((plinact=afegeix_linia(plinact,miss_err_comu_fi[i]))==NULL)
        //n'hi ha error de memòria
        return;
    ++i;
}
if (envia_linia("Content-Type: text/html",pfsocket,pfil,1)!=0) {
    return;
};
tlongithtml=longitud_linies(plinies);
sprintf(ttbuffer,"Content-Length: %d",tlongithtml);
//enviem longitud missatge html
if (envia_linia(ttbuffer,pfsocket,pfil,1)!=0) {
    return;
};
//enviem linia en blanc
if (envia_linia("",pfsocket,pfil,1)!=0) {
    return;
};
if (ptipuscomanda==ID_GET) {
    //si la comanda és GET
    //envia les linies corresponent del missatge d'error, estan en format html
    if (envia_linies(plinies,pfsocket,pfil,1,1)!=0) {
        return;
    };
}
//alliberem memòria assignada a les linies
destrueix_linies(plinies);
}













//funció que es crida quan el fil finalitza, es el "cleanup handler" del
//fil del proxy http
void fil_proxy_http_cleanup(void *arg) {
struct fil *pfil;


pfil=(struct fil *)arg;
if (pfil->fsockclient==NULL) {
    //tenquem socket client
    if (pfil->sockclient)
	//si hem "obert" el socket
	close(pfil->sockclient);
    }
else
    //si ja hem usat fdopen, tencar socket amb fclose
    fclose(pfil->fsockclient);
pfil->fsockclient=NULL;
pfil->sockclient=0;
if (pfil->fsockweb==NULL) {
    //tenquem socket connexió plana web
    if (pfil->sockweb)
	//si hem "obert" el socket
        close(pfil->sockweb);
    }
else
    //si ja hem usat fdopen, tencar socket amb fclose
    fclose(pfil->fsockweb);
pfil->fsockweb=NULL;
pfil->sockweb=0;
//alliberem possible memòria assignada per llegir linies (de la petició HTTP, del fitxer
// html,...
if (pfil->linies!=NULL) destrueix_linies(pfil->linies);
pfil->linies=NULL;
if (pfil->linreemp!=NULL) destrueix_linies(pfil->linreemp);
pfil->linreemp=NULL;
if (pfil->bufferbin!=NULL) free(pfil->bufferbin);
pfil->bufferbin=NULL;
if (pfil->buffer2!=NULL) free(pfil->buffer2);
pfil->buffer2=NULL;
if (sidebug) {
    flockfile(stderr);
    fprintf(stderr,"Fil %lu finalitzat\n",pfil->pid);
    funlockfile(stderr);
}

//finalment, alliberem entrada del fil
pfil->ocupat=0;
}









//processa la petició i mira si esta ben formada
//arguments:
//  ppeticio:es la petició (o sigui, la primera linia que envia el client al proxy http)
//  purl:punter per enmagatzemar la url (treu l''http://' i si està especificat el port, 
//  també),no assigna memòria
//  pport:punter a int per enmagatzemar el port si està especificat en la petició (en la URI)
//  ptipuscomanda:punter a TIPUSCOMANDA per emmagatzemar el tipus de comanda de la petició 
//  (GET, POST o HEAD)
//format de la petició HTTP és
//  mètode <SP> URI <SP> versió <CRLF>
//  <SP> és un espai
//  CRLF son els codis 13 y 10
//  métode ha de ser "GET", "POST" o "HEAD" que són els mètodes de la especificació HTTP/1.0
//  URL és l'adreça que es vol obtenir, el proxy només enten protocol http://
//  Versió és opcional (si no está s'enten HTTP/0.9)està en el format HTTP/x.y, on x és el
// número de versió i y el de subversió del
//  protocol HTTP, el nostre proxy només accepta fins a la versió 1.0 (inclosa)

int processa_peticio(char *ppeticio,char *purl,unsigned int *lport,
int *ptipuscomanda) {


char *tbuffers[60];
char buffer[LONG_MAX_PETICIO_HTTP+60],*tdelim=" ";
unsigned int tversio,tsubversio;
char *p,*p2;
char *tcadhttp="http://";
char *tcadhttp2="HTTP/";

*lport=80; //port per defecte per accedir al servidor WEB és 80
strncpy(buffer,ppeticio,sizeof(buffer) -1);
//cerquem primer caracter que no sigui espai
p=strtok_r(buffer,tdelim,tbuffers);
//p=mètode
if (p==NULL) 
    //no n'hi ha res o excepte espais
    return PROCESA_ERR_NO_LIN;
//mètode="GET", "POST" o "HEAD"?
if (!strcasecmp(p,"GET")) {
    *ptipuscomanda=ID_GET;
}
else {
     if (!strcasecmp(p,"POST")) {
         *ptipuscomanda=ID_POST;
     }
     else {
        if (!strcasecmp(p,"HEAD")) {
            *ptipuscomanda=ID_HEAD;
        }
        else {
              //no es cap dels tres
              return PROCESA_ERR_METODE_DESCONEGUT;
        }
     }
}
p=strtok_r(NULL,tdelim,tbuffers);
if (p==NULL) {
    //no hi ha URI
    return PROCESA_ERR_NO_URI;
}
//p=URI
if (strncasecmp(p,tcadhttp,strlen(tcadhttp))) {
    //protocol no es http://
    return PROCESA_ERR_PROTOCOL_NO_SOPORTAT;
}
//mirem si despres de http:// n'hi ha alguna cosa
p+=strlen(tcadhttp);
if (*p==0) {
    //no hi ha cap URL especificada
    return PROCESA_ERR_NO_URL;
}
//mirem si s'especifica el port
p2=strchr(p,':');
if (p2!=NULL) {
    char buffer2[LONG_MAX_PETICIO_HTTP+7+1],*p3;
    //està especificat número de port
    ++p2;
    //guardem numero de port a lport
    sscanf(p2,"%u",lport);
    //copiem url a buffer temporal
    strcpy(buffer2,p);
    //treiem  ':'
    p3=strchr(buffer2,':');
    *p3=0;
    //el copiem a purl
    strncpy(purl,buffer2,LONG_MAX_PETICIO_HTTP);
    //ara mirem si després de l'especificació del port s'especifica un fitxer o directori
    ++p3;
    while (isdigit(*p3) && (*p3)!=0) ++p3;
    if ((*p3)!=0) {
        //si n'hi ha més després del digit
        //ho afegim a l'adreça url
        strcat(purl,p3);
    };
}
else
    //copiem la url a purl
    strcpy(purl,p);
p=strtok_r(NULL,tdelim,tbuffers);
//p=versió HTTP
if (p==NULL) {
    //no hi ha especificat versió de HTTP, assumim 0.9
    if (*ptipuscomanda==ID_HEAD)
        //HEAD només a partir d'HTTP/1.0
        return PROCESA_ERR_HEAD_A_PARTIR_HTTP_10;
     //retornem correctament
    return 0;
}
if (strncasecmp(p,tcadhttp2,strlen(tcadhttp2))) {
    //format de versió http no correcte
    return PROCESA_ERR_FORMAT_VERSIO_HTTP_ERRONI;
}
p+=strlen(tcadhttp2);
if ((*p)==0) {
    //no hi ha res despres d' HTTP/
    //suposem HTTP/0.9
    if (*ptipuscomanda==ID_HEAD)
        //HEAD només a partir d'HTTP/1.0
        return PROCESA_ERR_HEAD_A_PARTIR_HTTP_10;
    //no passa res, ho ignorem i retornem correctament
    return 0;
}
tversio=0;
tsubversio=9;
sscanf(p,"%u.%u",&tversio,&tsubversio);
if (tversio>1) {
    //versió d'http és major que 1
    return PROCESA_ERR_VERSIO_HTTP_NO_SUPORTAT;
}
if (tversio==1 && tsubversio>0) {
    //versió d'http major que 1.0, no està suportada
    return PROCESA_ERR_VERSIO_HTTP_NO_SUPORTAT;
}
if (tversio<1 && *ptipuscomanda==ID_HEAD)
    return PROCESA_ERR_HEAD_A_PARTIR_HTTP_10;
return 0;
}











//escriu entrada al fitxer log
//paràmetres:
//  pcadcodi: cadena especificant codi en el fitxer log, pot ser URLOK, URLBLOQ o PARAULABLOQ
//  purl: url que ha provocat l'escritura del fitxer log
//  ptemps_inici: variable tipus time_t que indica quan va ser que va iniciar el fil 
//  d'aquesta url
//  ptemps_ara: variable tipus time_t que indica el temps actual(hora/dia)
//  pipcli: cadena tipus x.x.x.x del client que ha fet la petició de l'url
//  pparaula: paraula que ha fet l'escritura de la linia del fitxer log, només s'ha 
//  d'especificar si pcadcodi="PARAULABLOQ", o sigui,ha aparegut una paraula que no ha
//  passat el filtre
//format fitxer log:
//  -cada camp separat per un spai
//  -cada linia és un registre
//  Valor 1er camp  !               Resta dels camps
//  ------------------------------------------------------------------    
//   URLOK          !  valor URL, temps tardat, IP client, data
//   URLBLOQ        !  valor URL, IP client, data
//   PARAULABLOQ    !  valor URL, paraula , IP client, data

//  si el 1er camp val un dels tres valors especificats, no fa res i retorna 0
//  retorna el valor retornat per ferror
int escriu_fitxer_log(char *pcadcodi,char *purl,struct timeval *ptemps_inici,struct timeval *ptemps_ara,char *pipcli,char *pparaula) {
char tdataact[256];
struct tm tresult;


if (flog==NULL)
    //si handle del fitxer d'error es NULL, no fem res
    //retornem com si res
    return 0;
localtime_r(&ptemps_ara->tv_sec,&tresult);
strftime(tdataact,sizeof(tdataact)-1,"%d/%m/%Y/%H:%M:%S",&tresult);
if (!strcmp(pcadcodi,URLOK)) {
    time_t tsecinic,tsecfin;
    long difsec,difmsec;
    suseconds_t tmsinic,tmsfin;
    
    //fem diferencia de temps
    tsecinic=ptemps_inici->tv_sec;
    tsecfin=ptemps_ara->tv_sec;
    difsec=tsecfin-tsecinic;
    tmsinic=ptemps_inici->tv_usec;
    tmsfin=ptemps_ara->tv_usec;
    if (tmsfin<tmsinic) {
        //ajustem els segons i els microsegons
        //treiem un segon
        difmsec=tmsinic-(tmsfin+10);
        --difsec;
    }
    else difmsec=tmsfin-tmsinic;
    flockfile(flog); //bloquejem fitxer per evitar la mescla de continguts en entorn 
    //                 multithread
    fprintf(flog,"%s %s %lu.%lu %s %s\n",pcadcodi,purl,difsec,difmsec,pipcli,tdataact);
    funlockfile(flog); //desbloquejem fitxer
    fflush(flog);
    return ferror(flog);
}
if (!strcmp(pcadcodi,URLBLOQ)) {
    flockfile(flog); //bloquejem fitxer per evitar la mescla de continguts en entorn 
    //                 multithread
    fprintf(flog,"%s %s %s %s\n",pcadcodi,purl,pipcli,tdataact);
    funlockfile(flog); //desbloquejem fitxer
    fflush(flog);
    return ferror(flog);
}
if (!strcmp(pcadcodi,WEBBLOQ)) {
    flockfile(flog); //bloquejem fitxer per evitar la mescla de continguts en entorn 
    //                 multithread
    fprintf(flog,"%s %s %s %s\n",pcadcodi,purl,pipcli,tdataact);
    funlockfile(flog); //desbloquejem fitxer
    fflush(flog);
    return ferror(flog);
}
if (!strcmp(pcadcodi,PARAULABLOQ)) {
    flockfile(flog); //bloquejem fitxer per evitar la mescla de continguts en entorn 
    //                 multithread
    fprintf(flog,"%s %s %s %s %s\n",pcadcodi,purl,pparaula,pipcli,tdataact);
    funlockfile(flog); //desbloquejem fitxer
    fflush(flog);
    return ferror(flog);
}
//si pcadcodi no es si URLOK,URLBLOQ o PARAULABLOQ, retorna 0 sense fer res
return 0;
}















//llegeix n bytes d'un socket i els transfereix a un altre socket
//els llegeix en mode binari
//si n'hi ha hagut error llegint retorna -1, si enviant, -2
//si no hi ha hagut error, retorna 0
static int transfereix_dades(FILE *pdesde,FILE *phasta,unsigned long plongitud,void *pbuffer,
unsigned long plongbuffer) {
  unsigned long tqueda,tllegit;

tqueda=plongitud;
while (tqueda>0) {
  //anem llegint fins que no quedi res
  pthread_testcancel();
  tllegit=fread(pbuffer,1,(tqueda > plongbuffer)
  ? plongbuffer : tqueda,pdesde);
  if (ferror(pdesde)) 
    //n'hi ha hagut error
    return -1;
  tqueda-=tllegit;
  //enviem les dades llegides
  pthread_testcancel();
  fwrite(pbuffer,1,tllegit,phasta);
  if (ferror(phasta)) 
     //n'hi ha hagut error
    return -2;
}
 fflush(phasta); //netegem buffer perque envii dades no enviades del buffer
 return 0;
}








//mira si el camp indica que el contingut es comprimit o si es vol
//contingut comprimit
//!!!no cridar directament!!!
//les funcions sirespostacomprimida i si es vol comprimir criden
//a aquesta funció
//paràmetres d'entrada:
//  p: Apunta a després de  "Accept-Encoding:" o de "Content-Encoding:"
//retorna 1 si indica que es vol contingut comprimit o si es comprimit
//0 si no
static int siescomprimit(char *pp) {

  char *tbuffers[60],*tdelim=" ,",*p,*p2;
  int i;
  char *cgzip[]={"gzip","deflate","x-gzip","x-deflate",NULL};


 p=strtok_r(pp,tdelim,tbuffers);
 //treiem possibles espais
 while ((*p)==' ') {
   if ((*p)==0) break;
   ++p;
 }
 //treiem possibles espais finals
 p2=strchr(p,0);
 --p2;
 while ((*p2)==' ') {
   if (p2==p) break;
   --p2;
 }
 ++p2;
 *p2=0;
 while (p!=NULL) {
   //mentres n'hi hagi valors en el camp
   //els valors estan separats per el caracter ','
   i=0;
   while (cgzip[i]!=NULL) {
     //cerquem valor entre els 
     //possibles que indiquen valor comprimit
     if (!strcasecmp(p,cgzip[i]))
       //si n'hem trobat el valor entre els que indiquem valor comprimit
       //sortim del bucle
       break;
     ++i;
   }
   if (cgzip[i]!=NULL) 
     //n'hem trobat el valor entre els que indiquem valor comprimit
     //ho hem de comprobat 2 cops ja que un cop es per sortir d'un while
     //i l'altre cop es per sortir de l'altre while (el de while (p!=NULL))
     //sortim del bucle
     break;
   p=strtok_r(NULL,tdelim,tbuffers);
 }
 if (p!=NULL)
   //hem trobat que la linia indica que es vol contingut comprimit
   return 1;
 //si no
 return 0;
}












//mira si el camp indica que es vol contingut comprimit
//paràmetres d'entrada:
//  plinia: Linia amb el camp "Content-Encoding:"
//retorna 1 si indica que es vol contingut comprimit
//0 si no
static int siesvolcomprimit(char *plinia) {

  char *buffer,*p;
  int tlongcamp=strlen(HTTP_CAMP_CONTENT_ENCODING),tres;


  //primer mirem si el camp és "Accept-Encoding:"
  if (strncasecmp(plinia,HTTP_CAMP_CONTENT_ENCODING,tlongcamp))
      //si no ho és
      //retornem com que no es el camp que busquem
      return 0;
 buffer=strdup(plinia);
 //p="Accept-Encoding"
 p=buffer;
 p+=tlongcamp;
 //p=el que hi ha despres
 tres=siescomprimit(p);
 //alliberem memoria
 free(buffer);
 //retornem el que ens retorna la funcio tres
 return tres;

}





//mira si el camp indica que el contingut es comprimit
//paràmetres d'entrada:
//  plinia: Linia amb el camp "Accept-Encoding:"
//retorna 1 si indica que es vol contingut comprimit
//0 si no
static int sirespostacomprimida(char *plinia) {

  char *buffer,*p;
  int tlongcamp=strlen(HTTP_CAMP_CONTINGUT_RESPOSTA_ENCODING),tres;

  //primer mirem si el camp és "Accept-Encoding:"
  if (strncasecmp(plinia,HTTP_CAMP_CONTINGUT_RESPOSTA_ENCODING,tlongcamp))
      //si no ho és
      //retornem com que no es el camp que busquem
      return 0;
 buffer=strdup(plinia);
 //p="Accept-Encoding"
 p=buffer;
 p+=tlongcamp;
 //p=el que hi ha després
 tres=siescomprimit(p);
 //alliberem memòria
 free(buffer);
 //retornem el valor retornat per siescomprimit
 return tres;

}
















//fil del proxy http
//anem cridant a pthread_testcancel perque si volem finalitzar el fil, es finalitzi el fil 
//(ja que els fils es creen DETACHED i SYNCHRONOUSLY CANCELABLE)
//(i com que està assignat el cleanup handler, s'allibera memòria)
void* fil_proxy_http(void* pargs){
struct fil *pfil;
struct linia *plin=NULL;
struct element *pelem;
int lsortir=0,i;
//char vbuffer[512];
char lurl[LONG_MAX_PETICIO_HTTP],lurlcerca[LONG_MAX_PETICIO_HTTP];
char *urltemp;
unsigned int lport;
char *pnomcontenttexthtml="text/html"; //tipus text/html, tipus html
char *pnomcontenttext="text/plain";  //tipus text/plain, es un fitxer texte
char *p;
int plongnomconnection,plongnomproxyconnection,plongnomkeepalive;
int lerror,lres;
struct timeval ldatainici,ldatafi;
int tipuscomanda,twebenc;
unsigned long int tqueda;
size_t longbufferbin=0;




gettimeofday(&ldatainici,NULL); //comptador temps inicial
pfil=(struct fil *)pargs; //pfil=entrada en la llista de fils del fil en execució
//registrem el cleanup handler, o sigui, registrem la funció que es cridarà quan el fil
//finalitzi
pthread_cleanup_push(fil_proxy_http_cleanup,(void *)pfil);
//per poder usar fgets,fputs,... usem una funció de "conversió"
//el que realment fa es emplenar una variable tipus FILE amb
//el handle del socket per poder usar fgets,fputs,fread,...
if ((pfil->fsockclient=fdopen(pfil->sockclient,"r+"))==NULL) {
    //n'hi ha hagut error
    flockfile(stderr);
    posa_missatge_fil_nocrlf(pfil->pid,"Error en la funció fdopen:");
    posa_error(errno);
    funlockfile(stderr);
    pthread_exit(pfil);
}
//possem punter a NULL perque en la primera linia que s'afegeixi ja es crei l'estructura
pfil->linies=NULL;
//anem llegin linies
plongnomconnection=strlen(HTTP_CAMP_CONNECTION);
plongnomproxyconnection=strlen(HTTP_CAMP_PROXY_CONNECTION);
plongnomkeepalive=strlen(HTTP_CAMP_KEEP_ALIVE);
longbufferbin=0;
while (!lsortir) {
    pthread_testcancel();
    if (getline(&pfil->bufferbin,&longbufferbin,pfil->fsockclient)==-1) {
      //if (fgets(vbuffer,sizeof(vbuffer)-1,pfil->fsockclient)==NULL) {
        //n'hi ha hagut error llegint o eof
        if (sidebug) {
	    flockfile(stderr);
            posa_missatge_fil_nocrlf(pfil->pid,"Error llegint de socket:");
            posa_error(errno);
	    funlockfile(stderr);
            pthread_exit(pfil);
        }
     }
    //treiem salt de linia final(CRLF) si existeix
    treu_crlf(pfil->bufferbin);
    if (sidebug) {
        flockfile(stderr);
        posa_missatge_fil_nocrlf(pfil->pid,"<<");
        fprintf(stderr,"%s\n",pfil->bufferbin);
        funlockfile(stderr);
    };
    if (strlen(pfil->bufferbin)==0) {
       //si n'hi ha hagut linia amb només salt de linia,significa final de petició
       //si la comanda ha sigut GET o HEAD
       lsortir=1;
    }
    if (siesvolcomprimit(pfil->bufferbin)==0 && strncasecmp(pfil->bufferbin,HTTP_CAMP_PROXY_CONNECTION,plongnomproxyconnection) && strncasecmp(pfil->bufferbin,HTTP_CAMP_CONNECTION,plongnomconnection) && strncasecmp(pfil->bufferbin,HTTP_CAMP_KEEP_ALIVE,plongnomkeepalive)) {
      //copiem linia llegida si la linia no conté en l'inici ni 'Connection: '
      //ni 'Proxy-Connection: '
      //i si no s'especifica que es vol contingut comprimit
      //i l'afegim a la llista de linies
      if ((plin=afegeix_linia(plin,pfil->bufferbin))==NULL) {
	//n'hi ha hagut error
	posa_missatge_fil(pfil->pid,"Error de memòria");
	pthread_exit(pfil);
      }
      if (pfil->linies==NULL) {
	//si n'era la primera linia
	pfil->linies=plin;
      }
    }
    else {
      if (sidebug) {
	flockfile(stderr);
	posa_missatge_fil_nocrlf(pfil->pid,"Camp ");
	fprintf(stderr,"%s",pfil->bufferbin);
	fprintf(stderr," ignorat\n");
	funlockfile(stderr);
      }
    }
}
//alliberem memoria
if (pfil->bufferbin!=NULL){
  free(pfil->bufferbin);
  pfil->bufferbin=NULL;
  longbufferbin=0;
}
//ara hem llegit la petició 
//anem a processar la linia de petició que és la primera linia, o sigui
//es la que especifica quina URL vol obtenir el client
//el format d'aquesta primera linia és:
//  mètode <SP> URI <SP> versió <CRLF>
//  <SP> és un espai
//  CRLF son els codis 13 y 10
//  métode ha de ser "GET", "POST" o "HEAD" que són els mètodes de la especificació HTTP/1.0
//  URL és l'adreça que es vol obtenir, el proxy només enten protocol http://
//  Versió és opcional (si no está s'enten HTTP/0.9)està en el format HTTP/x.y, on x és el
// número de versió i y el de subversió del
//  protocol HTTP, el nostre proxy només accepta fins a la versió 1.0 (inclosa)
if ((lerror=processa_peticio(pfil->linies->texte,lurl,&lport,&tipuscomanda))!=0) {
    //n'hi ha hagut error en la petició, ja sigui que el format de la linia de la
    //petició és incorrecte o no suportat o que l'adreça a la que es vol accedir
    //está a la llista del filtre i , per tant , es prohibida
    char *ttmissatges[]={
    //missatge curt   ,   missatge llarg
    "No hi ha linia de petició","No hi ha petició",
    "No URI","No hi ha URI",
    "Protocol no suportat","Aquest servidor proxy nomes suporta el protocol http://",
    "NO hi ha URL","Escrigui una URL en el navegador",
    "Versio HTTP no suportada","Aquest proxy nomes suporta fins la versio 1.0 de HTTP",
    "Metode desconegut","Aquest proxy nomes suporta GET, POST o HEAD",
    "Format versio HTTP erroni","El format de l'especificacio d'HTTP es erroni",
    "Peticio erronia","HEAD nomes esta implementat a partir d'HTTP 1.0",
    "Error intern","No hauria de sortir aquest error",
    };
    int ti=8,tlerror=HTTP_ERROR_INTERN_SERVIDOR;


    
    if (lerror==PROCESA_ERR_NO_LIN || lerror==PROCESA_ERR_NO_URI ||
     lerror==PROCESA_ERR_PROTOCOL_NO_SOPORTAT || lerror==PROCESA_ERR_NO_URL ||
     lerror==PROCESA_ERR_VERSIO_HTTP_NO_SUPORTAT ||
     lerror==PROCESA_ERR_FORMAT_VERSIO_HTTP_ERRONI ||
     lerror==PROCESA_ERR_HEAD_A_PARTIR_HTTP_10)
        tlerror=HTTP_PETICIO_ERRONIA;
    if (lerror==PROCESA_ERR_NO_LIN) ti=0;
    if (lerror==PROCESA_ERR_NO_URI) ti=1;
    if (lerror==PROCESA_ERR_PROTOCOL_NO_SOPORTAT) ti=2;
    if (lerror==PROCESA_ERR_NO_URL) ti=3;
    if (lerror==PROCESA_ERR_VERSIO_HTTP_NO_SUPORTAT) ti=4;
    if (lerror==PROCESA_ERR_METODE_DESCONEGUT)
        tlerror=HTTP_OPERACIO_NO_IMPLEMENTADA;
    if (lerror==PROCESA_ERR_METODE_DESCONEGUT) ti=5;
    if (lerror==PROCESA_ERR_FORMAT_VERSIO_HTTP_ERRONI) ti=6;
    if (lerror==PROCESA_ERR_HEAD_A_PARTIR_HTTP_10) ti=7;
    pthread_testcancel();
    envia_missatge_error(pfil->fsockclient,tlerror,ttmissatges[ti*2],ttmissatges[(ti*2)+1],
    retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
    //finalitzem fil
    pthread_exit(pfil);
}
//afegeix numero de port a URL si el numero de port no és el 80
afegeixportaurl(lurl,lport,lurlcerca);
//ara mirem si url està en la llista de urls prohibides
pthread_testcancel();
//ara convertim només l'adreça web a minuscules
p=lurlcerca;
while ((*p)!=0 && (*p)!='/' && (*p)!=' ' && (*p)!=':') {
  *p=tolower(*p);
  ++p;
}
twebenc=0;
//ara fem la cerca, primer cerquem url sencera
//pero primer bloquejem mutex mutex_url per evitar race condition
//que pot consistir en que un fil cerca i un altre fil afegeix paraules a la
//llista(el del socket local)
pthread_mutex_lock(&mutex_url);
//ara ja podem fer la cerca tranquilament
pelem=cerca_element(configuracio->urls,lurlcerca);
//ja podem desbloquejar mutex
pthread_mutex_unlock(&mutex_url);
pthread_testcancel();
if (pelem==NULL) {
    //no ha trobat URL,
    //ara cerquem només maquina (ex:www.yahoo.es, www.miweb.com:8854)
    p=strchr(lurlcerca,'/');
    if (p!=NULL) {
        *p=0;
        pthread_testcancel();
        //bloquejem mutex
        pthread_mutex_lock(&mutex_url);
        //ara ja podem fer la cerca tranquilament
        pelem=cerca_element(configuracio->urls,lurlcerca);
        //ja podem desbloquejar mutex
        pthread_mutex_unlock(&mutex_url);
        pthread_testcancel();
        //restaurem caràcter
        *p='/';
	twebenc=1;
    }
}
if (pelem!=NULL) {
    struct timeval ttv;
    //ha trobat l'element, o sigui,està a la llista de urls o
    //maquina (ex:www.yahoo.es, www.miweb.com:8854) de la URL està a la llista de urls
    gettimeofday(&ttv,NULL);
    //en entorn multithread, cada thread(fil) te el seu buffer per la crida a
    //inet_ntoa(el buffer estatic que retorna es diferent a cada fil, però
    //subsecuents crides a la funció en el mateix fil si que reemplaça
    //el seu valor)
    char lturl[LONG_MAX_PETICIO_HTTP+5]; //buffer temporal per afegir el numero
    //de port a la url si el port no és el 80
  
    afegeixportaurl(lurl,lport,lturl);
    if (escriu_fitxer_log((twebenc==1) ? WEBBLOQ : URLBLOQ,lturl,&ldatainici,&ttv,inet_ntoa(pfil->infocl.sin_addr),NULL)) {
        posa_missatge_fil_nocrlf(pfil->pid,"Error escrivint fitxer log:");
        posa_error(errno);
    }
    //eviem error al client
    envia_missatge_error(pfil->fsockclient,HTTP_PROHIBIT,(twebenc==1) ? "Plana prohibida" : "URL prohibida",(twebenc==1) ? "Aquesta plana es prohibida" : "Aquesta URL es prohibida",retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
    //finalitzem fil
    pthread_exit(pfil);
}
//alliberem memoria
if (pfil->bufferbin!=NULL)
  free(pfil->bufferbin);
//inicialitzem buffer de lectura
//si tot va be, o el necessitarem amb la comanda POST o despres
if ((pfil->bufferbin=malloc(LONG_BUFFER_BINARI))==NULL) {
  //n'hi ha hagut error
  posa_missatge_fil(pfil->pid,"Error de memòria");
  pthread_exit(pfil);
}
//mirem si la comanda ha sigut POST
if (tipuscomanda==ID_POST) {
    int ttrobatcamp=0,tlonghttpcampcontentlength;
    struct linia *tlinact;
    char *p;
    
    //cerquem el camp Content-Length
    tlinact=pfil->linies;
    tlonghttpcampcontentlength=strlen(HTTP_CAMP_CONTENT_LENGTH);
    while (tlinact!=NULL) {
      if (!strncasecmp(HTTP_CAMP_CONTENT_LENGTH,retorna_cadena_linia(tlinact),tlonghttpcampcontentlength)) {
            //trobat el camp
            ttrobatcamp=1;
	    break;
      }
      tlinact=seguent_linia(tlinact);
    };
    if (!ttrobatcamp) {
        //camp no trobat
        envia_missatge_error(pfil->fsockclient,HTTP_LONGITUD_REQUERIDA,"Longitud requerida",
        "Es requereix especificar la longitud amb la comanda POST",
        retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
        //finalitzem fil
        pthread_exit(pfil);
    }
    p=retorna_cadena_linia(tlinact);
    p+=strlen(HTTP_CAMP_CONTENT_LENGTH);
    //convertim valor a long
    sscanf(p,"%lu",&tqueda);
    
} //fin de if(tipuscomanda==ID_POST)


//ara cerquem ip del servidor web
urltemp=strdup(lurl);
//finalitzem la cadena quan trobem el primer '/'
p=strchr(urltemp,'/');
if (p!=NULL) *p=0;
//apuntem p despres de 'http://'
p=urltemp;
//p=nom plana web
//cerquem ip de la plana web


int longbuffer;
int ttipusfitxer=CONTINGUT_HTML,itemp;
int aparegutlong; //indica si ha aparegut el camp que indica la longitud del contingut que envia el servidor web (Content-Length: )
struct hostent resultipsbuffer,*resultips;
struct in_addr *paddr;
struct sockaddr_in cweb;
char lcontenttype[80],*lpt,*lpt2;
char *httpvers10="HTTP/1.0";
int tvarfiltre; //aquesta variable és per al filtre
struct element *tpelement ;//ens servira perque el filtre apunti a la paraula prohibida
char ppeticio[512]; 
int contingutgzipped=0; //si =1 ens indica que el contingut del fitxer que retorna el servidor web està comprimit amb gzip




longbuffer=1024;
pfil->buffer2=malloc(longbuffer);
if (pfil->buffer2==NULL) {
  //si n'hi ha hagut error de memòria
  posa_missatge_fil(pfil->pid,"Error de memòria");
  pthread_exit(pfil);
}
if (sidebug) {
    char lttbuffer2[LONG_MAX_PETICIO_HTTP];
    lttbuffer2[sizeof(lttbuffer2)-1]=0;
    snprintf(lttbuffer2,sizeof(lttbuffer2)-1,"Cercant adreça IP de %s",p);
    posa_missatge_fil(pfil->pid,lttbuffer2);
}
while ((lres=gethostbyname_r(p,&resultipsbuffer,pfil->buffer2,longbuffer,&resultips,&lerror))==ERANGE) {
    //n'hi ha hagut error de memòria, hem d'assignar més memòria al buffer temporal
    longbuffer<<=1;
    pfil->buffer2=realloc(pfil->buffer2,longbuffer);
    if (pfil->buffer2==NULL) {
      //no es pot assignar més memòria
      posa_missatge_fil(pfil->pid,"Error de memòria");
      pthread_exit(pfil);
    }
    pthread_testcancel();

}
if (lres || resultips==NULL) {
    //n'hi ha hagut error
    char *tterrors[]= {
    "HOST no trobat",
    "Servidor no s'ha pogut contactar",
    "Error no recuperable",
    "No s'ha trobat direccio IP",
    "Error desconegut",
    };
    int ti;
    
    ti=4; //per defecte, error desconegut
    if (lerror==HOST_NOT_FOUND) ti=0;
    if (lerror==TRY_AGAIN) ti=1;
    if (lerror==NO_RECOVERY) ti=2;
    if (lerror==NO_ADDRESS) ti=3;
    if(sidebug)  {
	char ttbuffer3[256];
	ttbuffer3[sizeof(ttbuffer3)-1]=0;
        snprintf(ttbuffer3,sizeof(ttbuffer3)-1,"Error %u cercant adreça IP: %s",lres,tterrors[ti]);
        posa_missatge_fil(pfil->pid,ttbuffer3);
    }
    envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error DNS",tterrors[ti],retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
    pthread_exit(pfil);
}
//no ha donat error de DNS, ens intentarem connectar
//creem socket
if ((pfil->sockweb=socket(PF_INET,SOCK_STREAM,0))==-1) {
    posa_missatge_fil(pfil->pid,"Error creant socket");
    envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error de socket","Error creant socket de connexio",retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
    pthread_exit(pfil);
}
//mig preprarem connexió
cweb.sin_family=AF_INET;
cweb.sin_port=htons(lport);
//fem bucle, un host pot denir diverses adreces IP
i=0;
while ((p=resultips->h_addr_list[i])!=NULL) {
    paddr=(struct in_addr *)p;
    cweb.sin_addr.s_addr=paddr->s_addr;
    if (sidebug) {
	char ttbuffer3[256];
	ttbuffer3[sizeof(ttbuffer3)-1]=0;
	snprintf(ttbuffer3,sizeof(ttbuffer3)-1,"Intent de connexió a %s",inet_ntoa(*paddr));
	posa_missatge_fil(pfil->pid,ttbuffer3);
    };
    //"connect crida a pthread_testcancel()"
    if (connect(pfil->sockweb,(struct sockaddr *)&cweb,sizeof cweb)==0) 
	//no hi ha error de connexió,sortim del bucle
	break;
    else {
        ++i; //incrementem comptador perque llegeixi l'altra IP si n'hi ha
	if (sidebug) {
	    flockfile(stderr);
	    posa_missatge_fil_nocrlf(pfil->pid,"Error de connexió :");
	    posa_error(errno);
	    funlockfile(stderr);
	}
    }
}
if (p==NULL) {
    //no s'ha pogut connectar a cap de les IP's del host
    envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error de connexio",
    "No s'ha pogut connectar a servidor web",retorna_cadena_linia(pfil->linies),pfil,tipuscomanda);
    pthread_exit(pfil);
}
if (sidebug)
    posa_missatge_fil(pfil->pid,"Connexió OK");
pthread_testcancel();
//copiem primera linia de petició
ppeticio[sizeof(ppeticio)-1]=0;
strncpy(ppeticio,retorna_cadena_linia(pfil->linies),sizeof(ppeticio)-1);
//ara creem la linia de reemplaç,la que reemplaçarà a la primera linia que hem
//d'enviar al servidor web

//dupliquem primera linia
//llegim longitud de linia original
longbuffer=strlen(retorna_cadena_linia(pfil->linies));
//li afegim longitud de "HTTP/1.0" i la longitud d'un espai més final de cadena
longbuffer+=strlen(httpvers10)+4;
//copiem cadena original a buffer temporal
if (pfil->buffer2!=NULL)
   //primer alliberem memòria
   free(pfil->buffer2);
//ara assignm memòria
if((pfil->buffer2=malloc(longbuffer))==NULL) {
  //n'hi ha hagut error de memòria
  posa_missatge_fil(pfil->pid,"Error de memòria");
  pthread_exit(pfil);
}
strcpy(pfil->buffer2,retorna_cadena_linia(pfil->linies));
lpt=pfil->buffer2;
//descartem possibles espais inicials
while ((*lpt)==' ') ++lpt;
//ens movem després de la comanda (GET,POST,HEAD)
lpt=strchr(lpt,' ');
if (lpt==NULL) {
  //no pot ser NULL
  posa_missatge_fil(pfil->pid,"Error intern, no hauria de sortir, s'ha"
		    " produït quan lpt=strchr(lpt,' ') ha retornat NULL");
  pthread_exit(pfil);
}
//ens movem a l'URI, descartant els espais
while ((*lpt)==' ') ++lpt;
lpt2=lpt;
lpt2=strchr(lpt2,' '); //cerquem espai
if (lpt2==NULL) {
  //si no el trobem, 
  lpt2=strchr(lpt,0); //cerquem final de cadena
  *lpt2=' '; //possem un espai en el final de cadena
}
++lpt2;
*lpt2=0; //"tenquem cadena despres de l'espai"
//ara descartem "http://" ja que el proxy només soporta http://
lpt2=lpt; //lpt2=inici URI
lpt2+=7; // 7=longitud("http://");
lpt2=strchr(lpt2,'/'); //cerquem /
if (lpt2==NULL) {
    //no hem trobat el '/' després de http://
    //afegim "/ \x0"
    *lpt='/';
    ++lpt;
    *lpt=' ';
    ++lpt;
    *lpt=0;
    lpt-=2; //actualitzem punter perque apunti a '/'
}
else {
  //no podem usar strcpy quan la cadena d'origen i destí es solapen
  //i segons la informació de info, no se sap com actuarà strcpy
  //usarem memmove
  int ttlongit;
  ttlongit=strlen(lpt2)+1; //guardem longitud de "cadena" de lpt2 sumant-li el 
  //0 de final de cadena
  memmove(lpt,lpt2,ttlongit);
}
lpt=strchr(lpt,' ');
if (lpt==NULL) {
  //no pot ser NULL
  posa_missatge_fil(pfil->pid,"Error intern, no hauria de sortir, s'ha "
		    "produït en el segon lpt=strchr(lpt,' '), ha retornat NULL");
  pthread_exit(pfil);
}
++lpt;
strcpy(lpt,httpvers10); //reemplaçem el que possi després de la URI, o sigui
//reemplaçem l'especificació de la versió d'HTTP a HTTP/1.0
if ((pfil->linreemp=afegeix_linia(pfil->linreemp,pfil->buffer2))==NULL) {
   //error de memòria
   posa_missatge_fil(pfil->pid,"Error de memòria");
   pthread_exit(pfil);
}
//ja podem alliberar memòria usada per la linia de reemplaç (el buffer, no la linia de reemplaç)
if (pfil->buffer2!=NULL) {
  free(pfil->buffer2);
  pfil->buffer2=NULL;
}
if ((pfil->fsockweb=fdopen(pfil->sockweb,"r+"))==NULL) {
    //n'hi ha hagut error
    flockfile(stderr);
    posa_missatge_fil_nocrlf(pfil->pid,"Error en la funció fdopen:");
    posa_error(errno);
    funlockfile(stderr);
    envia_missatge_error(pfil->fsockclient,HTTP_ERROR_INTERN_SERVIDOR,"Error intern del proxy",
    "Error intern del proxy: error en fdopen",ppeticio,pfil,tipuscomanda);
    pthread_exit(pfil);
}
//ens connectem al servidor i si la comanda era POST, enviarem la petició
//al servidor juntament am el primer bloque llegit i anirem llegit peticio i enviant-la
//al servidor web
//si la comanda no era POST
//enviem el que ens ha enviat el client
itemp=1; //1=enviar primera linia,2=enviar resta,3=enviat resta
//primer enviem la linia de reemplaç
//despres la resta (sense enviar la primera linia original)
 if (sidebug)
   posa_missatge_fil(pfil->pid,"Enviant petició a servidor web");

 while (itemp!=3) {
   if (envia_linies((itemp==1) ? pfil->linreemp : seguent_linia(pfil->linies),pfil->fsockweb,pfil,1,1)) {
     //n'hi ha hagut error
     if (sidebug) {
       flockfile(stderr);
       posa_missatge_fil_nocrlf(pfil->pid,"Error enviant a plana web:");
       posa_error(errno);
       funlockfile(stderr);
     }
     //enviem l'error
     envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error enviant dades","Error enviant dades",ppeticio,pfil,tipuscomanda);	
     pthread_exit(pfil);
   }
   ++itemp;
   fflush(pfil->fsockweb);//flushem buffer de connexió al servidor web perque envii totes les dades que tingui en el buffer
 }

 if (tipuscomanda==ID_POST) {
   //si la comanda era POST
    //si n'hi ha hagut error llegint retorna -1, si enviant, -2
    //si no hi ha hagut error, retorna 0
    i=transfereix_dades(pfil->fsockclient,pfil->fsockweb,tqueda,pfil->bufferbin,LONG_BUFFER_BINARI);
    if(i==-1) {
      //n'hi ha hagut error
      if (sidebug) {
	flockfile(stderr);
    	posa_missatge_fil_nocrlf(pfil->pid,"Error llegint:");
	posa_error(errno);
	funlockfile(stderr);
      }
      //enviem l'error
      envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error llegint dades",
      "Error llegint dades",ppeticio,pfil,tipuscomanda);	
      pthread_exit(pfil);
    }
    if(i==-2) {
      //n'hi ha hagut error
      if (sidebug) {
	flockfile(stderr);
    	posa_missatge_fil_nocrlf(pfil->pid,"Error enviant dades a plana web:");
	posa_error(errno);
	funlockfile(stderr);
      }
      //enviem l'error
      envia_missatge_error(pfil->fsockclient,HTTP_SERVEI_NO_DISPONIBLE,"Error enviant dades",
      "Error enviant dades a plana web",ppeticio,pfil,tipuscomanda);	
      pthread_exit(pfil);
    };
 }

//ara llegim la resposta per sapiguer si el que ens retorna el servidor
//web es html,o una altra cosa i quina és la seva longitud
lsortir=0;
i=0; //no hem llegit encara el valor del camp Content-Type
tqueda=0; //per defecte, longitud de contingut=0
if (sidebug)
   posa_missatge_fil(pfil->pid,"Llegint resposta del servidor web");


//alliberem buffer de les linies que hem llegit abans de la petició
//del client
destrueix_linies(pfil->linies);
pfil->linies=NULL;
aparegutlong=0;
plin=pfil->linies;
if (pfil->buffer2!=NULL) {
  free(pfil->buffer2);
  pfil->buffer2=NULL;
}
longbufferbin=0;
contingutgzipped=0;
while (lsortir==0) {
 //anem llegint resposta i enmagatzemant-la 
 pthread_testcancel();
 if (getline(&pfil->buffer2,&longbufferbin,pfil->fsockweb)==-1) {
      //if (fgets(vbuffer,sizeof(vbuffer)-1,pfil->fsockweb)==NULL) {
      //n'hi ha hagut error llegint o eof
      if (sidebug) {
	  flockfile(stderr);
          posa_missatge_fil_nocrlf(pfil->pid,"Error llegint de socket:");
          posa_error(errno);
	  funlockfile(stderr);
      }
      pthread_exit(pfil);
 }
 pthread_testcancel();
 //emmagatzem el que llegim
 if ((plin=(afegeix_linia(plin,pfil->buffer2)))==NULL) {
   //n'hi ha hagut error de memòria
   posa_missatge_fil(pfil->pid,"Error de memòria");
   envia_missatge_error(pfil->fsockclient,HTTP_ERROR_INTERN_SERVIDOR,"Error intern del proxy","Error intern del proxy: error de memòria",ppeticio,pfil,tipuscomanda);
   pthread_exit(pfil);
 }
 if (pfil->linies==NULL) {
   //si hem afegit la primera linia, guardem el punter a on correspon
   pfil->linies=plin;
 }
 if (sidebug) {
   posa_missatge_fil_nocrlf(pfil->pid,"<"); //amb '<' indiquem el que ens envia el servidor de la plana web
   flockfile(stderr);
   fprintf(stderr,"%s",pfil->buffer2);
   funlockfile(stderr);
 };
 treu_crlf(pfil->buffer2); //treien crlf
 if (strlen(pfil->buffer2)==0) {
    //si n'hi ha hagut linia amb només salt de linia(CRLF),significa final de petició
    //si la comanda ha sigut GET o HEAD
    lsortir=1;
 }
 else {
   //mirem si ja hem rebut el Content-Type: 
   if (!strncasecmp(pfil->buffer2,HTTP_CAMP_CONTENT_TYPE,strlen(HTTP_CAMP_CONTENT_TYPE))) {
     p=pfil->buffer2;
     p+=strlen(HTTP_CAMP_CONTENT_TYPE);
     //copiem el contingut
     strncpy(lcontenttype,p,sizeof(lcontenttype)-1);
     i=1; //ja hem llegit el valor del camp
 }
   else {
     if (!strncasecmp(HTTP_CAMP_CONTENT_LENGTH,pfil->buffer2,
		      strlen(HTTP_CAMP_CONTENT_LENGTH))) {
       //hem trobat el camp que diu la longitud del fitxer
       p=pfil->buffer2;
       p+=strlen(HTTP_CAMP_CONTENT_LENGTH);
       sscanf(p,"%lu",&tqueda);
       aparegutlong=1;
     }
     else {
       /*if (!strncasecmp(HTTP_CAMP_CONTINGUT_GZIPPED,vbuffer,
	 strlen(HTTP_CAMP_CONTINGUT_GZIPPED))) {*/
       if (sirespostacomprimida(pfil->buffer2)) {
	 //hem trobat capçalera que ens indica contingut comprimit amb gzip
	 contingutgzipped=1;
       }
     }
   }
 }
}
//ja hem llegit la resposta
if (i==0) {
   //no ha aparegut el camp Content-type
   //assumim que el seu valor es text/html
  strncpy(lcontenttype,"text/html",sizeof(lcontenttype)-1);
  i=1; //hem trobat un format que podem filtrar
}
else {
   i=0;
   if (!strncasecmp(lcontenttype,pnomcontenttexthtml,strlen(pnomcontenttexthtml))) {
     //text/html el podem filtrar
     i=1;
     ttipusfitxer=CONTINGUT_HTML;
   }
   if (!strncasecmp(lcontenttype,pnomcontenttext,strlen(pnomcontenttext))) {
     //text/plain el podem filtrar
     i=1;
     ttipusfitxer=CONTINGUT_TEXT;
   }
}
if (i==0 || contingutgzipped==1 || retorna_numelements(configuracio->paraules)==0) {
  //es un tipus que no podem filtrar o està comprimit el contingut amb gzip o
  //no tenim paraules a la llista de paraules prohibides
  //si n'hi ha hagut error llegint retorna -1, si enviant, -2
  //si no hi ha hagut error, retorna 0
  if (sidebug && i==1)
    posa_missatge_fil(pfil->pid,"No n'hi han paraules a la llista de prohibides, filtre html/texte no usat");
  if (envia_linies(pfil->linies,pfil->fsockclient,pfil,0,1)!=0) {
    //error enviant resposta(la 1a part(HTTP/1.0 200 OK, Content-Type: ...)
    // no el contingut del fitxer) del servidor web al client
    if (sidebug) {
      flockfile(stderr);
      posa_missatge_fil_nocrlf(pfil->pid,"Error enviant resposta al client: ");
      posa_error(errno);
      funlockfile(stderr);
    }
    pthread_exit(pfil);
  }
  if (tipuscomanda!=ID_HEAD) {
    //si la comanda no era HEAD
    if (sidebug)
      posa_missatge_fil(pfil->pid,
  		        "transferint fitxer desde servidor web a client");
    if (aparegutlong==1) {
      //si sabem la longitud, o sigui, a aparegut la longitud
      i=transfereix_dades(pfil->fsockweb,pfil->fsockclient,tqueda,pfil->bufferbin,LONG_BUFFER_BINARI);
      if (i==-1) {
        if (sidebug) {
          flockfile(stderr);
          posa_missatge_fil_nocrlf(pfil->pid,"Error llegint dades desde el servidor web:");
          posa_error(errno);
          funlockfile(stderr);
        }
      }
      pthread_exit(pfil);
      if (i==-2) {
        if (sidebug) {
          flockfile(stderr);
          posa_missatge_fil_nocrlf(pfil->pid,"Error enviant dades:");
          posa_error(errno);
          funlockfile(stderr);
        }
        pthread_exit(pfil);
      }
    }
    else {
      //no ha aparegut
      size_t ttllegit;
      //anem llegint i enviant al client mentre no n'hi hagi error
      while (1==1) {
	pthread_testcancel();
	ttllegit=fread(pfil->bufferbin,1,LONG_BUFFER_BINARI,pfil->fsockweb);
	if (ttllegit!=0)
	  //si numero de dades llegides!=0
	  //ho enviem al client
	  fwrite(pfil->bufferbin,1,ttllegit,pfil->fsockclient);
	if (ferror(pfil->fsockweb) || feof(pfil->fsockweb) || ferror(pfil->fsockclient) || feof(pfil->fsockclient))
	  //n'hi ha hagut error o eof en lectura o escritura
	  break;
      }
    }
    if (sidebug)
      posa_missatge_fil(pfil->pid,"transferencia de fitxer completada");
  }
  if (tipuscomanda!=ID_POST && tipuscomanda!=ID_HEAD) {
    //si comanda no era POST
    //escribim log
    char lturl[LONG_MAX_PETICIO_HTTP+5];
  
    afegeixportaurl(lurl,lport,lturl);
    gettimeofday(&ldatafi,NULL); //comptador temps final
    escriu_fitxer_log(URLOK,lturl,&ldatainici,&ldatafi,
		      inet_ntoa(pfil->infocl.sin_addr),NULL);
  }
  pthread_exit(pfil);
}
//si hem arribat aqui es que el fitxer es un tipus que podem filtrar
//o es html o es un fitxer texte
//alliberem memòria que ja no usarem
//com que filtrarem texte, no treurem el salt de linia
//ja que el salt de linia no es el mateix en UNIX que en WINDOWS
//alliberem memòria que ja no usem
if (pfil->bufferbin!=NULL) {
  free(pfil->bufferbin);
  pfil->bufferbin=NULL;
}
if (tipuscomanda!=ID_HEAD) {
  //si la comanda no era head
  //inicialitzem filtre
  size_t tplongbuffer=0;
  inicialitza_filtre(&tvarfiltre);
  if (sidebug) {
    char ttttbuffer[80];
     sprintf(ttttbuffer,"Filtrant plana %s",(ttipusfitxer==CONTINGUT_HTML) ? "html" : "texte");
    posa_missatge_fil(pfil->pid,ttttbuffer);
  }
  plin=pfil->linies;
  fflush(pfil->fsockweb);
  while((tqueda>0 && aparegutlong==1) || aparegutlong==0) {
    //llegim mentres quedi bytes per llegir
    pthread_testcancel();
    if (getline(&pfil->bufferbin,&tplongbuffer,pfil->fsockweb)==-1) {
    //if (fgets(vbuffer,sizeof(vbuffer)-1,pfil->fsockweb)==NULL) {
      //n'hi ha hagut error llegint o eof
      int terrno;
      terrno=ferror(pfil->fsockweb);
      if (terrno==ESPIPE || feof(pfil->fsockweb))  { //ESPIPE=29,ILLEGAL SEEK
	 //si ha sigut eof, sortim del bucle
          break;
      }
      if (sidebug) {
        flockfile(stderr);
        posa_missatge_fil_nocrlf(pfil->pid,"Error llegint de socket:");
        posa_error(errno);
        funlockfile(stderr);
      }
      //sortim del bucle perque envii el que ha llegit fins ara
      break;
    }
    if ((plin=(afegeix_linia(plin,pfil->bufferbin)))==NULL) {
     //n'hi ha hagut error de memòria
     posa_missatge_fil(pfil->pid,"Error de memòria");
     envia_missatge_error(pfil->fsockclient,HTTP_ERROR_INTERN_SERVIDOR,"Error intern del proxy","Error intern del proxy: error de memòria",ppeticio,pfil,tipuscomanda);
      pthread_exit(pfil);   
  }
    if (pfil->linies==NULL) {
      //si hem afegit la primera linia, guardem el punter a on correspon
      pfil->linies=plin;
    }
    if (filtra(pfil->bufferbin,ttipusfitxer,configuracio->paraules,&tvarfiltre,&tpelement,&mutex_paraules)==-1) {
      //si retorna -1 és que ha trobat una paraula prohibida
      char lttbuffer[512];
      char lturl[LONG_MAX_PETICIO_HTTP+5];
  
      afegeixportaurl(lurl,lport,lturl);
      gettimeofday(&ldatafi,NULL); ///comptador temps final
      escriu_fitxer_log(PARAULABLOQ,lturl,&ldatainici,&ldatafi,
		      inet_ntoa(pfil->infocl.sin_addr),retorna_cadena(tpelement));
      snprintf(lttbuffer,sizeof(lttbuffer)-1,"No es pot accedir a la plana , ha "
  	     "sortit <I><U>%s</U></I> que es una paraula prohibida",retorna_cadena(tpelement));
      envia_missatge_error(pfil->fsockclient,HTTP_OK,"Paraula prohibida",lttbuffer,ppeticio,pfil,tipuscomanda);
      if (sidebug) {
        posa_missatge_fil(pfil->pid,lttbuffer);
      }
      pthread_exit(pfil);
    }  
    tqueda-=strlen(pfil->bufferbin); //descomptem bytes llegits
  }
}


//si hem arribat aqui, el fitxer html o texte ha passat el filtre
//o la comanda era HEAD
//ja el podem enviar al client
plin=pfil->linies;
if (sidebug) {
   char ttbuffer[256];
   sprintf(ttbuffer,"Plana %s ha passat filtre, enviant al client",(ttipusfitxer==CONTINGUT_HTML) ? "html" : "texte");
   posa_missatge_fil(pfil->pid,ttbuffer);
}
//enviem tot(la resposta del servidor web i el contingut del fitxer de la
//resposta)
//el contingut del fitxer, per suposadament, només s'envia si la comanda no era
//HEAD. Si era HEAD només s'enviarà la resposta del servidor web
if(envia_linies(pfil->linies,pfil->fsockclient,pfil,0,0)!=0) {
  //n'hi ha hagut error enviat les linies al client
  if (sidebug) {
    flockfile(stderr);
    posa_missatge_fil_nocrlf(pfil->pid,"Error enviant resposta i fitxer al client:");
    posa_error(errno);
    funlockfile(stderr);
  }
  pthread_exit(pfil);
}
fflush(pfil->fsockclient); //netegem buffer perque envii el que quedi en
//el buffer
if (sidebug)
  posa_missatge_fil(pfil->pid,"Enviat");
//si arribem aqui és que el texte html o texte ha passat el filtre i
// ja l'hem enviat correctament
 if (tipuscomanda!=ID_POST && tipuscomanda!=ID_HEAD) {
   //si comanda no era POST
   //escribim log
   char lturl[LONG_MAX_PETICIO_HTTP+5];
   
   afegeixportaurl(lurl,lport,lturl);
   gettimeofday(&ldatafi,NULL); //comptador temps final
   escriu_fitxer_log(URLOK,lturl,&ldatainici,&ldatafi,
		  inet_ntoa(pfil->infocl.sin_addr),NULL);
 };

//en finalitzar en fil, desregistrem el cleanup handler, fent-ho amb el parèmetre 1 crida a 
//la funció del cleanup handler
pthread_cleanup_pop(1);
return pfil;
}




//funció que es crida abans de finalitzar el fil de la connexió al socket local
void cleanup_fil_socket_local(void *pargs) {


  //tenquem sockets
  if (fsockcli_socket_local!=NULL)
    fclose(fsockcli_socket_local);
else
  close(sockclient_socket_local);
  sockclient_socket_local=0;
  fsockcli_socket_local=NULL;
  pid_fil_socket_local=0;
  //alliberem memoria
  free(buffer_local);
  buffer_local=NULL;
  long_buffer_local=0;

}



//funcio local
//escriu un missatge per finestra possat a l'inici 'Fil socket local:'
//paràmetre d'entrada:
//  pmiss:missatge a escriure
void escriu_fil_socket_local(char *pmiss) {
  flockfile(stderr);
  fprintf(stderr,"Fil del socket local:%s",pmiss);
  funlockfile(stderr);
}







//fil per manejar les connexions al socket local
//pel socket local es poden afegir paraules i urls dinàmicament al filtre
void* fil_socket_local(void* pargs) {
  enum tcontingut {ID_PARAULES=0,ID_URLS};
  int pcontingut,i;
  pthread_mutex_t *pmutexact; //apunta al mutex que usarem, ja sigui el de urls o el de paraules
  struct llista *tllistaact;
  char *p,*p2;


  //registrem el cleanup handler, o sigui, registrem la funció que es cridarà quan el fil
  //finalitzi
  pthread_cleanup_push(cleanup_fil_socket_local,pargs);
  //obrim un FILE * amb la connexió amb el socket local
  if((fsockcli_socket_local=fdopen(sockclient_socket_local,"r"))==NULL) {
    //n'hi ha hagut error
    flockfile(stderr);
    escriu_fil_socket_local("Error en fdopen:");
    posa_error(errno);
    funlockfile(stderr);
  }
  //llegim primera linia
  if (getline(&buffer_local,&long_buffer_local,fsockcli_socket_local)==-1) {
    //n'hi ha hagut error
    if (sidebug) {
      flockfile(stderr);
      escriu_fil_socket_local("Error en fgets:");
      posa_error(errno);
      funlockfile(stderr);
    }
    //finalitzem fil
    pthread_exit(pargs);
  }
  //treiem CRLF
  treu_crlf(buffer_local);
  //mirem el que posa la primera linia
  if (!strcasecmp(buffer_local,CONTINGUT_PARAULES)) {
    //es paraules
    pcontingut=ID_PARAULES;
    pmutexact=&mutex_paraules;
    tllistaact=configuracio->paraules;
    escriu_fil_socket_local("Afegint paraules a llista de prohibides\n");
  }
  else {
    //si no
    if (!strcasecmp(buffer_local,CONTINGUT_URLS)) {
      //es urls
      pcontingut=ID_URLS;
      pmutexact=&mutex_url;
      tllistaact=configuracio->urls;
      escriu_fil_socket_local("Afegint urls a llista de prohibides\n");
    }
    else {
      //no es ni paraules no urls
      if (sidebug) {
	char vbuffer2[512];

	vbuffer2[sizeof(vbuffer2-1)]=0;
	snprintf(vbuffer2,sizeof(vbuffer2)-1,"Primera linia no conté ni %s ni %s""\n",CONTINGUT_PARAULES,CONTINGUT_URLS);
	escriu_fil_socket_local(vbuffer2);
      }
      //finalitzem fil
      pthread_exit(pargs);
    }
  }
  //si arribem aqui és que el contingut son paraules o urls
  i=0;
  //bloquejem mutex corresponent
  pthread_mutex_lock(pmutexact);
  while (getline(&buffer_local,&long_buffer_local,fsockcli_socket_local)!=-1) {
    //mentres no n'hi hagi error llegint, o sigui, mentres no es tanqui la 
    //connexió o no n'hi hagi error
    if (sidebug) {
      escriu_fil_socket_local(buffer_local);
    }
    //treiem crlf
    treu_crlf(buffer_local);
    //descartem possibles espais inicials
    p=buffer_local;
    while ((*p)==' ') ++p;
    //descartem possibles espais finals
    p2=strchr(p,' ');
    if (p2!=NULL)
      *p2=0;
    if (strlen(p)==0)
      //si es una linia en blanc, la ignorem
      continue;
    if (pcontingut==ID_PARAULES) {
      //si s'afegeixen paraules
      //convertim paraula a minuscules
      aminuscules(p);
    }
    if (pcontingut==ID_URLS) {
      //si s'afegeixen urls
      //es passa a minuscules només la part que indica la plana web
      p2=p;
      while ((*p2)!=0 && (*p2)!='/') {
	*p2=tolower(*p2);
	++p2;
      }
    }
    //afegim element llegit
    if (afegeix_element(tllistaact,p)!=0) {
      //si n'hi ha hagut error afegint element
      escriu_fil_socket_local("Error afegint element, sense memòria\n");
      //sortim del bucle
      break;
    }
    ++i;
    if ((i%5)==0) {
      //si i multiple de 5
      //desbloquejem mutex
      pthread_mutex_unlock(pmutexact);
      //mirem si s'ha intentat finalitzar aquest fil
      pthread_testcancel();
      //bloquejem mutex altre cop
      pthread_mutex_lock(pmutexact);
    }
  }
  //desbloquejem mutex
  pthread_mutex_unlock(pmutexact);
  escriu_fil_socket_local("Finalitzat afegint\n");
  //actualiza fitxer de configuracio
  if (guarda_configuracio(configuracio,NOM_FITXER_CFG)!=0) {
    escriu_fil_socket_local("Error escrivint fitxer de configuracio:");
    posa_error(errno);
  }
  //en finalitzar en fil, desregistrem el cleanup handler, fent-ho amb el parèmetre 1 crida a 
  //la funció del cleanup handler
  pthread_cleanup_pop(1);
  return pargs;
}















//funcio principal
int main(int argc, char *argv[]) {
//de moment només suporta l'argument -h i -v
int nomesparametres=0;
fd_set pllista,esperant;


//possa capçalera
fputs(NOM_SERVIDOR" Versió "VERSIO_SERVIDOR" 25/3/2005      Autor: David Hernandez " "Baño\n",stderr);
//llegeix paràmetres
do
    switch(getopt(argc,argv,"hv")) {
    case '?':
        //s'ha possat una opció no suportada
        fprintf(stderr,"Opció %c no reconeguda\n",optopt);
    case 'h':
        //-h
        fputs("Paràmetres possibles:\n",stderr);
        fputs("  -h: Mostra aquest missatge d'ajuda\n",stderr);
        fputs("  -v: Mostra missatges opcionals de depurat\n",stderr);
        return 0;
    case 'v':
        //-v
        sidebug=1;
        break;
    case -1:
        nomesparametres=1;
}while (!nomesparametres);

if (sidebug)
    fputs("Activats missatges de depurat\n",stderr);
if (llegeix_configuracio())  return -1;
//obrim fitxer de log en mode escritura, afegint continguts
if ((flog=fopen(NOM_FITXER_LOG,"a"))==NULL) {
    fputs("Error obrint fitxer log en mode escritura per afegir contingut:",stderr);
    posa_error(errno);
    fputs("El fitxer de log no s'actualizarà\n",stderr);
};
if (inicialitza_socket_servidor()) return -1;
if (inicialitza_socket_local()) return -1;
//assignem memòria per a la estructura dels fils
if ((fils=(struct fil *)malloc(sizeof(struct fil)))==NULL) {
    fputs("Error assignant memòria per emmagatzemar estructura per als fils\n",stderr);
    return -1;
}
//possem tots els camps a 0
memset(fils,0,sizeof(struct fil));
//inicialitza atribut que tindran tots els fils (o sigui, per especififar que son DETACHED)
pthread_attr_init(&atribut_fil);
pthread_attr_setdetachstate(&atribut_fil,PTHREAD_CREATE_DETACHED);
//definim signal traps, si es rep SIGINT , SIGQUIT o SIGTERM es crida a ctrl_break
signal(SIGINT,ctrl_break);
signal(SIGQUIT,ctrl_break);
signal(SIGTERM,ctrl_break);
signal(SIGPIPE,SIG_IGN); //s'ignora senyal SIGPIPE
FD_ZERO(&pllista); //inicialitzem la llista de sockets a esperar connexió de client
FD_SET(socket_s,&pllista); //afegim socket del servidor proxy HTTP
FD_SET(socket_local,&pllista); //afegim socket del socket local
//bucle espera d'un client, ja sigui per al servidor proxy http o per al socket local
while (1) {
    esperant=pllista;
    if (select(FD_SETSIZE, &esperant,NULL,NULL,NULL)<0) {
        //si n'hi ha hagut error esperant connexió
        flockfile(stderr);
        fputs("Error esperant connexió:",stderr);
        posa_error(errno);
        funlockfile(stderr);
        break;
    }
    if (FD_ISSET(socket_s,&esperant)) {
        int sockclient;
        struct sockaddr_in ladr;
        socklen_t long_adr=sizeof(ladr);
        struct fil *pfilact,*pfilant;
        
        //s'ha rebut petició per al proxy http
        //l'accepta
        if (sidebug) {
            flockfile(stderr);
            fputs("Nova petició per al proxy http\n",stderr);
            funlockfile(stderr);
        }
        if ((sockclient=accept(socket_s,(struct sockaddr *)&ladr,&long_adr))==-1) {
            //n'hi ha hagut error acceptant petició
            flockfile(stderr);
            fputs("Error acceptant petició:",stderr);
            posa_error(errno);
	    funlockfile(stderr);
            //ignora petició i n'espera una altra
            continue;
        }
        if (sidebug) {
            flockfile(stderr);
            fprintf(stderr,"Acceptada petició del client amb IP %s Port %d\n",
            inet_ntoa(ladr.sin_addr),ntohs(ladr.sin_port));
            funlockfile(stderr);
            }
        pfilact=fils;
        pfilant=fils;
        //cerquem entrada lliure
        while (pfilact!=NULL) {
            if (pfilact->ocupat==0) break;
            pfilant=pfilact;
            pfilact=pfilact->segfil;
        }
        if (pfilact==NULL) {
            //no n'hem trobat entrada lliure
            //creem entrada
            if ((pfilant->segfil=malloc(sizeof(struct fil)))==NULL) {
                flockfile(stderr);
                fputs("Error assignant memòria per poder posar fil a la estructura de fils\n"
                "El fil no es pot crear\n",stderr);
                funlockfile(stderr);
                //anem a la següent petició
                close(sockclient);
                continue;
            }
            pfilact=pfilant->segfil;
            //possem entrades a NULL
            memset(pfilact,0,sizeof(struct fil));
            //possem camp de fil anterior
            pfilact->antfil=pfilant;
            //possem camp de fil seguent (de l'anterior fil)
            pfilact->antfil->segfil=pfilact;
       }
       pfilact->ocupat=1;
       pfilact->sockclient=sockclient;
       //copiem informació del client
       memcpy(&pfilact->infocl,&ladr,sizeof(struct sockaddr_in));
       //creem fil
       if (pthread_create(&pfilact->pid,&atribut_fil,&fil_proxy_http,(void *)pfilact)!=0) {
	 //error creant fil
	 flockfile(stderr);
	 fputs("Error creant fil per a petició del proxy:",stderr);
	 posa_error(errno);
	 funlockfile(stderr);
       }
       if (sidebug) {
            flockfile(stderr);
            fprintf(stderr,"Fil creat amb pid %lu\n",(unsigned long int)pfilact->pid);
            funlockfile(stderr);
       }
       }
    if (FD_ISSET(socket_local,&esperant)) {
        struct sockaddr_in ladr;
        socklen_t long_adr=sizeof(ladr);
        //s'ha rebut petició pel socket local
        flockfile(stderr);
        fputs("Petició de connexió pel socket local\n",stderr);
        funlockfile(stderr);
	//acceptem connexio
        if ((sockclient_socket_local=accept(socket_local,(struct sockaddr *)&ladr,&long_adr))==-1) {
            //n'hi ha hagut error acceptant petició
            flockfile(stderr);
            fputs("Error acceptant petició:",stderr);
            posa_error(errno);
	    funlockfile(stderr);
            //ignora petició i n'espera una altra
            continue;
        }
	if (sidebug) {
	  flockfile(stderr);
	  fputs("Nova petició per al socket local\n",stderr);
	  funlockfile(stderr);
	}
	//creem fil, com que només es permet una sola connexió al socket local
	//no fa falta usar cap estructura com a les peticions per al proxy
	if (pthread_create(&pid_fil_socket_local,&atribut_fil,&fil_socket_local,NULL)!=0) {
	 //error creant fil
	 flockfile(stderr);
	 fputs("Error creant fil per a petició del socket local:",stderr);
	 posa_error(errno);
	 funlockfile(stderr);
       }
	if (sidebug) {
	  flockfile(stderr);
	  fprintf(stderr,"Creat fil amb pid %lu per a manejar connexió pel socket local\n",(unsigned long int)&pid_fil_socket_local);
	  funlockfile(stderr);
	}
    }
}

ctrl_break(0);
return 0;
};

