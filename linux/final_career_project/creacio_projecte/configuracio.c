//fitxer configuracio.h
//Llegeix la configuraci� que est� enmagatzemada en un fitxer
//Les paraules i les webs a filtre les enmagatzemada cadascuna
//en una variable tipus struct llista
//La configuraci� del servidor la emmagatzema en una variable
//tipus struct llista_amb_valor
//El fitxer de configuraci� t� el seg�ent format:
//  -Si una linia comen�a amb '#', es considerar� com a comentari
//  -Si comen�a amb '[' especifica el comen�ament d'una nova secci�, el final del nom de la
//  secci� ha d'acabar amb ']'
//  -Per a definir una variable amb el seu valor, es posa el nom de la variable, seguit
//  de '=' i seguit del seu valor. Els espais al comen�ament i al final de la variable i el
//  seu valor no es tindran en compte. Si falta '=' la variable no es tindra en compte.
//  -Tots els noms de seccions, els noms de variables junt amb el seu valor, cada element
//  de la secci� de webs i cada element de la secci� de paraules es pasaran tots a min�scules
//  -La secci� [global] es a on s'especifica els par�metres del servidor proxy. Consta d'un
//  munt de variables amb el seu valor. Els valors possibles de les variables son:
//      -port : Es un valor num�ric que especifica el port a on escoltar� les peticions. Si //      el nom del port es menor de 1024, s'hauran de tenir permissos de root per poder-ho
//      assignar.
//      -bind: Es una adre�a IP que especifica la interficie per on la qual escoltar� les
//      peticions. Si es igual a 127.0.0.1 nom�s escoltar� per la interf�cie loopback (o   
//      sigui, nom�s peticions desde el mateix ordinador), si es 0.0.0.0 llavors escoltar�
//      peticions desde qualsevol adre�a IP i si �s igual a la adre�a IP assignada a la tarja
//      de xarxa local, nom�s escoltar� peticions que provinguin desde la xarxa local.
//
//  -La seccio [paraules] conte les paraules que faran que la plana no passi el filtre. O
//  sigui, si l'adre�a cont� una de les paraules especificades en aquesta secci�, no passar�
//  el filtre i no es veur�. Cont� una llista de paraules, en cada linia, una paraula.
//  -La secci� [urls] cont� les adreces que no es volen que es puguin accedir. Cada url no 
//  ha de contenir l''http://'  de la URL. Pot contenir fitxers espec�fics. Cada URL que es
//  possi es comparar� amb la URL que el client fica en la petici�. Per mirar si la plana 
//  passa o no el filtre, primer es mirar� si l'adre�a web de la petici�, sense cap mena de
//  refer�ncia a cap fitxer, est� en la llista de URLs, si no est�, mira si la petici� sencera
//  sense tenir en compte l''http://' est� en la llista, si no est�, la plana passar� el
//  primer filtre, o sigui , el filtre de urls, si apareix en les 2 comparacions d'abans,no
//  passar� el filtre i no es mostrar� pel navegador. Cada linia cont� una URL.



#include "configuracio.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>



//variables locals

static int l_error_config=0;  //especifica si n'hi ha hagut error en l'us de les funcions
//obre_configuracio, guarda_configuracio, tencar_configuracio .Si =0 es que no hi ha hagut
// error . El seu significat del seu valor est� als defines que comencen amb CONFIG_ERR_



//funcions-----------------------------------------------------------------------

// assigna mem�ria per a una configuraci�, usant malloc
// i possa els camps a 0
static struct configuracio * inicialitza_configuracio(void) {
struct configuracio *pc;

pc=malloc(sizeof (struct configuracio));
if (pc!=NULL)
    memset(pc,0,sizeof(struct configuracio));
return pc;
}




//destrueix configuraci�, allibera mem�ria assignada a la configuraci�
static void destrueix_configuracio(struct configuracio *pconfiguracio) {

destrueix_llista_amb_valor(pconfiguracio->global_config);
destrueix_llista(pconfiguracio->paraules);
destrueix_llista(pconfiguracio->urls);
free(pconfiguracio);

}





//retorna una configuracio per defecte
//si no ha pogut assignar mem�ria, retorna NULL
static struct configuracio * config_default(void) {

struct configuracio *pconfiguracio;
char *pelementscg[]= {
NOM_OPCIO_PORT,
#ifdef PROVES
//si esta definit PROVES, es defineix el nom de fitxer de configuracio, el de log
//i el port per defecte del servidor perque es pugui provar amb un compte d'usuari
//normal
    "8080",
#else
    "80",
#endif
NOM_OPCIO_BIND,"0.0.0.0"
};
char *pelementsp[]= {
"sexe",
"sexo",
"xxx"
};
char *pelementsu[]= {
"www.sexofree.org",
"www.viendosexo.com"
};
int i;

if ((pconfiguracio=inicialitza_configuracio())==NULL) return NULL;
if ((pconfiguracio->global_config=inicialitza_llista_amb_valor())==NULL) return NULL;
if ((pconfiguracio->paraules=inicialitza_llista())==NULL) return NULL;
if ((pconfiguracio->urls=inicialitza_llista())==NULL) return NULL;
//afegeix la configuraci� global
for (i=0;i<(sizeof(pelementscg) / sizeof(char *)) /2 ;++i) {
    if (afegeix_element_amb_valor(pconfiguracio->global_config,pelementscg[(i*2)],
    pelementscg[(i*2)+1])!=0) return NULL;
};
//afegeix les paraules per defecte
for (i=0;i<sizeof(pelementsp) / sizeof(char *);++i) {
    if (afegeix_element(pconfiguracio->paraules,pelementsp[i])!=0) return NULL;
};
//afegeix les urls per defecte
for (i=0;i<sizeof(pelementsu) / sizeof(char *);++i) {
    if (afegeix_element(pconfiguracio->urls,pelementsu[i])!=0) return NULL;
};
return pconfiguracio;
}








// retorna valor de la variable l_error_cnfig
// aquesta variable mira si n'hi ha hagut error en la crida a
// obre_configuracio, guarda_configuracio o tencar_configuracio 
int config_error(void) {
return l_error_config;
}




//posa error segons variables l_error_config
void posa_error_config(void) {
int i;
char *errorscfg[]= {
"Error de mem�ria",
"Error escrivint fitxer de configuracio",
"Error llegint fitxer de configuracio",
"Error desconegut, no hauria de sortir"
};


if (l_error_config==CONFIG_ERR_MEM) i=0;
else {
    if (l_error_config==CONFIG_ERR_ESCRIVINT) i=1;
    else {
        if (l_error_config==CONFIG_ERR_LLEGINT) i=2;
        else {
            i=3;
        }
    }
}
fprintf(stderr,"%s\n",errorscfg[i]);
}



//obre un fitxer de configuraci�, si no existeix, crea un per defecte
//par�metre d'entrada �s el nom del fitxer, s'ha de poder escriure el fitxer
//retorna la configuracio en forma de struct configuracio *
//si ha hagut algun error en assignar mem�ria o creant el fitxer, retorna NULL
struct configuracio * obre_configuracio(char *pnom_fitxer) {

struct configuracio *pconfiguracio;
FILE *pf;
char buffer[256],*pi,*p;
int estat=-1; //si =-1 no estem en cap secci�, si =1 en global, si =2 en paraules, si =3 en
              // urls

l_error_config=0; //esborrem possible error
//creem configuracio per defecte
if ((pconfiguracio=config_default())==NULL) {
    l_error_config=CONFIG_ERR_MEM;
    return NULL;
}
pf=fopen(pnom_fitxer,"r");
if (pf==NULL) {
    //no hem pogut obrir el fitxer, el creem amb la configuraci� per defecte
    if (guarda_configuracio(pconfiguracio,pnom_fitxer)!=0) {
        //si ha hagut error creant el fitxer de configuraci�
        l_error_config=CONFIG_ERR_ESCRIVINT;
        return NULL;
    }
    else
        //si no ha hagut error
        return pconfiguracio;
}
//el fitxer existeix, anem a llegir-ho
while (fgets(buffer,sizeof(buffer)-1,pf)!=NULL) {
    if (ferror(pf)) {
        //error llegint fitxer
        l_error_config=CONFIG_ERR_LLEGINT;
        fclose(pf);
        return NULL;
    }
    //primer esborrem salt de l�nia, si n'hi ha
    treu_crlf(buffer);
    //punter inicial=primer caracter que no sigui espai
    pi=buffer;
    while ((*pi!=0) && (*pi==' ')) ++pi;
    p=strchr(pi,0);
    //ara treiem espais de final
    while ((p!=pi) && (*pi==' ')) --p;
    ++p;
    *p=0;
    if (strlen(pi)==0) continue; //si despres de treure els espais no queda res
    if(*pi=='#') continue; //si 1er car�cter es '#',ignorar la linia
    if (*pi!='[' && estat==-1) continue; //si trobem altra cosa que no sigui el comen�ament
                                         //de secci� i encara no s'ha trobat cap comen�ament
                                         //de secci�, saltem a la seg�ent linia
    if (*pi=='[') {
        //inici de secci�
        //mirem si tenim el corresponent ']'
        if ((p=strchr(pi,']'))==NULL) continue; //si no tenim el corresponent ']' saltem a la 
                                             //seg�ent linia
        //incrementem en 1 el punter de pi perque ignori '['
        ++pi;
        //si arribem aqui es que hem trobat el corresponent ']'
        *p=0;
        //convertim nom de secci� a min�scules
        aminuscules(pi);
        if (strcmp(pi,NOM_SECCIO_CONFIG_GLOBAL)==0) {
            //secci� global
            estat=1;
        }
        else {
            if (strcmp(pi,NOM_SECCIO_PARAULES)==0) {
                //seccio de paraules
                estat=2;
                //esborrem possible llista de paraules
                if (pconfiguracio->paraules!=NULL)                
                    destrueix_llista(pconfiguracio->paraules);
                //tornem a crear la llista
                if ((pconfiguracio->paraules=inicialitza_llista())==NULL) {
                    //n'hi ha hagut error
                    l_error_config=CONFIG_ERR_MEM;
                    fclose(pf);
                    return NULL;
                }
            }
            else {
                if (strcmp(pi,NOM_SECCIO_URLS)==0) {
                    //seccio de urls
                    estat=3;
                    //esborrem possible llista de urls
                    if (pconfiguracio->urls!=NULL)
                        destrueix_llista(pconfiguracio->urls);
                    //tornem a crear la llista
                    if ((pconfiguracio->urls=inicialitza_llista())==NULL) {
                        //n'hi ha hagut error
                        l_error_config=CONFIG_ERR_MEM;
                        fclose(pf);
                        return NULL;
                    }
                }
                else {
                    //no es la seccio de global, paraules o urls
                    //fem com si no estuguessim en cap secci�
                    estat=-1;
                }
           }
        }
    }
    else { //else 1
        //estem en una secci�
        //processem nom de variable i valor
        char *pcd,*token,*pnom,*pvalor;
        const char pdelimitadors[]="=";
        struct llista *pllista;
        
        pcd=strdup(pi); //dupliquem cadena, ja que fa falta per usar la funci� strtok
        token=strtok(pcd,pdelimitadors);
        pnom=strdup(token); //dupliquem el nom del nom de la variable
        //treiem espais al final del nom
        p=strchr(pnom,0);
        while (p!=pnom && *p==' ') --p;
        ++p;
        *p=0;
        token=strtok(NULL,pdelimitadors);
        if (token==NULL && estat==1) {
            //si ens retorna NULL estem en la secci� global es que no hi ha el caracter =
            //alliberem memoria de pnom i pcd
            free(pnom);
            free(pcd);
            continue; //ignorem la linia
        }
        if (estat==1) {
            //si estem en seccio global
            pvalor=strdup(token); //dupliquem el seu valor
            //treiem espais a l'inici de valor (el que fem es posicionar un punter a 
            //l'inici de la cadena, despres dels espais inicials
            p=pvalor;
            while (*p!=0 && *p==' ') ++p;
            //ho afegim 
            if (afegeix_element_amb_valor(pconfiguracio->global_config,pnom,p)!=0) {
                //si n'hi ha hagut error afegint element amb valor
                l_error_config=CONFIG_ERR_MEM;
                free(pnom);
                free(pvalor);
                free(pcd);
                fclose(pf);
                return NULL;
            }
            //alliberem mem�ria
            free(pnom);
            free(pvalor);
            free(pcd);
       }
       else { //else 2
         if (estat==2) 
           pllista=pconfiguracio->paraules;
         else
           pllista=pconfiguracio->urls;
         if (afegeix_element(pllista,pnom)!=0) {
            //si n'hi ha hagut error afegint element
            l_error_config=CONFIG_ERR_MEM;
            free(pnom);
            free(pcd);
            fclose(pf);
            return NULL;
         }
         //alliberem mem�ria
         free(pnom);
         free(pcd);
      }//fin de else 2
    }//fin de else 1
}//fin de while
fclose(pf);
return pconfiguracio;
}





//guarda la configuraci� emmagatzemada en l'estructura tipus configuracio
//1er par�metre �s el punter a una estructura tipus configuracio
//2on par�metre es el nom del fitxer
//retorna 0 si tot ha anat b�
//retorna -1 en cas d'error escrivint fitxer
int guarda_configuracio(struct configuracio *pconfiguracio,char *pnomfitxer) {

FILE *f;
struct element *pl=NULL;
struct element_amb_valor *plv;
int c;

//obrim fitxer en mode escritura, sobreescrivint possible fitxer amb el mateix nom
if ((f=fopen(pnomfitxer,"w"))==NULL) {
    //en cas d'error
    return -1;
}
//escrivim secci� global
fputs("["NOM_SECCIO_CONFIG_GLOBAL"]\n",f);
if (ferror(f)) {
    //n'hi ha hagut error
    fclose(f);
    return -1;
}
//escrivim variables amb valors
plv=recorre_elements_amb_valor(pconfiguracio->global_config);
while (esfinal_amb_valor(plv)==0) {
    //escrivim linia
    fprintf(f,"%s=%s\n",plv->cadena,plv->valor);
    if (ferror(f)) {
        //n'hi ha hagut error
        fclose(f);
        return -1;
    }
    //seg�ent element
    ++plv;
}
c=0; //0=paraules,1=urls
while (c<2) {
    if (c==0) {
        //escrivim secci� paraules
        fputs("["NOM_SECCIO_PARAULES"]\n",f);
        pl=recorre_elements(pconfiguracio->paraules);
    }
    if (c==1) {
        //escrivim secci� urls
        fputs("["NOM_SECCIO_URLS"]\n",f);
        pl=recorre_elements(pconfiguracio->urls);
    }
    if (ferror(f)) {
        //n'hi ha hagut error
        fclose(f);
        return -1;
    }
    while (esfinal(pl)==0) {
        //escrivim linia
        fprintf(f,"%s\n",pl->cadena);
        if (ferror(f)) {
            //n'hi ha hagut error
            fclose(f);
            return -1;
        }
        //seg�ent linia
        ++pl;
   }
   ++c; //seg�ent secci�
}
fclose(f);
return 0;
}








//tenca configuraci�, allibera mem�ria assignada a la configuraci�
//par�metre d'entrada es la configuraci� a "destruir"
void tenca_configuracio(struct configuracio * pconfiguracio) {

destrueix_configuracio(pconfiguracio);
}

