#ifndef __CONFIGURACIO_H
#define __CONFIGURACIO_H
//fitxer capçalera de configuracio.c
//fitxer configuracio.h
#include "llista.h"
#include "llista_amb_valor.h"



//defines
#define NOM_SECCIO_PARAULES    "paraules"  //nom que té la secció paraules
#define NOM_SECCIO_URLS        "urls"  //nom que té la secció urls
#define NOM_SECCIO_CONFIG_GLOBAL   "global" //nom que té la secció global
#define NOM_OPCIO_PORT              "port"  //nom de l'opció del port
#define NOM_OPCIO_BIND              "bind"  //nom de l'opció de bind
#define CONFIG_ERR_MEM          -1 //error assignant memòria
#define CONFIG_ERR_ESCRIVINT    -2 //error escrivint fitxer de configuració
#define CONFIG_ERR_LLEGINT      -3 //error llegint fitxer de configuració

//estructures
struct configuracio {
    struct llista_amb_valor *global_config; //configuració global
    struct llista *paraules; //llista de paraules
    struct llista *urls; //llista de URLs
    };



//funcions----------------------------------------------------------------------
struct configuracio * obre_configuracio(char *pnom_fitxer);
int guarda_configuracio(struct configuracio *pconfiguracio,char *pnomfitxer);
void tenca_configuracio(struct configuracio * pconfiguracio);
int config_error(void);
void posa_error_config(void);
#endif
