#pragma once
#ifndef _highscores_h
#define _highscores_h
#include <pebble.h>

void muestra_highscores(void);
void init_muestra_highscores(Window *window);
void uninit_highscores(void);
void init_highscores(void);
void destroy_window_highscores(void);
void siguiente_ventana_highscores(void);
#endif