//fitxer linia.c
//conte les linies per manejar linies
//en realitat usa una llista enlla�ada de cadenes (char *), cada linia esta enlla�ada
//amb la linia anterior i posterior
#include <string.h>
#include <stdlib.h>
#include "linia.h"





//afegeix una linia
//els par�metres d'entrada son:
//  plin:estructura tipus linia * que conte l'estructura a on s'ha d'afegir la linia,si es
//  NULL, crea una nova estructura tipus linia per afegir linies i retorna aquesta
//  nova estructura amb pcadena
//  no fa falta passar la �ltima linia afegida ja que la rutina la cerca
//  pcadena:cadena a afegir(char *)
//retorna la linia afegida,si n'hi ha hagut error de mem�ria, retorna NULL
struct linia *afegeix_linia(struct linia *plin,char *pcadena) {

struct linia *pnewlin,*plinact;


//assigna mem�ria
if ((pnewlin=malloc(sizeof(struct linia)))==NULL) {
    return NULL;
}
//possa els camps a 0
memset(pnewlin,0,sizeof(struct linia));
//possa els valors al camp de cadena
pnewlin->texte=strdup(pcadena);
if (pnewlin->texte==NULL) {
    //si n'hi ha hagut error de mem�ria
    free(pnewlin);
    return NULL;
}
//busca ultima linia afegida
if (plin!=NULL) {
    plinact=plin;
    while(plinact->seglinia!=NULL) plinact=plinact->seglinia;
    //ja la tenim
    //possem els camps
    pnewlin->antlinia=plinact;
    plinact->seglinia=pnewlin;
    return pnewlin;
}
else {
    //no n'hi havia linies
    return pnewlin;
}
}







//retorna la cadena de la linia actual
//el par�metre d'entrada es la linia actual
char *retorna_cadena_linia(struct linia *plin) {
return plin->texte;
}




//retorna la seg�ent linia
//si es l'�ltima retorna NULL, o sigui, si despres de l'actual no n'hi ha mes
//el parametre d'entrada es la linia actual
struct linia *seguent_linia(struct linia *plin) {

return plin->seglinia;
}




//retorna la linia anterior
//si es la primera, retorna NULL, o sigui, si la linia actual �s la primera
//el parametre d'entrada es la linia actual
struct linia *anterior_linia(struct linia *plin) {

return plin->antlinia;
}





//destrueix les linies, allibera mem�ria
//o sigui, allibera la mem�ria assignada a totes les linies (inclosa la primera)
//el parametre d'entrada es la primera linia,per� si no es la primera no passa res,
//funciona igualment
void destrueix_linies(struct linia *plin) {

struct linia *plin2,*plin3;



//primer anem a l'�ltima linia
plin2=plin;
while (plin2->seglinia!=NULL) plin2=plin2->seglinia;
//ara anem alliberant mem�ria
while (plin2!=NULL) {
    plin3=plin2;
    plin2=plin2->antlinia;
    if (plin3->texte!=NULL) free(plin3->texte);
    free(plin3);
};
}




