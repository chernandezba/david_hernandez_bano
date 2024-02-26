#include <pebble.h>
#include "configuracion.h"
#ifndef utiles_h
#define utiles_h


void pon_puntuacion_en_layer(TextLayer *playertexto,uint32_t ppuntuacion);
void pon_tiempo_en_layer(TextLayer *playertiempo,uint32_t ptiempo);
void pon_game_over(GContext *ctx,Layer *player);
void graba_score_vidas(uint32_t pPuntuacion,struct stConfig *pConfig);
void graba_score_tiempo(uint32_t pPuntuacion,struct stConfig *pConfig);
void graba_score(uint32_t pPuntuacion, struct stConfig *pConfig);
void vibrar(void);
void reemplazar(char *cadena,char *cadenaareemplazar, char *cadenadestino);
#endif