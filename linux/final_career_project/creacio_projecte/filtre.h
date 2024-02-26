//fitxer filtre.h
//conte les definicions de filtre.c

#ifndef __FILTRE_H
#define __FILTRE_H
#include "llista.h"
#include <pthread.h>




//enums----------------------------------------------------


//aquests enum serveix per a la funcio filtre per especificar el tipus
//de contingut que conte el buffer que li enviem, si es texte o html
enum TIPUS_CONTINGUT {
  CONTINGUT_HTML=0,
  CONTINGUT_TEXT,};




//funcions--------------------------------------------------
void inicialitza_filtre(int *pvarfiltre);
int filtra(char *pbuffer,int ptipuscontingut,struct llista *pparaules,int *pvarfiltre,struct element **pelement,pthread_mutex_t *pmutexparaules);











#endif
