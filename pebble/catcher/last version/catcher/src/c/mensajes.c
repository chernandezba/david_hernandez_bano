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
    "Catcher \n (c) David Hernandez 2016-8 \nVersión 1.5",
    "Catcher \n (c) David Hernandez 2016-8 \nVersion 1.5",
    "Catcher \n (c) David Hernandez 2016-8 \nVersion 1.5",
    "Catcher \n (c) David Hernandez 2016-8 \nVersion 1.5",
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
"Modo Tiempo","Time mode","Time mode","Time mode",
"Ayuda","Help","Tulungan","Help",
"Controles : ","Controls","Kontrol","Controls",
"Izquierda : Boton arriba","Left : button up","Kaliwa: button up","Left : button up",
"Derecha : Boton abajo","Right : button down","Kanan: button sa ibaba","Right : button down",
"","","","",
"Este juego se trata de recoger los objetos que caen","In that game you have to catch falling objects","Ang larong ito ay upang mangolekta ng mga bumabagsak na bagay","In that game you have to catch falling objects",
"Caen 3 tipos de objetos:","Falls 3 object types:","Caen 3 uri ng mga bagay","Falls 3 object types:",
"Gota blanca, gota negra y reloj","White drop, black drop and clock","Puting drop, drop at itim na orasan","White drop, black drop and clock",
"Si se recoge gota blanca se suman xxx puntos","Catching white drop xxx points are added","Kung puting drop ay kinokolekta xxx puntos magdagdag ng hanggang","Catching white drop xxx points are added",
"Gota negra se suman xxx puntos","Black drop xxx points are added","si xxx black drop point","Black drop xxx points are added",
"Si coges el reloj, se te suman xxx segundos","Clock adds xxx seconds to time","Kung magdadala sa iyo ang relos, idagdag mo xxx seconds","Clock adds xxx seconds to time",
"Hay 2 modos de juego","There are two game types","Mayroong 2 mga mode ng laro","There are two game types"    ,
"Vidas : cuando te quedas sin vidas se acaba","Lifes mode : when no lifes game over","Nakatira: kapag naubusan ka ng buhay ikaw lamang","Lifes mode : when no lifes game over",
"Tiempo : cuando te quedas sin tiempo se acaba","Time mode : when no time left, game over","Time: Kapag naubusan ka ng oras ay lamang","Time mode : when no time left, game over"
};


//devuelve el mensaje, segun id de idioma actual
//primer idioma=0
char *devuelve_mensaje_i18n(int pnummensaje,int pnumidioma) {
    int lindice;
    
    lindice=(pnummensaje*IDIOMAS_POR_MENSAJE) + pnumidioma;
    return mensajes_i18n[lindice];
}