#ifndef __CONFIGURACIO_H
#define __CONFIGURACIO_H
//fitxer cap�alera de configuracio.c
//fitxer configuracio.h
#include "llista.h"
#include "llista_amb_valor.h"



//defines
#define NOM_SECCIO_PARAULES    "paraules"  //nom que t� la secci� paraules
#define NOM_SECCIO_URLS        "urls"  //nom que t� la secci� urls
#define NOM_SECCIO_CONFIG_GLOBAL   "global" //nom que t� la secci� global
#define NOM_OPCIO_PORT              "port"  //nom de l'opci� del port
#define NOM_OPCIO_BIND              "bind"  //nom de l'opci� de bind
#define CONFIG_ERR_MEM          -1 //error assignant mem�ria
#define CONFIG_ERR_ESCRIVINT    -2 //error escrivint fitxer de configuraci�
#define CONFIG_ERR_LLEGINT      -3 //error llegint fitxer de configuraci�

//estructures
struct configuracio {
    struct llista_amb_valor *global_config; //configuraci� global
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
