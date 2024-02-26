// fitxer llista.c
// conte les rutines per manejar llistes ordenades de cadenes (char *)
// la llista cont� una marca de final que consisteix en posar a NULL el valor de cadena
#include <string.h>
#include <stdlib.h>
#include "llista.h"

//numero d'elements per bloc. Per no haver de canviar continuament la longitud
//del bloc de mem�ria que allotja els elements, es va assignant mem�ria perque capiguen
//n (ELEMENTS_PER_BLOC) cada cop que es necessiti
#define ELEMENTS_PER_BLOC 256

//inicialitza una llista
//no t� par�metres d'entrada
//retorna una llista inicialitzada
//si retorna NULL es que no n'hi havia mem�ria per inicialitzar la llista
struct llista * inicialitza_llista(void) {

struct llista *rllista;


//li assigna mem�ria
rllista=(struct llista *) malloc(sizeof(struct llista));
if (rllista!=NULL) {
    //si s'ha pogut assignar mem�ria
    //possem valors camps a 0
    memset((void *)rllista,0,sizeof(struct llista));
}
//assignem primer bloc de mem�ria per als elements
if ((rllista->elements=(struct element *)(calloc(ELEMENTS_PER_BLOC,sizeof (struct element))))
==NULL) return NULL;
//possa la marca de final
rllista->elements[0].cadena=NULL;
return rllista;
}


//funci� per a la comparaci� de 2 elements, serveix per ordenar i per fer la cerca
static int elem_compara(const void *a,const void *b) {
struct element *ta,*tb;

ta=(struct element *)a;
tb=(struct element *)b;
return strcmp(ta->cadena,tb->cadena);
}


//destrueix una llista
//llibera la mem�ria assignada a la llista, tamb� la de cada element
//com a par�metre d'entrada es passa la llista a destruir
void destrueix_llista(struct llista *pllista) {

    int i;
    

if (pllista==NULL) return; //si es NULL,no fa res
//allibera mem�ria de cada element
for (i=pllista->numelements;i>0;--i) {
    if (pllista->elements[i-1].cadena!=NULL) free(pllista->elements[i-1].cadena);
}
if (pllista->elements!=NULL) free(pllista->elements);
}







//afegeix un element a la llista
//i l'ordena, ja que es una llista ordenada
//si tot va be, retorna 0
//si ha d'assignar m�s mem�ria per als elements
//i no pot, retorna -1
int afegeix_element(struct llista *pllista, char *pelement) {
struct element *pe;

//primer mirem si l'element existeix
pe=cerca_element(pllista,pelement);
if (pe!=NULL) {
    //si existeix
    //no fa res ja que ja hi �s
    return 0;
}   
pllista->numelements++;
//mirem si hem d'assignar un altre bloc per a mes elements
if (((pllista->numelements + 1) % ELEMENTS_PER_BLOC)==0) {
   //quan reassignem mem�ria no fa falta copiar res si la mem�ria
   //del bloc es mou a una altra direcci� ni alliberar mem�ria
   //aquesta rutina ho fa automaticament
    pllista->elements=(struct element *)realloc(pllista->elements,(((pllista->numelements +1)
    / ELEMENTS_PER_BLOC)+1) * ELEMENTS_PER_BLOC * sizeof(struct element));
    if (pllista->elements==NULL) return -1;
}
//copiem cadena, assignant mem�ria per a la cadena
if ((pllista->elements[pllista->numelements-1].cadena=strdup(pelement))==NULL) return -1;
//ara afegim la marca de final
pllista->elements[pllista->numelements].cadena=NULL;
//ara ordenem els elements si tenim m�s d'un
if (pllista->numelements>1) 
  qsort((void *)pllista->elements,pllista->numelements,sizeof(struct element),elem_compara);
return 0;
}






//cerca element
//retorna punter a element
//els parametres d'entrada son la llista i l'element(cadena) a 
//cercar de la llista, si no el troba, retorna NULL
struct element * cerca_element(struct llista * pllista, char *pcadena) {
struct element pb;


if (pllista->numelements==0) 
    //si no hi ha elements, no el pot trobar
    return NULL;
if (pllista->numelements==1) {
    //si nom�s n'hi ha un element
    if (!strcmp(pllista->elements->cadena,pcadena))
        return pllista->elements;
    else
        return NULL;
}
//ho fem perque l'element a cercar a de ser del mateix tipus que els elements de la
//llista
pb.cadena=pcadena;
return (struct element *)(bsearch(&pb, pllista->elements,pllista->numelements,
sizeof(struct element),elem_compara));
}



//retorna un punter per poder rec�rrer tots els
//elements de la llista
//retorna un punter tipus struct element *
//per trobar el final nom�s s'ha de comprobar que el camp cadena no sigui NULL
//per accedir al seg�ent element, nom�s s'ha d'incrementar en 1
//la variable de tipus struct element * ja que els elements son consecutius
//el par�metre d'entrada es la llista
struct element * recorre_elements(struct llista * pllista) {

return pllista->elements;

}




//mira si s'ha arrivat al final recorrent elements usant recorre_elements
//si s'ha arribat, retorna 1, si no, retorna 0
int esfinal(struct element *pelement) {
if (pelement->cadena==NULL) 
    return 1;
else
    return 0;
}




//esborra un element de la llista
//si no el troba, no fa res
//els par�metres son la llista i l'element a esborrar(char *)
//retorna 0 si tot ha anat b� o si no l'ha trobat, 
//-1 si ha hagut un error en reassignar mem�ria
int esborra_element(struct llista * pllista,char *pelement) {
struct element *pec,*ppe;
int pindex;
void *pmem;

ppe=&pllista->elements[0];
pec=cerca_element(pllista,pelement);
if (pec==NULL) return 0; //si no el troba, retorna sense fer res
//esbrina l'ordre de l'element en la llista, si es el primer, segon,...
pindex=pec-ppe; 
//guarda punter de la cadena per alliberar mem�ria
pmem=(void *)pllista->elements[pindex].cadena;
if (pindex==0) {
    //si es el primer
    if (pllista->numelements==1) {
        //si nom�s n'hi ha un element
        //posa la marca de fi en el primer element i posa numelements=0
        pllista->elements[0].cadena=NULL;
    }
    else {
        //si n'hi ha m�s d'un
        //mou els dem�s,inclos la marca de final
        memcpy((void *)&pllista->elements[0],(void *)&pllista->elements[1],
        pllista->numelements * sizeof(struct element));
        
        
    }
}
else {
    //si no es el primer
    if (pindex==(pllista->numelements-1)) {
        //si es l'�ltim
        //possa la marca de final
        pllista->elements[pllista->numelements-1].cadena=NULL;
    }
    else {
        //si no �s l'�ltim
        //mou els elements necessaris
        memcpy((void *)(&pllista->elements[pindex]),(void *)(&pllista->elements[pindex+1]),
        ((pllista->numelements-1) - pindex) * sizeof (struct element));
        }
    }
//actualitza el comptador de n�mero d'elements
pllista->elements--;        
//allibera mem�ria de la cadena de l'element eliminat
free(pmem);
//desassigna mem�ria si fa falta
if (((pllista->numelements+2) % ELEMENTS_PER_BLOC) == 0) {
            pllista->elements=(struct element
            *)realloc(pllista->elements,(((pllista->numelements +1)
            / ELEMENTS_PER_BLOC)+1) * ELEMENTS_PER_BLOC * sizeof(struct element));
            if (pllista->elements==NULL) return -1;
        }
return 0;
}







//retorna la cadena de l'element
char *retorna_cadena(struct element *pelement) {

  return pelement->cadena;
}






//retorna el numero d'elements de la llista
//par�metres:
//  pllista:pllista
// retorna: numero d'elements de pllista
int retorna_numelements(struct llista *pllista) {

  return pllista->numelements;


}
