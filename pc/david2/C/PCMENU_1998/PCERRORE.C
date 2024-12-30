#include <dos.h>
#include <stdio.h>
#include <ctype.h>
#include <pcventan.h>
#include <pcutil.h>

/*------------------------------------------------------------------------*/
#define IGNORAR  0
#define REPETIR   1
#define ABORTAR   2
/*------------------------------------------------------------------------*/
BOOLEAN error_critico;
int atrisave;
static char msg2[80];
static char bufferpantalla[512];
const static char *err_msg[] = {
	 "protecci¢n contra escritura",
	 "unidad desconocida",
	 "disco no preparado",
	 "comando desconocido",
	 "error datos(CRC)",
	 "longitud de escritura de llamada err¢nea",
	 "error de b£squeda",
	 "tipo desconocido de medio",
	 "sector no encontrado",
	 "falta papel en impresora",
	 "fallo en escritura",
	 "fallo en lectura",
	 "fallo general",
	 "reservado",
	 "reservado",
	 "cambio inv lido de disco"
};
static struct ventann _ventana={
10,8,55,10,siguarda,SOMBRA,"ERROR DOS",(void *)&bufferpantalla,/*PANTALLA DE PCERRORE (¡ndice 10) */
};
/*------------------------------------------------------------------------*/
static void near hacerven2(void);
static void near cerrarven2(void);
/*------------------------------------------------------------------------*/
int error_win(char *msg)
{
int retval;


writestr(2,2,0,msg);
if (!error_critico) writestr(2,3,1,"~A~bortar, ~R~epetir, ~I~gnorar: ");
//esperar tecla y mirar si se pulsa Abortar,Reintentar o Ignorar
while(1) {
  retval= toupper(tecla());
  if (retval == 'A') {
	 retval = ABORTAR;
	 break;
	 }
  if (retval =='R') {
	 retval = REPETIR;
	 break;
	 }
  if (retval == 'I') {
	 retval = IGNORAR;
	 break;
	 }
  }
cerrarven2();
pon_atributo(atrisave);
return(retval);
}
/*----------------------------------------------------------------------*/

/*
para que el compilador no ponga el mensaje de que los par metros no se usan
se pone pragma warn -par ,
*/
#pragma warn -par

int handler(int errval,int ax,int bp,int si)
{
   
unsigned di;


di= _DI;
atrisave=coge_atributo();
hacerven2();
error_critico=FALSE;
if (ax < 0)
  //si no es un error de un disco
  {
  error_win("Error en dispositivo");
  error_critico=TRUE;
  cerrarven2();
  pon_atributo(atrisave);
  //abortar el programa
  hardretn(ABORTAR);
	}
//si ha sido un error de disco
sprintf(msg2, "Error: %s en unidad %c",err_msg[di & 0x00FF], 'A' + 
(ax & 0x00FF));
//retornar al programa via interrupci¢n 0x23 con abortar,reintentar o
//ignorar seg£n lo que haya especificado el usuario
hardresume(error_win(msg2));
cerrarven2();
pon_atributo(atrisave);
return ABORTAR;
}
#pragma warn +par
/*----------------------------------------------------------------------*/
void errorespon(void)  {
  harderr(handler); 
  }
/*----------------------------------------------------------------------*/
static void near hacerven2(void) {

hacerven(&_ventana,colorf2(PCERRORE));
}
/*----------------------------------------------------------------------*/
static void near cerrarven2(void) {

cerrarven(&_ventana);
_ventana.memptr=&bufferpantalla;
}