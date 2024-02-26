#include <pebble.h>
#include "main.h"
#include "objetos.h"
#include "utiles.h"

uint32_t ultpuntuacionaleatdecrementa; //ultima puntuacion cuando ha decrementado cada n ticks empieza aleat para nuevo objeto
uint32_t ultpuntuacionincrobj; //ultima puntuacion cuando ha incrementado máximo de objetos en pantalla
int posxpaleta;
int posypaleta;
int maxxpaleta;
int maxx;
int maxy; //máxima y del window del juego
int maxyjuego; //maxima posición y del layer del juego
int direcc=1;
uint32_t puntuacion; //32 bits sin signo
int vidas;
int estado; //0=en menú, 1=jugando , 2=mostrando highscores
Window *s_main_window=NULL;
Window *s_menu_window=NULL;
Window *s_highscores_window=NULL;
Window *s_acercade_window=NULL;
Layer *s_canvas_layer=NULL;
Layer *s_lifes_layer=NULL;
TextLayer *s_highscore_layer_text=NULL;
TextLayer *s_highscores_layer[MAX_PUNTUACIONES];
TextLayer *s_score_layer=NULL;
TextLayer *s_acercade_layer_text=NULL;
GBitmap *s_objeto1=NULL;
GBitmap *s_objeto1_animaccaida[3]={NULL,NULL,NULL}; //estados del objeto 1, para cuando cae , animación del objeto 1 que ha caido
GBitmap *s_objeto2=NULL;
GBitmap *s_objeto2_animaccaida[3]={NULL,NULL,NULL}; //estados del objeto 2, para cuando cae , animación del objeto 1 que ha caido
GBitmap *s_catcher1=NULL;
GBitmap *s_vida=NULL;
MenuLayer *s_menu_layer=NULL;
int ancho_paleta;
int alto_paleta;
struct objeto *objetos=NULL;
int lrelparaobj;
int relparaempiezaleat;
long ltickspasados;
int mticksesperamover;
int maltolife;
int mancholife;
char buffer_highscores[MAX_PUNTUACIONES][10]; //solo usado para mostrar el texto de los highscores, ha de ser sólo usado para esto ya que se usa el buffer para dibujar el texto
char *buffer_highscores_etiqueta="Records";
char *texto_acercade="Catcher \n (c) David Hernandez 2016 \nVersión 1.1";


//static BitmapLayer *s_bitmap_layer;
int MAX_OBJETOS;
bool mgameover; //si =true, game over, juego acabado.
struct stConfig mhighscores;


//static int s_current_icon = 0;



void pon_puntuacion(void) {

    pon_puntuacion_en_layer(s_score_layer,puntuacion);
}

void pon_vidas(void) {
    layer_mark_dirty(s_lifes_layer);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_vidas");
}




void timer_callback(void *data) {
    int laltoobjeto;
    struct objeto *loactual;
    int lnumobjetos;
    int lnumaleat;
    GBitmap *lgobj , **lganimacobj;
    int lnumanimaccaida;
    int lmaxxobj;
    int ltipoobjeto;
    int lticksslow;
    bool lhaycaidos;    

  if (estado==1) {
      if (!mgameover) {
          lhaycaidos=0;          
          loactual=objetos;
          while (loactual!=NULL) {              
              if (loactual->activado) {
                  if (loactual->estado==1) {
                      lhaycaidos=true;
                      break;
                  }    
              }
              loactual=loactual->siguiente;
          }
          if (lhaycaidos) {
              //cambiamos animación cada click de los caidos
              loactual=objetos;
              while (loactual!=NULL) {
                  if (loactual->activado) {
                      if (loactual->estado==1)     {
                          if (loactual->ianimaccaida==loactual->numanimaccaida) {
                              //ya no hay mas "bitmaps de caida"
                              loactual->activado=false;
                              //quita la vida cuando la animación de caida se ha acabado
                              vidas--;
                              //APP_LOG(APP_LOG_LEVEL_DEBUG, "vidas: %d",vidas);
                              pon_vidas();
                              if (vidas==0) {
                                  //no hay mas vidas
                                  //graba puntuación en los high-scores
                                  graba_score(puntuacion,&mhighscores);
                                  //pon game over y el juego no se mueve mas
                                  mgameover=true;                                  
                                  layer_mark_dirty(s_canvas_layer);
                                  layer_mark_dirty(s_lifes_layer); //para que se vea que no quedan vidas
                                  //APP_LOG(APP_LOG_LEVEL_DEBUG, "GAME OVER");
                              }
                          }
                          else {
                              loactual->imagen=loactual->animaccaida[loactual->ianimaccaida++];
                          }
                      }
                  }
                  loactual=loactual->siguiente;
              }
              layer_mark_dirty(s_canvas_layer); //para que redibuje los objetos
          }
          else {
              //no hay ningun objeto caido
              if (puntuacion<500) {
                  lticksslow=2;          
              }
              else {
                  lticksslow=1;
              }
              if (mticksesperamover>=lticksslow) {
                  mticksesperamover=0;
                  layer_mark_dirty(s_canvas_layer);
                  loactual=objetos;
                  //APP_LOG(APP_LOG_LEVEL_DEBUG, "timer_callback: antes del while");
                  lnumobjetos=0;
                  ltickspasados++;
                  while (loactual!=NULL) {
                      if (loactual->activado && (loactual->estado==0)) {
                          loactual->y+=2;
                          laltoobjeto=gbitmap_get_bounds(loactual->imagen).size.h;
                          if (colision(loactual,posxpaleta,posypaleta,ancho_paleta,alto_paleta)) {
                              loactual->activado=false;
                              puntuacion+=devuelve_puntuacion(loactual->tipo);
                              if ((puntuacion-ultpuntuacionincrobj)>=CADA_N_PUNTOS_INCREMENTA_OBJ_MAX) {
                                  ultpuntuacionincrobj=puntuacion;
                                  MAX_OBJETOS++;                                  
                              }   
                              if ((puntuacion-ultpuntuacionaleatdecrementa)>=CADA_N_PUNTOS_DECREMENTA_ALEAT_N_OBJ) {
                                  ultpuntuacionaleatdecrementa=puntuacion;
                                  if (relparaempiezaleat>0) {
                                      relparaempiezaleat-=2;
                                      if (relparaempiezaleat<0) {
                                          relparaempiezaleat=0;
                                      }
                                  }
                              }
                              pon_puntuacion();
                          }
                          else {
                              lnumobjetos++;
                          }
                          if (loactual->activado && (loactual->estado==0)) {
                              //no ha habido colision con paleta   
                              if ((loactual->y+laltoobjeto)>maxyjuego) {
                                  loactual->estado=1;           
                                  loactual->y=maxyjuego-laltoobjeto;
                              }
                          }
                      }
                      loactual=loactual->siguiente;
                  } 
                  //miramos si el número de objetos en pantalla
                  //es menor al número máximo de objetos.
                  if (lnumobjetos<MAX_OBJETOS) {
                      if (lrelparaobj>=relparaempiezaleat) {                  
                          lnumaleat=rand() % 4;
                          if (lnumaleat>1) {
                              lrelparaobj=0;
                              //añadimos otra gota
                              lnumaleat=rand() % 100;
                              if (lnumaleat>50) {
                                  lgobj=s_objeto1;
                                  lganimacobj=s_objeto1_animaccaida;
                                  lnumanimaccaida=3;
                                  ltipoobjeto=0; //gota de agua negra
                              }
                              else
                                  {
                                  lgobj=s_objeto2;
                                  lganimacobj=s_objeto2_animaccaida;
                                  lnumanimaccaida=3;
                                  ltipoobjeto=1; //gota de agua blanca
                              }
                              lmaxxobj=maxx -gbitmap_get_bounds(lgobj).size.w;
                              lnumaleat=rand() % lmaxxobj;
                              anyadeobjetoaobjetos(lgobj,lganimacobj,lnumanimaccaida, 
                                                   ltipoobjeto,lnumaleat,objetos);
                          }
                      }
                      else
                      {
                          lrelparaobj++;
                      }
                  }
              }
              else {
                  mticksesperamover++;
              }
          }
      }
      app_timer_register(SALTO_MS , timer_callback, NULL); //salta de nuevo el reloj
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "timer_callback: fin");
  }
}



void quita_menu_window(void) {
    if (window_stack_contains_window(s_menu_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_menu_window");
        window_stack_remove(s_menu_window,false);
    }
}


void pon_menu_window(void) {
    if (!window_stack_contains_window(s_menu_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_menu_window");
        window_stack_push(s_menu_window,false);        
        }
}

void pon_juego_window(void) {
    if (!window_stack_contains_window(s_main_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_main_window,false);        
        }
}


void quita_juego_window(void) {
    if (window_stack_contains_window(s_main_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_main_window");
        window_stack_remove(s_main_window,false);
    }
}


void pon_highscores_window(void) {
    

    //creamos highscores window en el momento de mostrar
    //los highscores solamente, la destruiremos
    //al salir de los highscores
    if (s_highscores_window==NULL) {
        s_highscores_window=window_create();
        window_set_window_handlers(s_highscores_window, (WindowHandlers) {
            .load=highscores_window_load,
            .unload=highscores_window_unload
        });
    }
    if (!window_stack_contains_window(s_highscores_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_highscores_window,false);
    }
}



void quita_highscores_window(void) {
    if (window_stack_contains_window(s_highscores_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_main_window");
        window_stack_remove(s_highscores_window,false);
    }
    //destruimos highscores window
    window_destroy(s_highscores_window);
    s_highscores_window=NULL;
}



void pon_acercade_window(void) {
    

    //creamos highscores window en el momento de mostrar
    //los highscores solamente, la destruiremos
    //al salir de los highscores
    if (s_acercade_window==NULL) {
        s_acercade_window=window_create();
        window_set_window_handlers(s_acercade_window, (WindowHandlers) {
            .load=acercade_window_load,
            .unload=acercade_window_unload
        });
    }
    if (!window_stack_contains_window(s_acercade_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_acercade_window,false);
    }
}


void quita_acercade_window(void) {
    if (window_stack_contains_window(s_acercade_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_remove s_main_window");
        window_stack_remove(s_acercade_window,false);
    }
    //destruimos highscores window
    window_destroy(s_acercade_window);
    s_acercade_window=NULL;
}




void pon_juego()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_juego");
    quita_menu_window();    
    puntuacion=0;
    posxpaleta=0;
    mticksesperamover=0;
    relparaempiezaleat=TICKS_EMPIEZA_ALEATORIO_SALIR_GOTA; //cada n ticks sale una gota nueva si no hay mas del máximo de gotas
    lrelparaobj=0; //para saber cuando poner en escena otro objeto
    ltickspasados=0; //nos servirá para ir subiendo de nivel (acelerando, mas gotas seguidas, ....)
    MAX_OBJETOS=6;
    estado=1;
    mgameover=false;
    pon_juego_window();        
    //app_timer_register(SALTO_MS , timer_callback, NULL); //salta de nuevo el reloj
}


void pon_menu()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_menu");
    graba_high_scores(&mhighscores);
    quita_juego_window();
    estado=0;
    pon_menu_window();       
}


void pon_highscores()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_menu");
    //graba_high_scores(&mhighscores);
    estado=2;
    pon_highscores_window();       
    quita_menu_window();    
    
}


void pon_acercade() {
    estado=3;
    pon_acercade_window();
    quita_menu_window();
}



uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_sections_callback");
  return NUM_MENU_SECTIONS;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_rows_callback");
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
/*    case 1:
      return NUM_SECOND_MENU_ITEMS;*/
    default:
      return 0;
  }
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_header_height_callback");
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_draw_header_callback");
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Menú");
      break;
    /*case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "One more");
      break;*/
  }
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_draw_row_callback");
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // jugar
          menu_cell_title_draw(ctx, cell_layer, "Jugar");
          //menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
          break;
        case 1:
          //hi-scores
          menu_cell_title_draw(ctx, cell_layer, "Records");
          //menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", s_menu_icons[s_current_icon]);
          break;
          case 2:
          menu_cell_title_draw(ctx, cell_layer, "Acerca De");
          break;
        /*case 2: 
          {
            // Here we use the graphics context to draw something different
            // In this case, we show a strip of a watchface's background
            GSize size = layer_get_frame(cell_layer).size;
            graphics_draw_bitmap_in_rect(ctx, s_background_bitmap, GRect(0, 0, size.w, size.h));
          }
          break;*/
      }
      break;
    /*case 1:
      switch (cell_index->row) {
        case 0:
          // There is title draw for something more simple than a basic menu item
          menu_cell_title_draw(ctx, cell_layer, "Final Item");
          break;
      }*/
  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_select_callback");
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
      case 0:
          //jugar          
          pon_juego();
          break;
      case 1:
          //muestra highscores
          pon_highscores();
          break;
      case 2:
          //muestra acerca de
          pon_acercade();
          break;
  }
  /*switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 1:
      // Cycle the icon
      s_current_icon = (s_current_icon + 1) % NUM_MENU_ICONS;
      // After changing the icon, mark the layer to have it updated
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
  }*/

}


//para cuando redibuje layer de las vidas disponibles
//se llama automaticamente cuando redibuja el layer
//o cuando se fuerza el redibujado
void lifes_update_proc(Layer *this_layer, GContext *ctx) {
    int x,i;
    
    if (estado==1) {
        graphics_context_set_fill_color(ctx, GColorBlack);
        x=0;
        for (i=0;i<vidas;++i) {
            graphics_draw_bitmap_in_rect(ctx,s_vida,GRect(x,0,
                                           mancholife,
                                           maltolife));
            x+=mancholife+3;
        }        
    }
    
}




void canvas_update_proc(Layer *this_layer, GContext *ctx) {
    struct objeto *loactual;
    
  //GRect bounds = layer_get_bounds(this_layer);

  // Get the center of the screen (non full-screen)
  //GPoint center = GPoint(bounds.size.w / 2, (bounds.size.h / 2));

  // dibuja cuadrado
  //dibujamos paleta
  if (estado==1) {
      graphics_context_set_fill_color(ctx, GColorBlack);            
      //graphics_fill_rect(ctx,GRect(posxpaleta,posypaleta,ANCHO_PALETA,ALTO_PALETA),0,GCornerNone);
      graphics_draw_bitmap_in_rect(ctx,s_catcher1,GRect(posxpaleta,posypaleta,
                                           ancho_paleta,
                                           alto_paleta));
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "posxpaleta %d posypaleta %d",posxpaleta,posypaleta);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "canvas_update_proc");
      loactual=objetos;
      while (loactual!=NULL) {
          dibuja_objeto(loactual,ctx);
          loactual=loactual->siguiente;
      }      
      if (mgameover) {
          pon_game_over(ctx,this_layer);
      }
  }
    
}



void highscores_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  int i,y,lalto;

    APP_LOG(APP_LOG_LEVEL_DEBUG,"highscores_window_load");
    //creamos textlayer con el texto de highscores y los highscores
    s_highscore_layer_text = text_layer_create(GRect(0, 0, window_bounds.size.w,
                                                    window_bounds.size.h / 3));
    text_layer_set_font(s_highscore_layer_text, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text_alignment(s_highscore_layer_text, GTextAlignmentCenter);
    text_layer_set_background_color(s_highscore_layer_text, GColorClear);
    text_layer_set_text_color(s_highscore_layer_text, GColorBlack);      
    layer_add_child(window_layer, text_layer_get_layer(s_highscore_layer_text));  
    text_layer_set_text(s_highscore_layer_text,buffer_highscores_etiqueta );
    y=window_bounds.size.h / 3;
    lalto=((window_bounds.size.h- (window_bounds.size.h / 5) - y) / MAX_PUNTUACIONES);
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        s_highscores_layer[i]=text_layer_create(GRect(0, y, window_bounds.size.w,
                                                    window_bounds.size.h / 3));
        text_layer_set_font(s_highscores_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
        text_layer_set_text_alignment(s_highscores_layer[i], GTextAlignmentCenter);
        text_layer_set_background_color(s_highscores_layer[i], GColorClear);
        text_layer_set_text_color(s_highscores_layer[i], GColorBlack);      
        layer_add_child(window_layer, text_layer_get_layer(s_highscores_layer[i]));  
        y+=lalto;
    }
    //dibujamos las max_puntuaciones
    for (i=0;i<MAX_PUNTUACIONES;++i) {
        snprintf(&buffer_highscores[i][0],80,"%05lu",mhighscores.puntuaciones[i]);
        text_layer_set_text(s_highscores_layer[i], buffer_highscores[i]);
    }
    //configuramos handler de cada click
    window_set_click_config_provider(s_highscores_window, config_escucha_botones_highscores);
}


void highscores_window_unload(Window *window) {
    //liberamos memoria usada por los layer creados
    int i;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG,"highscores_window_unload");
    if (s_highscore_layer_text!=NULL) {
      layer_destroy(text_layer_get_layer(s_highscore_layer_text));  
      s_highscore_layer_text=NULL;
    };
    if (s_highscores_layer!=NULL) {
        for (i=0;i<MAX_PUNTUACIONES;++i) {
            if (s_highscores_layer[i]!=NULL) {
                layer_destroy(text_layer_get_layer(s_highscores_layer[i]));  
                s_highscores_layer[i]=NULL;
            }
        }
    }
}



void acercade_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
    
    APP_LOG(APP_LOG_LEVEL_DEBUG,"acercade_window_load");
    s_acercade_layer_text = text_layer_create(GRect(0, 0, window_bounds.size.w,
                                                    window_bounds.size.h));
    text_layer_set_font(s_acercade_layer_text, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)); //ó FONT_KEY_GOTHIC_14_BOLD ó FONT_KEY_GOTHIC_28_BOLD
    text_layer_set_text_alignment(s_acercade_layer_text, GTextAlignmentCenter);
    text_layer_set_background_color(s_acercade_layer_text, GColorClear);
    text_layer_set_text_color(s_acercade_layer_text, GColorBlack);      
    text_layer_set_text(s_acercade_layer_text,texto_acercade);    
    layer_add_child(window_layer, text_layer_get_layer(s_acercade_layer_text));  
    //configuramos que al pulsar boton back vuelva al menu
    window_set_click_config_provider(s_acercade_window, config_escucha_botones);
}



void acercade_window_unload(Window *window) {
    //liberamos memoria usada por los layer creados
    
    APP_LOG(APP_LOG_LEVEL_DEBUG,"acercade_window_unload");
    if (s_acercade_layer_text!=NULL) {
      layer_destroy(text_layer_get_layer(s_acercade_layer_text));  
      s_acercade_layer_text=NULL;
    };
}



void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  int lnumaleat;
  GBitmap *lgobj;
  GBitmap **lanimaccaida;
  int lnumanimaccaida;
  int lmaxxobj;
  int ltipoobjeto;
  int lposx;
  int lmaxalto,lalto;;

  
  //capa de puntuación
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load");
  lmaxalto=-1;
  lalto=ALTO_LAYER_SCORE;
  if (lalto>lmaxalto) lmaxalto=lalto;
  s_score_layer = text_layer_create(GRect(0, 0, window_bounds.size.w / 2,lalto));
  lposx=  window_bounds.size.w / 2;
  lalto=maltolife;
  if (lalto>lmaxalto) lmaxalto=lalto;  
  s_lifes_layer=layer_create(GRect(lposx,0,window_bounds.size.w -lposx,lalto));
    layer_add_child(window_layer, s_lifes_layer);
  text_layer_set_font(s_score_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_score_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_score_layer, GColorClear);
  text_layer_set_text_color(s_score_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(s_score_layer));  
    
  // Create Layer
  maxyjuego=window_bounds.size.h - lmaxalto;
  s_canvas_layer = layer_create(GRect(0, lmaxalto, window_bounds.size.w, 
                                      maxyjuego));
  layer_add_child(window_layer, s_canvas_layer);
  
  
  
  // poner handler de redraw personalizado a s_canvas_layer y
  // s_lifes_layer
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  vidas=VIDAS_INICIALES; //empezamos con n vidas , ponemos variable aqui por si
  //llama a lifes_update_proc inmediatamente despues
  //de poner un "handler de redraw personalizado"
  layer_set_update_proc(s_lifes_layer, lifes_update_proc);
  //maxx=window_bounds.size.w - ancho_gota1;
  maxxpaleta=window_bounds.size.w - ancho_paleta;
  maxy=window_bounds.size.h;
  maxx=window_bounds.size.w;
  srand(time(NULL)); //inicializamos generador numeros aleatorios
  posypaleta=window_bounds.size.h-alto_paleta-ALTO_LAYER_SCORE;  
  //inicializamos los objetos (de momento solo gotas)
  libera_memoria_objetos(objetos);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load :libera_memoria_objetos hecho");
  objetos=NULL;
  lnumaleat=rand() % 100;
  if (lnumaleat>50) {
      lgobj=s_objeto1;
      ltipoobjeto=0; //gota de agua negra
      lanimaccaida=s_objeto1_animaccaida;
      lnumanimaccaida=3;
  }
  else
      {
      lgobj=s_objeto2;
      ltipoobjeto=1; //gota de agua blanca
      lanimaccaida=s_objeto2_animaccaida;
      lnumanimaccaida=3;
  }
  //ponemos una gota inicialmente
  lmaxxobj=maxx -gbitmap_get_bounds(lgobj).size.w;
  lnumaleat=rand() % lmaxxobj;
  objetos=inicializa_objeto(ltipoobjeto,lnumaleat,lgobj,lanimaccaida,lnumanimaccaida);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load :asignados objetos");
  ultpuntuacionaleatdecrementa=0; //ultima vez que ha decrementado variable para empezar a aleatorio salir nuevo objeto si no hay en pantallas maximo objetos
  ultpuntuacionincrobj=0; //ultima puntuacion donde ha incrementado el máximo de objetos en pantalla
  pon_puntuacion();  
  //pon_vidas();
  app_timer_register(SALTO_MS , timer_callback, NULL); //programamos que salte el reloj para que se muevan los objetos
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load :hecho app_timer_register");  
    //visibilidad_juego(false);  
  //visibilidad_menu(true);  
    
}

void main_window_unload(Window *window) {
  // Destroy Layer
 APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload");
  if (s_canvas_layer!=NULL) {
      layer_destroy(s_canvas_layer);
      s_canvas_layer=NULL;
  }
  if (s_score_layer!=NULL) {
      layer_destroy(text_layer_get_layer(s_score_layer));
      s_score_layer=NULL;
  }
  if (s_lifes_layer!=NULL) {
      layer_destroy(s_lifes_layer);
      s_lifes_layer=NULL;
  }
  libera_memoria_objetos(objetos);
  objetos=NULL;
  //bitmap_layer_destroy(s_bitmap_layer);
}




void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  
     
    APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_window_load");
  s_menu_layer = menu_layer_create(window_bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

    
}

void menu_window_unload(Window *window) {
  // Destroy Layer
  APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_window_unload");
  if (s_menu_layer!=NULL) {
        menu_layer_destroy(s_menu_layer);
        s_menu_layer=NULL;
  }

    //bitmap_layer_destroy(s_bitmap_layer);
}



//cuando se pulsa el boton de abajo
void pulsado_abajo_handler(ClickRecognizerRef recognizer, void *context) {  
  //Window *window = (Window *)context;
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pulsado abajo");
    //if (posxpaleta<window.size.w) {
    if (estado==1) {
        //en juego
        if (posxpaleta<maxxpaleta) {    
            posxpaleta++;
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Pulsado boton abajo, posxpaleta %d , maxxpaleta %d",posxpaleta,maxxpaleta);
        }
    }
}

//cuando se pulsa el boton de arriba
void pulsado_arriba_handler(ClickRecognizerRef recognizer, void *context) {  
  //Window *window = (Window *)context;

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pulsado arriba");
    if (estado==1) {
        //en juego
        if (posxpaleta>0) {
            posxpaleta--;
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "Pulsado boton arriba, posxpaleta %d",posxpaleta);
        }
    }
}

void pulsado_back_handler(ClickRecognizerRef recognizer, void *context) {  
  //Window *window = (Window *)context;
        
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pulsado back, estado %d",estado);
    if (estado==1) {
        //en juego
        estado=0;
        pon_menu();
    }
    else {
        if (estado==0) {
            //en menú
            //visibilidad_menu(false);
            window_stack_remove(s_main_window,false);
            window_stack_remove(s_menu_window,false);
            window_stack_pop_all(false); //quitamos todas las ventanas del stack de ventanas y salimos
        }
        else {
            if (estado==2) {
                //en highscores
                estado=0; //en menu
                pon_menu_window();
                quita_highscores_window();                
            }
            else {
                if (estado==3) {
                    //en acerca de
                    estado=0; //en menu
                    pon_menu_window();
                    quita_acercade_window();
                }
            }
        }
    }
}


void config_escucha_botones_highscores(void *context) {
    //configurar handles cuando se pulsa boton arriba y boton abajo
    window_single_click_subscribe(BUTTON_ID_BACK, pulsado_back_handler);
}


void config_escucha_botones_acercade(void *context) {
    //configurar handles cuando se pulsa boton arriba y boton abajo
    window_single_click_subscribe(BUTTON_ID_BACK, pulsado_back_handler);
}


                     

void config_escucha_botones(void *context) {
    //configurar handles cuando se pulsa boton arriba y boton abajo
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30,pulsado_abajo_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 30,pulsado_arriba_handler);
    window_single_click_subscribe(BUTTON_ID_BACK, pulsado_back_handler);
}



void init(void) {
    
  GRect lmedidas;
    
estado=0; // en menú principal
// Create main Window
 s_main_window = window_create();
 window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
});  
window_set_click_config_provider(s_main_window, config_escucha_botones);
  //cargamos imagenes
s_objeto1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_1);
s_objeto1_animaccaida[0]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_1_2);
s_objeto1_animaccaida[1]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_1_3);
s_objeto1_animaccaida[2]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_1_4);
s_objeto2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_2);
s_objeto2_animaccaida[0]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_2_2);
s_objeto2_animaccaida[1]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_2_3);
s_objeto2_animaccaida[2]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OBJETO_2_4);
s_catcher1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CATCHER1);
s_vida = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LIFE1); //bitmap con el dibujo de cada vida
lmedidas=gbitmap_get_bounds(s_vida);
mancholife=lmedidas.size.w;
maltolife=lmedidas.size.h;  
lmedidas=gbitmap_get_bounds(s_catcher1);
ancho_paleta=lmedidas.size.w;
alto_paleta=lmedidas.size.h;
//create menu window
s_menu_window = window_create();
window_set_window_handlers(s_menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload,
}); 
inicializa_high_scores(&mhighscores);
lee_high_scores(&mhighscores);
pon_menu();
}



    




void deinit(void) {
struct objeto *loactual;
struct objeto *losave;
int i;
    
  //destruimos los objetos
  loactual=objetos;
  while (loactual!=NULL) {
      losave=loactual;
      loactual=loactual->siguiente;
      free(losave);
  }
  // Destroy main Window
  if (s_main_window!=NULL) {
      window_destroy(s_main_window);
      s_main_window=NULL;
  }
  if (s_menu_window!=NULL) {
      window_destroy(s_menu_window);     
      s_menu_window=NULL;
  }
  if (s_highscores_window!=NULL) {
      window_destroy(s_highscores_window);     
      s_highscores_window=NULL;
  }
  if (s_acercade_window!=NULL) {
      window_destroy(s_acercade_window);     
      s_acercade_window=NULL;
  }    
  if (s_objeto1!=NULL) {
      gbitmap_destroy(s_objeto1); //destruir handle de imagen de objeto 1
      s_objeto1=NULL;
  }
  for (i=0;i<=2;++i) {
      if (s_objeto1_animaccaida[i]!=NULL) {
          gbitmap_destroy(s_objeto1_animaccaida[i]);
          s_objeto1_animaccaida[i]=NULL;
      }
  }
  if (s_objeto2!=NULL) {
      gbitmap_destroy(s_objeto2); //destruir handle de imagen de objeto 2
      s_objeto2=NULL;
  }
  for (i=0;i<=2;++i) {
      if (s_objeto2_animaccaida[i]!=NULL) {
          gbitmap_destroy(s_objeto2_animaccaida[i]);
          s_objeto2_animaccaida[i]=NULL;
      }
  }
   if (s_catcher1!=NULL) {
      gbitmap_destroy(s_catcher1); //destruir handle de imagen de objeto 2
      s_catcher1=NULL;
  }
   if (s_vida!=NULL) {
      gbitmap_destroy(s_vida); //destruir handle de imagen de objeto 2
      s_vida=NULL;
  }
    libera_memoria_objetos(objetos);
    objetos=NULL;

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}