#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <alloc.h>
#include <stdlib.h>
#include <process.h>
#include <dir.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <pcventan.h>
#include <pcutil.h>
#include <pcdisco.h>
#include <pcejec.h>

char *logo2[]={"PCMENU V 1.1","(c) BIT SOFT","1993-99"};
/*-------------------------------------------------------------------------*/
void nada(void);
void Salir(void);
void cambconf(void);
void logot(void);
void infod(void);
void nuevconf(void);
/*-------------------------------------------------------------------------*/
struct meninn menin[]={
{1,"Ficheros"},
{11,"Disco"},
{18,"Configuraci¢n"},
{33,"Especial"},
{43,"Aplicaciones"},};
struct Menuin *menuin;
struct opciontipo opciones[]=  {
{"~C~opiar fichero",nada,"Copia de un directorio a otro o a otra unidad"},
{"Copy ~+~",nada,"Distribuye los archivos marcados en discos"},
{"C~o~mparar fichero",nada,"Compara byte a byte un/os fichero/s"},
{"~R~enombrar fichero",nada,"Cambia el nombre"},
{"~B~orrar fichero",nada,"Borra una lista de ficheros"},
{"~L~ocalizar ficheros",nada,"A trav‚s de un filtro localiza ficheros"},
{"~M~over ficheros",nada,"Traslada un fichero de un lugar a otro"},
{"B~u~scar texto",nada,"Localiza una cadena dada en un fichero"},
{"~I~mprimir fichero",nada,"Imprime con el formato especificado"},
{"~V~erificar fichero",nada,"Mira que el fichero est‚ bien grabado"},
{"I~m~primir directorio",nada,"Imprime la lista de ficheros"},
{"~U~ndelete",nada,"Recupera ficheros borrados"},
{"Lim~p~iar fichero",nada,"Borra ficheros de manera que no se puedan recuperar"},
{"Cambiar ~a~tributo",nada,"Cambia los atributos del fichero"},
{"V~e~r fichero",nada,"Visualiza fichero"},
{"Encrip~t~ar fichero",nada,"Oculta con un codigo secreto un fichero"},
{"E~j~ecutar {CTRL-ENTER}",nada,"Ejecuta un programa ejecutable"},
{"Ejecutar coma~n~do",nada,"Ejecuta un comando del MS-DOS"},
{"~S~alir",Salir,"Sale de Pcmenu"},
{"-",nada,""}, /* comando de separaci¢n de menu */
{"~I~nformacion disco",infod,"Informa sobre capacidad , libre , etc."},
{"~C~opiar disco",nada,"Duplica disco de formato estandard"},
{"C~a~mbiar unidad",cambunidproc,"Cambia unidad activa"},
{"~F~ormatear disco",nada,"Prepara un disco para ser utilizado por el MS-DOS"},
{"~D~irectorio",pfun_arbol,"Mantenimiento de directorios"},
{"~B~uscar cadena",nada,"Busca una cadena dada en todo el disco"},
{"~R~enombrar etiqueta",nada,"Cambia el nombre al disco"},
{"~V~erificar disco",nada,"Examina un disco sobre posibles sectores erroneos"},
{"V~e~r disco",nada,"Visualiza el contenido del disco"},
{"-",nada,""}, /* comando de separaci¢n de menu */
{"~C~ambiar configuraci¢n",cambconf,"Cambia la configuraci¢n de carga de programas"},
{"~H~acer nueva configuraci¢n",nuevconf,"Hace una nueva configuraci¢n de carga de programas"},
{"-",nada,""},
{"~I~nformaci¢n del sistema",nada,"Visualiza el hardware del ordenador"},
{"~M~irar memoria",nada,"Visualiza la memoria"},
{"~L~ogotipo",logot,"Muestra el logotipo"},
{"-",nada,""},
{"*",nada,""},/*comando de fin de men£s*/
};
/*-------------------------------------------------------------------------*/
void borratemp(void);
int _compara (const void *,const void *);
static void near mostrarayuda(int);
static void near selecciona(int,int);
static void near desselecciona(int,int);
static void near abriropcion(int);
static void near cerraropcion(int);
void   sel_fich(void);
static void near actualiza_ven_seleccionado(void);
static void near actualiza_fich_sel(unsigned int);
static int near ejecutapcejec(void);
static void near down_arbol(void);
static void near up_arbol(void);
static void near right_arbol(void);
static void near left_arbol(void);
static void near ac_var_fich(void);
static void near p_c_enter(void);
static void near ponopcion2(int numo,char *b[]);
/*-------------------------------------------------------------------------*/
BOOLEAN sif10;
DATOS datos[MAXSOFT];
ESTRUCTARB *arbolptr;
/*-------------------------------------------------------------------------*/
unsigned int totalfich_sel,_num_fich_sel,ULTIMOMENU,numfich,arboly,filey;
//arboly: coordenada y absoluta del directorio que aparece arriba del todo
unsigned long total_byt_sel;
char dirsel[MAXPATH+1]; /*cadena del directorio seleccionado por fun_arbol si se entra con 1*/
unsigned char ancho2;
int cscreen2,cscreenA2,alto2,atrsalva,atritemp,pruebas,menu=1,opcion=0,
intento,fin=0,j,i,yrel,salmenu,longitudclave;
//yrel: desplazamiento relativo a la ventana del directorio

int ventanaactual; //para después poder activar la ventana anterior al actualizar el nombre del volumen de la unidad actual
char *clave="\x1\x1\x9\x3pc",diractual[MAXPATH+1],*menu2 =
"~F~icheros  ~D~isco  ~C~onfiguraci¢n  ~E~special  ~A~plicaciones";
/*cada opcion2 en realidad ocupa 2*/
char *opciones2[4]= {
"Movimiento con \x18 \x19 \x1A \x1B    y enter",
"~F1~ Ayuda ³ ~ESC~ Salir de menus ³ ~Alt-S~ Salir de PCMENU",
"~F1~ Ayuda  ³ ~F2~ Cambiar unidad ³ ~F3~ Shell al dos  ³ ~F4~ Desseleccionar",
"~F5~ Copiar ³ ~F10~ Menu ³ ~Alt-S~ Salir",
};
/*------------------------------------------------------------------------*/
static void near tirar_sel_abajo_fich(void)
{
if (numfich+1!=totalfich)
	  {writeatrib(2,filey,ventana_ancho()-1,colorf2(FICHEROSV));
		filey++;
		numfich++;
		if (filey>ventana_alto())
	  {filey=ventana_alto();
		scrollficherosarriba();
	  }
	};
  sel_fich();
}
/*-------------------------------------------------------------------------*/
static void near tirar_sel_arriba_fich(void)
{
if (numfich>0) {
  if (filey==3) scrollficherosabajo();
  writeatrib(2,(filey==3) ? (filey+1) : (filey),ventana_ancho()-1,
  colorf2(FICHEROSV));
  if (filey>3) filey--;
  numfich--;
  sel_fich();
  }
}
/*------------------------------------------------------------------------*/
void nuevconf(void) {
  confproc(0);
  }
/*------------------------------------------------------------------------*/
void cambconf(void) {
  confproc(1);
  }
/*------------------------------------------------------------------------*/
void dessel_ficheros(void) {
unsigned int _filey=filey,_numfich=numfich,cont;

for (cont=0;cont<totalfich;cont++) ficherosdir[cont].num_fich_sel=0;
ac_var_fich();
filey=_filey;
numfich=_numfich;
sel_fich();
actualiza_ven_seleccionado();
mostrar_ficheros((numfich-filey)+3);
}
/*-------------------------------------------------------------------------*/
void infod(void) {
poninfodisco(cogedisco());
}
/*-------------------------------------------------------------------------*/
void logot(void)  {
register i;

if (hacerven(&ventanas[23],colorf(LOGOTIPO))==-1) ponerror(1);
for (i=0;i<3;i++)
  centrar((i+1)*2,0,(char *)logo2[i]);
centrar(ventana_alto(),0,"PULSE TECLA");
tecla();
borra_tec_buffer();
cerrarven(&ventanas[23]);
}
/*-------------------------------------------------------------------------*/
void nada(void) {
  return ; }
/*-------------------------------------------------------------------------*/
void Salir(void) {
  salmenu=1;}

/*-------------------------------------------------------------------------*/
static void near mostrarayuda(int numerogeneral)  {
register a=coge_atributo();
struct ventann *_vents=ventana_actual();

  cambiarven(&ventanas[15]);
  pon_atributo(colorf2(LOGOTI));
  fillwin(' ');
  if (menu==ULTIMOMENU)  centrar(1,0,datos[numerogeneral].aplicacion);
  else centrar(1,0,opciones[numerogeneral].instruccion);
  pon_atributo(a);
  cambiarven(_vents);
  }

/*-------------------------------------------------------------------------*/
static void near selecciona(int numop,int nummenu)  {
//numop:numero de opcion del menu,empieza en 0
register j,i;
char *caractermismo="~";
char *longg;
char poner[81];

j=menuin[nummenu].inicio+numop;
pon_atributo(colorf2(ILUOPCMENU));
if (nummenu==ULTIMOMENU)  {
  strcpy(poner,datos[numop].nombre);
  i=opcion;  }
else  {
  strcpy(poner,opciones[j].utilidad);
  i=menuin[menu].inicio+opcion;
  }
longg=poner;
while (longg!=NULL) {
  longg=strpbrk(longg,caractermismo);
  if (longg!=NULL)
	 strcpy(longg,longg+1);
  }
writestr(2,numop+2,0,poner);
mostrarayuda(i);
}
/*-------------------------------------------------------------------------*/
static void near desselecciona(int numop,int nummenu)  {
//numop:numero de opcion del menu,empieza en 0
register j,i;
char *_ptr;

j=menuin[nummenu].inicio+numop;
pon_atributo(colorf2(MENUDESP));
if (nummenu==ULTIMOMENU)  {
  _ptr=datos[numop].nombre;
  i=opcion;  }
else  {_ptr=opciones[j].utilidad;
		i=menuin[menu].inicio+opcion;}
writestr(2,numop+2,1,_ptr);
mostrarayuda(i);
}
/*-------------------------------------------------------------------------*/
void sel_fich(void) {
register _at=coge_atributo();
struct ventann *_ve=ventana_actual();

cambiarven(&ventanas[8]);
pon_atributo(colorf2(FICHSEL));
if (totalfich) escribefichero(filey,&ficherosdir[numfich]);
pon_atributo(_at);
cambiarven(_ve);
}
/*-------------------------------------------------------------------------*/
static void near actualiza_fich_sel(unsigned int fichrel) {
//actualiza el n£mero de seleccion de todos los ficheros a partir del
//n£mero de ficherosdir[fichrel].num_fich_sel
//y ademas quita la seleccion del fichero
struct filee huge *temp=ficherosdir;
unsigned int desdenum=ficherosdir[fichrel].num_fich_sel;
unsigned int cont;

temp[fichrel].num_fich_sel=0;
for (cont=0;cont<totalfich;cont++,temp++)
  if (temp->num_fich_sel>desdenum)
	 temp->num_fich_sel--;
}
/*-------------------------------------------------------------------------*/
static int near ejecutapcejec(void) {
//ejecuta pcejec.com(que ejecuta el programa de pcmenu.tem
char drive[MAXDRIVE],dir[MAXDIR],file[MAXFILE],ext[MAXEXT],
nombreejec[MAXDRIVE+MAXDIR+MAXFILE+MAXEXT];

fnsplit(_argv[0],drive,dir,file,ext);
fnmerge(nombreejec,drive,dir,"PCEJEC",".EXE");
return (spawnl(P_OVERLAY,nombreejec,_argv[0],NULL));
}
/*-------------------------------------------------------------------------*/
void escribedirectorio(char *_dir) {
register atr=coge_atributo();
struct ventann *ven=ventana_actual();

cambiarven(&ventanas[20]);
pon_atributo(colorf2(ARBOLV));
fillwin(' ');
centrar(1,0,_dir);
cambiarven(ven);
pon_atributo(atr);
}
/*-------------------------------------------------------------------------*/
static void near _cambiadir(void) {
cogerdiractual(diractual,MAXPATH);
mostrardirectorio(cogedisco());
escribedirectorio(diractual);
}
/*-------------------------------------------------------------------------*/
static void near down_arbol(void) { // desplaza abajo arbol directorios

yrel+=arbolptr->proxdir->y-arbolptr->y;
if (yrel>altoarbol) {
  arboly=arbolptr->proxdir->y;
  yrel=2;
  ponerarbol(arboly,arbol);
  }
};
/*-------------------------------------------------------------------------*/
static void near up_arbol(void) { // desplaza abajo arbol directorios
yrel-=arbolptr->y-arbolptr->antdir->y;
if (yrel<2) {
  yrel=2;
  arboly=arbolptr->antdir->y;
  ponerarbol(arboly,arbol);
  }
};
/*-------------------------------------------------------------------------*/
static void near right_arbol(void) { // desplaza derecha arbol directorios
yrel+=arbolptr->subdir->y-arbolptr->y;
if (yrel>altoarbol-1) {
  //si se sale de la ventana
  yrel=2;
  arboly=arbolptr->subdir->y;
  ponerarbol(arboly,arbol);
  }
};
/*-------------------------------------------------------------------------*/
static void near left_arbol(void) { // desplaza izquierda arbol directorios
yrel-=arbolptr->y-arbolptr->dirsup->y;
if (yrel<2) {
  //si se sale de la ventana
  yrel=2;
  arboly=arbolptr->dirsup->y;
  ponerarbol(arboly,arbol);
  }
};
/*-------------------------------------------------------------------------*/
static void near ac_var_fich(void) {
//reinicializa las variables relacionadas con los ficheros
_num_fich_sel=0;
totalfich_sel=0;
total_byt_sel=0;
filey=3;
numfich=0;
}
/*-------------------------------------------------------------------------*/
void sel_dir(int _atributo) {
writeatrib(arbolptr->x+1,yrel,strlen(arbolptr->nombredir),
_atributo);
};
/*-------------------------------------------------------------------------*/
void preparanuevoarbol() {

ac_var_fich();
actualiza_ven_seleccionado();
seleccionaventana(&ventanas[8],colorf2(VENTSEL));
arboly=arbol->y;
yrel=2;
}
/*-------------------------------------------------------------------------*/
void seleccionaopcion(void)  {
int teclalee,tempf;
struct ejecutar ejecutar;
struct ventann *ventsa;
unsigned char tem;

salmenu=0;
cogerdiractual(diractual,MAXPATH);
escribedirectorio(diractual);
cambiarven(&ventanas[6]);
sel_dir(colorf2(DIRSEL));
cambiarven(&ventanas[8]);
ventanaactual=8;
ac_var_fich();
actualiza_ven_seleccionado();
seleccionaventana(&ventanas[8],colorf2(VENTSEL));
sif10=FALSE;
menu=0;
opcion=0;
numfich=0;
sel_fich();
ponopcion2(1,opciones2);
  do {
	 teclalee=tecla();
	 if (sif10==TRUE)  {
	 //si se esta en la barra de menus
	 switch(teclalee)  {
		 case A_S:cerraropcion(menu);
					 fin=1;
					 break;
		 case UP:
		 desselecciona(opcion,menu);
		 if (!opcion)  {
		 if (menu!=ULTIMOMENU) {
			 opcion=menuin[menu].fin-menuin[menu].inicio;}
		 else {
			 opcion=registros-1;
			 }
		 }
		 else  --opcion;
		 selecciona(opcion,menu);
		 break;
		 case DOWN:
		 desselecciona(opcion,menu);
		 if (menu==ULTIMOMENU) {
			if (opcion==registros-1)  {
				opcion=0;}
			else {
				++opcion;
				}
		  }
	  else {
		 if (opcion==menuin[menu].fin-menuin[menu].inicio)  {  
			opcion=0;  }
		 else {
			++opcion; };
		 }
		 selecciona(opcion,menu);
		 break;
	  case ESC:sif10=FALSE;
				  cerraropcion(menu);
				  ponopcion2(1,opciones2);
				  tempf=coge_atributo();
				  cambiarven(&ventanas[15]);
				  pon_atributo(colorf2(FICHEROSV));
				  fillwin(' ');
				  pon_atributo(tempf);
				  cambiarven(ventsa);
				  break;
	  case RIGHT:cerraropcion(menu);
			if (menu==ULTIMOMENU)
			menu=0;
			  else
			++menu;
		abriropcion(menu);
		if (menu!=ULTIMOMENU) {
		  if (opcion>menuin[menu].fin-menuin[menu].inicio) 
		  opcion=menuin[menu].fin-menuin[menu].inicio;
		  }
		else if (opcion>registros-1) opcion=registros-1;
		selecciona(opcion,menu);
		break;
	  case LEFT:cerraropcion(menu);
			 if (menu==0)
				menu=ULTIMOMENU;
				else
				--menu;
			 abriropcion(menu);
			 if (menu!=ULTIMOMENU) {
				if (opcion>menuin[menu].fin-menuin[menu].inicio) 
				opcion=menuin[menu].fin-menuin[menu].inicio;
				}
			 else if (opcion>registros-1) opcion=registros-1;
			 selecciona(opcion,menu);
			 break;
		case ENTER:
		cerraropcion(menu);
		if (menu!=ULTIMOMENU)  {
		  i=menuin[menu].inicio+opcion;
		  opciones[i].salida();;
		if (salmenu==1)  fin=1;  }
		else  {
		borratemp();
		#if __BORLANDC__	== 0x452
		if ((tempf=_rtl_creat(nombretemp,FA_HIDDEN))==-1) ficheroerror(errno);
		#else
		if ((tempf=_creat(nombretemp,FA_HIDDEN))==-1) ficheroerror(errno);
		#endif
		else {
		  strcpy(ejecutar.directorio,(char *)datos[opcion].directorio);
		  strcpy(ejecutar.ejecutable,(char *)datos[opcion].ejecutable);
		  strcpy(ejecutar.parametros,(char *)datos[opcion].parametros);
		  #if __BORLANDC__	== 0x452
		  _rtl_write(tempf,&ejecutar,sizeof (struct ejecutar));
		  _rtl_close(tempf);
		  _rtl_chmod(nombretemp,1,FA_HIDDEN);
		  #else
		  _write(tempf,&ejecutar,sizeof (struct ejecutar));
		  _close(tempf);
		  _chmod(nombretemp,1,FA_HIDDEN);
		  #endif
		  pruebas=ejecutapcejec();
		  borratemp();
		  if (pruebas==-1) ponerror(11);}
		  }
		abriropcion(menu);
		selecciona(opcion,menu);
		break;
		 }
	 }
	 if (sif10==FALSE)
		switch(teclalee)
		 {
		case A_S:fin=1;
			break;
		case C_ENTER:if ((strstr(ficherosdir[numfich].nombre,".COM")==NULL &&
						 strstr(ficherosdir[numfich].nombre,".EXE")==NULL &&
						 strstr(ficherosdir[numfich].nombre,".BAT")==NULL))
							ponerror(19);
						 else
							p_c_enter();
						 break;
		case F3:if ((ejecutacomando(getenv("COMSPEC"),0))==-1)
					 ponerror(2);
				  if (ventanaactual==8)
					 seleccionaventana(&ventanas[8],colorf2(VENTSEL));
					 else
					seleccionaventana(&ventanas[6],colorf2(VENTSEL));
				  break;
		case F2:cambunidproc();
				  break;
		case F4:if (totalfich) {
					 dessel_ficheros();
					 sel_fich();
					 }
				  break;
		case F5:nada();
				  break;
		case F10:sif10=TRUE;
					ventsa=ventana_actual();
					abriropcion(menu);
					selecciona(opcion,menu);
					ponopcion2(0,opciones2);
					break;
		case A_U:sif10=TRUE;
					menu=1;
					opcion=0;
					abriropcion(menu);
					selecciona(opcion,menu);
					ponopcion2(0,opciones2);
					break;
		case A_C:sif10=TRUE;
					menu=2;
					opcion=0;
					abriropcion(menu);
					selecciona(opcion,menu);
					ponopcion2(0,opciones2);
					break;
		case A_P:sif10=TRUE;
					menu=3;
					opcion=0;
					abriropcion(menu);
					selecciona(opcion,menu);
					ponopcion2(0,opciones2);
					break;
		case DOWN:if (ventanaactual==8 && totalfich) tirar_sel_abajo_fich();
					 if (ventanaactual==6 && arbolptr->proxdir) {
						sel_dir(colorf2(ARBOLV));
						down_arbol();
						arbolptr=arbolptr->proxdir;
						sel_dir(colorf2(DIRSEL));
						cambiadir("..");
						cambiadir(arbolptr->nombredir);
						_cambiadir();
						ac_var_fich();
						sel_fich();
						actualiza_ven_seleccionado();};
						break;
		case UP:if (ventanaactual==8 && totalfich)tirar_sel_arriba_fich();
				  if (ventanaactual==6 && arbolptr->antdir) {
					 sel_dir(colorf2(ARBOLV));
					 up_arbol();
					 arbolptr=arbolptr->antdir;
					 sel_dir(colorf2(DIRSEL));
					 cambiadir("..");
					 cambiadir(arbolptr->nombredir);
					 _cambiadir();
					 ac_var_fich();
					 sel_fich();
					 actualiza_ven_seleccionado();};
					 break;
		case RIGHT:if (ventanaactual==6 && arbolptr->subdir) {
						sel_dir(colorf2(ARBOLV));
						right_arbol();
						arbolptr=arbolptr->subdir;
						sel_dir(colorf2(DIRSEL));
						cambiadir(arbolptr->nombredir);
						_cambiadir();
						ac_var_fich();
						sel_fich();
						actualiza_ven_seleccionado();};
						break;
		case LEFT:if (ventanaactual==6 && arbolptr->dirsup) {
						sel_dir(colorf2(ARBOLV));
						left_arbol();
						arbolptr=arbolptr->dirsup;
						sel_dir(colorf2(DIRSEL));
						cambiadir("..");
						_cambiadir();
						ac_var_fich();
						sel_fich();
						actualiza_ven_seleccionado();};
						break;
		case PGDN:if (ventanaactual==8 && totalfich) {
						 numfich+=ventana_alto()-2;
						 if (numfich>=totalfich) {
							numfich=totalfich-1; //numfich empieza en 0 y totalfich empieza en 1
							filey=ventana_alto();
							if (ventana_alto()-2>totalfich) 
							  //si hay menos ficheros que el alto de la ventana de poner ficheros
							  filey=totalfich+2;
							};
						 mostrar_ficheros(numfich-(filey-3));
						 sel_fich();
						 };
						 break;
		case PGUP:if (ventanaactual==8 && totalfich) {
					  tem=ventana_alto()-2;
					  if (totalfich<=tem || numfich<=tem) {
						 numfich=0;
						 filey=3;
						 }
					  else {
						 numfich-=tem;
						 if (numfich<tem)
							filey=3+numfich;
						 };
						 mostrar_ficheros(numfich-(filey-3));
						 sel_fich();
					  }
					  break;
		case END:if (ventanaactual==8 && totalfich) {
						 numfich=totalfich-1;
						 if (totalfich<ventana_alto()) 
							filey=(unsigned char)totalfich+3;
						 else filey=ventana_alto();
						 mostrar_ficheros(numfich-(filey-3));
						 };
						 sel_fich();
						 break;
		case HOME:if (ventanaactual==8 && totalfich) {
						 numfich=0;
						 filey=3;
						 mostrar_ficheros(0);
						 };
						 sel_fich();
						 break;
		case TAB:if (ventanaactual==8) {
					 seleccionaventana(&ventanas[8],colorf2(MARCO));
					 seleccionaventana(&ventanas[6],colorf2(VENTSEL));
					 ventanaactual=6;
					 } else {
					 seleccionaventana(&ventanas[6],colorf2(MARCO));
					 seleccionaventana(&ventanas[8],colorf2(VENTSEL));
					 ventanaactual=8;
					 };
					 break;
		case ENTER:if (ventanaactual==8 && totalfich) {
						 if (ficherosdir[numfich].num_fich_sel) {
							total_byt_sel-=ficherosdir[numfich].longitud;
							totalfich_sel--;
							--_num_fich_sel;
							actualiza_fich_sel(numfich);}
							else {
							total_byt_sel+=ficherosdir[numfich].longitud;
							totalfich_sel++;
							++_num_fich_sel;
							ficherosdir[numfich].num_fich_sel=_num_fich_sel;
							}
						 actualiza_ven_seleccionado();
						 mostrar_ficheros((numfich-filey)+3);
						 tirar_sel_abajo_fich();
						 };
					  break;
		 }
	 }while (!fin);

}
/*-------------------------------------------------------------------------*/
static void near abriropcion(int numerop) {
//abrir menu desplegable
unsigned int i,l,j1;

cambiarven(&ventanas[21]);
pon_atributo(15);
writestr(menin[numerop].empieza,1,1,menin[numerop].menu);
if (hacerven(&ventanas[numerop],colorf2(MENUDESP))==-1)
  ponerror(1);
j1=0;
if (menu!=ULTIMOMENU)  {
  for (i=(menuin[numerop].inicio);i<=(menuin[numerop].fin);i++) {
	 l=2+j1;
	 writestr(2,l,1,opciones[i].utilidad);
	 j1++;}
  }
else
  for (i=0;i<registros;i++)  {
	 l=2+j1;
	 writestr(2,l,1,datos[i].nombre);
	 ++j1;  };
}
/*-------------------------------------------------------------------------*/
static void near cerraropcion(int numerop) {
//cerrar menu desplegable

cerrarven(&ventanas[numerop]);
cambiarven(&ventanas[21]);
pon_atributo(colorf2(LOGOTI));
writestr(1,1,1,menu2);
}
/*---------------------------------------------------------------------*/
//lee pcmenu.dat,comprueba si es v lido,si es v lido lo lee en memoria
//y si no llama a nuevconfproc para crear nuevo pcmenu.dat
void compruebaconfiguracion(void)  {
char compclave[7];
int fdatos;


#if __BORLANDC__	== 0x452
if (((fdatos=_rtl_open(nombrefichero,O_RDONLY))==-1)) {
#else
if (((fdatos=_open(nombrefichero,O_RDONLY))==-1))  {
#endif
	 ponerror(15);
	 confproc(2);}
	 else {
#if  __BORLANDC__ == 0x452
	 _rtl_read(fdatos,compclave,longitudclave);
#else
		  _read(fdatos,compclave,longitudclave);
#endif
		  pruebas=memcmp(compclave,clave,longitudclave);
		  if (pruebas==0)  {
#if  __BORLANDC__ == 0x452
		  _rtl_read(fdatos,&registros,sizeof(registros));
#else
		  _read(fdatos,&registros,sizeof(registros));
#endif
		  ventanas[4].yy=registros+2;
#if  __BORLANDC__ == 0x452
		  _rtl_read(fdatos,&datos,sizeof(datos));
#else
		  _read(fdatos,&datos,sizeof(datos));
#endif
#if  __BORLANDC__ == 0x452
        _rtl_close(fdatos);
#else
		  _close(fdatos);
#endif
		  }
		else {
		ponerror(9);
#if  __BORLANDC__ == 0x452
		_rtl_close(fdatos);
#else
		_close(fdatos);
#endif
		confproc(2);
		}
  }
}
/*-------------------------------------------------------------------------*/
static void near ponopcion2(int numo,char *b[]) {
register at=coge_atributo();
struct ventann *vents=ventana_actual();

cambiarven(&ventanas[14]);
pon_atributo(colorf2(MENUTECLAS));
fillwin(' ');
writestr(1,1,1,b[2*numo]);
writestr(1,2,1,b[2*numo+1]);
pon_atributo(at);
cambiarven(vents);
}
/*--------------------------------------------------------------------*/
static void near actualiza_ven_seleccionado(void) {
register _at=coge_atributo();
char buf_sel[80];
struct ventann *_ve=ventana_actual();

cambiarven(&ventanas[19]);
pon_atributo(colorf2(FICHEROSV));
fillwin(' ');
sprintf(buf_sel,"%u ficheros seleccionados , %lu bytes seleccionados",
totalfich_sel,total_byt_sel);
centrar(1,0,buf_sel);
sprintf(buf_sel,"%u ficheros en total , %lu bytes en total",totalfich,
sumatotalfich);
centrar(2,0,buf_sel);
cambiarven(_ve);
pon_atributo(_at);
}
/*--------------------------------------------------------------------*/
void pcopcion_init(void) {
unsigned int contador,anchomas,ancho2;
unsigned long queda;

queda=coreleft()-16L;
if ((menuin=(struct Menuin *)malloc((queda>65530L) ? 65500U : 
(unsigned int)queda))==NULL)
  ponerror(1);
menuin[0].inicio=0;
anchomas=0;
for (contador=0,ULTIMOMENU=65535U;opciones[contador].utilidad!="*";
  contador++) {
  ancho2=strlen(opciones[contador].utilidad);
  if (ancho2>anchomas) anchomas=ancho2;
  if (opciones[contador].utilidad=="-")  {
	 ++ULTIMOMENU;
	 menuin[ULTIMOMENU].fin=contador-1;
	 menuin[ULTIMOMENU+1].inicio=contador+1;
	 ventanas[ULTIMOMENU].xx=ventanas[ULTIMOMENU].x+anchomas;
	 anchomas=0;
	 continue;
	 };
  };
++ULTIMOMENU;
if ((menuin=(struct Menuin *)realloc(menuin,(ULTIMOMENU+2)*
  sizeof(struct Menuin)))==NULL) ponerror(1);
for (contador=0;contador<ULTIMOMENU;contador++)
	ventanas[contador].yy=((menuin[contador].fin-menuin[contador].inicio)+1)+
	ventanas[contador].y;
if (hacerven(&ventanas[28],colorf(COLORFONDO2))==-1)
  ponerror(1);
__setcursortype(_NOCURSOR);
fillwin('²');
pon_atributo(colorf2(LOGOTI));
writechar (1,1,ventana_ancho(),' ');
writestr(1,1,1,menu2);
ponopcion2(1,opciones2);
hacerven(&ventanas[19],colorf2(FICHEROSV));
hacerven(&ventanas[20],colorf2(ARBOLV));
anchoarbol=(ventanas[6].xx-ventanas[6].x)+1;
altoarbol=(ventanas[6].yy-ventanas[6].y)+1;
};
/*--------------------------------------------------------------------*/
static void near p_c_enter(void) {
int at=coge_atributo(),tecs;
char paramet[MAXPATH+1];
char programa[14+MAXPATH+1];
unsigned int tecsalir[]={ESC,0};
struct ventann *vents=ventana_actual();

if (hacerven(&ventanas[25],colorf2(P_P))==-1)
  ponerror(1);
paramet[0]=0;
tecs=dial(2,2,32,255,MAXPATH-4,paramet,colorf2(P_P),FALSE,TRUE,tecsalir);
cerrarven(&ventanas[25]);
if (tecs==ENTER) {
  sprintf(programa,"%s %s",ficherosdir[numfich].nombre,paramet);
  if ((ejecutacomando(programa,1))==-1) ponerror(21);
  if (ventanaactual==8)
	 seleccionaventana(&ventanas[8],colorf2(VENTSEL));
	 else
	 seleccionaventana(&ventanas[6],colorf2(VENTSEL));
  };
cambiarven(vents);
pon_atributo(at);
}