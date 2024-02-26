#include <pebble.h>
#ifndef objetos_h
#define objetos_h

//estructuras
struct objeto {
    int x;
    int y;
    int tipo; //0=gota de agua, 1=....
    int estado; //0=cayendo, 1=ha caido
    GBitmap *imagen; //imagen del tipo de objeto
    GBitmap **animaccaida; //n imagenes con la caida. para hacer una animacion de caida
    int numanimaccaida; //el número de imagenes que comprende animaccaida. el valor n comentado arriba
    int ianimaccaida; //(número de 0 a numanimaccaida-1) cuando cae para saber que GBitmap mostrar de animaccaida (animaccaida[ianimaccaida])
    bool activado;
    struct objeto *siguiente;
    struct objeto *anterior;
};

//funciones
struct objeto *inicializa_objeto(int ptipoobjeto,int pxinic,GBitmap *pimagenobjeto,
                                 GBitmap **panimaccaida, int pnumanimaccaida);
void inicializa_structura_objeto(struct objeto *pobjeto, int ptipoobjeto,int pxinic,
                                  GBitmap *pimagenobjeto,
                                 GBitmap **panimaccaida, int pnumanimaccaida,
                                 bool pnotocaanterior, bool pnotocasiguiente);
void dibuja_objeto(struct objeto *pobjeto,  GContext *ctx);
bool colision(struct objeto *pobjeto1, int pposxobj2,int pposyobj2, int panchoobj2, 
              int paltoobj2);
void libera_memoria_objetos(struct objeto *pprimerobjeto);
void anyadeobjetoaobjetos(GBitmap *pobjeto,GBitmap **panimaccaida,
                          int pnumanimaccaida,
                          int ptipoobjeto, int pposx,
                          struct objeto *pprimobjeto);
#endif