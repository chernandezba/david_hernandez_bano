#include <pebble.h>
#ifndef utiles_h
#define utiles_h

#define MAX_PUNTUACIONES 5
#define NUM_VERSION_CONFIG 1 //versión de config
#define KEY_HIGH_SCORES 0x1a80 //key para leer high_scores

//estructura para guardar en persistent configuración, de momento sólo high-scores
struct stConfig { 
    uint32_t puntuaciones[MAX_PUNTUACIONES];
    uint8_t pVersion; //version de la estructura, 1era versión=1
};


void inicializa_high_scores(struct stConfig *pConfig);
void lee_high_scores(struct stConfig *pHighScores);
void graba_score(uint32_t pPuntuacion,struct stConfig *phighScores);
void graba_high_scores(struct stConfig *pHighScores);
void pon_puntuacion_en_layer(TextLayer *playertexto,uint32_t ppuntuacion);
int devuelve_puntuacion(int pTipoObjeto);
void pon_game_over(GContext *ctx,Layer *player);
#endif