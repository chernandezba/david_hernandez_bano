// #pragma inline

#include <pcescrib.h>
#include <pcventan.h>
#include <conio.h>
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
#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif
#ifndef __PAS__
#define _CType _Cdecl
#else
#define _CType pascal
#endif
#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2
/*-------------------------------------------------------------------------*/
const int oldnegritacolor=MAGENTA + (CYAN << 4);
const int oldnegritamono=WHITE + (BLACK << 4);
/*-------------------------------------------------------------------------*/
unsigned char atributo,atre,ancho,oldmodo,actmodo,atr2,alto,wx,wy,wx1,wy1,
ultimoatributo;
unsigned int sipon;
int cscreenA,cscreen; //,adaptador;
/*-------------------------------------------------------------------------*/
static void pascal near __VPTR2(int x,int y); /*Devuelve el resultado en AX:offset DX:segmento*/
size_t  _CType strlen	(const char *__s);
/*-------------------------------------------------------------------------*/

extern struct SCREEN  {
  struct text_info pinfo;
  unsigned salto,vseg;
  } _video;
/*-------------------------------------------------------------------------*/
void old_fillwin(unsigned char ch3) {
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
void old_writechar(int x,int y,int contador,char caracter){
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
void old_writestr(int x,int y,int sinegrita,char *frase)  {
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
void old_writeatrib(int x,int y,int veces,char ch3){

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
	mov 	al,ch3
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
void old_campana(void){
asm {
	push	dx
	mov 	ah,2
	mov 	dl,7
	int 	0x21
	pop	dx};
}
/*-------------------------------------------------------------------------*/
void old_writeinit(void)  {
asm {
	  push 	ds
	  push	bx
	  push	cx
	  push	dx
	  xor		bl,bl
	  mov		ax,0x1003
	  int		0x10
	  mov 	ax,word ptr _video.vseg
	  mov		cscreen,ax
	  cmp		byte ptr _video.pinfo.screenheight,24
	  jz		c25
	  mov		ax,3
	  int		0x10
};
c25:asm{
	  xor 	bl,bl
	  mov		wx,bl
	  mov		wy,bl
	  mov 	ah,0x0f
	  int 	0x10
	  push 	ax
	  cmp 	al,7
	  jnz 	color
	  mov 	adaptador,0
	  mov 	cscreen,0xB000
	  mov		al,7
	  mov		actmodo,al
	  jmp 	final
	  }
color: asm{
	 mov 	adaptador,1
	 mov	al,3
	 mov 	actmodo,al
	 }
final: asm {
	 mov 	ax,adaptador
	 or 	ax,ax
	 jz 	mono}
atr2=(unsigned char) negritacolor;
goto fin2;
mono:atr2=(unsigned char) negritamono;
fin2:asm {
	pop 	ax
	mov 	oldmodo,al
	cmp 	al,7
	je 	fin3
	cmp 	al,2
	je 	fin3
	cmp 	al,3
	je 	fin3
	xor	ah,ah
	mov 	al,3
	mov 	actmodo,al
	int 	10h};
fin3:asm {
	pop	dx
	pop	cx
	pop	bx
	pop 	ds
	}
}
/*-------------------------------------------------------------------------*/
void old_writefin(void) {
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
void old_centrar(int linea, int condicion,char *frase)  {
  writestr((ancho-strlen(frase)+1) /2,linea,condicion,frase);   }
/*-------------------------------------------------------------------------*/
void old_right(int linea,int condicion,char *frase)  {
  writestr(linea,ancho-strlen(frase)+1,condicion,frase);   }
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
	push ax
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
void old__setcursortype (int tipo) {
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
void old_gotoxy(unsigned char x,unsigned char y) {
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

int old_wherex(void) {
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
int old_wherey (void) {
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
/*------------------------------------------------------------------*/
int writemargen (int x1,int y1,int condi,char *fr)
{char buffer[80];
int longi;

longi=strlen(fr);


if (x1>ancho || x1==ancho) return 0;
if (y1>alto) return 0;
if (x1<1) return 0;
if (y1<1) return 0;
if (x1+longi!=ancho) return 0;

writestr(x1,y1,condi,fr);
noigual:

strcpy(buffer,fr);

buffer[ancho-x1]='\x0';

writestr(x1,y1,condi,buffer);
fin:return (((x1+longi)!=ancho) ? (longi) : (ancho-x1-1));
}
/*-------------------------------------------------------------------------*/
void old_guardaven() {
cscreen2=cscreen;
cscreenA2=cscreenA;
alto2=alto;
ancho2=ancho;
atrsalva=atributo;}
/*-------------------------------------------------------------------------*/
void old_restauraven(){
cscreen=cscreen2;
cscreenA=cscreenA2;
alto=alto2;
ancho=ancho2;
atributo=atrsalva;}
/*----------------------------------------------------------------------*/
void old_cambiaratrmarco(int aa){
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
