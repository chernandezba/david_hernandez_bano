#ifndef __PCUTIL.H__
#define __PCUTIL.H__
#include <pcventan.h>

/*------------------------------------------------------------------*/
typedef struct estructarb ESTRUCTARB;
typedef struct FICHERO DATOS;
/*------------------------------------------------------------------*/
#define RIGHT2  0x01
#define LEFT2   0x02
#define colorf2(num) (color2[(num)][adaptador])
#define colorf(num) (color[(num)][adaptador])
#define MAXSOFT 19
#define VENTANAPROGRAMAS ventanas[ULTIMOMENU]
#define NUMVENT 7
/*------------------------------------------------------------------*/
#ifndef MAXPATH
#define MAXPATH 80
#endif
/*------------------------------------------------------------------*/
enum NUMCOLOR {
COLORFONDO2=0,LOGOTIPO,SALIDA,MENUOPCIO,MENUOPCIO2};
/*------------------------------------------------------------------*/
enum NUMCOLOR2 {
COLORFONDO=0,MARCO,MENUPRI,LOGOTI,VENTAN,MENUINV,CAMBIARCONFIGURA,SALIDADOS,
TITULO,ARBOLV,SELECCIONADO,VENTSEL,FICHSEL,PCERRORE,UNIDSELECCIO,CAMBUNIDA,
MENUTECLAS,INFODISCO,SELUNI,FICHEROSV,DIRSEL,ERRORN,ERRORC,OPSEL,P_P,
ILUMINA,DESILUMINA,MENUDESP,ILUOPCMENU};
/*------------------------------------------------------------------*/
//estructura para almacenar el nombre del fichero y su informaci¢n
struct filee {
char nombre[13];
unsigned long longitud;
unsigned char atributo;
struct hora {
  unsigned int segundos:5; //en incrementos de 2 segundos (va de 0 a 29) , por ejemplo si pone 2 es que son 4 segundos
  unsigned int minutos:6;
  unsigned int hora:5;
  } hora;
struct fecha {
  unsigned int dia:5;
  unsigned int mes:4;
  unsigned int anyo:7; //el a¤o que maneja el MSDOS va de 1980 a 2107;si pone 17 el a¤o es 1980+17=1997
  }fecha;
unsigned int num_fich_sel;};
/*------------------------------------------------------------------*/
struct Menuin {int  inicio,fin;};
/*------------------------------------------------------------------*/
struct meninn {int empieza;char *menu;};
/*------------------------------------------------------------------*/
//estructura relacionada con los menus,contiene el mensaje del desplegable
//su aclaraci¢n abajo y la rutina a ejecutar
struct opciontipo  {
  char *utilidad;
  void (*salida)(void);
  char *instruccion;
  };
/*------------------------------------------------------------------*/
//estructura para mostrar errores por pantalla
//condici¢n puede ser SISALIR o NOSALIR
//menerr es el mensaje de error
struct errore {
unsigned char condicion;
char *menerr;};
/*------------------------------------------------------------------*/
//esta estructura sirve para almacenar los directorios/subdirectorios
//del disco seleccionado,nombredir es el nombre del directorio
//subdir es el puntero al subdirectorio de nombre nombredir
//proxdir es el proximo directorio
//dirsup es el directorio padre,directorio ..
//antdir es el anterior directorio
//si subdir,proxdir,dirsup o antdir es NULL significa que no hay m s
//x,y es la posici¢n del directorio que empieza en 0,0 y que tiene
//como l¡mite de y 32767 y como x el ancho de la ventana de directorios
//atributo es el atributo del directorio(oculto,sistema,...
//clusterinicial es el cluster donde empieza dicho directorio
struct estructarb {
char nombredir[13];
struct estructarb *subdir,*proxdir,*dirsup,*antdir;
int x,y;
char atributo;
unsigned int clusterinicial;
};
/*------------------------------------------------------------------*/
//esta estructura es para el menu aplicaciones y almacena
//nombre de aplicaciones,directorio,nombre,...
//aplicacion es la explicaci¢n(max 76) de lo que trata dicha aplicacion
#define LONGITUDDIRECTORIO 76+1
#define LONGITUDEJECUTABLE 12+1
#define LONGITUDNOMBRE 25+1
#define LONGITUDPARAMETROS 76+1
#define LONGITUDAPLICACION 76+1
struct FICHERO {
  char directorio[LONGITUDDIRECTORIO];
  char ejecutable[LONGITUDEJECUTABLE];
  char nombre[LONGITUDNOMBRE];
  char parametros[LONGITUDPARAMETROS];
  char aplicacion[LONGITUDAPLICACION];
};
/*------------------------------------------------------------------*/
void ponerror(int);
void compruebaconfiguracion(void);
void seleccionaopcion(void);
void escanea(char *);
void buscararbol(int);
int ponesubdirs(struct estructarb *);
void liberamem(void);
void liberamemfich(void);
void liberamemarb(void);
void ponerunidlogicas(void);
void ponerarbol(unsigned int,ESTRUCTARB *);
int meteficheros (struct filee huge *);
void escribefichero(int,struct filee huge *);
void restaura(void);
void mostrardirectorio(int);
void scrollficherosarriba(void);
void scrollficherosabajo(void);
void cambunidproc(void);
void nuevconfproc(void);
void dessel_ficheros(void);
void poninfodisco(int );
void borra_tec_buffer(void);
void mostrar_ficheros(unsigned int);
void escribedirectorio(char *);
void poner_etiqueta_disco(void);
void ficheroerror(int);
void borrafich(char *);
void pcopcion_init(void);
void pfun_arbol(void);
int ejecutacomando (char *,int);
void far *calculadireccion (void far *oldir,unsigned long asumar);
void errorespon(void);
void borratemp(void);
void sel_dir(int _atributo);
void confproc(int c);
/*------------------------------------------------------------------*/
extern unsigned long sumatotalfich;
/*------------------------------------------------------------------*/
extern int unidexe,color[][2],color2[][2],longitudclave;
/*------------------------------------------------------------------*/
extern struct FICHERO datos[MAXSOFT];
/*------------------------------------------------------------------*/
extern unsigned int numfich,totalfich,ULTIMOMENU;
/*------------------------------------------------------------------*/
extern unsigned char unidadesdisco,registros;
/*------------------------------------------------------------------*/
extern struct Menuin *menuin;
/*------------------------------------------------------------------*/
extern struct estructarb *arbol;
/*------------------------------------------------------------------*/
extern char diractual[MAXPATH+1],pathori[MAXPATH+1],nombretemp[MAXPATH+1],
nombrefichero[MAXPATH+1],*clave,anchoarbol,altoarbol;
/*------------------------------------------------------------------*/
extern unsigned int arboly;
extern int yrel;
/*------------------------------------------------------------------*/
extern ESTRUCTARB *arbolptr;
/*------------------------------------------------------------------*/
extern struct filee huge *ficherosdir;
/*------------------------------------------------------------------*/
extern struct ventann ventanas[];
/*------------------------------------------------------------------*/
#endif