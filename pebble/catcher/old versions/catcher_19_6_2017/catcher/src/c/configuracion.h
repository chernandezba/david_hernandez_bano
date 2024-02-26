#include <pebble.h>
#ifndef configuracion_h
#define configuracion_h
#define MAX_PUNTUACIONES_v1 5 //numero de max puntuaciones guardadas en v1
#define MAX_PUNTUACIONES 5
#define NUM_VERSION_CONFIG 2 //versión de config actual
#define KEY_CONFIG 0x1a80 //key para leer config
#define KEY_VERS_CONFIG 0x1a7f //key para leer versión de config
#define MODO_JUEGO_VIDAS 0
#define MODO_JUEGO_TIEMPO 1

//esta estructura sirverá para migrar config de versión 1 a versión 2
struct stConfig_v1 {
    uint32_t puntuaciones[MAX_PUNTUACIONES];
    uint8_t Version; //version de la estructura, 1era versión=1
};

//estructura para guardar en persistent configuración, de momento sólo high-scores
struct stConfig { 
    uint32_t puntuaciones_vidas[MAX_PUNTUACIONES];
    uint32_t puntuaciones_tiempo[MAX_PUNTUACIONES];
    uint8_t Version; //version de la estructura, 1era versión=1
    int idioma; //0=castellano,1=ingles,2=tagalog,3=dialecto tagalog (waray)
    int modojuego; //0=modo por vidas, 1=modo por tiempo
    bool vibrar_al_caer_objeto; //si true, vibrar al caer objeto
};
extern struct stConfig gconfig;
void lee_config(struct stConfig *pHighScores);
void graba_config(struct stConfig *pHighScores);
void inicializa_config(struct stConfig *pConfig);
void siguiente_idioma(struct stConfig *pConfig);
#endif