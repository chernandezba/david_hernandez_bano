#include <pebble.h>
#include "utiles.h"
#include "configuracion.h"
    



static char buffer_score_texto[80]; //usado para poner el texto en el score
//no usar para otras cosas. como no copia la cadena al usar
//text_layer_set_text, este buffer solo se ha de usar para poner
//la cadena del score
static char buffer_tiempo_texto[80]; //usado para poner el tiempo en modo de juego por tiempo
//no usar para otras cosas.. como no copia la cadena al usar
//text_layer_set_text, este buffer solo se ha de usar para poner
//la cadena del tiempo


void vibrar(void){
    vibes_long_pulse(); //larga vibración
}


//graba puntuación en tabla de high-scores, de modo vidas, sólo en memoria
void graba_score_vidas(uint32_t pPuntuacion,struct stConfig *pconfig) {
    int i,pos;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "graba_score_vidas");
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        if (pPuntuacion>(pconfig->puntuaciones_vidas[i])) break;
    }
    if (i<MAX_PUNTUACIONES) {
        pos=i;
        //ahora desplazamos high-scores a la derecha para hacer hueco
        for (i=MAX_PUNTUACIONES+1;i>pos;--i) {
            pconfig->puntuaciones_vidas[i]=pconfig->puntuaciones_vidas[i-1];
        }
        pconfig->puntuaciones_vidas[pos]=pPuntuacion;        
    }
}



//graba puntuación en tabla de high-scores, de modo vidas, sólo en memoria
void graba_score_tiempo(uint32_t pPuntuacion,struct stConfig *pconfig) {
    int i,pos;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "graba_score_tiempo");
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        if (pPuntuacion>(pconfig->puntuaciones_tiempo[i])) break;
    }
    if (i<MAX_PUNTUACIONES) {
        pos=i;
        //ahora desplazamos high-scores a la derecha para hacer hueco
        for (i=MAX_PUNTUACIONES+1;i>pos;--i) {
            pconfig->puntuaciones_tiempo[i]=pconfig->puntuaciones_tiempo[i-1];
        }
        pconfig->puntuaciones_tiempo[pos]=pPuntuacion;        
    }
}

void graba_score(uint32_t pPuntuacion, struct stConfig *pConfig) {
    //graba puntuacion en tabla high_scores, mira si ha de hacerlo en modo
    //vidas o en modo tiempo. Usa pConfig->modojuego para saber donde
    //guardarlo
    if (pConfig->modojuego==MODO_JUEGO_VIDAS) {
        //modo por vidas
        graba_score_vidas(pPuntuacion, pConfig);
    }
    if (pConfig->modojuego==MODO_JUEGO_TIEMPO) {
        //modo por tiempo
        graba_score_tiempo(pPuntuacion, pConfig);        
    }
}


void pon_puntuacion_en_layer(TextLayer *playertexto,uint32_t ppuntuacion) {
    
    snprintf(buffer_score_texto,80,"%lu",ppuntuacion);
    text_layer_set_text(playertexto, buffer_score_texto);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Puesta puntuación: %s",buffer_score_texto);
}



void pon_tiempo_en_layer(TextLayer *playertiempo,uint32_t ppuntuacion) {
    
    snprintf(buffer_tiempo_texto,80,"%lu",ppuntuacion);
    text_layer_set_text(playertiempo, buffer_tiempo_texto);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Puesto tiempo: %s",buffer_tiempo_texto);
}

    




void pon_game_over(GContext *ctx,Layer *player) {
    int x,y;
    GRect window_bounds = layer_get_bounds(player);
    GSize lsize;
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_text_color(ctx, GColorBlack);
    x=0;
    y=window_bounds.size.h / 3;
    /*lsize=graphics_text_layout_get_content_size_with_attributes("GAME OVER",
           fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(x,y,window_bounds.size.w-x,
           window_bounds.size.h-y), GTextOverflowModeWordWrap, GTextAlignmentCenter,
           NULL);*/
    /*APP_LOG(APP_LOG_LEVEL_DEBUG,  
            "ancho maximo game over ancho %d alto %d",lsize.w,lsize.h);*/
    graphics_draw_text(ctx,"GAME OVER",fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                       GRect(x,y,window_bounds.size.w-x,
                       window_bounds.size.h-y),GTextOverflowModeWordWrap,GTextAlignmentCenter,
                       NULL);    
}
