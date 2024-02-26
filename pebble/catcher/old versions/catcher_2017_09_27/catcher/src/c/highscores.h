#pragma once
#ifndef _highscores_h
#define _highscores_h
#include <pebble.h>

void pon_highscores_window(void);
void quita_highscores_window(void);
void muestra_highscores(void);
void init_muestra_highscores(Window *window);
void highscores_window_load(Window *window);
void highscores_window_unload(Window *window);
void uninit_highscores_window(void);
void init_highscores_window(void);
void destroy_window_highscores(void);
void siguiente_ventana_highscores(void);
#endif