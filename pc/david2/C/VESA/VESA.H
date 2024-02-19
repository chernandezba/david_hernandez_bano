#ifndef _VESA_H
#define _VESA_H
//definiciones de funciones
int initvesa(int idmodo);
void closevesa(void);
int obtenervesainfo(struct vesa_info *p);
int obtenervesainfo2(struct vesa_info2 *p);
void obtenermodoinfo(struct vesa_mode_info *p);
void punto256c(unsigned int x,unsigned int y,unsigned long color);
void linea256c(unsigned int x1,unsigned int y1,unsigned int x2,
unsigned int y2,unsigned long color);
void circulo256c(unsigned int cenx,unsigned int ceny,unsigned int radio,
unsigned long color);


//estructuras
struct vesa_info {
  char identificacion[4];
  unsigned int vesa_version;
  char far *puntero_oem_name;
  unsigned long capabilities;
  unsigned int far *vesa_oem_modes;
  unsigned int num_blockes_mem; //número de blockes de 64 Kb de la memoria de video
  char reserved[238];
  };
struct vesa_info2 {
  char identificacion[4];
  unsigned int vesa_version;
  char far *fabricante;
  unsigned long posibil_hard_video;
  unsigned int far *vesa_oem_modes;
  unsigned int bloques64k;
  unsigned int software_version;
  char far *vendedor;
  char far *producto;
  char far *rev_producto;
  char implemen[222];
  char caddatos[256];
  };

struct vesa_mode_info {
  struct mode_attrib {
  unsigned soportado:1;
  unsigned opcional_info_disponible:1;
  unsigned salida_bios_soportado:1;
  unsigned color_monocromo:1;
  unsigned grafico_texto:1;
  //solo para VESA >=2.0
  unsigned mode_not_vga_compat:1; //si =1 modo no compatible VGA
  unsigned modo_cambio_bancos_soportado:1; //si =1 modo de cambio de bancos soportado
  unsigned buffer_lineal:1; //si =1 buffer lineal disponible
  unsigned :8;
  } mode_attrib;
  struct attrib_window {
	 unsigned existe:1;
	 unsigned se_puede_leer:1;
	 unsigned se_puede_escribir:1;
	 } windowa;
  struct attrib_window windowb;
  unsigned int window_granularity;
  unsigned int kb_window;
  unsigned int segment_windowa;
  unsigned int segment_windowb;
  void far *funcion_posicionamiento_window; //equivalente a funci¢n 4fh subfunci¢n 5h
  unsigned int bytes_scan_line; //lo que viene ahora es opcional para los modos VESA , necesitado para los modos OEM
  unsigned int ancho;
  unsigned int alto;
  unsigned char ancho_caracter;
  unsigned char alto_caracter;
  unsigned char numero_planos_memoria;
  unsigned char bits_pixel;
  unsigned char bancos;
  unsigned char tipo_modelo_memoria;
  unsigned char longitud_banco_kb;
  unsigned char num_imagenes;
  unsigned char reserv1;
  //VESA 1.2 o superior
  unsigned char tam_rojo; //en bits
  unsigned char pos_rojo; //posición del rojo en el color
  unsigned char tam_verde; //en bits
  unsigned char pos_verde; //posición del verde en el color
  unsigned char tam_azul; //en bits
  unsigned char pos_azul; //posición del azul en el color
  unsigned char tam_val_res; //en bits
  unsigned char pos_res; //posición del reservado
  unsigned char info_col_directo; //Bit 0:si =0 rampa de color del DAC es fija
											 //      si =1 rampa es programable
											 //Bit 1:si =0 bits reservados no pueden ser usados
											 //		si =1 bits reservados pueden ser usados
  //VESA 2.0 o superior
  unsigned long dir_fisica_buffer_lineal;
  unsigned long offset_com_mem_video_fuera_pant;
  unsigned int mem_video_kb; //memoria de video en Kb
  char reserved[206];
  };
  
#endif
