//fitxer linia.h
//conte les definicions pel fitxer linia.c

#ifndef __LINIA_H
#define __LINIA_H


//estructures-----------------------------------------------------------------
struct linia {
    char *texte; //texte de la linia
    struct linia *antlinia; //linia anterior, si es la primera, NULL
    struct linia *seglinia; //linia seguent, si és l'ultima, NULL
};
 

 
 
//funcions--------------------------------------------------------------------
struct linia *afegeix_linia(struct linia *plin,char *pcadena);
char *retorna_cadena_linia(struct linia *plin);
struct linia *seguent_linia(struct linia *plin);
struct linia *anterior_linia(struct linia *plin);
void destrueix_linies(struct linia *plin);






#endif
