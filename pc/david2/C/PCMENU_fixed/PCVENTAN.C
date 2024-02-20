#if __BORLANDC__	< 0x452
#pragma inline //para que ya compile directo en asm
#endif

#include <alloc.h>
#include <pcventan.h>

/*-------------------------------------------------------------------------*/
#if	!defined(__COLORS)
#define __COLORS
enum COLORS {
	BLACK,			/* dark colors */
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,		/* light colors */
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};
#endif

#if defined(__LARGE__) || defined(__HUGE__) || defined(__COMPACT__)
#define FARPOINTERS
#else
#define NEARPOINTERS
#endif
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif
#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2
#define amayus(c) ((c>='a' && c<='z') ? c-32 : c)
/*-------------------------------------------------------------------------*/
const unsigned char negritacolor=MAGENTA + (CYAN << 4);
const unsigned char negritamono=WHITE + (BLACK << 4);
/*-------------------------------------------------------------------------*/
static unsigned char oldmodo,actmodo,atr2,wx,wy,wx1,wy1,ultimoatributo;
static unsigned int sipon;
static int cscreenA,cscreen;
static int cscreenA2,cscreen2;
static unsigned char ancho2; //,alto2,atrsalva;
static unsigned char ancho,alto,atributo;
static struct ventann *VENTANASEL; /*ventana actual seleccionada*/
static struct ventann *ULTIMAVENTANASEL; /* ultima ventana hecha con hacerven*/
static char ventanaa1= '²',ventana2='±',vent='²',marco= 'É',marco2='»',
marco3='È',marco4='¼',marco5='Í',marco6='º';
static char buffer_ventanas_privadas[6*79*2];
static struct ventann ventanas_privadas[]={
{1,11,80,14,siguarda,SOMBRA,"",&buffer_ventanas_privadas},/*para pon_mensaje(indice 0)*/
{1,11,80,12,siguarda,SOMBRA,"",&buffer_ventanas_privadas},/*para siconfirmacion*/
};
static int _colorf2[][2]={
{LIGHTGRAY + (BLACK << 4),WHITE + (BLUE << 4)},         /* _MARCO       */
{LIGHTGRAY + (BLACK << 4),WHITE + (BLUE << 4)},	        /* _TITULO */
{WHITE + (BLACK << 4),YELLOW + (BLUE << 4)},              /* _TITULO2 */
{BLACK + (LIGHTGRAY << 4),WHITE + (CYAN << 4)},         /* _LOGOTI    */
};
/*-------------------------------------------------------------------------*/
int adaptador;
unsigned char ultimoatributo;
struct estados estadotecla; //para tecla,se actualiza cuando se llama a tecla()
int bufteclado; //c¢digo devuelto de la funci¢n 0 de la int 16h(tecla())
//los 8 bits m s altos son el c¢digo de la tecla
//y los 8 m s bajos es el caracter ASCII de la tecla
/*-------------------------------------------------------------------------*/
static void pascal near __VPTR2(int x,int y); /*Devuelve el resultado en AX:offset DX:segmento*/
static void near ponstring(unsigned char x,unsigned char y,char *cad,
unsigned int longi);
static void near pascal copiastr(char *dest,char *source);
static void near pascal copiamem(void *dest,void *source,int n);
//-----------------------------------------------------------------------
enum _colores {_MARCO,_TITULO,_TITULO2,_LOGOTI};
/*-------------------------------------------------------------------------*/
unsigned char ventana_ancho(void) {
//devuelve el ancho de la ventana actual
return ancho;
}
/*-------------------------------------------------------------------------*/
unsigned char ventana_alto(void) {
//devuelve el alto de la ventana actual
return alto;
}
/*-------------------------------------------------------------------------*/
void pon_atributo(unsigned char atr) {
atributo=atr;
}
/*-------------------------------------------------------------------------*/
unsigned char coge_atributo(void) {
return atributo;
}
/*-------------------------------------------------------------------------*/
void fillwin(unsigned char ch3) {
int x,y;
char buffer[2];

//TODO: esta funcion fillwin tiene algun error
//y se cuelga si se ejecuta el asm tal cual
//llamando a writestr tambien falla
int maxx;

return;
maxx=ancho;
if (maxx>40) maxx=40;

for (x=1;x<maxx;x++) {
for (y=1;y<20;y++) {
buffer[0]=ch3;
buffer[1]=0;
writestr(x,y,0,buffer);
}
}
return; //temporal
asm {
	push	dx
	push	cx
	push	bx
	cld
	xor	cx,cx
	mov 	al,ch3
	mov 	bx,cscreen
	mov 	es,bx
	mov 	di,cscreenA
	mov 	ah,atributo}
fillww:asm mov cl,alto;
bucle1:asm {
	push 	cx
	push 	di
	xor 	cx,cx
	mov 	cl,ancho}
bucle2:asm {
	stosw
	loop 	bucle2
	pop	di
	pop 	cx
	add 	di,160
	loop 	bucle1
	pop	bx
	pop	cx
	pop	dx
	 }
	}
/*-------------------------------------------------------------------------*/
void writechar(int x,int y,int contador,char caracter){
	asm {
	push	bx
	push	cx
	push	dx
	};
	 __VPTR2(x,y);
	 asm {
	mov 	di,ax
	mov 	es,dx
	mov 	ah,atributo
	mov 	al,caracter
	mov 	cx,contador
	cld
	rep stosw
	pop	dx
	pop	cx
	pop	bx}
}
/*-------------------------------------------------------------------------*/
/* con '~'(0 no 1 si)*/
void writestr(int x,int y,int sinegrita,char *frase)  {
unsigned char atrib2=atr2;

asm {
	push	bx
	push	cx
	push	dx
	};
#ifdef FARPOINTERS
asm push ds;
#endif
asm {mov 	ax,sinegrita
	 mov 	sipon,ax
	 mov 	ax,x
	 dec 	ax
	 mov 	ch,0
	 mov 	cl,ancho
	 sub 	cl,al
	 test cl,cl
	 jz 	fin2
	 };
	 __VPTR2(x,y);
	 asm {
	 mov 	di,ax
	 mov 	es,dx
	 mov 	ah,atributo};
#if defined(NEARPOINTERS)
asm mov si,frase;
#else
asm lds si,frase;
#endif
asm cld;
bucle3: asm {
	 lodsb
	 push	ax
	 mov 	ax,sipon
	 cmp 	ax,1
	 pop 	ax
	 jnz 	aa1};
aa: asm {
	 cmp 	al,126
	 jnz 	aa1
	 push dx
	 mov 	dh,atrib2
	 mov 	atrib2,ah
	 mov 	ah,dh
	 pop 	dx
	 lodsb};
aa1: asm {
	 test al,al
	 jz 	fin2
	 stosw
	 loop bucle3};
fin2:
#ifdef FARPOINTERS
asm pop ds;
#endif
asm {
	pop 	dx
	pop	cx
	pop	bx
	}
}
/*-------------------------------------------------------------------------*/
void writeatrib(int x,int y,int veces,char _atrib){

asm {
	push	dx
	push	cx
	push	bx
	};
__VPTR2(x,y);
asm {
	mov 	di,ax
	mov 	es,dx
	xor 	ah,ah
	mov 	al,_atrib
	mov 	cx,veces
	inc 	di
	cld
	 }
aa00:	asm {
	stosb
	inc 	di
	loop 	aa00
	pop	bx
	pop	cx
	pop	dx
	 }
}
/*-------------------------------------------------------------------------*/
void campana(void){
asm {
	push	dx
	mov 	ah,2
	mov 	dl,7
	int 	0x21
	pop	dx};
}
/*-------------------------------------------------------------------------*/
void writeinit(void)  {
asm {
	  push 	ds
	  push	bx
	  push	cx
	  push	dx
	  xor		bl,bl
	  mov		ax,0x1003
	  int		0x10
	  mov    ah,0x0f
	  int    0x10
	  mov    oldmodo,al
	  cmp    al,8
	  jl     nomodoespecial
	  int    11h
	  mov    cl,4
	  shr    ax,cl
	  and    ax,3
	  mov    bl,7
	  cmp    ax,3
	  jnz    noamonocromo
	  mov    bl,3}
noamonocromo:asm {
	  mov    ah,0
	  mov    al,bl
	  int    10h}
nomodoespecial:asm {
	  mov    cscreen,0xB800  //segmento de color
	  mov    adaptador,1
	  xor 	bl,bl
	  mov		wx,bl
	  mov		wy,bl
	  mov 	ah,0x0f
	  int 	0x10
	  mov    actmodo,al
	  cmp 	al,7
	  jnz 	color
	  mov 	adaptador,0
	  mov 	cscreen,0xB000 //segmento de monocromo
	  }
color: asm {
	 mov 	ax,adaptador
	 mov  bl,negritamono
	 or 	ax,ax
	 jz 	ennegritamono
	 mov  bl,negritacolor}
ennegritamono:asm {
	 mov  atr2,bl
	 pop	dx
	 pop	cx
	 pop	bx
	 pop 	ds
	}
}
/*-------------------------------------------------------------------------*/
void writefin(void) {
asm {
	push	bx
	push	cx
	push	dx
	mov 	al,oldmodo
	xor 	ah,ah
	int 	0x10
	cmp 	oldmodo,7
	jz 	modomono
	mov 	ch,6
	mov 	cl,7
	jmp 	setcursor
  };
modomono:asm {
	mov 	ch,11
	mov 	cl,12
  };
setcursor:asm {
	mov 	ah,0x01
	int 	0x10
	pop	dx
	pop	cx
	pop	bx
  };
};
/*-------------------------------------------------------------------------*/
void centrar(int linea, int condicion,char *frase)  {
int anchostr;

asm   {
  push es
  push di
  push si}
#ifdef NEARPOINTERS
asm  {
  mov  di,frase
  mov  ax,ds
  mov  es,ax
  }
#else
asm  les di,frase;
#endif
asm  xor si,si
caden:asm {
  mov  al,es:[di]
  or   al,al
  jz   fincaden
  inc  si
  inc  di
  jmp  short  caden
  }
fincaden:asm {
  mov  ax,si
  pop  si
  pop  di
  pop  es
  }
anchostr=_AX;
writestr((ancho-anchostr+1) /2,linea,condicion,frase);   }
/*-------------------------------------------------------------------------*/
void right(int linea,int condicion,char *frase)  {
int anchostr;

asm   {
  push es
  push di
  push si}
#ifdef NEARPOINTERS
asm  {
  mov  di,frase
  mov  ax,ds
  mov  es,ax
  }
#else
asm  les di,frase;
#endif
asm  xor si,si
cadenn:asm {
  mov  al,es:[di]
  or   al,al
  jz   fincadenn
  inc  si
  inc  di
  jmp  short  cadenn
  }
fincadenn:asm {
  mov  ax,si
  pop  si
  pop  di
  pop  es
  }
anchostr=_AX;
writestr(linea,ancho-anchostr+1,condicion,frase);   }
/*-------------------------------------------------------------------------*/
static void pascal near __VPTR2(int x,int y) {
asm {
	xor 	ax,ax
	mov 	al,wx
	dec 	ax
	mov 	dx,x
	dec 	dx
	add 	ax,dx
	shl 	ax,1
	push  ax
	xor	ax,ax
	mov 	al,wy
	dec 	ax
	mov 	dx,y
	dec 	dx
	add 	ax,dx
	mov 	dx,160
	mul 	dx
	pop 	dx
	add 	ax,dx
	mov 	dx,cscreen
  };
};
/*-------------------------------------------------------------------------*/
void __setcursortype (int tipo) {
asm {
	push 	bx
	push	cx
	push	dx
	mov 	ah,0x0f
	int 	0x10
	mov 	bx,tipo
	cmp 	bx,_NOCURSOR
	je 	ponnocursor
	cmp 	bx,_NORMALCURSOR
	je 	ponnormalcursor
	cmp 	bx,_SOLIDCURSOR
	je 	ponsolidcursor
	jmp 	salsetcursortype
	};
ponnocursor:asm {
	mov 	ch,0x20
	jmp 	setupcursor
	};
ponnormalcursor:asm {
	cmp 	al,7
	je	enmonocromo
	mov 	ch,6
	mov	cl,7
	jmp	setupcursor
	};
enmonocromo:asm {
	mov	ch,11
	mov 	cl,12
	jmp	setupcursor};
ponsolidcursor:asm {
	cmp 	al,7
   mov 	ch,0
	je 	enmonocromo2
	mov 	cl,7
	jmp	setupcursor};
enmonocromo2:asm 	mov 	cl,12
setupcursor:asm {
	mov 	ah,0x01
	int 	0x10};
salsetcursortype:asm {
	pop	dx
	pop	cx
	pop	bx
	}
};
/*-------------------------------------------------------------------------*/
void _gotoxy(unsigned char x,unsigned char y) {
asm {
	push	dx
	push	bx
	mov 	ah,0x0f
	int 	0x10
	mov 	ah,0x02
	mov 	dh,y
	mov	dl,x
	dec 	dh
	dec	dl
	add 	dh,wy
	add	dl,wx
	dec	dh
	dec	dl
	int 	0x10
	pop	bx
	pop	dx
	};
};
/*-------------------------------------------------------------------------*/
#pragma warn -rvl

int _wherex(void) {
asm {
	push	dx
	push	bx
	mov	ah,0x0f
	int	0x10
	mov	ah,0x03
	int	0x10
	dec	dl
	add	dl,wx
	dec	dl
	xor 	ah,ah
	mov	al,dl
	pop	bx
	pop	dx
	};
};
/*-------------------------------------------------------------------------*/
int _wherey (void) {
asm {
	push	bx
	push	dx
	mov	ah,0x0f
	int	0x10
	mov	ah,0x03
	int	0x10
	dec	dh
	add	dh,wx
	dec	dh
	xor 	ah,ah
	mov	al,dh
	pop	dx
	pop	bx
	};
};

#pragma warn +rvl
/*-------------------------------------------------------------------------*/
int hacerven(struct ventann *pventana,int colorfff){
//si hay error al asignar memoria devuelve -1
//si no hay error devuelve 0
int _xx,_yy,tipoborder,xxx,yyy;
char despx,despy,flags;

xxx=pventana->xx;
yyy=pventana->yy;
tipoborder=pventana->tipo_border;
if (tipoborder==NOBORDER) {
  flags=0;
  despx=0;
  despy=0;
  }
if (tipoborder==BORDER) {
  flags=1;
  despx=1;
  despy=1;
  }
if (tipoborder==SOMBRA) {
  flags=2;
  despx=3;
  despy=2;
  }
ventana(pventana->x,pventana->y,xxx,yyy);
if (pventana->guarda==siguarda) {
  if (pventana->memptr==NULL) 
  //si ->memptr es NULL asigna memoria,si no no la asigna ya que significa que ya ha sido asignada
	 if ((pventana->memptr=malloc(2*(ancho+despx)*
	 (alto+despy)+16))==NULL) return -1;
  _xx=xxx+despx;
  _yy=yyy+despy;
  getventan(pventana->x,pventana->y,_xx,_yy,pventana->memptr);
  };
hacerfondo(colorfff);

if (flags) hacermarco(_colorf2[_MARCO][adaptador]);
if (flags==2) hacersombra(_colorf2[_MARCO][adaptador]);
ultimoatributo=atributo;
if (pventana->titulo[0]!='\x0' && flags!=0)
	//s¢lo las ventanas con BORDER o SOMBRA pueden tener t¡tulo
	{
	atributo=_colorf2[_TITULO][adaptador];
	centrar(1,0,pventana->titulo);
	}
atributo=colorfff;
cambiarven(pventana);
return 0;
}
/*-------------------------------------------------------------------------*/
void cerrarven(struct ventann *pventana){

if (pventana->guarda==siguarda && pventana->memptr!=NULL) {
 putventan(pventana->x,pventana->y,pventana->memptr);
 free(pventana->memptr);
 pventana->memptr=NULL;
};
cambiarven(ULTIMAVENTANASEL);
atributo=ultimoatributo;
}
/*---------------------------------------------------------------------*/
void ventana(int x,int y,int x1,int y1) {

wx=x;
wy=y;
wx1=x1;
wy1=y1;
ancho=(wx1-wx)+1;
alto=(wy1-wy)+1;
cscreenA =((wx-1) << 1) + 160*(wy-1);
}
/*-------------------------------------------------------------------------*/
void cambiarven(struct ventann *pventana)
{
  ULTIMAVENTANASEL=VENTANASEL;
  ventana(pventana->x,pventana->y,pventana->xx,pventana->yy);
  VENTANASEL=pventana;
}
/*----------------------------------------------------------------------*/
void cambiatitulo(char *titulo,char iluminacion) {
//cambia el t¡tulo de la ventana actual
//si iluminacion=1 , t¡tulo resaltado,si no t¡tulo con color del marco
unsigned char atrisal=atributo;

atributo=(iluminacion==1) ? _colorf2[_TITULO2][adaptador] :
_colorf2[_TITULO][adaptador];
hacermarco(_colorf2[_MARCO][adaptador]);
centrar(1,0,titulo);
atributo=atrisal;
}
/*----------------------------------------------------------------------*/
void hacermarco(int at) {
unsigned char ats=atributo;
register contador2;      

atributo=at;
writechar (1,1,1,marco);
writechar (2,1,(ancho-1),marco5);
writechar ((1+ancho),1,1,marco2);
for (contador2=1;(contador2-1)<alto;contador2++)
	{
	writechar (1,(contador2+1),1,marco6);
	writechar ((ancho+1),(contador2+1),1,marco6);
	}
writechar (1,(alto+1),1,marco3);
writechar (2,(alto+1),(ancho-1),marco5);
writechar ((1+ancho),(alto+1),1,marco4);
atributo=ats;
}

/*-------------------------------------------------------------------------*/
void hacerfondo(unsigned char colorff){
unsigned char oldatri=atributo;

atributo=colorff;
fillwin(' ');
atributo=oldatri;
}
/*-------------------------------------------------------------------*/
void hacersombra(int t){
register contador2,ats=atributo;

atributo=t;
writechar (2,(alto+2),ancho+2,ventana2);
for (contador2=2;(contador2-2)<alto;contador2++)
  writechar (ancho+2,contador2,2,ventana2);
atributo=ats;
}
/*--------------------------------------------------------------------*/
void seleccionaventana(struct ventann *a_vent,int atrib) {
register atrs=atributo;

cambiaratrmarco(_colorf2[_MARCO][adaptador]);
cambiarven(a_vent);
cambiaratrmarco(atrib);
atributo=atrs;
}
/*-------------------------------------------------------------------------*/
struct ventann *ventana_actual(void) {
return VENTANASEL;
}
/*-------------------------------------------------------------------------*/
void ajventana(struct ventann *pventana,int longi) { //ajusta el ancho de la ventana a longi y la centra
char *pun;
int sum; //para saber el factor de suma en ventanas con borders,etc
int lonmax; //la longitud de lo que ocupa m s(longi o la longitud del t¡tulo de la ventana)

if (pventana->tipo_border==BORDER || pventana->tipo_border==SOMBRA) sum=2;
if (pventana->tipo_border==NOBORDER) sum=0;
pun=pventana->titulo;
asm {
  push es
  push si
  push di}
#ifdef NEARPOINTERS
asm {
  mov  di,pun
  mov  ax,ds
  mov  es,ax
  }
#else
asm les di,pun;
#endif
asm xor si,si;
cad6:asm {
  mov  al,es:[di]
  cmp  al,0
  jz	 fincad6
  inc  si
  inc  di
  }
fincad6:asm {
  mov  di,longi
  mov  ax,di
  cmp  di,si
  jg   dimayorsi2
  mov  ax,si
  }
dimayorsi2:asm {
  pop	 di
  pop  si
  pop  es
  };
lonmax=_AX;
pventana->x=(80-lonmax+1) /2;
pventana->xx=pventana->x+lonmax+sum;
}
/*-------------------------------------------------------------------------*/
void guardaven() {
cscreen2=cscreen;
cscreenA2=cscreenA;
alto2=alto;
ancho2=ancho;
atrsalva=atributo;}
/*-------------------------------------------------------------------------*/
void restauraven(){
cscreen=cscreen2;
cscreenA=cscreenA2;
alto=alto2;
ancho=ancho2;
atributo=atrsalva;
}
/*----------------------------------------------------------------------*/
void cambiaratrmarco(int aa){
register contador2;

writeatrib (1,1,1,aa);
writeatrib (2,1,(ancho-1),aa);
writeatrib ((1+ancho),1,1,aa);
for (contador2=1;(contador2-1)<alto;contador2++)
	{
	writeatrib (1,(contador2+1),1,aa);
	writeatrib ((ancho+1),(contador2+1),1,aa);
	}
writeatrib (1,(alto+1),1,aa);
writeatrib (2,(alto+1),(ancho-1),aa);
writeatrib ((1+ancho),(alto+1),1,aa);
}
//-----------------------------------------------------------------------
void pon_mensaje(char *cabecera,char *mensaje,int cond) {
//si cond=1 suena campana,si =0 no suena
int masancho;

asm  {
  push   si
  push   di
  push   es}
#ifdef NEARPOINTERS
asm  {
  mov		di,cabecera
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  les		di,cabecera;
#endif
asm xor si,si;
cad3:asm {
  mov    al,es:[di]
  cmp		al,0
  jz		fincad3
  inc		si
  inc		di
  jmp		short	cad3
  }
fincad3:asm push   si;
#ifdef  NEARPOINTERS
asm  {
  mov		di,mensaje
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  les		di,mensaje;
#endif
asm xor si,si;
cad4:asm {
  mov    al,es:[di]
  cmp		al,0
  jz		fincad4
  inc		si
  inc		di
  jmp		short	cad4
  }
fincad4:asm {
  pop    di
  cmp		di,si
  mov		ax,di
  jg		dimayorsi2
  mov		ax,si}
dimayorsi2:asm {
  pop		es
  pop		di
  pop		si
  }
masancho=_AX+3;
ajventana(&ventanas_privadas[0],masancho);
hacerven(&ventanas_privadas[0],_colorf2[_LOGOTI][adaptador]);
cambiatitulo(cabecera,1);
writestr(2,2,1,mensaje);
centrar(4,0,"Pulse tecla");
if (cond==1) campana();
tecla();
cerrarven(&ventanas_privadas[0]);
ventanas_privadas[0].memptr=&buffer_ventanas_privadas;
}
/*----------------------------------------------------------------------*/

unsigned int tecsalir[]={ESC,0};
char siconfirmacion(char *cabecera,char *mensaje) {
int masancho,tec;
char buffer[2];
int longi1,longi2;


longi1=strlen(cabecera);
longi2=strlen(mensaje);
if (longi1>longi2) masancho=longi1+3;
else masancho=longi2+3;

/*
asm  {
  push   si
  push   di
  push   es}
#ifdef NEARPOINTERS
asm  {
  mov		di,cabecera
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  les		di,cabecera;
#endif
asm  xor si,si
cad1:asm {
  mov    al,es:[di]
  cmp		al,0
  jz		fincad1
  inc		si
  inc		di
  jmp		short	cad1
  }
fincad1:asm push   si;
#ifdef  NEARPOINTERS
asm  {
  mov		di,mensaje
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  les		di,mensaje;
#endif
asm  xor si,si
cad2:asm {
  mov    al,es:[di]
  cmp		al,0
  jz		fincad2
  inc		si
  inc		di
  jmp		short	cad2
  }
fincad2:asm {
  pop    di
  cmp		di,si
  mov		ax,di
  jg		dimayorsi
  mov		ax,si}
dimayorsi:asm {
  pop		es
  pop		di
  pop		si
  }
masancho=_AX+3;
*/
ajventana(&ventanas_privadas[1],masancho);
hacerven(&ventanas_privadas[1],_colorf2[_LOGOTI][adaptador]);
centrar(1,0,cabecera);
writestr(2,2,1,mensaje);
__setcursortype(_NORMALCURSOR);
buffer[0]='N';
buffer[1]=0;
do {
  writechar(2+masancho,2,1,' ');
  tec=dial(2+masancho,2,'N','S',1,(char *)&buffer,coge_atributo(),TRUE,TRUE,
  (unsigned int *)&tecsalir);
  }while ((buffer[0]!='S' || buffer[0]!='N') && tec!=ESC);
cerrarven(&ventanas_privadas[1]);
ventanas_privadas[1].memptr=&buffer_ventanas_privadas;
__setcursortype(_NOCURSOR);
if (tec==ESC) return 'N';
return buffer[0];
}
/*----------------------------------------------------------------------*/
void getventan(int x,int y,int xx,int yy,char *buffer) {
//la longitud del buffer es ((xx-x)*(yy-y)*2)+2

asm {
  push ds
  push si
  push di
  push bx
  push cx
  push dx
  };
#ifdef NEARPOINTERS
asm {
  mov ax,ds
  mov es,ax
  }
#else
asm les di,buffer;
#endif
asm {
  xor  si,si
  mov  dx,x
  or   dx,dx
  jz   fin
  dec  dx
  mov  cx,xx
  cmp  cx,dx
  jle  fin //salir si cx <=dx
  sub  cx,dx //en cx las columnas
  cmp  cx,80
  jg   fin //si columnas > 80 salir
  mov  si,dx
  shl  si,1
  mov  es:[di],cl //mov  cs:@@columnas,cl
  inc  di
  mov  ax,y
  or   ax,ax
  jz   fin
  dec  ax
  mov  dx,yy
  cmp  dx,ax
  jle  fin  //salir si dx<=ax
  sub  dx,ax //en dx las filas
  cmp  dx,25
  jg   fin //si filas>25 salir
  mov  es:[di],dl //mov  cs:@@filas,dl
  inc  di
  mov  bx,dx //guardar dx en bx
  mov  dl,160 //160 columnas por fila(80 caracteres de 2 bytes por caracter(byte,atributo))
  mul  dl //ax=al*dl
  add  si,ax
  mov  dx,bx //recuperar el valor de dx
  cld
  mov  ds,cscreen
  }
bucle:asm {
  mov  ax,cx //guardar en ax las columnas
  mov  bx,si //guardar en bx el offset a la primera columna a guardar de la fila actual
  rep  movsw
  mov  cx,ax //recuperar el valor de cx
  add  bx,160 //ir a la fila siguiente
  mov  si,bx
  dec  dx
  jnz  bucle}
fin:asm {
  pop dx
  pop cx
  pop bx
  pop di
  pop si
  pop ds}
}
/*----------------------------------------------------------------------*/
void putventan(int x,int y,char *buffer) {
asm {
  push ds
  push si
  push di
  push bx
  push cx
  push dx
  mov  es,cscreen
  }
#ifdef NEARPOINTERS
asm mov si,buffer;
#else
asm lds si,buffer;
#endif
asm {
  xor  ch,ch
  mov  cl,ds:[si] //mov  cl,cs:@@columnas ,en cx las columnas
  inc  si
  mov  di,x
  or   di,di
  jz   fin2
  dec  di
  shl  di,1
  mov  ax,y
  or   ax,ax
  jz   fin2
  dec  ax
  xor  dx,dx
  mov  dl,ds:[si]  //mov  dl,cs:@@filas ,en dx las filas
  inc  si
  mov  bx,dx //guardar dx en bx
  mov  dl,160 //160 columnas por fila(80 caracteres de 2 bytes por caracter(byte,atributo))
  mul  dl //ax=al*dl
  add  di,ax
  mov  dx,bx //recuperar el valor de dx
  cld}
bucle2:asm {
  mov  ax,cx //guardar en ax las columnas
  mov  bx,di //guardar en bx el offset a la primera columna a guardar de la fila actual
  rep  movsw
  mov  cx,ax //recuperar el valor de cx
  add  bx,160 //ir a la fila siguiente
  mov  di,bx
  dec  dx
  jnz  bucle2}
fin2:asm {
  pop dx
  pop cx
  pop bx
  pop di
  pop si
  pop ds}

}
/*------------------------------------------------------------------*/               
int tecla(void)
{

nohaytecla:asm {
  mov 	ah,1h //buscar estado del teclado
  int 	16h
  jz  	nohaytecla //si no hay tecla pulsada
  mov 	ah,0 //funci¢n 0,lee un car cter del teclado
  //devuelve en ah el c¢digo de la tecla
  //y en al el caracter ASCII
  int 	16h
  mov 	bufteclado,ax
  push 	ax
  mov    ah,2 //devolver el estado del teclado en al
  int		16h
  mov    byte ptr estadotecla,al
  pop    ax
  test   ax,0xff
  jz		teclaascii0
  and    ax,0xff}
teclaascii0:return _AX;
}
/*-------------------------------------------------------------------------*/
static void near ponstring(unsigned char x,unsigned char y,char *cad,
unsigned int longi)  {
//para dial
writechar(x,y,longi,' ');
writestr(x,y,0,cad);
}
/*-------------------------------------------------------------------------*/
static void near pascal copiastr(char *dest,char *source) {

asm  {
  push   si
  push   di
  push   es}
#ifdef FARPOINTERS
asm  push   ds;
#endif
#ifdef NEARPOINTERS
asm  {
  mov		di,dest
  mov    si,source
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  {
  les		di,dest
  lds    si,source
  }
#endif
_cade:asm {
  mov    al,[si]
  mov    es:[di],al
  cmp		al,0
  jz		_fincade
  inc		di
  inc    si
  jmp		short	_cade
  }
_fincade:;
#ifdef FARPOINTERS
asm   pop ds;
#endif
asm {
  pop    es
  pop    di
  pop    si}
}
/*-------------------------------------------------------------------------*/
static void near pascal copiamem(void *dest,void *source,int n) {


if (n==0) //si no se copian bytes
  return;
asm  {
  push   si
  push   di
  push   cx
  push   es}
#ifdef FARPOINTERS
asm  push   ds;
#endif
asm     mov  cx,n;
#ifdef NEARPOINTERS
asm  {
  mov		di,dest
  mov    si,source
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  {
  les		di,dest
  lds    si,source
  }
#endif
asm  {
  cld
  rep movsb}
#ifdef FARPOINTERS
asm   pop ds;
#endif
asm {
  pop es
  pop cx
  pop di
  pop si
  }
}
/*-------------------------------------------------------------------------*/
int dial(unsigned char x,unsigned char y,unsigned char minimo,
unsigned char maximo,unsigned int longitud,char *salida,
unsigned char atrileer,BOOLEAN simayu,BOOLEAN sicursor,
unsigned int *teclasalir) 
//teclasalir:puntero acabado con 0000h a las teclas con las cuales saldr 
//devuelve la tecla de salida
{
unsigned int tec,*te,pun=0,lon;
char atrisal=coge_atributo(),siencontrada,*psalida=salida,buffer[81];
unsigned char xx=x,yy=y;

pon_atributo(atrileer);
writechar(x,y,longitud,' ');
asm  {
  push   si
  push   di
  push   es}
#ifdef NEARPOINTERS
asm  {
  mov		di,salida
  mov    si,salida
  mov 	ax,ds
  mov		es,ax
  }
#else
asm  {
  les		di,salida
  les    si,salida
  }
#endif
cade:asm {
  mov    al,es:[di]
  cmp		al,0
  jz		fincade
  inc		di
  jmp		short	cade
  }
fincade:asm{
  sub    di,si
  mov    ax,di
  pop    es
  pop    di
  pop    si}
lon=_AX;
if ((unsigned int)_AX>longitud) {
  *(salida + longitud)=0;
  pun=longitud;
  lon=longitud;
  }
  else {
  pun=(unsigned int)_AX;
  xx+=pun;
  }
writestr(x,y,0,salida);
if (sicursor==TRUE) __setcursortype(_NORMALCURSOR);
psalida+=pun;
_gotoxy(xx,yy);
 do {
 tec=tecla();
 if (tec<255 && simayu==TRUE) tec=amayus(tec);
 siencontrada=0;
 if (*teclasalir!=0) {
	for (te=teclasalir;*te!=0;++te)
	  if (*te==tec) {
		 siencontrada=1;
		 break;
		 }
	}
 if(siencontrada==0)  {
	switch(tec)  {
	  case BS:
	  if (pun>0)  {
		 --lon;
		 writechar(xx-1,yy,1,' ');
		 if (pun<longitud) 
			copiastr(psalida-1,psalida);
			else
			*(psalida-1)=0;
		 --psalida;
		 --xx;
		 --pun;
		 _gotoxy(xx,yy);
		 ponstring(x,y,salida,longitud);
		 };
	  break;
	  case LEFT:if (pun>0) {
					--psalida; 
					--xx;
					--pun;
					_gotoxy(xx,yy);};
					break;
	  case RIGHT:if (pun<longitud) {
					 if (*psalida==0) {
						*psalida=' ';
						*(psalida+1)=0;
						lon++;
						}
					 ++psalida; 
					 ++xx;
					 ++pun;
					 _gotoxy(xx,yy);};
					 break;
		case HOME:xx=x;
					 psalida=salida;
					 pun=0;
					 _gotoxy(xx,yy);
					 break;
		case END:xx=x+lon;
					psalida=salida+lon;
					pun=lon;
					_gotoxy(xx,yy);
					break;
		case DEL:if (pun<longitud && lon>0) {
					copiastr(psalida,psalida+1);
					ponstring(x,y,salida,longitud);
					--lon;
					}
					break;
		default:if (pun<longitud && tec>=minimo && tec<=maximo) {
					 if (lon<longitud) ++lon; //s¢lo se incrementa lon si no se ha llegado al l¡mite de caracteres
					 if (pun<longitud) {
						copiamem(&buffer,psalida,longitud-pun-1);
						*psalida=(unsigned char) tec;
						copiamem(psalida+1,&buffer,longitud-pun-1);}
					 ++psalida;
					 ++xx;
					 ++pun;
					 _gotoxy(xx,yy);
					 ponstring(x,y,salida,longitud);
					 }
				  break;
	  }
	}
 }while (tec!=ENTER && siencontrada==0);
pon_atributo(atrisal);
if (sicursor==TRUE) __setcursortype(_NOCURSOR);
return(tec);
}