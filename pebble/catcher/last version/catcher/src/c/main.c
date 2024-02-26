#include <pebble.h>
#include "main.h"
#include "objetos.h"
#include "utiles.h"
#include "mensajes.h"
#include "highscores.h"
#include "configuracion.h"
#include "ayuda.h"

uint32_t ultpuntuacionaleatdecrementa; //ultima puntuacion cuando ha decrementado cada n ticks empieza aleat para nuevo objeto
uint32_t ultpuntuacionincrobj; //ultima puntuacion cuando ha incrementado máximo de objetos en pantalla
uint32_t ultpuntuacionincrtiempo; //ultima puntuación cuando ha incrementado tiempo
uint32_t ultpuntuacionincrvida; //ultima vez que ha incrementado una vida
bool siprimeravidaincr; //indica si se ha añadido la 1a vida. la 1a vida se da a SI_N_PUNTOS_VIDA_INCR puntos , las restantes cada CADA_N_PUNTOS_VIDA_INCR
int maccel; //aceleración, dejando pulsado boton arriba o abajo aceleración incrementa o decrementa, positivo si pulsado boton abajo, negativo si pulsado botón arriba
int maccel2; //para que no acelere tan rápido
int posxpaleta;
int posypaleta;
int maxxpaleta;
int maxx;
int maxy; //máxima y del window del juego
int maxyjuego; //maxima posición y del layer del juego
int direcc=1;
uint32_t puntuacion; //32 bits sin signo
uint32_t pultincraleatreloj; //ultima puntuacion que ha incrementado el aleatorio de reloj para que aparezca menos seguido
int maleatreloj; //numero que si sale en aleatorio sale reloj
int vidas;
int estado; //0=en menú, 1=jugando , 2=mostrando highscores , 3=mostrando acerca de, 4=mostrando ayuda
Window *s_main_window=NULL;
Window *s_menu_window=NULL;
Window *s_submenu_window=NULL;
Window *s_acercade_window=NULL;
Layer *s_canvas_layer=NULL;
Layer *s_lifes_layer=NULL;
TextLayer *s_score_layer=NULL;
TextLayer *s_tiempo_layer=NULL;
TextLayer *s_acercade_layer_text=NULL;
GBitmap *s_objeto1=NULL;
GBitmap *s_objeto1_animaccaida[3]={NULL,NULL,NULL}; //estados del objeto 1, para cuando cae , animación del objeto 1 que ha caido
GBitmap *s_objeto2=NULL;
GBitmap *s_objeto2_animaccaida[3]={NULL,NULL,NULL}; //estados del objeto 2, para cuando cae , animación del objeto 1 que ha caido
GBitmap *s_catcher1=NULL;
GBitmap *s_vida=NULL;
GBitmap *s_reloj=NULL;
GBitmap *s_reloj_animaccaida[4]={NULL,NULL,NULL,NULL}; //la animación de caida del reloj
MenuLayer *s_menu_layer=NULL;
MenuLayer *s_submenu_layer=NULL; //layer para submenú
int ancho_paleta;
int alto_paleta;
struct objeto *objetos=NULL;
int mrelparaobj;
int relparaempiezaleat;
long ltickspasados;
int mticksesperamover;
int maltolife;
int mancholife;
int mseccactmenu; //sección de donde sale submenu, para saber que submenu poner
int msaveseccactmenu; //para cuando volvamos de submenu , para saber donde volvemos : si a menú principal, submenú,...
bool mnovamenuprincdespuessubmenu; //para que cuando salgamos de submenú no abra menú principal (util cuando va a submenu jugar y se selecciona modo de juego de jugar)
//static BitmapLayer *s_bitmap_layer;
bool mgameover; //si =true, game over, juego acabado.
//variables para el comportamiento del juego, cada cuanto pueden caer gotas, cada cuanto salen mas a la vez,... ----------
int ticks_empieza_aleatorio_salir_gota;
uint32_t cada_n_puntos_decrementa_aleat_n_obj;
uint32_t cada_n_puntos_incrementa_obj_max;
int max_objetos;
uint32_t mtiempoqueda; //tiempo, en segundos que quedan si modo juego es por tiempo
uint32_t moldparaupdtiempo; //para saber ultimo segundo puesto, para saber cuando actualiza mtiempoqueda. lee 
uint32_t mrelupdtiempo; //relativo para saber cuando ha pasado un segundo, ticks relativos a la última actualización
uint32_t mtickscadaseg; //para saber cuantos ticks (llamadas a timer_callback representa un segundo)
//-------------------------------------------------------------------------------------------------------------------------







void pon_puntuacion(void) {

    pon_puntuacion_en_layer(s_score_layer,puntuacion);
}

void pon_vidas(void) {
    layer_mark_dirty(s_lifes_layer);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_vidas");
}



static void animac_caida_objetos() {
    struct objeto *loactual;
    //cambiamos animación cada click de los caidos    
    loactual=objetos;
    while (loactual!=NULL) {
          if (loactual->activado) {
              if (loactual->estado==1)     {
                  if (loactual->ianimaccaida==loactual->numanimaccaida) {
                      //ya no hay mas "bitmaps de caida"
                      loactual->activado=false;
                      if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
                          //modo por vidas
                          //quita la vida cuando la animación de caida se ha acabado
                          vidas--;
                          //APP_LOG(APP_LOG_LEVEL_DEBUG, "vidas: %d",vidas);
                          pon_vidas();
                          if (vidas==0) {
                              //no hay mas vidas
                              //graba puntuación en los high-scores
                              graba_score(puntuacion,&gconfig);
                              //pon game over y el juego no se mueve mas
                              mgameover=true;                                  
                              layer_mark_dirty(s_canvas_layer);
                              layer_mark_dirty(s_lifes_layer); //para que se vea que no quedan vidas
                              //APP_LOG(APP_LOG_LEVEL_DEBUG, "GAME OVER");
                          }
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


static void mirasianyadeobjeto() {
    int lnumaleat;
    GBitmap *lgobj , **lganimacobj;
    int lnumanimaccaida;
    int lmaxxobj;
    tTipoObjeto ltipoobjeto;
    int lnummax;
    int lnum2;
    
    
    if (mrelparaobj>=relparaempiezaleat) {
        if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
            //modo por vidas
            lnum2=1;
            lnummax=5;
        }
        if (gconfig.modojuego==MODO_JUEGO_TIEMPO) {
            //modo por tiempo
            lnum2=1;
            lnummax=4;
        }            
        lnumaleat=rand() % lnummax;        
        if (lnumaleat>lnum2) {
            mrelparaobj=0;
            //añadimos otra gota
            lnumaleat=rand() % 100;
            if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
                if (lnumaleat>50) {
                    ltipoobjeto=egota_negra; //gota de agua negra
                }
                else {
                    ltipoobjeto=egota_blanca; //gota de agua blanca
                }
            }
            else {
                //modo de juego por tiempo
                if (lnumaleat>=maleatreloj) {
                    ltipoobjeto=ereloj;
                }
                else {
                    if (lnumaleat<maleatreloj && lnumaleat>50) {
                        ltipoobjeto=egota_blanca;
                    }
                    else {
                        ltipoobjeto=egota_negra;
                    }
                }
            }
            lganimacobj=NULL;
            lnumanimaccaida=0;
            if (ltipoobjeto==egota_negra) {
                lgobj=s_objeto1;
                lganimacobj=s_objeto1_animaccaida;
                lnumanimaccaida=3;
            }
            if (ltipoobjeto==egota_blanca) {
                lgobj=s_objeto2;
                lganimacobj=s_objeto2_animaccaida;
                lnumanimaccaida=3;
            }
            if (ltipoobjeto==ereloj) {
                 lgobj=s_reloj;
                 lganimacobj=s_reloj_animaccaida;
                 lnumanimaccaida=4;
            }
            lmaxxobj=maxx -gbitmap_get_bounds(lgobj).size.w;
            lnumaleat=rand() % lmaxxobj;
            anyadeobjetoaobjetos(lgobj,lganimacobj,lnumanimaccaida, 
                                 ltipoobjeto,lnumaleat,objetos);
        }
    }
    else
        {
        mrelparaobj++;
    }
}

static void movimiento_objetos_y_caida() {
    
    struct objeto *loactual;
    int lnumobjetos;    
    int laltoobjeto;
    uint32_t pparc;
        
    layer_mark_dirty(s_canvas_layer);
    loactual=objetos;
    lnumobjetos=0;
    ltickspasados++;
    while (loactual!=NULL) {
        if (loactual->activado && (loactual->estado==0)) {
            loactual->y+=2;
            laltoobjeto=gbitmap_get_bounds(loactual->imagen).size.h;
            if (colision(loactual,posxpaleta,posypaleta,ancho_paleta,alto_paleta)) {
                loactual->activado=false;
                pparc=devuelve_puntuacion_objeto(loactual->tipo);
                puntuacion+=pparc;
                if (loactual->tipo==ereloj) {
                    mtiempoqueda+=SEGUNDOS_SUMA_SI_RELOJ; //sumamos n segundos
                }
                if ((puntuacion-ultpuntuacionincrobj)>=cada_n_puntos_incrementa_obj_max) {
                    ultpuntuacionincrobj=puntuacion;
                    max_objetos++;                                  
                }   
                if ((puntuacion-ultpuntuacionaleatdecrementa)>=cada_n_puntos_decrementa_aleat_n_obj) {
                    ultpuntuacionaleatdecrementa=puntuacion;
                    if (relparaempiezaleat>0) {
                        relparaempiezaleat-=2;
                        if (relparaempiezaleat<0) {
                            relparaempiezaleat=0;
                        }
                    }
                }
                if (gconfig.modojuego==MODO_JUEGO_TIEMPO) {
                    if ((puntuacion-ultpuntuacionincrtiempo)==CADA_PUNTOS_INCREMENTA_TIEMPO) {
                        mtiempoqueda+=TIEMPO_INCREMENTA_CADA_PUNTOS;
                        ultpuntuacionincrtiempo=puntuacion;
                    }                    
                    if ((puntuacion-pultincraleatreloj)==CADA_PUNTOS_DECREMENTA_POSIBILIDAD_RELOJ) {
                        if (maleatreloj<=98) {
                            maleatreloj+=1;
                            pultincraleatreloj=puntuacion;
                        }
                    }
                }
                if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
                     if (!siprimeravidaincr) {
                         if ((puntuacion-ultpuntuacionincrvida)>=SI_N_PUNTOS_VIDA_INCR) {
                             vidas++; //incrementa la 1a vida a los n puntos
                             pon_vidas();
                             ultpuntuacionincrvida=puntuacion;
                             siprimeravidaincr=true;
                         }
                     }
                    else {
                        if ((puntuacion-ultpuntuacionincrvida)>=CADA_N_PUNTOS_VIDA_INCR) {
                             vidas++; //incrementa cada n puntos una vida (la 1a a los n puntos)
                             pon_vidas();
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
                    //gota caida, empieza animación de caida
                    vibrar();
                    loactual->estado=1;
                    loactual->y=maxyjuego-laltoobjeto;
                }
            }
        }
        loactual=loactual->siguiente;
    } 
    //miramos si el número de objetos en pantalla
    //es menor al número máximo de objetos.
    if (lnumobjetos<max_objetos) {
        mirasianyadeobjeto();
    }
}


static void actualiza_contador_tiempo_restante(void) {
    
    if (mtiempoqueda>0) {
         mrelupdtiempo++;
         if ((mrelupdtiempo-moldparaupdtiempo)>=mtickscadaseg) {
             //ha pasado un segundo
             mrelupdtiempo=0;
             moldparaupdtiempo=0;
             mtiempoqueda--;
             pon_tiempo_en_layer(s_tiempo_layer,mtiempoqueda);
             if (mtiempoqueda==0) {
                 //no hay mas tiempo
                 //guardamos puntuación
                 graba_score(puntuacion,&gconfig);
                 //game over
                 mgameover=true;
             }
         }
    }
}

void timer_callback(void *data) {
    struct objeto *loactual;
    int lticksslow;
    bool lhaycaidos;    

  if (estado==1) {
      if (!mgameover) {
          //lo hacemos aqui ya que despues de llamar a actualiza_contador_tiempo_restante
          //puede ser que no quede tiempo
          if (gconfig.modojuego==MODO_JUEGO_TIEMPO) {
              actualiza_contador_tiempo_restante();
          }
      }
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
              animac_caida_objetos();              
          }
          if (puntuacion<250) {
              lticksslow=2;          
          }
          else {
              lticksslow=1;
          }
          if (mticksesperamover>=lticksslow) {
              mticksesperamover=0;
              movimiento_objetos_y_caida();
          }
          else {
              mticksesperamover++;
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
    //quita_menu_window();    
    puntuacion=0;
    posxpaleta=0;
    mticksesperamover=0;
    if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
        //modo de juego por vidas
        ticks_empieza_aleatorio_salir_gota=35;
        cada_n_puntos_decrementa_aleat_n_obj=80;
        cada_n_puntos_incrementa_obj_max=100;
        max_objetos=4; //número máximo de objetos iniciales
        siprimeravidaincr=false; //aun no se ha incrementado la 1a vida
        ultpuntuacionincrvida=0; //cuando se ha incrementado 1a vida
    } 
    if (gconfig.modojuego==MODO_JUEGO_TIEMPO) {
        //modo de juego por tiempo
        ticks_empieza_aleatorio_salir_gota=15;
        cada_n_puntos_decrementa_aleat_n_obj=40;
        cada_n_puntos_incrementa_obj_max=60;
        max_objetos=5; //número de objetos máximos iniciales 
        mtiempoqueda=SEGUNDOS_INICIALES_MODO_TIEMPO; //empezamos con 90 segundos
        mtickscadaseg=1000 / SALTO_MS;
        moldparaupdtiempo=0; //inicializamos contador relativo
        maleatreloj=90;
        pultincraleatreloj=0;
    }
    //inicilización de variables-------------------------------------
    relparaempiezaleat=ticks_empieza_aleatorio_salir_gota;  //cada n ticks sale una gota nueva si no hay mas del máximo de gotas    
    mrelparaobj=0; //para saber cuando poner en escena otro objeto
    ltickspasados=0; //nos servirá para ir subiendo de nivel (acelerando, mas gotas seguidas, ....)    
    ultpuntuacionaleatdecrementa=0;
    ultpuntuacionincrobj=0;
    ultpuntuacionincrtiempo=0;
    estado=1;
    mgameover=false;
    //-------------------------------------------------------------------
    pon_juego_window();
    //app_timer_register(SALTO_MS , timer_callback, NULL); //salta de nuevo el reloj
}


void pon_menu()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_menu");
    graba_config(&gconfig);
    quita_juego_window();
    estado=0;
    pon_menu_window();       
}


void pon_highscores()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_menu");
    //graba_high_scores(&gconfig);
    estado=2;
    pon_highscores_window();       
    quita_menu_window();    
    
}


void pon_ayuda()  {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pon_menu");
    //graba_high_scores(&gconfig);
    estado=4;
    pon_ayuda_window();      
    quita_menu_window();    
}




void pon_acercade() {
    estado=3;
    pon_acercade_window();
    quita_menu_window();
}



/*------para menú principal------------------------------------------------------*/
uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_sections_callback");
  return NUM_MENU_SECTIONS;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_rows_callback");
  switch (section_index) {
    case 0:
      //menú principal
      return NUM_FIRST_MENU_ITEMS;
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
      // menú principal
      menu_cell_basic_header_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_MENU,gconfig.idioma));
      break;
  }
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_draw_row_callback");
  // Determine which section we're going to draw in
  switch (cell_index->section) {
      //menú principal
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          // jugar
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_JUGAR,gconfig.idioma));
          //menu_cell_basic_draw(ctx, cell_layer, "Basic Item", "With a subtitle", NULL);
          break;
        case 1:
            //configuración
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_CONFIG,gconfig.idioma));
          break;
        case 2:
          //hi-scores
          menu_cell_title_draw(ctx, cell_layer, devuelve_mensaje_i18n(ID_RECORDS,gconfig.idioma));
          //menu_cell_basic_draw(ctx, cell_layer, "Icon Item", "Select to cycle", s_menu_icons[s_current_icon]);
          break;
          case 3:
          menu_cell_title_draw(ctx, cell_layer, devuelve_mensaje_i18n(ID_AYUDA,gconfig.idioma));
          break;
          case 4:
          menu_cell_title_draw(ctx, cell_layer, devuelve_mensaje_i18n(ID_ACERCADE,gconfig.idioma));
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
    switch (cell_index->section) {
    case 0:
        //sección principal de menú
          switch (cell_index->row) {
              case 0:
                  //jugar          
                  entra_submenu(cell_index->row);
                  break;
              case 1:
                  //config
                  //cremos window                  
                  entra_submenu(cell_index->row);
                  break;
              case 2:
                  //muestra highscores
                  pon_highscores();
                  break;
              case 3:
                  //muestra acerca de
                  pon_ayuda();
                  break;
              case 4:
                  //muestra acerca de
                  pon_acercade();
                  break;
          }
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



/*-fin de para menú principal----------------------------------------------------------------------------------*/


/*--para submenús------------------------------------------------------*/
uint16_t submenu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_sections_callback");
  return NUM_MENU_SECTIONS;
}

uint16_t submenu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_num_rows_callback");
  switch (mseccactmenu) {
    case 0:
      //jugar
      return NUM_JUGAR_MENU_ITEMS;
    case 1:
      //config
      return NUM_CONFIG_MENU_ITEMS;
    default:
      return 0;
  }
}

int16_t submenu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_get_header_height_callback");
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void submenu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_draw_header_callback");
  // Determine which section we're working with
  switch (mseccactmenu) {
    case 0:
      //jugar
      menu_cell_basic_header_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_JUGAR,gconfig.idioma));
      break;  
    case 1:
      //config
      menu_cell_basic_header_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_CONFIG,gconfig.idioma));
      break;
  }
}

void submenu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_draw_row_callback");
  // Determine which section we're going to draw in
    int lid;
  switch (mseccactmenu) {
      case 0:
          //jugar            
      switch (cell_index->row) {
              case 0:
          // modo de juego por vidas
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_MODOVIDAS,gconfig.idioma));
          break;
        case 1:
            //modo de juego por tiempo
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_MODOTIEMPO,gconfig.idioma));
          break;
          }
      break;
      case 1:
          //config
          switch (cell_index->row) {
              case 0:
          // Idioma
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(ID_NOMBRE_IDIOMA,gconfig.idioma));
          break;
        case 1:
            //vibrar
          if (gconfig.vibrar_al_caer_objeto) lid=ID_VIBRAR_ACT; else lid=ID_VIBRAR_DESACT;
          menu_cell_title_draw(ctx, cell_layer,devuelve_mensaje_i18n(lid,gconfig.idioma));
          break;
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


void submenu_select_callback(MenuLayer *pmenu_layer, MenuIndex *cell_index, void *data) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "menu_select_callback");
  // Use the row to specify which item will receive the select action
    switch (mseccactmenu) {
        case 0:
        //sección de jugar
        switch (cell_index->row) {
            case 0: 
                //jugar por vidas
                gconfig.modojuego=MODO_JUEGO_VIDAS;
                //pon_menu_window(); //para que al salir del juego vaya a menú principal
                sale_submenu(false);                
                pon_juego();
                break;
            case 1:
                //jugar por tiempo
                gconfig.modojuego=MODO_JUEGO_TIEMPO;
                //pon_menu_window(); //para que al salir del juego vaya a menú principal
                sale_submenu(false);                
                pon_juego();
                break;
        }
        break;
    case 1:
        //sección de configuración
        switch (cell_index->row) {
            case 0: 
            //idioma (seleccionar idioma)
            siguiente_idioma(&gconfig);
            layer_mark_dirty(menu_layer_get_layer(pmenu_layer)); //para que redibuje submenú            
            break;
            case 1:
            //vibrar (activa/desactiva check de vibrar)
                if (gconfig.vibrar_al_caer_objeto)
                    gconfig.vibrar_al_caer_objeto=false;
            else gconfig.vibrar_al_caer_objeto=true;
            layer_mark_dirty(menu_layer_get_layer(pmenu_layer)); //para que redibuje submenú            
            break;
        }
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
 /*fin de para submenús------------------------------------------------------------------------------------*/


void submenu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  
     
    APP_LOG(APP_LOG_LEVEL_DEBUG, "submenu_window_load");    
    s_submenu_layer = menu_layer_create(window_bounds);
  menu_layer_set_callbacks(s_submenu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = submenu_get_num_sections_callback,
    .get_num_rows = submenu_get_num_rows_callback,
    .get_header_height = submenu_get_header_height_callback,
    .draw_header = submenu_draw_header_callback,
    .draw_row = submenu_draw_row_callback,
    .select_click = submenu_select_callback,
  });
  menu_layer_set_click_config_onto_window(s_submenu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(s_submenu_layer));

    
}

void submenu_window_unload(Window *window) {
  // Destroy Layer
  APP_LOG(APP_LOG_LEVEL_DEBUG, "submenu_window_unload");  
  //sale_submenu();  
  //de un submenú de submenú  
  /*if (s_menu_layer!=NULL)
      layer_mark_dirty(menu_layer_get_layer(s_menu_layer)); //para que redibuje menú principal*/
 graba_config(&gconfig); //guardamos configuración por si submenú la ha tocado
  if (!mnovamenuprincdespuessubmenu) {
      pon_menu_window();
  }
      else mnovamenuprincdespuessubmenu=false; //reestablecemos variable
}


void sale_submenu(bool pVaAMenuPrincipal) {
    if (!pVaAMenuPrincipal) {
        mnovamenuprincdespuessubmenu=true;
    }
    if (s_submenu_layer!=NULL) {
        menu_layer_destroy(s_submenu_layer);
        s_submenu_layer=NULL;
    }    
    if (s_submenu_window!=NULL) {
        if (window_stack_contains_window(s_submenu_window)) {
            //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
            window_stack_remove(s_submenu_window,true);
        }
        window_destroy(s_submenu_window);     
        s_submenu_window=NULL;
        mseccactmenu=msaveseccactmenu;   
    }    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "sale_submenu");
}



void entra_submenu(int piseccion) {
    
    msaveseccactmenu=mseccactmenu;
    mseccactmenu=piseccion;
    s_submenu_window=window_create();
    window_set_window_handlers(s_submenu_window, (WindowHandlers) {
        .load=submenu_window_load,
        .unload=submenu_window_unload
    });
    if (!window_stack_contains_window(s_submenu_window)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "window_stack_push s_main_window");
        window_stack_push(s_submenu_window,true);
    }
    quita_menu_window();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "entra_submenu");
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
    text_layer_set_text(s_acercade_layer_text, devuelve_mensaje_i18n(ID_ACERCA_DE_MENSAJE,gconfig.idioma));    
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
  tTipoObjeto ltipoobjeto;
  int lposx;
  int lmaxalto,lalto,lalto_score,lalto_tiempo;

  
  //capa de puntuación
  APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load");
  lmaxalto=-1;
  lalto_score=ALTO_LAYER_SCORE;
  lalto_tiempo=ALTO_LAYER_TIEMPO;
  if (lalto_score>lmaxalto) lmaxalto=lalto_score;
  if (lalto_tiempo>lmaxalto) lmaxalto=lalto_score;
  lposx=  window_bounds.size.w / 2;
  lalto=maltolife;
  if (lalto>lmaxalto) lmaxalto=lalto;  
  if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
      //modo por vidas
      s_lifes_layer=layer_create(GRect(lposx,0,window_bounds.size.w -lposx,lalto));
      layer_add_child(window_layer, s_lifes_layer);
  }
  if (gconfig.modojuego==MODO_JUEGO_TIEMPO) {
      //modo por tiempo
      s_tiempo_layer=text_layer_create(GRect(lposx,0,window_bounds.size.w -lposx,lalto_tiempo));
      text_layer_set_font(s_tiempo_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
      text_layer_set_text_alignment(s_tiempo_layer, GTextAlignmentCenter);
      text_layer_set_background_color(s_tiempo_layer, GColorClear);
      text_layer_set_text_color(s_tiempo_layer, GColorBlack);
      layer_add_child(window_layer, text_layer_get_layer(s_tiempo_layer));        
  }
  s_score_layer = text_layer_create(GRect(0, 0, window_bounds.size.w / 2,lalto_score));
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
  if (gconfig.modojuego==MODO_JUEGO_VIDAS) {
      //si modo de juego por vidas
      vidas=VIDAS_INICIALES; //empezamos con n vidas , ponemos variable aqui por si
      //llama a lifes_update_proc inmediatamente despues
      //de poner un "handler de redraw personalizado"
      layer_set_update_proc(s_lifes_layer, lifes_update_proc);
  }
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
      ltipoobjeto=egota_negra; //gota de agua negra
      lanimaccaida=s_objeto1_animaccaida;
      lnumanimaccaida=3;
  }
  else
      {
      lgobj=s_objeto2;
      ltipoobjeto=egota_negra; //gota de agua blanca
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
  ultpuntuacionincrtiempo=0; //ultima puntuación cuando ha incrementado tiempo
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
  if (s_tiempo_layer!=NULL) {
      layer_destroy(text_layer_get_layer(s_tiempo_layer));
      s_tiempo_layer=NULL;
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
    mseccactmenu=0; //sección actual, la primera, menú principal
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
        posxpaleta+=maccel;
        maccel2++;
        if (maccel2>4) {
            maccel++;
            maccel2=0;
        }
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Pulsado boton abajo, posxpaleta %d , maxxpaleta %d",posxpaleta,maxxpaleta);
        if (posxpaleta>maxxpaleta) {
            posxpaleta=maxxpaleta;
        }
    }
    if (estado==2) {
        //en highscores
        siguiente_ventana_highscores();
    }
}

//cuando se pulsa el boton de arriba
void pulsado_arriba_handler(ClickRecognizerRef recognizer, void *context) {  
  //Window *window = (Window *)context;

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "pulsado arriba");
    if (estado==1) {
        //en juego
        posxpaleta+=maccel;
        maccel2++;
        if (maccel2>4)  {
            maccel--;
            maccel2=0;
        }
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Pulsado boton arriba, posxpaleta %d",posxpaleta);
        if (posxpaleta<0) {
            posxpaleta=0;
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
            APP_LOG(APP_LOG_LEVEL_DEBUG,"pulsado_back_handler, estado=0, en menú");
            window_stack_remove(s_main_window,false);
            if (s_menu_window!=NULL)
                window_stack_remove(s_menu_window,false);
            if (s_submenu_window!=NULL)
                window_stack_remove(s_submenu_window,false);            
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
    window_single_click_subscribe(BUTTON_ID_DOWN, pulsado_abajo_handler);
}


void config_escucha_botones_acercade(void *context) {
    //configurar handles cuando se pulsa boton arriba y boton abajo
    window_single_click_subscribe(BUTTON_ID_BACK, pulsado_back_handler);
}

void click_abajo_handler(ClickRecognizerRef recognizer, void *context) {  
    //al pulsar boton abajo
    maccel=1;
    maccel2=0;
}

void unclick_abajo_handler(ClickRecognizerRef recognizer, void *context) {  
    //al despulsar boton abajo
    maccel=1;
    maccel2=0;
}

void click_arriba_handler(ClickRecognizerRef recognizer, void *context) {  
    //al pulsar boton arriba
    maccel=-1;
    maccel2=0;
}

void unclick_arriba_handler(ClickRecognizerRef recognizer, void *context) {  
    //al despulsar boton arriba
    maccel=-1;
    maccel2=0;
}





void config_escucha_botones(void *context) {
    //configurar handles cuando se pulsa boton arriba y boton abajo
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 30,pulsado_abajo_handler);
    window_raw_click_subscribe(BUTTON_ID_DOWN,click_abajo_handler,unclick_abajo_handler,context);
    window_raw_click_subscribe(BUTTON_ID_UP,click_arriba_handler,unclick_arriba_handler,context);
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
s_reloj=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RELOJ);  //bitmap con el dibujo de un reloj, para en modo tiempo incrementar tiempo
s_reloj_animaccaida[0]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RELOJ_1);
s_reloj_animaccaida[1]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RELOJ_2);
s_reloj_animaccaida[2]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RELOJ_3);
s_reloj_animaccaida[3]=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RELOJ_4);

mancholife=lmedidas.size.w;
maltolife=lmedidas.size.h;  
lmedidas=gbitmap_get_bounds(s_catcher1);
ancho_paleta=lmedidas.size.w;
alto_paleta=lmedidas.size.h;
//create menu window
mnovamenuprincdespuessubmenu=false;
s_menu_window = window_create();
window_set_window_handlers(s_menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload,
}); 
lee_config(&gconfig);
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
  if (s_submenu_window!=NULL) {
      window_destroy(s_submenu_window);     
      s_submenu_window=NULL;
  }
  destroy_window_highscores();  
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
   if (s_reloj!=NULL) {
      gbitmap_destroy(s_reloj); //destruir handle de imagen de objeto 2
      s_reloj=NULL;
  }
   for (i=0;i<=3;++i) {
      if (s_reloj_animaccaida[i]!=NULL) {
          gbitmap_destroy(s_reloj_animaccaida[i]);
          s_reloj_animaccaida[i]=NULL;
      }
  }
    libera_memoria_objetos(objetos);
    objetos=NULL;

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}