#include <pebble.h>
#include "objetos.h"
    
struct objeto *inicializa_objeto(int ptipoobjeto,int pxinic,GBitmap *pimagenobjeto,
                                 GBitmap **panimaccaida, int pnumanimaccaida) {
    struct objeto *lobjeto;
    
    lobjeto=malloc(sizeof (struct objeto));
    inicializa_structura_objeto(lobjeto,ptipoobjeto,pxinic,pimagenobjeto,panimaccaida,
                               pnumanimaccaida,false,false);
    
    return lobjeto;
}

void inicializa_structura_objeto(struct objeto *pobjeto, int ptipoobjeto,int pxinic,
                                  GBitmap *pimagenobjeto,
                                 GBitmap **panimaccaida, int pnumanimaccaida,
                                 bool pnotocaanterior, bool pnotocasiguiente) {
    if (!pnotocaanterior) pobjeto->anterior=NULL;
    if (!pnotocasiguiente) pobjeto->siguiente=NULL;
    pobjeto->x=pxinic;
    pobjeto->y=0;
    pobjeto->tipo=ptipoobjeto;
    pobjeto->imagen=pimagenobjeto;
    pobjeto->animaccaida=panimaccaida;
    pobjeto->ianimaccaida=0;
    pobjeto->numanimaccaida=pnumanimaccaida;    
    pobjeto->activado=true;
    pobjeto->estado=0; //0=cayendo
}


void anyadeobjetoaobjetos(GBitmap *pobjeto,GBitmap **panimaccaida,
                          int pnumanimaccaida,
                          int ptipoobjeto, int pposx,
                          struct objeto *pprimobjeto) {
    struct objeto *lobjact, *lobjant;
    
    lobjact=pprimobjeto;
    lobjant=NULL;
    while (lobjact!=NULL) {
        if (!lobjact->activado) {
            //no está activado, usamos esta entrada
            break;
        }
        lobjant=lobjact;
        lobjact=lobjact->siguiente;
    }
    if (lobjact==NULL) {
        lobjact=inicializa_objeto(ptipoobjeto,pposx,
                                  pobjeto,panimaccaida,pnumanimaccaida);
        lobjact->anterior=lobjant;
        if (lobjant!=NULL) {
            lobjant->siguiente=lobjact;
        }
    }
    else {
        inicializa_structura_objeto(lobjact,ptipoobjeto,pposx,pobjeto,panimaccaida,
                                    pnumanimaccaida,true,true);
    }
    
}

void libera_memoria_objetos(struct objeto *pprimerobjeto) {
    struct objeto *lobjact, *lobjsave;
    
    lobjact=pprimerobjeto;
    while (lobjact!=NULL) {
        lobjsave=lobjact;
        lobjact=lobjact->siguiente;
        free(lobjsave);
        if (lobjact!=NULL)
            lobjact->anterior=NULL;
    }
}



void dibuja_objeto(struct objeto *pobjeto,  GContext *ctx) {
    
    
    if (pobjeto->activado) {
    graphics_draw_bitmap_in_rect(ctx,pobjeto->imagen,GRect(pobjeto->x,pobjeto->y,
                                           gbitmap_get_bounds(pobjeto->imagen).size.w,
                                           gbitmap_get_bounds(pobjeto->imagen).size.h));
    }
}

bool colision(struct objeto *pobjeto1, int pposxobj2,int pposyobj2, int panchoobj2, 
              int paltoobj2)  {
    int x1,x2,y1,y2;
    int x3,x4,y3,y4;
    GRect pmag;
    
    x1=pobjeto1->x;
    y1=pobjeto1->y;
    pmag=gbitmap_get_bounds(pobjeto1->imagen);
    x2=x1+pmag.size.w;
    y2=y1+pmag.size.h;
    x3=pposxobj2;
    y3=pposyobj2;
    x4=x3+panchoobj2;
    y4=y3+paltoobj2;
    /*APP_LOG(APP_LOG_LEVEL_DEBUG, "x1 %d x2 %d x3 %d x4 %d y1 %d y2 %d y3 %d y4 %d",
           x1,x2,x3,x4,y1,y2,y3,y4);*/
    if ((x1>=x3) && (x2<=x4) && (y1>=y3) && (y2<=y4)) {
        //pobjeto1 dentro de ppobjeto2
        return true;
    }
    else {
        if (((x1>=x3 && x1<=x4) || (x2>=x3 && x2<=x4)) && ((y1>=y3 && y1<=y4) || (y2>=y3 && y2<=y4))) {
            return true;
        }
        else {
            return false;
        }
    }
    
}




//devuelve la puntuación del objeto
uint32_t devuelve_puntuacion_objeto(int pTipoObjeto) {
    if (pTipoObjeto==egota_negra) {
        return  2; //gota de agua negra
    }
    if (pTipoObjeto==egota_blanca) {
        return 4;
    }
    if (pTipoObjeto==ereloj) {
        //reloj de tiempo
        return 0;
    }
    return 0;
}
