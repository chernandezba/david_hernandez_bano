#include <pebble.h>
#include "utiles.h"
    



char buffer_score_texto[80]; //usado para poner el texto en el score
//no usar para otras cosas. como no copia la cadena al usar
//text_layer_set_text, este buffer solo se ha de usar para poner
//la cadena del score



void inicializa_high_scores(struct stConfig *pConfig) {
    int i;
    
    pConfig->pVersion=NUM_VERSION_CONFIG;
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        pConfig->puntuaciones[i]=0;
    }
}


void lee_high_scores(struct stConfig *pHighScores) {
    
if (persist_exists(KEY_HIGH_SCORES))  {
    persist_read_data(KEY_HIGH_SCORES,pHighScores,sizeof (struct stConfig));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "lee_high_scores");
}
}

//graba puntuación en tabla de high-scores, sólo en memoria
void graba_score(uint32_t pPuntuacion,struct stConfig *phighScores) {
    int i,pos;
    
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        if (pPuntuacion>(phighScores->puntuaciones[i])) break;
    }
    if (i<MAX_PUNTUACIONES) {
        pos=i;
        //ahora desplazamos high-scores a la derecha para hacer hueco
        for (i=MAX_PUNTUACIONES+1;i>pos;--i) {
            phighScores->puntuaciones[i]=phighScores->puntuaciones[i-1];
        }
        phighScores->puntuaciones[pos]=pPuntuacion;        
    }
}


void graba_high_scores(struct stConfig *pHighScores) {
    persist_write_data(KEY_HIGH_SCORES,pHighScores,sizeof (struct stConfig));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "graba_high_scores");
}


void pon_puntuacion_en_layer(TextLayer *playertexto,uint32_t ppuntuacion) {
    
    snprintf(buffer_score_texto,80,"%lu",ppuntuacion);
    text_layer_set_text(playertexto, buffer_score_texto);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Puesta puntuación: %s",buffer_score_texto);
}

    
int devuelve_puntuacion(int pTipoObjeto) {
    if (pTipoObjeto==0) {
        return 2; //gota de agua negra
    }
    else {
        if (pTipoObjeto==1) {
            return 4; //gota de agua blanca
        }
        return 0;
    }
}



void pon_game_over(GContext *ctx,Layer *player) {
    int x,y;
    GRect window_bounds = layer_get_bounds(player);
    GSize lsize;
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_context_set_text_color(ctx, GColorBlack);
    x=0;
    y=window_bounds.size.h / 3;
    lsize=graphics_text_layout_get_content_size_with_attributes("GAME OVER",
           fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), GRect(x,y,window_bounds.size.w-x,
           window_bounds.size.h-y), GTextOverflowModeWordWrap, GTextAlignmentCenter,
           NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG,  
            "ancho maximo game over ancho %d alto %d",lsize.w,lsize.h);
    graphics_draw_text(ctx,"GAME OVER",fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
                       GRect(x,y,window_bounds.size.w-x,
                       window_bounds.size.h-y),GTextOverflowModeWordWrap,GTextAlignmentCenter,
                       NULL);    
}
