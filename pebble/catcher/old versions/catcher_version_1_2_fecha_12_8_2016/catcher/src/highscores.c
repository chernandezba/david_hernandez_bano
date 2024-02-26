#include <pebble.h>
#include "highscores.h"
#include "configuracion.h"
#include "main.h"
#include "mensajes.h"

//para la ventana de highscores, opción de menú de ver highscores

Window *s_highscores_window=NULL;
TextLayer *s_highscores_layer[MAX_PUNTUACIONES];
TextLayer *s_texto_siguiente=NULL;
TextLayer *s_highscores_titulo=NULL;
char buffer_highscores[MAX_PUNTUACIONES][10]; //solo usado para mostrar el texto de los highscores, ha de ser sólo usado para esto ya que se usa el buffer para dibujar el texto
int pnumhighscores; //indica que mostrar, para cambiar de tipo, se habra de pulsar boton abajo,0=highscores por vidas,1=highscores por tiempo

void muestra_highscores(void) {
    int i;
    uint32_t lpuntuacion;

    
    if (pnumhighscores==0) 
        i=ID_TITULO_RECORDS_VIDAS;
    else 
        i=ID_TITULO_RECORDS_TIEMPO;
    text_layer_set_text(s_highscores_titulo,devuelve_mensaje_i18n(i,gconfig.idioma));
    text_layer_set_text(s_texto_siguiente,">>>");
   //dibujamos las max_puntuaciones
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        if (pnumhighscores==0)
            lpuntuacion=gconfig.puntuaciones_vidas[i];
        else
            lpuntuacion=gconfig.puntuaciones_tiempo[i];
        snprintf(&buffer_highscores[i][0],80,"%05lu",lpuntuacion);
        text_layer_set_text(s_highscores_layer[i], buffer_highscores[i]);
    } 
}

void siguiente_ventana_highscores(void) {
    if (pnumhighscores==1) {
        pnumhighscores=0;
    }
    else pnumhighscores=1;
    muestra_highscores();
}

void uninit_highscores(void) {
    int i;
    

    if (s_texto_siguiente!=NULL) {
        layer_destroy(text_layer_get_layer(s_texto_siguiente));
        s_texto_siguiente=NULL;
    };
    if (s_highscores_titulo!=NULL) {
        layer_destroy(text_layer_get_layer(s_highscores_titulo));
        s_highscores_titulo=NULL;
    };
    if (s_highscores_layer!=NULL) {
        for (i=0;i<MAX_PUNTUACIONES;++i) {
            if (s_highscores_layer[i]!=NULL) {
                layer_destroy(text_layer_get_layer(s_highscores_layer[i]));  
                s_highscores_layer[i]=NULL;
            }
        }
    }        
}

void destroy_window_highscores(void) {
    if (window_stack_contains_window(s_highscores_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_main_window");
        window_stack_remove(s_highscores_window,false);
    }
    if (s_highscores_window!=NULL) {
        window_destroy(s_highscores_window);     
        s_highscores_window=NULL;
    }

}

void init_highscores(void) {
    //creamos highscores window en el momento de mostrar
    //los highscores solamente, la destruiremos
    //al salir de los highscores
    if (s_highscores_window==NULL) {
        s_highscores_window=window_create();
        window_set_window_handlers(s_highscores_window, (WindowHandlers) {
            .load=highscores_window_load,
            .unload=highscores_window_unload
        });
    }
    if (!window_stack_contains_window(s_highscores_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_highscores_window,false);
    }
}


void init_muestra_highscores(Window *pwindow) {

    
    Layer *window_layer = window_get_root_layer(pwindow);
  GRect window_bounds = layer_get_bounds(window_layer);
  int i,y,lalto;

    pnumhighscores=0;    
    //creamos textlayers
    
    //titulo
    s_highscores_titulo = text_layer_create(GRect(0, 0, window_bounds.size.w,
                                                    window_bounds.size.h / 3));
    text_layer_set_font(s_highscores_titulo, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text_alignment(s_highscores_titulo, GTextAlignmentCenter);
    text_layer_set_background_color(s_highscores_titulo, GColorClear);
    text_layer_set_text_color(s_highscores_titulo, GColorBlack);      
    layer_add_child(window_layer, text_layer_get_layer(s_highscores_titulo));  
 
    
    //indicando donde pulsar para siguiente
    s_texto_siguiente=text_layer_create(GRect(0,window_bounds.size.h-20,window_bounds.size.w,20));
    text_layer_set_font(s_texto_siguiente, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text_alignment(s_texto_siguiente, GTextAlignmentRight);
    text_layer_set_background_color(s_texto_siguiente, GColorClear);
    text_layer_set_text_color(s_texto_siguiente, GColorBlack);      
    layer_add_child(window_layer, text_layer_get_layer(s_texto_siguiente));
    
    
    //records
    y=window_bounds.size.h / 3;
    lalto=((window_bounds.size.h- (window_bounds.size.h / 5) - y) / MAX_PUNTUACIONES);
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        s_highscores_layer[i]=text_layer_create(GRect(0, y, window_bounds.size.w,
                                                    lalto));
        text_layer_set_font(s_highscores_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
        text_layer_set_text_alignment(s_highscores_layer[i], GTextAlignmentCenter);
        text_layer_set_background_color(s_highscores_layer[i], GColorClear);
        text_layer_set_text_color(s_highscores_layer[i], GColorBlack);      
        layer_add_child(window_layer, text_layer_get_layer(s_highscores_layer[i]));  
        y+=lalto;
    }
    
    //configuramos handler de cada click
    window_set_click_config_provider(s_highscores_window, config_escucha_botones_highscores);
}