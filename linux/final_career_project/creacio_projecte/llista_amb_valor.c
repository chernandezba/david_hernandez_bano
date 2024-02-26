// fitxer llista.c
// conte les rutines per manejar llistes ordenades de cadenes (char *)
// la llista conté una marca de final que consisteix en posar a NULL el valor de cadena i
// i el valor del valor
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "llista_amb_valor.h"



//defines-----------------------------------------------------------

//numero d'elements per bloc. Per no haver de canviar continuament la longitud
//del bloc de memòria que allotja els elements, es va assignant memòria perque capiguen
//n (ELEMENTS_PER_BLOC) cada cop que es necessiti
#define ELEMENTS_PER_BLOC 256








//funcions--------------------------------------------------------



//inicialitza una llista
//no té paràmetres d'entrada
//retorna una llista inicialitzada
//si retorna NULL es que no n'hi havia memòria per inicialitzar la llista
struct llista_amb_valor * inicialitza_llista_amb_valor(void) {

struct llista_amb_valor *rllista;


//li assigna memòria
rllista=(struct llista_amb_valor *) malloc(sizeof(struct llista_amb_valor));
if (rllista!=NULL) {
    //si s'ha pogut assignar memòria
    //possem valors camps a 0
    memset((void *)rllista,0,sizeof(struct llista_amb_valor));
}
//assignem primer bloc de memòria per als elements
if ((rllista->elements=(struct element_amb_valor *)(calloc(ELEMENTS_PER_BLOC,sizeof (struct element_amb_valor))))==NULL) return NULL;
//possa la marca de final
rllista->elements[0].cadena=NULL;
rllista->elements[0].valor=NULL;
return rllista;
}






//funció per a la comparació de 2 elements, serveix per ordenar i per fer la cerca
static int elem_compara_amb_valor(const void *a,const void *b) {
struct element_amb_valor *ta,*tb;


ta=(struct element_amb_valor *)a;
tb=(struct element_amb_valor *)b;
return strcmp(ta->cadena,tb->cadena);
}







//destrueix una llista
//llibera la memòria assignada a la llista, també la de cada element
//com a paràmetre d'entrada es passa la llista a destruir
void destrueix_llista_amb_valor(struct llista_amb_valor *pllista) {

    int i;
    

if (pllista==NULL) return; //si es NULL, no fa res
//allibera memòria de cada element
for (i=pllista->numelements;i>0;--i) {
    if (pllista->elements[i-1].cadena!=NULL) free(pllista->elements[i-1].cadena);
    if (pllista->elements[i-1].valor!=NULL) free(pllista->elements[i-1].valor);
}
if (pllista->elements!=NULL) free(pllista->elements);
}






//afegeix un element a la llista
//i l'ordena, ja que es una llista ordenada
//si tot va be, retorna 0
//si ha d'assignar més memòria per als elements
//i no pot, retorna -1
int afegeix_element_amb_valor(struct llista_amb_valor *pllista, char *pelement,char *pvalor) {

struct element_amb_valor *pe;


pe=cerca_element_amb_valor(pllista,pelement);
if (pe!=NULL) {
    //si ja existeix, només reemplaça el seu valor
    //primer allibera memòria del valor
    if (pe->valor!=NULL) free(pe->valor);
    if ((pe->valor=strdup(pvalor))==NULL) return -1;
    return 0;
}
pllista->numelements++;
//mirem si hem d'assignar un altre bloc per a mes elements
if (((pllista->numelements + 1) % ELEMENTS_PER_BLOC)==0) {
   //quan reassignem memòria no fa falta copiar res si la memòria
   //del bloc es mou a una altra direcció ni alliberar memòria
   //aquesta rutina ho fa automaticament
    pllista->elements=(struct element_amb_valor *) realloc(pllista->elements,(((pllista->numelements +1)
    / ELEMENTS_PER_BLOC)+1) * ELEMENTS_PER_BLOC * sizeof(struct element_amb_valor));
    if (pllista->elements==NULL) return -1;
}
//primer copiem cadena , assignant memòria per a la cadena
if ((pllista->elements[pllista->numelements-1].cadena=strdup(pelement))==NULL) return -1;
//ara el valor , assignant memòria per a la cadena
if ((pllista->elements[pllista->numelements-1].valor=strdup(pvalor))==NULL) return -1;

//ara afegim la marca de final
pllista->elements[pllista->numelements].cadena=NULL;
pllista->elements[pllista->numelements].valor=NULL;
//ara ordenem els elements si tenim més d'un
if (pllista->numelements>1) 
  qsort((void *)pllista->elements,pllista->numelements,sizeof(struct element_amb_valor), elem_compara_amb_valor);
return 0;
}







//cerca element
//retorna punter a element
//els parametres d'entrada son la llista i l'element(cadena) a 
//cercar de la llista, si no el troba, retorna NULL
struct element_amb_valor * cerca_element_amb_valor(struct llista_amb_valor * pllista, char *pcadena) {
struct element_amb_valor pb;

if (pllista->numelements==0) 
    //si no hi ha elements, no el pot trobar
    return NULL;
if (pllista->numelements==1) {
    //si només n'hi ha un element
    if (!strcmp(pllista->elements->cadena,pcadena))
        //si el primer element es el que cerquem
        return pllista->elements;
    else
        return NULL;
    }
//ho fem perque l'element a cercar a de ser del mateix tipus que els elements de la
//llista
pb.cadena=pcadena;
return (struct element_amb_valor *)(bsearch(&pb, (void *)pllista->elements,pllista->numelements,sizeof(struct element_amb_valor),elem_compara_amb_valor));
}



//retorna un punter per poder recòrrer tots els
//elements de la llista
//retorna un punter tipus struct element *
//per trobar el final només s'ha de comprobar que el camp cadena i el camp valor no siguin
//NULL i que per accedir al següent element, només s'ha d'incrementar en 1
//la variable de tipus struct element * ja que els elements son consecutius
//el paràmetre d'entrada es la llista
struct element_amb_valor * recorre_elements_amb_valor(struct llista_amb_valor * pllista) {

return pllista->elements;

}





//mira si s'ha arrivat al final recorrent elements usant recorre_elements_amb_valor
//si s'ha arribat, retorna 1, si no, retorna 0
int esfinal_amb_valor(struct element_amb_valor *pelement) {
if (pelement->cadena==NULL && pelement->valor==NULL) 
    return 1;
else
    return 0;
}








//esborra un element de la llista
//si no el troba, no fa res
//els paràmetres son la llista i l'element a esborrar(char *)
//retorna 0 si tot ha anat bé o si no l'ha trobat, 
//-1 si ha hagut un error en reassignar memòria
int esborra_element_amb_valor(struct llista_amb_valor * pllista,char *pelement) {
struct element_amb_valor *pec,*ppe;
int pindex;
void *pmem,*pmem2;

ppe=&pllista->elements[0];
pec=cerca_element_amb_valor(pllista,pelement);
if (pec==NULL) return 0; //si no el troba, retorna sense fer res
//esbrina l'ordre de l'element en la llista, si es el primer, segon,...
pindex=pec-ppe; 
//guarda punter de la cadena per alliberar memòria
pmem=(void *)pllista->elements[pindex].cadena;
pmem2=(void *)pllista->elements[pindex].valor;
if (pindex==0) {
    //si es el primer
    if (pllista->numelements==1) {
        //si només n'hi ha un element
        //posa la marca de fi en el primer element i posa numelements=0
        pllista->elements[0].cadena=NULL;
        pllista->elements[0].valor=NULL;
    }
    else {
        //si n'hi ha més d'un
        //mou els demés,inclos la marca de final
        memcpy((void *)&pllista->elements[0],(void *)&pllista->elements[1],
        pllista->numelements * sizeof(struct element_amb_valor));
        
        
    }
}
else {
    //si no es el primer
    if (pindex==(pllista->numelements-1)) {
        //si es l'últim
        //possa la marca de final
        pllista->elements[pllista->numelements-1].cadena=NULL;
        pllista->elements[pllista->numelements-1].valor=NULL;
    }
    else {
        //si no és l'últim
        //mou els elements necessaris
        memcpy((void *)(&pllista->elements[pindex]),(void *)(&pllista->elements[pindex+1]),
        ((pllista->numelements-1) - pindex) * sizeof (struct element_amb_valor));
        }
    }
//actualitza el comptador de número d'elements
pllista->elements--;        
//allibera memòria de la cadena de l'element eliminat
if (pmem!=NULL) free(pmem);
if (pmem2!=NULL) free(pmem2);
//desassigna memòria si fa falta
if (((pllista->numelements+2) % ELEMENTS_PER_BLOC) == 0) {
            pllista->elements=(struct element_amb_valor
            *)realloc(pllista->elements,(((pllista->numelements +1)
            / ELEMENTS_PER_BLOC)+1) * ELEMENTS_PER_BLOC * sizeof(struct element_amb_valor));
            if (pllista->elements==NULL) return -1;
        }
return 0;
}





//retorna_valor
//retorna el valor l'element
//argument d'entrada:
// pelement: element
//retorna el valor
char *retorna_valor(struct element_amb_valor *pelement) {
  return pelement->valor;
}


//retorna_element_del_valor
//retorna l'element (pelement->cadena)
//argument d'entrada:
// pelement: element
//retorna el valor
char *retorna_element_del_valor(struct element_amb_valor *pelement) {
  return pelement->cadena;
}








//retorna el numero d'elements de la llista
//argument d'entrada:
// pllista:llista
//retorna el numero d'elements
unsigned int retorna_numelements_amb_valor(struct llista_amb_valor *pllista) {

  return pllista->numelements;

}
