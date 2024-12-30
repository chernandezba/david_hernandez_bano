#ifndef __PCVENTAN.H__
#define __PCVENTAN.H__

//definiciones
#define siguarda 1
#define noguarda 0
#define CTRL   0x04
#define ALT    0x08
#define CAPS   0x64
#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2


//typedefs
enum boolean {FALSE=0,TRUE=1};
typedef enum boolean BOOLEAN;

//enums
enum _tipo_borders {NOBORDER=0,BORDER,SOMBRA,};
enum _tecladoex {ESC=0x001B,BS=0x0008,TAB=0x0009,C_ENTER=0X000A,ENTER=0X000D,
S_TAB=0X0f00,A_D=0X2000, A_F=0X2100, A_G=0X2200, A_H=0X2300, A_J=0X2400,
A_K=0X2500,A_L=0X2600,A_Z=0x2c00,A_X=0X2D00,A_C=0X2E00,A_V=0X2F00,
A_B=0X3000,A_N=0X3100,A_M=0X3200,A_Q=0X1000,A_W=0X1100,A_E=0X1200,
A_R=0X1300,A_T=0X1400,A_Y=0X1500,A_U=0X1600,A_I=0X1700,A_O=0X1800,
A_P=0X1900,A_A =0X1E00,A_S=0X1F00,F1=0x3b00,F2=0x3c00,F3=0x3d00,
F4=0x3e00,F5=0x3f00,F6=0X4000,F7=0X4100,F8=0X4200,F9=0X4300,F10=0X4400,
HOME=0X4700,UP=0X4800,PGUP=0X4900,LEFT=0X4B00,RIGHT=0X4D00,END=0X4F00,
DOWN=0X5000,PGDN=0X5100,PLUS=0X4E00,MINUS=0X4A00,DEL=0X5300,INS=0X5200,
NUMLOCK=0X4500,SCROLLOCK=0X4600,S_F1=0X5400,S_F2=0X5500,S_F3=0X5600,
S_F4=0X5700,S_F5=0X5800,S_F6=0X5900,S_F7=0X5A00,S_F8=0X5B00,S_F9=0X5C00,
S_F10=0X5D00,C_F1=0X5E00,C_F2=0X5F00,C_F3=0X6000,C_F4=0X6100,C_F5=0X6200,
C_F6=0X6300,C_F7=0X6400,C_F8=0X6500,C_F9=0X6600,C_F10=0X6700,A_F1=0X6800,
A_F2=0X6900,A_F3=0X6A00,A_F4=0X6B00,A_F5=0X6C00,A_F6=0X6D00,A_F7=0X6E00,
A_F8=0X6F00,A_F9=0X7000,A_F10=0X7100,ECHO=0X7200,C_LEFT=0X7300,
C_RIGHT=0X7400,C_END=0X7500,C_PG_DOWN=0X7600,C_HOME=0X7700,A_1=0X7800,
A_2=0X7900,A_3=0X7A00,A_4=0X7B00,A_5=0X7C00,A_6=0X7D00,A_7=0X7E00,
A_8=0X7F00,A_9=0X8000,A_0=0X8100,A_HYPHEN=0X8200,A_EQUAL=0X8300,
C_PG_UP=0X8400};



//estructuras
//estructura para crear ventanas
struct ventann {unsigned char x,y,xx,yy,guarda,
tipo_border;char *titulo;void  *memptr;};
//si memptr==NULL asigna memoria,si no no asigna al hacerven
//rutina relacionada con tecla,cuando se llama a tecla(),esta guarda
//en la variable estadotecla(tipo struct estados)el estado del teclado
struct estados {
unsigned int sicapsderecho:1;
unsigned int sicapsizquierdo:1;
unsigned int sicontrol:1;
unsigned int sialt:1;
unsigned int scrolllock:1; //si est  activado SCROLL-LOCK
unsigned int numlock:1; //si est  activado NUM-LOCK
unsigned int mayusculas:1; //si est  activado CAPS-LOCK
unsigned int insert:1; //si est  activado INSERT
};


//variables
extern int adaptador;
extern struct estados estadotecla;
extern int bufteclado;

//funciones
void writeinit(void);
void writefin(void);
unsigned char ventana_ancho(void);
unsigned char ventana_alto(void);
void pon_atributo(unsigned char);
unsigned char coge_atributo(void);
void writechar(int,int,int,char);
void writestr(int,int,int,char *); /* el tercer par metro es si quieres poner negrita si hay un '~'(0 no 1 si) */
void writeatrib(int,int,int,char);
void centrar(int ,int , char *);
void right(int , int ,char *);
void _gotoxy(unsigned char x,unsigned char y);
void __setcursortype (int tipo);
void borratemp();
int _wherex (void);
int _wherey (void);
void hacermarco(int);
void hacerfondo(unsigned char);
void getventan(int x,int y,int xx,int yy,char *buffer);
void putventan(int x,int y,char *buffer);
void ventana(int ,int ,int ,int);
void fillwin(unsigned char);
int hacerven(struct ventann *,int); //si hay error al asignar memoria devuelve -1,si no hay error devuelve 0
void cerrarven(struct ventann *);
void seleccionaventana(struct ventann *,int);
struct ventann *ventana_actual(void);
void ajventana(struct ventann *,int );
void cambiatitulo(char *titulo,char iluminacion);
void cambiarven(struct ventann *);
void hacersombra(int);
void pon_mensaje(char *cabecera,char *mensaje,int cond);
char siconfirmacion(char *cabecera,char *mensaje);
void cambiaratrmarco(int);
void guardaven(void);
void restauraven(void);
int tecla(void);
int dial(unsigned char x,unsigned char y,unsigned char minimo,
unsigned char maximo,unsigned int longitud,char *salida,
unsigned char atrileer,BOOLEAN simayu,BOOLEAN sicursor,
unsigned int *teclasalir);
void campana(void);


#endif