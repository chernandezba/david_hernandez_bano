#include <pebble.h>
#include "mensajes.h"
//aqui incluiremos los mensajes. cada mensaje en 4 idiomas
//primer idioma castellano
//segundo idioma ingles
//tercer idioma tagalog
//cuarto idioma waray, dialecto de tagalog
//todo irá en una variable, un indice para
//saber cual mensaje es
static char *mensajes_i18n[]= {
    "Catcher \n (c) David Hernandez 2016 \nVersión 1.2",
    "Catcher \n (c) David Hernandez 2016 \nVersion 1.2",
    "Catcher \n (c) David Hernandez 2016 \nVersion 1.2",
    "Catcher \n (c) David Hernandez 2016 \nVersion 1.2",
"Records","High Scores","Talaan","High Scores",
"Menú","Menu","Menu","Menu",
"Jugar","Play","Maglaro","Play",
"Acerca de","About","Tungkol sa","About",
"Config","Setup","Itayo","Setup",
"Idioma","Language","Wika","Language",
"Vibrar","Vibrate","Kumaba","Vibrate",
"Records Vidas","Life records","talaan buhay","Life records",
"Records tiempo","Time records","mga talaan ng oras","Time records",
"Vibrar act","Vibrate on","Kumaba on","Vibrate on",
"Vibrar desact","Vibrate off","Kumaba off","Vibrate off",
"Español","English","Tagalog","Waray",
"Modo Vidas","Lives mode","Buhay mode","Lives mode",
"Modo Tiempo","Time mode","Time mode","Time mode"    
};


//devuelve el mensaje, segun id de idioma actual
//primer idioma=0
char *devuelve_mensaje_i18n(int pnummensaje,int pnumidioma) {
    int lindice;
    
    lindice=(pnummensaje*IDIOMAS_POR_MENSAJE) + pnumidioma;
    return mensajes_i18n[lindice];
}