#ifndef __PCESCRIB.H__
#define __PCESCRIB.H__
//variables
extern int cscreen,cscreenA,adaptador,VENTANASEL,ULTIMAVENTANASEL;
extern unsigned char atributo,unidadesdisco,wx,wy,wx1,wy1,ancho,alto,
ultimoatributo;
extern int negritacolor,negritamono;

//funciones
void writeinit(void);
void writefin(void);
void writechar(int,int,int,char);
void writestr(int,int,int,char *); /* el tercer par metro es si quieres poner negrita si hay un '~'(0 no 1 si) */
int writemargen(int,int,int,char *);
void writeatrib(int,int,int,char);
void cambiaratrmarco(int);
void campana(void);
void guardaven(void);
void restauraven(void);
void centrar(int ,int , char *);
void right(int , int ,char *);
void _gotoxy(unsigned char x,unsigned char y);
void __setcursortype (int tipo);
void borratemp();
int _wherex (void);
int _wherey (void);
#endif