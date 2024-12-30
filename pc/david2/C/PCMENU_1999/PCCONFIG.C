#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <mem.h>
#include <errno.h>
#include <pcventan.h>
#include <pcutil.h>


char *mensajesconf[]= {
"DIRECTORIO DEL PROGRAMA:",
"",
"NOMBRE DEL PROGRAMA A EJECUTAR :",
"NOMBRE DEL PROGRAMA:",
"DESCRIPCION DEL PROGRAMA:",
"",
"PARAMETROS DEL PROGRAMA:",
}
;
char *lineasinf[]={
"~ESC~ SALIR  ~ENTER~ EDITAR  ~F2~ INSERTAR  ~F3~ BORRAR ~TAB~ y ~S-TAB~ MOVER ENTRE CAMPOS",
"~ESC~ CANCELAR  ~ENTER~ CONFIRMAR ENTRADA",
};

//posiciones ,direcciones y longitudes para configuraci¢n
struct info {
char x,y;
unsigned int offset; //desplazamiento con respecto al primer elemento (directorio)
unsigned char longi;
} info[]={
{2,3,0,sizeof(datos[0].directorio)-1},
{34,4,LONGITUDDIRECTORIO,sizeof(datos[0].ejecutable)-1},
{22,5,LONGITUDDIRECTORIO+LONGITUDEJECUTABLE,sizeof(datos[0].nombre)-1},
{2,7,LONGITUDDIRECTORIO+LONGITUDEJECUTABLE+LONGITUDNOMBRE+LONGITUDPARAMETROS,sizeof(datos[0].aplicacion)-1},
{2,9,LONGITUDDIRECTORIO+LONGITUDEJECUTABLE+LONGITUDNOMBRE,sizeof(datos[0].parametros)-1},
};
unsigned char registros;


/*----------------------------------------------------------------------*/
static void near quitaespaciosfinal(char *s);
static void near datospant(int r);
static void near ilumina(char j);
static void near desilumina(char j);
static void near ponlineainf(unsigned char op);
static void near ponnumreg(unsigned char r);
/*----------------------------------------------------------------------*/
static void near quitaespaciosfinal(char *s) {
char *p;

p=strchr(s,0);
if (p!=s) {
  while (p!=s) {
	 --p;
	 if (*p==' ') *p='\x0'; else break;
	 };
  };
};
/*----------------------------------------------------------------------*/
static void near datospant(int r) {
//borra de pantalla los antiguos datos introducidos
//y pone los nuevos/anteriores
unsigned char i;

for (i=0;i<sizeof(info)/sizeof(info[0]);++i) {
		writechar(info[i].x,info[i].y,info[i].longi,' ');
		writestr(info[i].x,info[i].y,0,(char *)
		((char *)&datos[r]+info[i].offset));
		}
}
/*----------------------------------------------------------------------*/
static void near ilumina(char j) {
writeatrib(info[j].x,info[j].y,info[j].longi,colorf2(ILUMINA));
}
/*----------------------------------------------------------------------*/
static void near desilumina(char j) {
writeatrib(info[j].x,info[j].y,info[j].longi,colorf2(DESILUMINA));
}
/*----------------------------------------------------------------------*/
static void near ponlineainf(unsigned char op) {
unsigned char atrisal=coge_atributo();

cambiatitulo(ventanas[24].titulo,0);
pon_atributo(colorf2(MARCO));
writestr(2,ventana_alto()+1,1,lineasinf[op]);
pon_atributo(atrisal);
}
/*----------------------------------------------------------------------*/
static void near ponnumreg(unsigned char r) {
char buffer[30];

sprintf(buffer,"% 2u/% 2u",(registros!=0) ? r+1 : 0,registros);
writestr(ventana_ancho()-6,2,0,buffer);
}
/*----------------------------------------------------------------------*/
int comparadatos(char *elem1,char *elem2) {
  //compara los nombres (elem1 es un puntero a una estructura tipo FICHERO(DATOS)
  return strcmp(((DATOS *)elem1)->nombre,((DATOS *)elem2)->nombre);
  }
/*----------------------------------------------------------------------*/
void confproc(int c) {
//c:condici¢n:0=nueva configuraci¢n(opcion de la barra del menu)
//1=modificar configuraci¢n,2=nueva configuraci¢n(no hay pcmenu.dat o
//pcmenu.dat corrupto)
const int tope=sizeof(info)/sizeof(struct info)-1;
int i,fichero,tecs,bandera,tecs2,r;
char nombre[MAXFILE],ext[MAXEXT],j,buffer[82],sib;
unsigned int tecsalir[]={ESC,0};

if (c==0 || c==2) {
  if (c==0) sib=siconfirmacion("BORRAR","¨ESTA SEGURO DE BORRAR TODAS LAS "
				"APLICACIONES DEL MENU(S/N)?");
  if ((sib=='S' && c==0) || c==2)  {
	 setmem((char *)&datos,sizeof(datos),0);
	 registros=0;
	 }
  }
if (hacerven(&ventanas[24],colorf2(CAMBIARCONFIGURA))==-1)
  ponerror(1);
for (i=0;i<sizeof(mensajesconf)/sizeof(mensajesconf[0]);i++)
	 writestr(2,i+2,1,mensajesconf[i]);
ponlineainf(0);
i=0;
r=0;
j=0;
datospant(0);
if (registros!=0) ilumina(j);
ponnumreg(r);
do {
  tecs=tecla();
  if (tecs==TAB && registros!=0) {
	 desilumina(j);
	 if (j==tope)
		j=0;
		else
		++j;
	 ilumina(j);
	 }
  if (tecs==S_TAB && registros!=0) {
	 desilumina(j);
	 if (!j)
		j=tope;
		else
		--j;
	 ilumina(j);
	 }
  if (tecs==DOWN && r<registros-1 && registros!=0) {
	 ++r;
	 datospant(r);
	 ponnumreg(r);
	 ilumina(j);
	 }
  if (tecs==UP && registros!=0 && r>0) {
	 --r;
	 datospant(r);
	 ponnumreg(r);
	 ilumina(j);
	 }
	 if (tecs==F2 && registros<MAXSOFT) {
	 //insertar
	 desilumina(j);
	 ++registros;
	 r=registros-1;
	 j=0;
	 setmem(&datos[r],sizeof(datos[0]),0);
	 datospant(r);
	 ponnumreg(r);
	 }
  if (tecs==ENTER && registros!=0 || (tecs==F2 && registros<MAXSOFT)) {
  while (1==1) { //bucle while para cuando se lee el nombre
	 strcpy(buffer,(char *)((char *)&datos[r]+info[j].offset));
	 ponlineainf(1);
	 desilumina(j);
	 tecs2=dial(info[j].x,info[j].y,32,254,info[j].longi,
	 buffer,colorf2(CAMBIARCONFIGURA),FALSE,TRUE,tecsalir);
	 ponlineainf(0);
	 ilumina(j);
	 quitaespaciosfinal(buffer);
	 if (j==0) escanea(buffer);
	 if (j==0 || j==1) strupr(buffer);
	 if (tecs2!=ESC)  {
		strcpy((char *)((char *)&datos[r]+info[j].offset),buffer);
		if (j==1 || j==0) {
		  bandera=fnsplit(buffer,NULL,NULL,nombre,ext);
		  if (bandera & WILDCARDS) {
			 pon_mensaje("ERROR",(j==1) ?
			 "EL NOMBRE DEL EJECUTABLE NO ADMITE COMODINES" :
			 "EL DIRECTORIO NO ADMITE COMIDINES",1);
			 goto sierrorlee;
			 }
		  if ((bandera & DRIVE) && j==1) {
			 pon_mensaje("ERROR","EL NOMBRE DEL EJECUTABLE NO ADMITE UNIDAD",1);
			 goto sierrorlee;
			 }
		  if ((bandera & DIRECTORY) && j==1) {
			 pon_mensaje("ERROR","EL NOMBRE DEL EJECUTABLE NO ADMITE DIRECTORIO",1);
			 goto sierrorlee;
			 }
		  if (!(bandera & EXTENSION) && j==1) {
			 pon_mensaje("ERROR","EL NOMBRE DEL EJECUTABLE DEBE INCLUIR EXTENSION",1);
			 goto sierrorlee;
			 }
		  if (!(bandera & FILENAME) && j==1) {
			 pon_mensaje("ERROR","EL NOMBRE DEL EJECUTABLE DEBE INCLUIR NOMBRE",1);
			 goto sierrorlee;
			 }
		  if ((bandera & EXTENSION) && (strcmp(ext,".COM") &&
		  strcmp(ext,".EXE") && strcmp(ext,".BAT")) && j==1) {
			 pon_mensaje("ERROR","LA EXTENSION HA DE SER ~COM~,~EXE~ O ~BAT~",1);
			 goto sierrorlee;
			 }
		  if ((bandera & FILENAME) && strchr(nombre,'.')) {
			 pon_mensaje("ERROR",(j==1) ? "NOMBRE INCORRECTO" :
			 "DIRECTORIO INCORRECTO",1);
			 goto sierrorlee;
			 }
		  writestr(info[j].x,info[j].y,0,(char *)((char *)&datos[r]+info[j].offset));
		  ilumina(j);
		  break;
sierrorlee:*(char *)((char *)&datos[r]+info[j].offset)=0;
		  continue;
		  }
		else break;
		}
		else {
		datospant(r);
		ilumina(j);
		break;
		}
	 }
  }
  if (tecs==F3 && registros!=0) {
	 //borrar
	 if (siconfirmacion("BORRAR","¨QUIERE BORRAR ESTA APLICACION DEL MENU(S/N)?")=='S') {
		memcpy(&datos[r],&datos[r+1],sizeof(datos[0])*(registros-r));
		--registros;
		if (r==registros) --r;
		datospant(r);
		ponnumreg(r);
		ilumina(j);
		}
	 }
  }while (tecs!=ESC);
borrafich(nombrefichero);
//ordena los datos por nombre
qsort(&datos,registros,sizeof(DATOS),(int (*)(const void *,const void *))
comparadatos);
#if __BORLANDC__ == 0x452
if ((fichero=_rtl_creat(nombrefichero,FA_ARCH))==-1)  ficheroerror(errno);
#else
if ((fichero=_creat(nombrefichero,FA_ARCH))==-1)  ficheroerror(errno);
#endif
 else {
#if __BORLANDC__ == 0x452
	if (_rtl_write(fichero,clave,longitudclave)==-1) ficheroerror(errno);
	if (_rtl_write(fichero,&registros,sizeof(registros))==-1) ficheroerror(errno);
#else
	if (_write(fichero,clave,longitudclave)==-1) ficheroerror(errno);
	if (_write(fichero,&registros,sizeof(registros))==-1) ficheroerror(errno);
#endif
	ventanas[4].yy=ventanas[4].y+registros;
#if __BORLANDC__ == 0x452
	if (_rtl_write(fichero,&datos,sizeof(datos))==-1) ficheroerror(errno);
	_rtl_close(fichero);
#else
	if (_write(fichero,&datos,sizeof(datos))==-1) ficheroerror(errno);
	_close(fichero);
#endif
	};
cerrarven(&ventanas[24]);
__setcursortype(_NOCURSOR);
}
/*-------------------------------------------------------------------------*/