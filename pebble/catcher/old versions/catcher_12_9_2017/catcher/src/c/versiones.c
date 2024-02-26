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
//    8/3/2016   : Cada 100 puntos hay una gota mas. Cada 80 puntos la variable aleatoria
//                 para que aparezca otra gota (cuando ha de aparecer una ya aun no hay
//                 en pantalla el número máximo de gotas) se decrementa en 2.
//    Versión 1.2: (12/8/2016)
//    10/3/2016 :  Ahora configuración es versión 2, en config añadido soporte para idioma
//                 actual, modo de juego y otro highscores para modo de juego
//                 por tiempo. Implementado actualizador de configuración versión 1 a versión 2.
//    18/5/2016 : Cuando abre submenú usa nuevo window, usa variable global msaveseccactmenu
//                para saber en que submenú estamos.
//    13/6/2016 : Ahora la paleta tiene acceleración. Usadas dos variables, maccel y maccel2 para
//                que no acelere muy rápido. Al soltar boton, frena de golpe.
//    12/8/2016 : Añadido multiioma : español , ingles , tagalog y waray (waray queda, puesto igual que english)
//    12/8/2016 : Añadido objeto reloj que incrementa el tiempo.
//    Versión 1.3: (20/9/2016)
//    12/8/2016 : Solucionado problema con navegación menu -> submenu -> menú. y con menú -> selecc modo juego -> juego -> sale
//                juego y va a menú principal.
//    20/9/2016 : Solucionado fallo al guardar score y desplazar los demas abajo. Se salir del buffer y tocaba cosas que no tenia
//                que tocar en la estructura de configuración (struct stConfig) como por ejemplo el idioma.
//    Versión 1.4
//    22/9/2016 : Cada 100 puntos decrementa posibilidad que aparezca el reloj en modo tiempo (incrementa variable maleatreloj 
//                en 1) hasta un máximo de 99. El número aleatorio es entre 1 y 99 . maleatreloj especifica que ha de salir
//                un número aleatorio igual o mayor que maleatreloj para que aparezca un reloj.
//    4/10/2016 : En modo vidas incrementa 1a vida a los SI_N_PUNTOS_VIDA_INCR(100) puntos y las siguientes vidas cada
//                CADA_N_PUNTOS_VIDA_INCR(500) puntos.
//    12/9/2017 : Cambiada letra de highscores a 18 (FONT_KEY_GOTHIC_18), antes era una 14.


//scroll de ayuda no va bien. mensaje de ayuda no se ve bien. todo junto y sin saltos
//de linea. no se ve toda la ayuda
//mensajes de ayuda de momento acabados, implementarlos en ayuda.c y ayuda.h y probar
//que se vea bien en los 4 idiomas
//haciendo ayuda (usar ScrollLayer para poner ayuda, en layer de Scroll https://developer.pebble.com/docs/c/User_Interface/Layers/ScrollLayer/),
//usa pon_ayuda_window en ayuda.c. usa ayuda.c y ayuda.h. Usaremos uno o mas layers mas altos que ventana principal
//del pebble que pondremos en el ScrollLayer.
//que se pueda configurar velocidad de aceleración de paleta
//que la vibración se pueda seleccionar para modo vidas y para modo por tiempo por separado
//mensajes en multiidioma, falta actualizar mensajes de waray
//Poner que de una vida cada n puntos o a ciertos puntos o una mezcla de los 2.
//         Poner manual de instrucciones explicando lo que hay que hacer y lo que vale
//         cada gota y explicar cada cuanto hay una vida y una gota mas.


