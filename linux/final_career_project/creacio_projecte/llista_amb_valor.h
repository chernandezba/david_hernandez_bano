// fitxer llista_amb_valor.h
// conte les definicions necessaries per a manejar llistes de cadenes ordenades (char *) 
// sense repetició amb valor , cada valor es una cadena (char *)
#ifndef __LLISTA_AMB_VALOR_H
#define __LLISTA_AMB_VALOR_H



//estructures----------------------------------------------------------------------


//definicio d'un element de la llista
struct element_amb_valor {
    char *cadena; //cadena de l'element, s'assigna memòria per emmagatzemar el seu valor
    char *valor; //valor de l'element, s'assigna memòria per emmagatzemar el seu valor
    
};

//definicio d'una llista amb valor
struct llista_amb_valor {
    unsigned int numelements; //numero d'elements de la llista
    struct element_amb_valor *elements; //elements de la llista
};





//funcions----------------------------------------------------------------------
struct llista_amb_valor * inicialitza_llista_amb_valor(void);
void destrueix_llista_amb_valor(struct llista_amb_valor *pllista);
int afegeix_element_amb_valor(struct llista_amb_valor * pllista, char *pelement,char *pvalor);
struct element_amb_valor * cerca_element_amb_valor(struct llista_amb_valor * pllista, char *pcadena);
struct element_amb_valor * recorre_elements_amb_valor(struct llista_amb_valor * pllista);
int esfinal_amb_valor(struct element_amb_valor *pelement);
int esborra_element_amb_valor(struct llista_amb_valor * pllista,char *pelement);
char *retorna_valor(struct element_amb_valor *pelement);
char *retorna_element_del_valor(struct element_amb_valor *pelement);
unsigned int retorna_numelements_amb_valor(struct llista_amb_valor *pllista);
#endif
