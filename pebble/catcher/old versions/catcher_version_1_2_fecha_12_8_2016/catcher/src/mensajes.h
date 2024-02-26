#include <pebble.h>
#ifndef mensajes_h
#define mensajes_h

#define IDIOMAS_POR_MENSAJE 4
#define ID_IDIOMA_CASTELLANO 0
#define ID_IDIOMA_INGLES 1
#define ID_IDIOMA_TAGALOG 2
#define ID_IDIOMA_WARAY 3
#define ID_ACERCA_DE_MENSAJE 0 //indice al mensaje que aparece en la opción de menú acerca de
#define ID_RECORDS 1 //indice al mensaje de records
#define ID_MENU 2 //indice al mensaje de menú
#define ID_JUGAR 3 //indice al mensaje de jugar
#define ID_ACERCADE 4 //indica al mensaje acerca de
#define ID_CONFIG 5 //indica al mensaje config (configuración) del menú
#define ID_IDIOMA 6 //indica al mensaje idioma
#define ID_VIBRAR 7 //indica al mensaje vibrar
#define ID_TITULO_RECORDS_VIDAS 8
#define ID_TITULO_RECORDS_TIEMPO 9
#define ID_VIBRAR_ACT 10 //indica al mensaje vibrar act
#define ID_VIBRAR_DESACT 11 //indica al mensaje vibrar desact
#define ID_NOMBRE_IDIOMA 12 //indica el nombre del idioma actual : español , english , tagalog o waray
#define ID_MODOVIDAS 13 //vidas
#define ID_MODOTIEMPO 14 //tiempo


char *devuelve_mensaje_i18n(int pnummensaje,int pnumidioma); //pnumidioma, primer mensaje=0, pnummensaje, primer mensaje=0, usar las constantes para localizar mensajes

#endif