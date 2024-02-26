#include <pebble.h>
#ifndef main_h
#define main_h
void pon_puntuacion(void);
void pon_vidas(void);
void timer_callback(void *data);
void quita_menu_window(void);
void pon_menu_window(void);
void pon_juego_window(void);
void quita_juego_window(void);
void quita_highscores_window(void);
void pon_highscores_window(void);
void pon_acercade_window(void);
void quita_acercade_window(void);
void pon_juego();
void pon_menu();
void pon_highscores();
void pon_acercade();
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
void lifes_update_proc(Layer *this_layer, GContext *ctx);
void canvas_update_proc(Layer *this_layer, GContext *ctx);
void acercade_window_load(Window *window);
void acercade_window_unload(Window *window);
void highscores_window_load(Window *window);
void highscores_window_unload(Window *window);
void main_window_load(Window *window);
void main_window_unload(Window *window);
void menu_window_load(Window *window);
void menu_window_unload(Window *window);
void pulsado_abajo_handler(ClickRecognizerRef recognizer, void *context);
void pulsado_arriba_handler(ClickRecognizerRef recognizer, void *context);
void pulsado_back_handler(ClickRecognizerRef recognizer, void *context);
void config_escucha_botones_highscores(void *context);
void config_escucha_botones_acercade(void *context);
void config_escucha_botones(void *context);
void init(void);
void deinit(void);


//parte de defines
#define VIDAS_INICIALES    5
#define SALTO_MS    50
#define NUM_OBJETOS_INICIAL     4
#define ALTO_LAYER_SCORE 16
#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 3
#define TICKS_EMPIEZA_ALEATORIO_SALIR_GOTA 35
#define CADA_N_PUNTOS_DECREMENTA_ALEAT_N_OBJ 80
#define CADA_N_PUNTOS_INCREMENTA_OBJ_MAX 100


#endif