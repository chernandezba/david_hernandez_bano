// fitxer llista.h
// conte les definicions necessaries per a manejar llistes de cadenes ordenades (char *) sense
// repeticio
#ifndef __LLISTA_H
#define __LLISTA_H



//estructures----------------------------------------------------------------------


//definicio d'un element de la llista
struct element {
    char *cadena; //cadena de l'element, s'assigna memòria per emmagatzemar el seu valor
};

//definicio d'una llista
struct llista {
    int numelements; //numero d'elements de la llista
    struct element *elements; //elements de la llista
};





//funcions----------------------------------------------------------------------
struct llista * inicialitza_llista(void);
void destrueix_llista(struct llista *pllista);
int afegeix_element(struct llista * pllista, char *pelement);
struct element * cerca_element(struct llista * pllista, char *pcadena);
struct element * recorre_elements(struct llista * pllista);
int esfinal(struct element *pelement);
int esborra_element(struct llista * pllista,char *pelement);
char *retorna_cadena(struct element *pelement);
int retorna_numelements(struct llista *pllista);





#endif
