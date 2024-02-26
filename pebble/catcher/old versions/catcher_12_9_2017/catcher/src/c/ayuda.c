#include <pebble.h>
#include "ayuda.h"
#include "mensajes.h"
#include "configuracion.h"
//creado 5/10/2016

//para mostrar la ayuda

//variables-----------------------
Window *s_ayuda_window=NULL;
ScrollLayer *s_ayuda_scroll_layer=NULL;
TextLayer *s_ayuda_texto_layer=NULL;
char *mtextoayuda=NULL;
//----------------------------------




//funciones
void init_ayuda_window(void) {
    //creamos ayuda window en el momento de mostrar
    //la ayuda, la destruiremos
    //al salir de la ayuda
    if (s_ayuda_window==NULL) {
        s_ayuda_window=window_create();
        window_set_window_handlers(s_ayuda_window, (WindowHandlers) {
            .load=ayuda_window_load,
            .unload=ayuda_window_unload
        });
    }
    if (!window_stack_contains_window(s_ayuda_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_ayuda_window,false);
    }
}


void ayuda_window_load(Window *window) {
    
    APP_LOG(APP_LOG_LEVEL_DEBUG,"highscores_window_load");
    init_muestra_ayuda(window);
    pon_ayuda_window();
}


void ayuda_window_unload(Window *window) {
    
    APP_LOG(APP_LOG_LEVEL_DEBUG,"highscores_window_unload");
    uninit_ayuda_window();
    
       
    
}



void uninit_ayuda_window(void) {

    if (s_ayuda_texto_layer!=NULL) {
        layer_destroy(text_layer_get_layer(s_ayuda_texto_layer));
        s_ayuda_texto_layer=NULL;
    };
    if (s_ayuda_scroll_layer!=NULL) {
        layer_destroy(scroll_layer_get_layer(s_ayuda_scroll_layer));
        s_ayuda_scroll_layer=NULL;
    };
    if (mtextoayuda!=NULL) {
        free(mtextoayuda);
        mtextoayuda=NULL;
    }    
}



void pon_ayuda_window(void) {


    init_ayuda_window();   
}

void destroy_window_ayuda(void) {
    if (window_stack_contains_window(s_ayuda_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_main_window");
        window_stack_remove(s_ayuda_window,false);
    }
    if (s_ayuda_window!=NULL) {
        window_destroy(s_ayuda_window);     
        s_ayuda_window=NULL;
    }

}

void quita_ayuda_window(void) {
     destroy_window_ayuda();
}

void init_muestra_ayuda(Window *pwindow) {
    int i;
    int llong;
    int lalto;
    Layer *window_layer = window_get_root_layer(pwindow);
    GRect window_bounds = layer_get_bounds(window_layer);
    
    //calculamos ancho total ayuda
    llong=0;
    lalto=0;
    for (i=ID_MENSAJE_AYUDA_1;i<=ID_MENSAJE_AYUDA_13;++i) {
        llong+=strlen(devuelve_mensaje_i18n(i,gconfig.idioma))+1;
        lalto+=14; //TODO: calcular alto total para redimensionar o dibujar text layer
    }
    if (mtextoayuda!=NULL) free(mtextoayuda);
    mtextoayuda=malloc(llong+1);
    //ahora copiamos el texto a mtextoayuda
    *mtextoayuda=0;
    for (i=ID_MENSAJE_AYUDA_1;i<=ID_MENSAJE_AYUDA_13;++i)
        strcat(mtextoayuda,devuelve_mensaje_i18n(i,gconfig.idioma));
    s_ayuda_scroll_layer=scroll_layer_create(GRect(0,0,window_bounds.size.w,window_bounds.size.h));
    // This binds the scroll layer to the window so that up and down map to scrolling
    // You may use scroll_layer_set_callbacks to add or override interactivity
    scroll_layer_set_click_config_onto_window(s_ayuda_scroll_layer, pwindow);
    s_ayuda_texto_layer=text_layer_create(GRect(0,0,window_bounds.size.w,lalto));
    text_layer_set_overflow_mode(s_ayuda_texto_layer,GTextOverflowModeWordWrap);
    text_layer_set_font(s_ayuda_texto_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text_alignment(s_ayuda_texto_layer, GTextAlignmentCenter);
    text_layer_set_background_color(s_ayuda_texto_layer, GColorClear);
    text_layer_set_text_color(s_ayuda_texto_layer, GColorBlack);      
    text_layer_set_text(s_ayuda_texto_layer,mtextoayuda);
    scroll_layer_add_child(s_ayuda_scroll_layer, text_layer_get_layer(s_ayuda_texto_layer));
    scroll_layer_set_content_size(s_ayuda_scroll_layer,GSize(window_bounds.size.w,lalto));
    layer_add_child(window_layer, scroll_layer_get_layer(s_ayuda_scroll_layer));
}