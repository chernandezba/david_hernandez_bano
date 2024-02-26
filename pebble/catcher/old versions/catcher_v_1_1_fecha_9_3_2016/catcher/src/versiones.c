#include <pebble.h>


//    Versión 1.0: Primera versión (14/12/2015)
//    Versión 1.1: (9/3/2016)
//    14/12/2015 :     Cambiado SALTO_MS a 80
//    15/12/2015 :     Cambiadas constantes para que salga nueva gota.
//    15/12/2015 :     Ahora cada 5 ticks se mueven las gotas
//    16/12/2015 :     Tipos de objetos : 0=gota de agua negra, 1=gota de agua blanca
//    11/1/2016  :     Variable s_vida tiene el dibujo de una vida. Layer de score
//                     ahora mide la mitad de ancho. Creada layer s_lifes_layer
//                     para poder las vidas. Creada variable vidas con el número de vidas.
//    25/1/2016  : añadidas variables a struct objeto para que soporte animación de la
//                caida de la gota (objeto tipo 0 y 1).
//    2/3/2016   : Ahora graba y carga highscores. Guarda un máximo de 5 puntuaciones . Guarda
//                 las mas altas.
//    8/4/2016   : Cada 100 puntos hay una gota mas. Cada 80 puntos la variable aleatoria
//                 para que aparezca otra gota (cuando ha de aparecer una ya aun no hay
//                 en pantalla el número máximo de gotas) se decrementa en 2.
//    Versión 1.2:

//queda :  Poner que de una vida cada n puntos o a ciertos puntos o una mezcla de los 2.
//         Poner manual de instrucciones explicando lo que hay que hacer y lo que vale
//         cada gota y explicar cada cuanto hay una vida y una gota mas.
