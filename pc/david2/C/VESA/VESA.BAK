#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "vesa.h"

#define paginar(__pag) \
					regs.r_ax=0x4f05;\
					regs.r_bx=0;\
					regs.r_dx=(unsigned int)__pag; \
					intr(0x10,&regs);
#define segmentopant vesamodeinfo.segment_windowa //segmento de la pantalla del modo
//en este macro tiene que haber en la función la variable
//struct REGPACK regs;
static unsigned int bancoact,anchopant,altopant,modoac,inicializadavesa=0;
static unsigned long longitudbanco;
static unsigned char oldmodo; //modo anterior antes de entrar en modo VESA
static struct vesa_mode_info vesamodeinfo;
static struct vesa_info infovesa;
static struct vesa_info2 infovesa2; //sólo si BIOS VESA>=2.00
void novideopun(unsigned int x,unsigned int y,
unsigned long color2);
void (*punto)(unsigned int x,unsigned int y,
unsigned long color2)=novideopun;
//(*punto) puntero a la rutina de punto,se adapta segun
//numero de colores del modo de video
void novideolin(unsigned int x1,unsigned int y1,unsigned int x2,
unsigned int y2,unsigned long color2);
void (*linea)(unsigned int x1,unsigned int y1,
unsigned int x2,unsigned int y2,unsigned long color2)=novideolin;
//(*linea) puntero a la rutina de linea,se adapta segun
//numero de colores del modo de video

static char *tiposmem[]= {
"texto",
"graficos",
"graficos hercules",
"arquitectura de 4-planos",
"arquitectura de pixeles empacados",
"256 color(unchained)",
"Color directo",
"color YUV"};

//---------------------------------------------------------

int initvesa(int idmodo) {
//idmodo=
//0:640X400 256 colores
//1:640X480 256 colores
//2:800X600 256 colores
//3:1024X768 256 colores
//4:1280X960 256 colores 
//retorna 0 si ha podido cambiar
//-1 si no hay una BIOS VESA
//-2 si ese modo no está disponible
//actualiza estructuras vesamodeinfo,infovesa y las variables
//anchopan,altopan,modoac

struct REGPACK regs;
unsigned int far *pmodos;
int sifin;


if (obtenervesainfo(&infovesa)!=0) return -1;
obtenervesainfo2(&infovesa2); //sólo lo hará si versión BIOS VESA >=2.00
pmodos=infovesa.vesa_oem_modes; //modos OEM y VESA soportados
if (idmodo==0) {
  altopant=400;
  anchopant=640;
  }
if (idmodo==1) {
  altopant=480;
  anchopant=640;
  }
if (idmodo==2) {
  altopant=600;
  anchopant=800;
  }
if (idmodo==3) {
  altopant=768;
  anchopant=1024;
  }
if (idmodo==4) {
  altopant=1280;
  anchopant=960;
  }
regs.r_ax=0x0f00;
intr(0x10,&regs);
oldmodo=(unsigned char)(regs.r_ax & 0xff);
sifin=0;
while (1==1) {
  regs.r_ax=0x4f01; // obtener informacion adicional sobre modo de VESA SuperVGA BIOS
  regs.r_di=FP_OFF(&vesamodeinfo);
  regs.r_es=FP_SEG(&vesamodeinfo);
  regs.r_cx=*pmodos;
  intr(0x10,&regs);
  if (!(regs.r_ax & 0x0100)) {
	 if (vesamodeinfo.ancho==anchopant && vesamodeinfo.alto==altopant &&
	 vesamodeinfo.bits_pixel==8 /*256 colores*/ && 
	 vesamodeinfo.tipo_modelo_memoria==4/*pixeles empaquetados*/) break;
	 }
  pmodos++;
  if (*pmodos==0xffff) {
	 if (sifin==1) break;
	 sifin=1;
	 pmodos++;
	 }
}
if (*pmodos==0xffff) return -2;
modoac=*pmodos;
regs.r_ax=0x4f02; //poner modo VESA
regs.r_bx=modoac;
intr(0x10,&regs);
if (regs.r_ax & 0x0100) return -2;
regs.r_ax=0x4f05;
regs.r_bx=0;
regs.r_dx=0;
intr(0x10,&regs); //poner página 0
bancoact=0;
longitudbanco=vesamodeinfo.window_granularity * 1024L;
inicializadavesa=1;
if (idmodo>=0 && idmodo<=4) {
  //modos de 256 colores
  linea=linea256c;
  punto=punto256c;
  }
return 0;
}

//---------------------------------------------------------

void closevesa(void) {
struct REGPACK regs;

if (inicializadavesa==1) {
  regs.r_ax=oldmodo;
  intr(0x10,&regs);
  }
}
//---------------------------------------------------------
int obtenervesainfo(struct vesa_info *p) {
//p=puntero donde se copiará estructura de vesa_info
//si devuelve -1 BIOS VESA no encontrada
//si devuelve 0 no hay errores
struct REGPACK regs;

regs.r_ax=0x4f00;
regs.r_di=FP_OFF(p);
regs.r_es=FP_SEG(p);
intr(0x10,&regs); // obtener informacion sobre VESA SuperVGA BIOS
if ((regs.r_ax & 0x00ff)!=0x4f)
  //VESA SuperVGA BIOS no instalada
  return -1;
return 0;
}

//---------------------------------------------------------

int obtenervesainfo2(struct vesa_info2 *p) {
//igual que obtenervesainfo pero para la BIOS VESA 2.0 o superior
//si devuelve -1 BIOS VESA no encontrada
//si devuelve -2 version BIOS VESA < 2.00
//si devuelve 0 no hay errores
char buffer[512];
struct REGPACK regs;


regs.r_ax=0x4f00;
regs.r_di=FP_OFF(buffer);
regs.r_es=FP_SEG(buffer);
intr(0x10,&regs); // obtener informacion sobre VESA SuperVGA BIOS
if ((regs.r_ax & 0x00ff)!=0x4f)
  //VESA SuperVGA BIOS no instalada
  return -1;
if (((struct vesa_info2 *)buffer)->vesa_version>=0x0200) {
  memcpy(p->identificacion,"VBE2",4); //para obtener info de VESA 2.0
  regs.r_ax=0x4f00;
  regs.r_di=FP_OFF(p);
  regs.r_es=FP_SEG(p);
  intr(0x10,&regs); // obtener informacion sobre VESA SuperVGA BIOS
  }
else return -2;
return 0;
}

//---------------------------------------------------------

void obtenermodoinfo(struct vesa_mode_info *p) {
//p=puntero donde se copiará estructura de vesa_info
if (inicializadavesa==1) 
  memcpy(p,&vesamodeinfo,sizeof(struct vesa_mode_info));
}

//---------------------------------------------------------
void novideopun(unsigned int x,unsigned int y,
unsigned long color2) {
//rutina usada si se llama a punto sin inicializar modo gráfico
//no hace nada
return ;
}
//---------------------------------------------------------
void punto256c(unsigned int x,unsigned int y,
unsigned long color2) {
//las coordenadas van desde (1,1) esquina superior izquierda
//hasta (anchopant,altopant)
//punto para modos de 256 colores
ldiv_t division;
struct REGPACK regs;
unsigned char color=(unsigned char)color2;

if (inicializadavesa==0 || vesamodeinfo.bits_pixel!=8 ||
vesamodeinfo.tipo_modelo_memoria!=4 || x>anchopant || y>altopant) return;
division=ldiv(((unsigned long)(y-1)*anchopant)+(x-1),longitudbanco);
if (bancoact!=division.quot) {
  paginar(division.quot);
  bancoact=(unsigned int)division.quot;
  }
*(unsigned char far *)MK_FP(segmentopant,division.rem)=color;
}

//---------------------------------------------------------
void novideolin(unsigned int x1,unsigned int y1,unsigned int x2,
unsigned int y2,unsigned long color2) {

//rutina usada si se llama a linea sin inicializar modo video
//no hace nada
return ;
}

//---------------------------------------------------------
void linea256c(unsigned int x1,unsigned int y1,unsigned int x2,
unsigned int y2,unsigned long color2) {
//las coordenadas van desde (1,1) esquina superior izquierda
//hasta (anchopant,altopant)
//linea para modos de 256 colores
//dibuja una linea mediante el algoritmo de Bresenham
int x,y,delp,delr,delrx,delry,deldx,deldy,delre,
delde,si,di,dx,cx,bx,temp;
unsigned char color=(unsigned char)color2;
unsigned char far *punpant;
struct REGPACK regs;
ldiv_t division;
long desp; //desplazamiento con respecto a la página de video

if (inicializadavesa==0 || vesamodeinfo.bits_pixel!=8 ||
vesamodeinfo.tipo_modelo_memoria!=4) return;
division=ldiv(((unsigned long)(y1-1)*anchopant)+(x1-1),longitudbanco);
if (bancoact!=division.quot) {
  paginar((unsigned int)division.quot);
  bancoact=(unsigned int)division.quot;
  }
punpant=MK_FP(segmentopant,division.rem);
desp=division.rem;
si=1;
di=1;
dx=y2-y1;
if (dx<0) {
  di=-di;
  dx=-dx;
  }
deldy=di;
cx=x2-x1;
if (cx<0) {
  si=-si;
  cx=-cx;
  }
deldx=si;
if (cx>=dx)
  di=0;
else {
  si=0;
  temp=dx;
  dx=cx;
  cx=temp;
  }
delr=cx;
delp=dx;
delrx=si;
delry=di;
si=x1;
di=y1;
delre=delp << 1;
delde=delre - delr;
bx=delde;
delde-=delr;
x=si;
y=di;
do {
  *punpant=color;
  //punto(x,y,color);
  if (bx>=0) {
	 x+=deldx;
	 y+=deldy;
	 bx+=delde;
	 desp+=deldx;
	 if (deldy==1) desp+=anchopant;
	 if (deldy==-1) desp-=anchopant;
	 if (desp<0) {
		desp=longitudbanco+desp;
		bancoact--;
		if (cx>1) //si aun no ha acabado de hacer la linea
		  paginar(bancoact);
		}
	 if (desp>=(long)longitudbanco) {
		desp=desp-longitudbanco;
		bancoact++;
		if (cx>1) //si aun no ha acabado de hacer la linea
		  paginar(bancoact);
		}
	 }
  else {
	 x+=delrx;
	 y+=delry;
	 bx+=delre;
	 desp+=delrx;
	 if (delry==1) desp+=anchopant;
	 if (delry==-1) desp-=anchopant;
	 if (desp<0) {
		desp=longitudbanco+desp;
		bancoact--;
		if (cx>1) //si aun no ha acabado de hacer la linea
		  paginar(bancoact);
		}
	 if (desp>=(long)longitudbanco) {
		desp=desp-longitudbanco;
		bancoact++;
		if (cx>1) //si aun no ha acabado de hacer la linea
		  paginar(bancoact);
		}
	 }
  punpant=MK_FP(segmentopant,desp);
  }while (--cx>=0);
}

//---------------------------------------------------------

/* draws a circle at (xc, yc) with radius r in color c con el algoritmo
** Bresenham
** note: the scaling factor of (anchopant / altopant) is used when
** updating d.  This makes round circles.  If you want ellipses, you can
** modify that ratio.
*/

void circulo256c(unsigned int xc,unsigned int yc,unsigned int r,
unsigned long c)
{
		int x = 0, y = r, d = 2 * (1 - r);

		while(y > 0)
		{
				punto(xc + x, yc + y, c);
				punto(xc + x, yc - y, c);
				punto(xc - x, yc + y, c);
				punto(xc - x, yc - y, c);
				if(d + y > 0)
				{
						y -= 1;
						d -= (2 * y * (anchopant / altopant)) - 1;
				}
				if(x > d)
				{
						x += 1;
						d += (2 * x) + 1;
				}
		}
}

//---------------------------------------------------------

void vesa(void) {

struct vesa_info info;
struct vesa_info2 info2;
struct vesa_mode_info modoinfo;
char temp[256];
int r;
unsigned int x,y;
int modoprobar=2;


if (obtenervesainfo(&info)!=0) {
  printf("VESA SuperVGA BIOS no instalada");
  return;
  };
r=obtenervesainfo2(&info2);
puts("BIOS VESA");
if (r==0) {
  printf("Version %u.%u\nVersion del software:%u\n",(info2.vesa_version>> 8),
  info2.vesa_version & 255,info2.software_version);
  printf("Memoria de video:%u Kb\n",info2.bloques64k * 64U);
  _fstrcpy(temp,info2.fabricante);
  printf("Informacion del fabricante:%s\n",temp);
  _fstrcpy(temp,info2.vendedor);
  printf("Vendedor:%s\n",temp);
  _fstrcpy(temp,info2.producto);
  printf("Producto:%s\n",temp);
  if (info2.rev_producto[0]!=0) {
	 _fstrcpy(temp,info2.rev_producto);
	 printf("Revision del producto:%s\n",temp);
	 }
  else
	 puts("No hay informacion de revision del producto");
  puts("Posibilidades del Hardware de video:");
  if (info2.posibil_hard_video & 1)
	 puts ("     Ancho del DAC por color primario variable(6 u 8 bits)");
  else
	 puts ("     Ancho del DAC por color primario fijo(6 bits)");
  if (!(info2.posibil_hard_video & 2))
	 puts ("     Hardware compatible VGA");
  else
	 puts ("     Hardware no compatible VGA");
  if (info2.posibil_hard_video & 4)
	 puts ("     Debe ser testeado bit de blanqueo para evitar parpadeos o nieve");
  else
	 puts ("     No debe ser testeado bit de blanqueo para evitar parpadeos o nieve");
  }
else {
  _fstrcpy((char far *)temp,info.puntero_oem_name);
  printf("Software:%s\nversi¢n %u.%u\r\nMemoria de video:%u Kb",temp,
 (info.vesa_version & 0xff00) >> 8,info.vesa_version & 0x00ff,
 (unsigned int)(info.num_blockes_mem * 64U));
 };
printf("\r\nPulse tecla");
getch();
if (initvesa(modoprobar)!=0) 
  puts("Error al inicializar modo video");
else {
  obtenermodoinfo(&modoinfo);  
  closevesa();
  vesamodeinfo=(struct  vesa_mode_info *)&buffer2;
  printf("\r\nSalida por BIOS soportada:%s  Tipo modo:%s Color/Monocromo:%s\r\n",
  (vesamodeinfo->mode_attrib.salida_bios_soportado==1) ? "Si" : "No",
  (vesamodeinfo->mode_attrib.grafico_texto==1) ? "Gr fico" : "Texto",
  (vesamodeinfo->mode_attrib.color_monocromo==1) ? "Color" : "Monocromo");
  if (infovesa->vesa_version>=0x0200) 
	 printf("Modo compatible VGA:%s Cambio de bancos soportado:%s  ¿Hay buffer lineal?:%s\r\n",
	 (vesamodeinfo->mode_attrib.mode_not_vga_compat==1) ? "NO" : "SI",
	 (vesamodeinfo->mode_attrib.modo_cambio_bancos_soportado==1) ? "SI" : "NO",
	 (vesamodeinfo->mode_attrib.buffer_lineal==1) ? "SI" : "NO");
  printf("\r\nVentana A:\r\nExiste:%s  Se puede escribir:%s  "
			"Se puede leer:%s\r\n"
			"Ventana B:\r\nExiste:%s  Se puede escribir:%s  "
			"Se puede leer:%s\r\n"
			"Granularidad(incremento menor en Kb por el cual la ventana puede ser movida):%u\r\n"
			"Tama¤o de ventana(en Kb):%u  "
			"Segmento de Ventana A:0x%04X\r\nSegmento de Ventana B:0x%04X\r\n"
			"Bytes por linea de barrido:%u  Ancho en pixels:%u  "
			"Alto en pixels:%u\r\nAncho de caracter en pixels:%u  "
			"Alto de car cter en pixels:%u\r\n"
			"N£mero de planos de memoria:%u  N£mero de bits por pixel:%u\n"
			"N£mero de bancos:%u  Tipo de modelo de memoria:%s\n"
			"Longitud del banco en Kb:%u\n",
			(vesamodeinfo->windowa.existe==1) ? "Si" : "No",
			(vesamodeinfo->windowa.se_puede_leer==1) ? "Si" : "No",
			(vesamodeinfo->windowa.se_puede_escribir==1) ? "Si" : "No",
			(vesamodeinfo->windowb.existe==1) ? "Si" : "No",
			(vesamodeinfo->windowb.se_puede_leer==1) ? "Si" : "No",
			(vesamodeinfo->windowb.se_puede_escribir==1) ? "Si" : "No",
			vesamodeinfo->window_granularity,vesamodeinfo->kb_window,
			vesamodeinfo->segment_windowa,vesamodeinfo->segment_windowb,
			vesamodeinfo->bytes_scan_line,vesamodeinfo->ancho,
			vesamodeinfo->alto,vesamodeinfo->ancho_caracter,
			vesamodeinfo->alto_caracter,vesamodeinfo->numero_planos_memoria,
			vesamodeinfo->bits_pixel,vesamodeinfo->bancos,
			tiposmem[vesamodeinfo->tipo_modelo_memoria],vesamodeinfo->longitud_banco_kb);
  if (infovesa->vesa_version>=0x0102) 
	 printf("Tama¤o del rojo en bits:%u    Posici¢n del rojo en bits:%u\n"
			  "Tama¤o del verde en bits:%u    Posici¢n del verde en bits:%u\n"
			  "Tama¤o del azul en bits:%u    Posici¢n del azul en bits:%u\n"
			  "Tama¤o de valor reservado en bits:%u Posici¢n del reservado en bits:%u\n"
			  "%s %s\n",
			  vesamodeinfo->tam_rojo,vesamodeinfo->pos_rojo,
			  vesamodeinfo->tam_verde,vesamodeinfo->pos_verde,
			  vesamodeinfo->tam_azul,vesamodeinfo->pos_azul,
			  vesamodeinfo->tam_val_res,vesamodeinfo->pos_res,
			  (vesamodeinfo->info_col_directo & 1) ? "La rampa de color del DAC es programable" : "La rampa de color del DAC es fija",
			  (vesamodeinfo->info_col_directo & 2) ? "Los bits reservados pueden ser usados" : "Los bits reservados no pueden ser usados"
			  );
  if (infovesa->vesa_version>=0x0200)
	 printf("Direcci¢n f¡sica del buffer lineal:%lu\n"
			  "Offset comienzo fin pantalla video:%lu Memoria de video %u Kb",
			  vesamodeinfo->dir_fisica_buffer_lineal,
			  vesamodeinfo->offset_com_mem_video_fuera_pant,
			  vesamodeinfo->mem_video_kb);
	 getch();
	 initvesa(modoprobar);
	 /*for (y=1;y<=(modoinfo.alto / 2);++y) 
	 for (x=1;x<=(modoinfo.ancho / 2);++x)
		punto256c(x,y,2);*/
	 /*for (y=altopant;y>0;--y)
		linea256c(1,altopant,anchopant,y,7);*/
	 linea256c(1,altopant,anchopant / 2,1,7);
	 linea256c(100,100,200,200,7);
	 for (x=0;x<(anchopant >> 1);++x)
	   circulo256c(400,301,x,132);
	 getch();	
	 }
closevesa();
}
