#include <pebble.h>
#include "configuracion.h"
//donde estan las rutinas de configuración
//para leer y guardar la configuración
//en la configuración está de momento los records,
//el idioma actual y el modo de juego 


struct stConfig gconfig;

void graba_config(struct stConfig *pConfig) {
    persist_write_data(KEY_CONFIG,pConfig,sizeof (struct stConfig));
    persist_write_int(KEY_VERS_CONFIG, NUM_VERSION_CONFIG);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "graba_config");
}

void siguiente_idioma(struct stConfig *pConfig) {
    pConfig->idioma++;
    if (pConfig->idioma>3) pConfig->idioma=0;
}

void inicializa_config(struct stConfig *pConfig) {
    int i;
    
    pConfig->Version=NUM_VERSION_CONFIG;
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        pConfig->puntuaciones_vidas[i]=0;
        pConfig->puntuaciones_tiempo[i]=0;
    }    
    pConfig->modojuego=0;
    pConfig->idioma=0;
    pConfig->vibrar_al_caer_objeto=false;    
}

static void migrarconfigv1av2(struct stConfig_v1 *pconfigv1, struct stConfig *pconfig) {
    //migramos configuracion a v2
    int i;
    //migramos puntuaciones
    for (i=0;i<MAX_PUNTUACIONES_v1;++i) {
        pconfig->puntuaciones_vidas[i]=pconfigv1->puntuaciones[i];
    }
    //el resto de puntuaciones las ponemos a 0
    for (i=MAX_PUNTUACIONES_v1;i<MAX_PUNTUACIONES;++i) {
        pconfig->puntuaciones_vidas[i]=0;
    }
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        pconfig->puntuaciones_tiempo[i]=0;
    }
    pconfig->vibrar_al_caer_objeto=false;
    pconfig->modojuego=0;
    pconfig->idioma=0;
    pconfig->Version=2;    
}

void lee_config(struct stConfig *pConfig) {
    int lVers;
    struct stConfig_v1 lconfigv1;

lVers=NUM_VERSION_CONFIG;
if (!persist_exists(KEY_VERS_CONFIG))    {
    //no existe clave de número de versión de config
    //entonces puede ser versión 1 si existe config guardada
    if (persist_exists(KEY_CONFIG))
        lVers=1;
}    
else {
    //leemos versión de config
    lVers=persist_read_int(KEY_VERS_CONFIG);
}
if (lVers==1)    {
    //ultima versión guardada es versión 1, entonces migramos a versión 2
    persist_read_data(KEY_CONFIG,&lconfigv1,sizeof (struct stConfig_v1));
    migrarconfigv1av2(&lconfigv1,pConfig);
}
else {
    //si versión guardada no es 1, entonces intentamos leer versión guardada
    //que será ultima versión de config
    if (persist_exists(KEY_CONFIG))  {
        persist_read_data(KEY_CONFIG,pConfig,sizeof (struct stConfig));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "lee_high_scores");
    }
    else {
        inicializa_config(pConfig);
    }
}
    
}