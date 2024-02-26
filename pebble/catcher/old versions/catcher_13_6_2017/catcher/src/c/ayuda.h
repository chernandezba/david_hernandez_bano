#ifndef _ayuda_h
#define _ayuda_h
#include <pebble.h>
//creado 5/10/2016
//fichero cabecera para la ayuda

void pon_ayuda_window(void);
void destroy_window_ayuda(void);
void quita_ayuda_window(void);
void ayuda_window_load(Window *window);
void ayuda_window_unload(Window *window);
void init_muestra_ayuda(Window *w);
void uninit_ayuda_window(void);
#endif