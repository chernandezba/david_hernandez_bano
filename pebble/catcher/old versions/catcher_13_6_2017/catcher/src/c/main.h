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
void pon_acercade_window(void);
void quita_acercade_window(void);
void pon_juego();
void pon_menu();
void pon_highscores();
void pon_acercade();
void pon_ayuda();
void entra_submenu(int piseccion);
void sale_submenu(bool pVaAMenuPrincipal);
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
uint16_t submenu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
uint16_t submenu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
int16_t submenu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
void submenu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
void submenu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void submenu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);
void lifes_update_proc(Layer *this_layer, GContext *ctx);
void canvas_update_proc(Layer *this_layer, GContext *ctx);
void acercade_window_load(Window *window);
void acercade_window_unload(Window *window);
void main_window_load(Window *window);
void main_window_unload(Window *window);
void menu_window_load(Window *window);
void menu_window_unload(Window *window);
void submenu_window_load(Window *window);
void submenu_window_unload(Window *window);
void pulsado_abajo_handler(ClickRecognizerRef recognizer, void *context);
void pulsado_arriba_handler(ClickRecognizerRef recognizer, void *context);
void pulsado_back_handler(ClickRecognizerRef recognizer, void *context);
void click_abajo_handler(ClickRecognizerRef recognizer, void *context);
void unclick_abajo_handler(ClickRecognizerRef recognizer, void *context);
void click_arriba_handler(ClickRecognizerRef recognizer, void *context);
void unclick_arriba_handler(ClickRecognizerRef recognizer, void *context);
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
#define ALTO_LAYER_TIEMPO 16
#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 5
#define NUM_CONFIG_MENU_ITEMS 2
#define NUM_JUGAR_MENU_ITEMS 2 //submenú de jugar, opciones : vidas, tiempo
#define CADA_PUNTOS_INCREMENTA_TIEMPO 40 //cada n puntos incrementa el tiempo disponible
#define CADA_PUNTOS_DECREMENTA_POSIBILIDAD_RELOJ 100 //cada n puntos decrementa la posibilidad que aparezca reloj. incrementa variable maleatreloj hasta maximo de 99 . el número aleatoria es entre 1 y 99
#define SEGUNDOS_INICIALES_MODO_TIEMPO 90 //empieza con 90 segundos al empezar en modo tiempo
#define TIEMPO_INCREMENTA_CADA_PUNTOS 15 //cada n puntos incrementa x segundos
#define SEGUNDOS_SUMA_SI_RELOJ 15 //segundos a incrementar si se coge un reloj
#define SI_N_PUNTOS_VIDA_INCR 100 //a los 100 puntos incrementa una vida
#define CADA_N_PUNTOS_VIDA_INCR 500 //despues de incrementar la 1a vida, cada n puntos una vida mas
#endif