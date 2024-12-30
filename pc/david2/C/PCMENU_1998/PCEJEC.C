#ifndef __COMPACT__
#error "Compilarlo en compact"
#endif
#if __BORLANDC__	< 0x452
#pragma inline //para que ya compile directo en asm
#endif
#include <process.h>
#include <pcejec.h>
#include <dir.h>
#include <stddef.h>
#include <dos.h>
//compilarlo en compact


/*------------------------------------------------------------------------*/
unsigned _stklen=512;
char *mensajes[]={
"IMPOSIBLE EJECUTAR PROGRAMA\x0A\x0D$",
"IMPOSIBLE ABRIR FICHERO PCMENU.TEM\x0A\x0D$",
"NO EXISTE ESE DIRECTORIO\x0A\x0D$",
"\x0A\x0D$",
"PULSA TECLA\x0A\x0D$",
"NO PUEDO EJECUTAR PCMENU.EXE\x0A\x0D$",
"ERROR LEYENDO PCMENU.TEM\x0A\x0D$"},*parametros2="",*sale,*para[41];
/*------------------------------------------------------------------------*/
void separaparametros(char *,char *[]);
void escanea(char *);
void borratodapant(void);
void esperatecla(void);
void escribe(char *);
void veaxy(int,int);
int cogerunidactual (void);
int cambiaunidad (unsigned char);
int cambiadir (char *);
int abrirfichero (char *);
int leerdatos(int handle,void *buffer,int numbites);
void cerrarfichero (int handle);
void copiastr (char *dest,char *source);
char *busca (char *,char);
int obtenerdir (char *);
int ejecuta(char **);
/*------------------------------------------------------------------------*/
int main(int argc,char **argv)  {
int sa;


borratodapant();
veaxy(0,0);
sa=ejecuta(argv);
if (sa==-1) escribe(mensajes[0]);
if (sa==-2) escribe(mensajes[1]);
if (sa==-3) escribe(mensajes[2]);
if (sa==-3) escribe(mensajes[6]);
if (sa!=0) {
  escribe (mensajes[4]);
  esperatecla();  }
if (spawnl(P_OVERLAY,argv[1],NULL)==-1) {
	escribe(mensajes[5]);
	return 1;
  }
return 0;
}
/*-------------------------------------------------------------------------*/
int ejecuta(char **argv)  {
int res,leer,unid;
unsigned char unidprog;
char *pun;
struct ejecutar ejecutar;
char pathori[MAXPATH+1],nombretemp[128];

pun=busca(argv[0],0);
while (*pun!='\\') pun--;
copiastr(++pun,"PCMENU.TEM");
copiastr(nombretemp,argv[0]);
obtenerdir(pathori);
unid=cogerunidactual();
if ((leer=abrirfichero(nombretemp))==-1) return(-2);
if ((leerdatos(leer,(void *)&ejecutar,
sizeof (struct ejecutar)))==-1) return -4;
cerrarfichero(leer);
separaparametros((char *)&ejecutar.parametros,para);
escanea((char *)&ejecutar.directorio);
if (ejecutar.directorio[0]>='a' && ejecutar.directorio[0]<='z')
  ejecutar.directorio[0]-=32;
unidprog=ejecutar.directorio[0]-'A';
cambiaunidad(unidprog);
if ((cambiadir((char *)busca(&ejecutar.directorio[0],'\\')))==-1) return(-3);
res=spawnv(P_WAIT,ejecutar.ejecutable,para);
cambiaunidad(unid);
cambiadir(pathori);
return(res);
}
/*-------------------------------------------------------------------------*/
char *busca(char *c,char car)  {


asm {
	  les si,c};
sitb:asm {
	  mov al,es:[si]
	  cmp al,car
	  jz sitf
	  cmp al,0
	  jz sitf
	  inc si
	  jmp sitb};
sitf:asm {
  mov word ptr sale,si;
  mov word ptr sale+2,es
  }
return(sale);
}
/*-------------------------------------------------------------------------*/
void separaparametros(char *z,char *pp[])  {
  unsigned char i,fin;
  char *situa,*t;

fin=0;
i=0;
t=z;
if (*z!='\x0')  {
  while (!fin)  {
  situa=busca(t,0);
  if (*situa=='\x0') fin=1;
  *situa='\x0';
  pp[i]=t;
  t=situa+1;
  ++i;
  }
  pp[i++]=NULL;
  }
}
/*-------------------------------------------------------------------------*/
void escanea (char *escan)  {
auto unsigned char sibarrainc;

sibarrainc=0;
asm { push si
		les si,escan};
bucw:asm {
		mov al,es:[si]
		cmp al,0
		jz finw
		cmp al,'\'
		jnz buc2};
sibarrainc=1;
buc2:asm {
		inc si
		jmp bucw};
finw:asm{
		mov al,sibarrainc
		cmp al,0
		jnz nos
		mov al,'\'
		mov es:[si],al
		inc si
		xor al,al
		mov es:[si],al};
nos:asm pop si;
}
/*-------------------------------------------------------------------------*/
void esperatecla (void)  {

asm {
	mov ah,0x0c
	mov al,0x07
	int 21h
	}
}
/*-------------------------------------------------------------------------*/
void borratodapant(void) {
asm {
  push 	bx
  push	cx
  push	dx
  xor		al,al
  mov 	ah,7
  xor		cx,cx
  mov 	dh,24
  mov 	dl,79
  mov 	bh,8
  int 	10h
  pop 	dx
  pop		cx
  pop		bx
  }
}
/*-------------------------------------------------------------------------*/
void escribe(char *strptr) /* INT 21 AH=9 DS:DX=TEXTO*/
{
asm {
	push	dx
	push  ds
	lds   dx,strptr;
	mov 	ah,9
	int 	21h
	pop	ds
	pop	dx
	};
}
/*-------------------------------------------------------------------------*/
void veaxy(int x,int y) {
asm { 
		push	dx
		push	bx
		mov 	ah,0fh
		int 	10h
		mov 	ah,02h
		mov 	dl,byte ptr x
		mov 	dh,byte ptr y
		int 	10h
		pop 	bx
		pop 	dx
		}
}
/*-------------------------------------------------------------------------*/
int cogerunidactual (void)
{
 asm {
	mov 	ah,19h
	int 	21h
	xor	ah,ah
	};
  return (_AX);
  }
/*-------------------------------------------------------------------------*/
int cambiaunidad (unsigned char queunid)
{
	asm {
		push	dx
		mov 	ah,0eh
		mov 	dl,queunid
		int 	21h
		pop	dx
		xor ah,ah};
return (_AX);
}
/*-------------------------------------------------------------------------*/
int cambiadir (char *direc) {
  asm {
		 push	dx
		 push	ds
		 lds 	dx,direc
		 mov 	ah,3bh
		 int 	21h
		 pop	ds
		 pop	dx
		 jnc noerrorc
		 };
return (-1);
noerrorc:return(0);
}
/*-------------------------------------------------------------------------*/
int obtenerdir (char *pdir) {
asm {
	  push	dx
	  push	ds
	  xor		dl,dl
	  lds 	si,pdir
	  inc    si
	  mov 	ah,47h
	  int 	21h
	  pop		ds
	  pop		dx
	  jnc 	noer
	  };
return(-1);
noer:
*pdir='\\';
return(0);
}
/*-------------------------------------------------------------------------*/
int abrirfichero (char *nombrefiche) {
  asm {
		push  dx
		push	ds
		mov 	ah,3dh
		xor	al,al
		lds 	dx,nombrefiche
		int 	21h
		pop	ds
		pop   dx
		jnc 	noerror};
return (-1);
noerror:return(_AX);
}
/*-------------------------------------------------------------------------*/
int leerdatos(int handle,void *buffer,int numbytes) {
//devuelve 0 si no hay error y -1 si lo hay
auto int ax2;
	asm {
		push	dx
		push	cx
		push	bx
		push	ds
		lds 	dx,buffer
		mov 	bx,handle
		mov 	cx,numbytes
		mov 	ah,3fh
		int 	21h
		mov   ax2,ax
		mov   ax,0
		jnc   noerrorleyendo
		mov   ax,-1};
noerrorleyendo:asm {
		pop	ds
		pop	bx
		pop	cx
		pop	dx
		mov   ax,ax2
		cmp   ax,numbytes
		mov   ax,0
		je		sehanleidotodosbytes
		mov   ax,-1
		};
sehanleidotodosbytes:return _AX;
}
/*-------------------------------------------------------------------------*/
void cerrarfichero (int handle) {
asm {
		push	bx
		mov 	ah,3eh
		mov 	bx,handle
		int 	21h
		pop	bx
		};
}
/*-------------------------------------------------------------------------*/
void copiastr (char *dest,char *source) {
asm {push ds
	  lds si,dest
	  les di,source};
buclep:asm {
	  mov al,es:[di]
	  mov [si],al
	  or al,al
	  jz finp
	  inc si
	  inc di
	  jmp buclep;
	  };
finp:asm pop ds;
};