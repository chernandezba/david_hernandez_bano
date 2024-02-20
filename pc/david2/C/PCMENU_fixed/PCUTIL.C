#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <bios.h>
#include <alloc.h>
#include <dir.h>
#include <string.h>
#include <mem.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pcventan.h>
#include <pcutil.h>
#include <pcdisco.h>



/*-----------------------------------------------------------------------*/
ESTRUCTARB *temp,*temp2,*temp3;
ESTRUCTARB *arbol=NULL;
struct filee huge *ficherosdir=NULL;
enum errorrr {SISALIR=0,NOSALIR=1};
struct errore errores[] = {
{SISALIR,"ERROR INTERNO"},
{SISALIR,"MEMORIA INSUFICIENTE"},
{NOSALIR,"NO PUEDO HACER SHELL AL DOS"},
{SISALIR,"NO HAY SUFICIENTE MEMORIA"},
{SISALIR,"ERROR EN EL FORMATO DE FICHERO DE CONFIGURACION"},
{SISALIR,"NO HAY MEMORIA O DEMASIADOS REGISTROS"}, //error 5
{NOSALIR,"NO SE PUEDE GRABAR EL FICHERO"},
{NOSALIR,"NO PUEDO EJECUTAR EL PROGRAMA O INTENTO FALLIDO"},
{NOSALIR,"INTENTO FALLIDO DE HACER FICHERO TEMPORAL"},
{NOSALIR,"FICHERO PCMENU.DAT ERRONEO"},
{NOSALIR,"INSUFICIENTE MEMORIA PARA GESTIONAR EL FICHERO"}, //error 10
{NOSALIR,"NO PUEDO EJECUTAR PCEJEC.EXE"},
{NOSALIR,"ERROR AL ACCEDER AL DISCO"},
{NOSALIR,"INSUFICIENTE MEMORIA PARA LEER FICHEROS O DEMASIADOS FICHEROS"},
{SISALIR,"ERROR AL REALOCALIZAR MEMORIA"},
{NOSALIR,"FICHERO PCMENU.DAT NO ENCONTRADO"}, //error 15
{SISALIR,"ERROR AL BUSCAR DIRECTORIO"},
{SISALIR,"HEAP CORROMPIDO"},
{NOSALIR,"TIENE QUE TENER NOMBRE"},
{NOSALIR,"EXTENSION HA DE SER ~COM~ ~EXE~ O ~BAT~"},
{NOSALIR,"PROGRAMA NO ENCONTRADO"}, //error 20
{NOSALIR,"NO PUEDO EJECUTAR PROGRAMA"},
{SISALIR,"STACK AGOTADO"},
{SISALIR,"ERROR AL LIBERAR MEMORIA"}};
struct ventann ventanas[]={
{1,2,22,3,siguarda,SOMBRA,"",NULL},  /* FICHEROS  (¡ndice 0) */
{11,2,29,3,siguarda,SOMBRA,"",NULL},  /* DISCO */
{18,2,38,3,siguarda,SOMBRA,"",NULL}, /* CONFIGURACION */
{33,2,53,3,siguarda,SOMBRA,"",NULL}, /* ESPECIAL */
{43,2,75,3,siguarda,SOMBRA,"",NULL}, /* APLICACIONES */
{6,8,68,13,siguarda,SOMBRA,"ERROR",NULL},/* PANTALLA DE ERRORES */
{1,3,20,19,noguarda,BORDER,"",NULL},/*PANTALLA DE MOSTRAR ARBOL DIRECTORIOS*/
{30,10,50,12,siguarda,SOMBRA,"",NULL},/*PANTALLA DE MENSAJE*/
{22,3,78,19,noguarda,BORDER,"",NULL},/*PANTALLA DE MOSTRAR FICHEROS*/
{35,8,70,12,siguarda,SOMBRA,"ERROR DISCO",NULL},/*PANTALLA SI ERROR DEL DOS(0X25)*/
{10,8,55,10,siguarda,SOMBRA,"ERROR DOS",NULL},/*PANTALLA DE PCERRORE (¡ndice 10) */
{6,8,68,13,noguarda,SOMBRA,"ERROR CRITICO",NULL},/* PANTALLA DE ERROR CRITICO*/
{4,8,8,10,siguarda,SOMBRA,"CAMBIAR UNIDAD",NULL},/*PANTALLA DE CAMBIAR UNIDAD*/
{1,3,24,21,siguarda,BORDER,"INFORMACION",NULL},/*PANTALLA DE INFORMACION DEL DISCO*/
{1,24,80,25,noguarda,NOBORDER,"",NULL},/*PANTALLA DE MENSAJES INFERIORES(teclas)*/
{1,23,80,23,noguarda,NOBORDER,"",NULL},/*PANTALLA DE mostrarayuda*/
{30,10,53,12,siguarda,SOMBRA,"",NULL},/*PANTALLA DE "LEYENDO INFORMACION"*/
{1,3,78,21,siguarda,BORDER,"DIRECTORIO",NULL},/*FUNCION FUN_ARBOL*/
{1,1,80,25,siguarda,NOBORDER,"",NULL},/*SALIDADOS*/
{1,21,80,22,noguarda,NOBORDER,"",NULL},/*VENTANA DE FICHEROSSELECCIONADOS Y LO QUE REPRESENTAN*/
{1,2,80,2,noguarda,NOBORDER,"",NULL},/*ESCRIBEDIRECTORIO  (¡ndice 20) */
{1,1,80,2,noguarda,NOBORDER,"",NULL},/*ABRIROPCION*/
{6,9,68,12,siguarda,SOMBRA,"ERROR DE FICHERO",NULL},/* PANTALLA DE ERRORES (FICHEROERROR)*/
{20,8,48,15,siguarda,SOMBRA,"LOGOTIPO",NULL}, /* LOGOTIPO */
{1,5,79,13,siguarda,BORDER,"CONFIGURACION",NULL},  /* CAMBIARCONFIGURACION */
{1,10,77,12,siguarda,SOMBRA,"PARAMETROS",NULL},    /*PREGUNTA PARAMETROS (CTRL-ENTER*/
{1,1,80,25,noguarda,NOBORDER,"",NULL},/*SALIR*/
{6,9,68,12,siguarda,SOMBRA,"ERROR MSDOS(INT 25H)",NULL},/*ERRORES DE _absread*/
{1,1,80,25,noguarda,NOBORDER,"",NULL},/*PANTALLA INICIAL*/
};
/*-----------------------------------------------------------------------*/
extern int leidosub,leido;
extern unsigned char actmodo;
extern BOOLEAN leeboot;
/*-----------------------------------------------------------------------*/
unsigned long punteros,npuntarb,gastado,sumatotalfich;
static unsigned long libre2,libre;
unsigned int totalfich,numestru;
int co2,y0,x0,long_nom,__er;
char etiqueta_disco[15];
char anchoarbol,altoarbol;
ESTRUCTARB *ultdireccion;
char far *cadenaptr=" Ä";
static BOOLEAN utilizachdir;
/*-------------------------------------------------------------------------*/
static void near borraarbol(void);
static void near pascal ponesubdirsp (ESTRUCTARB *);
static char * near pascal ponecadenanumsubdir (ESTRUCTARB *);
static void near pascal juntadossubdirs_up (ESTRUCTARB *,unsigned int );
static void near pascal juntadossubdirs_down (ESTRUCTARB *,unsigned int );
static int unidac; //para ponesubdirsp
/*-------------------------------------------------------------------------*/
void borra_tec_buffer(void) {
bioskey(1);
}
/*------------------------------------------------------------------*/
void escanea (char *escan)  {
char *tempp;

tempp=strrchr(escan,0);
*tempp=0;
}
/*------------------------------------------------------------------------*/
void liberamemfich(void) {
//libera la memoria asignada para los ficheros
//si antes se ha desasignado no la vuelve a desasignar

if (ficherosdir!=NULL) {
  farfree((void far *)ficherosdir);
  ficherosdir=NULL;
  }
}
/*------------------------------------------------------------------------*/
void liberamemarb(void) {
//libera la memoria asignada al arbol de subdirectorios
//si antes se ha desasignado no la vuelve a desasignar

if (arbol!=NULL) {
  farfree((void far *)arbol);
  arbol=NULL;
  }
npuntarb=0;
gastado=0;
}
/*------------------------------------------------------------------------*/
void liberamem(void) { 
//libera memoria asignada para los ficheros y el
//arbol de directorios

liberamemfich();
liberamemarb();
}
/*--------------------------------------------------------------------*/
static void near borraarbol(void) {
int r;
unsigned char valto=ventana_alto(),vancho=ventana_ancho();
//borra el arbol de subdirectorios,tiene que estar la ventana del arbol
//de subdirectorios activada

for (r=0;r<valto-1;r++) writechar (2,r+2,vancho-2,' ');
}
/*------------------------------------------------------------------*/
static void near pascal juntadossubdirs_down (ESTRUCTARB *ptr,
unsigned int yrela) {
unsigned int contad=ptr->y+1,yprox=ptr->proxdir->y,cont2;
unsigned char valto=ventana_alto();
//junta dos subdirectorios con una linea , uno se visualiza y el otro no
// hacia abajo
for (cont2=0;yrela+cont2<=valto;++cont2) {
  if (contad+cont2==yprox) break;
  writestr (ptr->x,yrela+cont2,0,"³");
  };
}
/*------------------------------------------------------------------*/
static void near pascal juntadossubdirs_up (ESTRUCTARB *ptr,
unsigned int yrela) {
unsigned int yant=ptr->antdir->y,contad=ptr->y-1,cont2;
//junta dos subdirectorios con una linea , uno se visualiza y el otro no
// hacia arriba
for (cont2=0;yrela-cont2>1;++cont2) {
  if (contad-cont2==yant) break;
  writestr (ptr->x,yrela-cont2,0,"³");
  };
}

/*------------------------------------------------------------------*/
static char * near pascal ponecadenanumsubdir (ESTRUCTARB *ptr) {
unsigned char subnivel;

subnivel=0;
while (ptr->dirsup!=NULL) {
  ++subnivel;
  ptr=ptr->dirsup;
  };
*cadenaptr=subnivel+'0';
return(cadenaptr);
};
/*------------------------------------------------------------------*/
void ponerarbol(unsigned int _yrel,ESTRUCTARB *a) {
//_yrel y absoluta que corresponde al directorio de arriba de todo de la pantalla
//a puntero al arbol de directorios
register at=coge_atributo();
unsigned int n,yr;
unsigned char valto;
struct ventann *aven=ventana_actual();

cambiarven(&ventanas[6]);
pon_atributo(colorf2(ARBOLV));
borraarbol();
valto=ventana_alto();
n=0;
while (a->y<_yrel) {
  a=calculadireccion(a,(unsigned long)sizeof(struct estructarb));
  ++n;
  };
for (yr=a->y-2;n<npuntarb;++n,a=calculadireccion(a,(unsigned long)
  sizeof(struct estructarb))){
  if (a->y-yr>valto) continue;
  if (a->dirsup!=NULL)  {
	 if (a->x==a->dirsup->x)
		writestr(a->x,a->y-yr,0,ponecadenanumsubdir(a));
	 else
		writestr(a->x,a->y-yr,0,(a->proxdir==NULL && a->antdir==NULL) ? "À" :
		(a->proxdir!=NULL) ? "Ã" : "À");
  }
  writestr(a->x+1,a->y-yr,0,a->nombredir);
  if (a->proxdir!=NULL) juntadossubdirs_down(a,(a->y-yr)+1);
  if (a->antdir!=NULL)  juntadossubdirs_up(a,(a->y-yr)-1);
	};
pon_atributo(at);
cambiarven(aven);
}
/*-------------------------------------------------------------------------*/
void ponerror(int errorno) {
  char buffererror[80];
  register atr=coge_atributo();


if (errores[errorno].condicion==SISALIR)  {
  sprintf(buffererror,"ERROR %d : %s",errorno,errores[errorno].menerr);
  pon_mensaje("ERROR CRITICO",buffererror,1);
  ventana(1,1,80,25);
  pon_atributo(colorf2(SALIDADOS));
  fillwin(' ');
  _gotoxy(1,1);
  __setcursortype(_NORMALCURSOR);
  //exit(EXIT_FAILURE);
  }
if (errores[errorno].condicion==NOSALIR) {
  sprintf(buffererror,"ERROR %d : %s ",errorno,errores[errorno].menerr);
  pon_mensaje("ERROR",buffererror,1);
  pon_atributo(atr);
  }
}
/*-----------------------------------------------------------------------*/
int ponesubdirs (ESTRUCTARB *pones)
{
//devuelve 0 si no ha habido error al leer los directorios
//si hay error devuelve el c¢digo de error

unidac=cogedisco();
utilizachdir=FALSE;
if (unidad[unidac].atribs.unidad_red==1 || unidad[unidac].atribs.unidad_comprimida==1
  || unidad[unidac].atribs.cd_rom==1) 
	 //si es una unidad de red,esta comprimida o es un cd-rom
	 //utilizar chdir en vez de leer donde esta el directorio y leer
	 //las entradas del directorio directamente
	 utilizachdir=TRUE;
ponesubdirsp(pones);
utilizachdir=FALSE;
if (__er!=0 && __er!=-1) return __er; //va saliendo del arbol si se ha producido un error antes
return 0;
};
/*-----------------------------------------------------------------------*/
static void near pascal ponesubdirsp (ESTRUCTARB *pones) {
struct ffblk_ex arbsub;
unsigned int oldclusteract;


poneotro:
if (_SP < sizeof(struct ffblk_ex)+sizeof(unsigned int)+20) ponerror(22);
if ((__er=buscaprimero("*.*",&arbsub,FA_DIREC,TRUE))==-1) return;
if (__er!=0) return ;
if ((__er=buscadir(&arbsub))==-1) return;
if (__er!=0) return; //si hay error al buscar directorio va saliendo del arbol
temp=pones;
pones->subdir=ultdireccion=calculadireccion(ultdireccion,(unsigned long)
sizeof(ESTRUCTARB));
npuntarb++;
gastado+=(unsigned long)sizeof(ESTRUCTARB);
if (gastado>libre) ponerror(1);
y0++;
if (x0+13<anchoarbol) ++x0;
pones=pones->subdir;
setmem(pones,sizeof(ESTRUCTARB),0);
pones->dirsup=temp;
pones->y=y0;
pones->x=x0;
strcpy(pones->nombredir,arbsub.ff_name);
pones->atributo=arbsub.ff_attrib;
pones->clusterinicial=arbsub.ff_clusini;
oldclusteract=unidad[unidac].cluster_in_dir_actual;
if (utilizachdir==TRUE)
  cambiadir(arbsub.ff_name); //cambiadir en estos casos llama a chdir
else unidad[unidac].cluster_in_dir_actual=arbsub.ff_clusini;
ponesubdirsp(pones);
buscaotro:
if (__er!=0 && __er!=-1) return ; //va saliendo del arbol si se ha producido un error antes
x0=pones->x;
if (utilizachdir==TRUE)
  cambiadir(".."); //cambiadir en estos casos llama a chdir
else unidad[unidac].cluster_in_dir_actual=oldclusteract;
leidosub=0;
if ((__er=buscasiguiente(&arbsub,TRUE))==-1) return;
if (__er!=0) return;
if ((__er=buscadir(&arbsub))==-1) return;
if (__er!=0) return;
temp2=pones;
temp3=pones->dirsup;
pones->proxdir=ultdireccion=calculadireccion(ultdireccion,(unsigned long)
sizeof(ESTRUCTARB));
npuntarb++;
gastado+=(unsigned long)sizeof(ESTRUCTARB);
if (gastado>libre) ponerror(1);
pones=pones->proxdir;
setmem(pones,sizeof(ESTRUCTARB),0);
pones->antdir=temp2;
pones->atributo=arbsub.ff_attrib;
pones->clusterinicial=arbsub.ff_clusini;
pones->dirsup=temp3;
y0++;
pones->y=y0;
pones->x=x0;
strcpy(pones->nombredir,arbsub.ff_name);
oldclusteract=unidad[unidac].cluster_in_dir_actual;
if (utilizachdir==TRUE)
  cambiadir(arbsub.ff_name); //cambiadir en estos casos llama a chdir
else unidad[unidac].cluster_in_dir_actual=arbsub.ff_clusini;
ponesubdirsp(pones);
if (__er!=0 && __er!=-1) return ; //va saliendo del arbol si se ha producido un error antes
goto buscaotro;
}
/*----------------------------------------------------------------------*/
int meteficheros (struct filee huge *fi)  {
//si no hay error devuelve 0
//si hay error leyendo el disco devuelve el c¢digo de error
struct ffblk_ex f;
struct filee huge *p=fi; //puntero al fichero a almacenar para hacer m s r pida la rutina
int done;

totalfich=0;
done=buscaprimero("*.*",&f,FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH,
TRUE);
while (!done)
{
  if (!done && (!(f.ff_attrib & (FA_DIREC | FA_LABEL)))) {
	memcpy(&p->hora,&f.ff_ftime,4);
	/*p->fecha=f.ff_fdate;
	p->hora=f.ff_ftime;*/
	p->atributo=f.ff_attrib;
	p->longitud=f.ff_fsize;
	strcpy(p->nombre,f.ff_name);
	p->num_fich_sel=0;
	++p;
	++totalfich;
	if (totalfich*sizeof(struct filee)>libre2) ponerror(13);
	if (totalfich==65535U) {
	  pon_mensaje("DIRECTORIO","NO SE ADMITEN MAS DE 65534 FICHEROS",1);
	  return 0;
	  }
	}
  done=buscasiguiente(&f,TRUE);
  }
if (done!=-1) return done;
return 0;
}
/*----------------------------------------------------------------------*/
void buscararbol(int unidad)
  {
  struct ffblk_ex fich;
  char bufint[MAXPATH+1];
  int diski=cogedisco(),pru2,at=coge_atributo(),er;
  struct ventann *ventsa=ventana_actual();

if (hacerven(&ventanas[7],colorf2(ARBOLV))==-1)
  ponerror(1);
writestr(3,2,0,"LEYENDO DIRECTORIOS");
cambiadisco(unidad);
cogerdiractual(bufint,MAXPATH);
cambiadir("\\");
poner_etiqueta_disco();
pon_atributo(colorf2(MARCO));
pru2=buscaprimero("*.*",&fich,FA_DIREC,TRUE);
if (pru2==0) {
  npuntarb=0;
  libre=farcoreleft()-16L;
  //si hay memoria asignada a arbol(liberamemarb mira si antes no se ha liberado)
  liberamemarb();
  if ((arbol=(ESTRUCTARB *)farmalloc(libre))==NULL) ponerror(1);
  ++npuntarb;
  ultdireccion=arbol;
  setmem(arbol,sizeof(ESTRUCTARB),0);
  y0=2;
  x0=2;
  arbol->x=x0;
  arbol->y=y0;
  sprintf(arbol->nombredir,"%c:\\",(unsigned char)unidad+'A');
  arbol->atributo=fich.ff_attrib;
  arbol->clusterinicial=0;
  if ((er=ponesubdirs(arbol))!=0)
	 errordisco(er);
  arbol=farrealloc(arbol,gastado+sizeof(struct estructarb));
  }
else if (pru2!=-1) errordisco(er);
cerrarven(&ventanas[7]);
cambiadisco(diski);
cambiadir(bufint);
pon_atributo(at);
cambiarven(ventsa);
}
/*------------------------------------------------------------------------*/
void escribefichero(int _y,struct filee huge *n) {
char bufferfich[81],*atributos="....",u[MAXDIR],u1[MAXDRIVE],
nombre[MAXFILE+1],extension[MAXEXT+1];
int atributo=n->atributo,bandera;

strset(atributos,'.');
if (atributo & FA_HIDDEN) atributos[0]='O';
if (atributo & FA_SYSTEM) atributos[1]='S';
if (atributo & FA_RDONLY) atributos[2]='L';
if (atributo & FA_ARCH) atributos[3]='A';
bandera=fnsplit(n->nombre,u1,u,nombre,extension);
if (!(bandera & EXTENSION)) {
  extension[0]=' ';
  extension[1]=0;
  }
if (!(bandera & FILENAME))  {
  nombre[0]=' ';
  nombre[1]=0;
  }
  if (n->num_fich_sel)
  sprintf(bufferfich,"%-8s%-4s % 10lu %02u/%02u/%04u %02u:%02u   %4s % 5u",
  nombre,extension,n->longitud,n->fecha.dia,n->fecha.mes,1980+n->fecha.anyo,
  n->hora.hora,n->hora.minutos,atributos,n->num_fich_sel);
else
  sprintf(bufferfich,"%-8s%-4s % 10lu %02u/%02u/%04u %02u:%02u   %4s      ",
  nombre,extension,n->longitud,n->fecha.dia,n->fecha.mes,1980+n->fecha.anyo,
  n->hora.hora,n->hora.minutos,atributos);
writestr(2,_y,0,bufferfich);
}
/*------------------------------------------------------------------------*/
void mostrar_ficheros(unsigned int desde) {
int _atributo=coge_atributo();
unsigned int contador;
char buf[80];
unsigned char vancho,valto;
struct ventann *_ventanasel=ventana_actual();

cambiarven(&ventanas[8]);
pon_atributo(colorf2(FICHEROSV));
vancho=ventana_ancho();
valto=ventana_alto();
writestr(2,2,0,"NOMBRE         LONGITUD FECHA      HORA   ATRIB NUM S");
setmem(&buf,vancho,' ');
buf[vancho]=0;
for (contador=0;contador<valto-2;contador++,desde++) {
  if (desde<totalfich)
	 escribefichero(contador+3,&ficherosdir[desde]);
  else writestr(2,contador+3,0,buf);
  }
if (!totalfich) writestr(2,3,0,"NINGUNO");
cambiarven(_ventanasel);
pon_atributo(_atributo);
}
/*------------------------------------------------------------------------*/
void mostrardirectorio(int unidad) {
register unids,_atr=coge_atributo(),n,er;
struct filee huge *temp;
struct ventann *_ventanasele=ventana_actual();

if (hacerven(&ventanas[7],colorf2(FICHEROSV))==-1)
  ponerror(1);
writestr(3,2,0,"LEYENDO FICHEROS");
unids=cogedisco();
cambiadisco(unidad);
//si hay memoria asignada a ficherosdir(liberamemfich mira si antes no se ha liberado)
liberamemfich();
libre2=farcoreleft()-32; //para que no de problemas de memoria
if ((ficherosdir=(struct filee huge *)farmalloc(libre2))==NULL) 
  ponerror(1);
if ((er=meteficheros(ficherosdir))!=0) errordisco(er);
if (!totalfich) 
  liberamemfich();
else
  if ((ficherosdir=(struct filee huge *)
	 farrealloc((void far *)ficherosdir,(unsigned long)totalfich*sizeof(struct filee)))==NULL)
		ponerror(1);
cerrarven(&ventanas[7]);
hacerven(&ventanas[8],colorf2(FICHEROSV));
mostrar_ficheros(0);
cambiadisco(unids);
pon_atributo(_atr);
cambiarven(_ventanasele);
sumatotalfich=0;
for (n=0,temp=ficherosdir;n!=totalfich;n++,temp++)
 sumatotalfich+=temp->longitud;
}
/*----------------------------------------------------------------*/
void scrollficherosarriba(void){

union REGS _registros;

_registros.h.ch=ventanas[8].y+1;
_registros.h.cl=ventanas[8].x;
_registros.h.dh=ventanas[8].yy-1;
_registros.h.dl=ventanas[8].xx-2;
_registros.h.al=1;
_registros.h.bh=(unsigned char) coge_atributo();
_registros.h.ah=0x06;
int86(0x10,&_registros,&_registros);
}
/*----------------------------------------------------------------*/
void scrollficherosabajo(void){

union REGS _registros;

_registros.h.ch=ventanas[8].y+1;
_registros.h.cl=ventanas[8].x;
_registros.h.dh=ventanas[8].yy-1;
_registros.h.dl=ventanas[8].xx-2;
_registros.h.al=1;
_registros.h.bh=(unsigned char) coge_atributo();
_registros.h.ah=0x07;
int86(0x10,&_registros,&_registros);
}

/*---------------------------------------------------------------------*/
void poner_etiqueta_disco() {
int atrisal=coge_atributo();
struct ventann *vent=ventana_actual();
struct ffblk_ex fich;
char *pv;

cambiarven(&ventanas[6]);
if (!buscaprimero("*.*",&fich,FA_LABEL,TRUE))
 strcpy(etiqueta_disco,fich.ff_name);
 else etiqueta_disco[0]=0;
if ((pv=strchr(etiqueta_disco,'.'))!=NULL) {
 strcpy(pv,pv+1);
 };
hacermarco(colorf2(MARCO));
pon_atributo(colorf2(ARBOLV));
centrar(ventana_alto()+1,0,etiqueta_disco);
pon_atributo(atrisal);
cambiarven(vent);
};
/*---------------------------------------------------------------------*/
void ficheroerror(int nerror) {
//funci¢n que se utiliza cuando hay un error producido por el
//acceso a un fichero
char _error[80];

switch (nerror) {
  case ENOFILE:strcpy(_error,"FICHERO NO ENCONTRADO");
					break;
  case ENOPATH:strcpy(_error,"CAMINO(PATH) NO ENCONTRADO");
					break;
  case EINVDRV:strcpy(_error,"UNIDAD INCORRECTA");
					break;
  case ENOTSAM:strcpy(_error,"NO ES EL MISMO DISPOSITIVO");
					break;
  case ENMFILE:strcpy(_error,"NO MAS FICHEROS");
					break;
  case EMFILE:strcpy(_error,"DEMASIADOS FICHEROS ABIERTOS");
				  break;
  case EACCES:strcpy(_error,"ACCESO DENEGADO");
				  break;
  case EBADF:strcpy(_error,"NUMERO INCORRECTO");
				 break;
  case EFAULT:strcpy(_error,"ERROR DESCONOCIDO");
				  break;
  case EEXIST:strcpy(_error,"FICHERO EXISTE");
				  break;
  default:strcpy(_error,"ERROR DESCONOCIDO");
			 break;
			 };
ajventana(&ventanas[22],strlen(_error)+2);
if (hacerven(&ventanas[22],colorf2(ERRORN))==-1)
  ponerror(1);
centrar(2,0,_error);
centrar(4,0,"PULSE TECLA");
campana();
tecla();
cerrarven(&ventanas[22]);
};
/*---------------------------------------------------------------------*/
void borrafich(char *name)  {
int handle;

#if __BORLANDC__ == 0x452
_rtl_chmod(name,1,FA_ARCH);
if ((handle=_rtl_open(name,O_RDONLY))!=-1) {
  _rtl_close(handle);
#else
_chmod(name,1,FA_ARCH);
if ((handle=_open(name,O_RDONLY))!=-1) {
  _close(handle);
#endif
  unlink(name);
  };
};
/*--------------------------------------------------------------------*/
int ejecutacomando (char *comando,int condicion) { /* condicion : 1 espera tecla , 0 no espera*/
int res;
union REGS regs;
auto unsigned char modoactual;

if (hacerven(&ventanas[18],colorf2(SALIDADOS))==-1) 
  ponerror(1);
if (!strcmp(comando,getenv("COMSPEC"))) {
 writestr(1,1,0,"TECLEE EXIT PARA VOLVER A PCMENU");
 _gotoxy(1,2);
 __setcursortype(_NORMALCURSOR);
 };
regs.h.ah=0x0f;
int86(0x10,&regs,&regs);
modoactual=regs.h.al;
res=system(comando);
regs.h.ah=0x00;
regs.h.al=modoactual;
int86(0x10,&regs,&regs);
regs.x.ax=0x1003;
regs.h.bl=0;
int86(0x10,&regs,&regs);
__setcursortype(_NOCURSOR);
if (condicion==1 && res!=-1) {
  writestr(_wherex(),_wherey(),0,"PULSE TECLA PARA VOLVER A PCMENU");
  tecla();
  };
cerrarven(&ventanas[18]);
if (res!=-1) {
  poner_etiqueta_disco();
  mostrardirectorio(cogedisco());
  }
return(res);
};
/*--------------------------------------------------------------------*/
void far * calculadireccion(void far *oldir,unsigned long asumar)  {
unsigned long dirabs;
unsigned int segmento,offset;

dirabs=((unsigned long)FP_SEG(oldir) << 4)+(unsigned long)FP_OFF(oldir);
dirabs+=asumar;
segmento=(unsigned int)(dirabs >> 4);
offset=(unsigned int)(dirabs-((unsigned long)segmento << 4));
return(MK_FP(segmento,offset));
};
