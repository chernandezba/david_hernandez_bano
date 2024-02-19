/*compilado en Large*/
#pragma inline /* para que pase directamente a asm*/
#include <dos.h>
#include <conio.h>

char *procesadormat[]={
"Ninguno",
"8087",
"80287",
"80387",
"Desconocido",};
char *procesador[]={
"8088",
"8086",
"NEC V20",
"NEC V30",
"80188",
"80186",
"80286",
"80386",
"80486",
"80486SX"};
unsigned sp2,si2,di2;

static int near cpu_type(void)
{
unsigned char cpu_num;

asm {
	  mov si2,si
	  mov di2,di
	  mov cpu_num,0
	  mov sp2,sp
	  PUSHF
	  PUSH   CX
	  PUSH   DX
     PUSH   DI
     PUSH   SI
     PUSH   ES
     XOR    AX,AX
     PUSH   AX
     POPF
     PUSHF
     POP    AX
     AND    AX,0xF000
     CMP    AX,0xF000
     JZ     short a286
     MOV    DL,0x06
     MOV    AX,0x7000
     PUSH   AX
     POPF
     PUSHF
     POP    AX
     AND    AX,0x7000
     JZ     short a2
     INC    DL
     CLI
     DB     0x66,0x8b,0xdc
     DB     0x66,0x83,0xe4,0xfc
     DB     0x66,0x9c
     DB     0x66,0x58
     DB     0x66,0x8b,0xc8
     DB     0x66,0x35,0,0,4,0
     DB     0x66,0x50
     DB     0x66,0x9d
     DB     0x66,0x9c
     DB     0x66,0x58
     DB     0x66,0x51
     DB     0x66,0x9d
     DB     0x66,0x33,0xc1
     DB     0x66,0xc1,0xe8,0x12
     DB     0x66,0x83,0xe0,0x01
     DB     0x66,0x8b,0xe3
     STI
     ADD    DL,AL
     JMP    short a2
    }

a286:
asm {
     MOV    DL,0x4
     MOV    AL,0xFF
     MOV    CL,0x21
     SHR    AL,CL
     JNZ    short a86
     MOV    DL,0x2
     STI
     MOV    SI,0
     MOV    CX,0xFFFF
     db     0xf3,0x26,0xac
     OR     CX,CX
     JZ     short a86
     MOV    DL,0
    }

a86:
asm {
     PUSH   CS
     POP    ES
     STD
     MOV    AL,0xFB
     MOV    CX,3
     JMP    short a3
    }

a1:
asm {
     CLI
     REP    STOSB
     CLD
     INC    DX
     STI
    }

a2:
asm {
     MOV    cpu_num,DL
     POP    ES
     POP    SI
     POP    DI
     POP    DX
     POP    CX
     POPF
     JMP    short a4
    }

a3:
asm {
     POP    DI
     ADD    DI,9
     JMP    short a1
    }

a4:
asm {
     INT    0x11
     AND    AL,2
     CMP    AL,0
     JNZ    short a5
     CMP    cpu_num,8
     JNZ    short a5
     ADD    cpu_num,1
    }

a5:
_SP=sp2;
_DI=di2;
_SI=si2;
return(cpu_num);

}

static void pascal near cogesegundo (void)
/* devuelve segundo en DH no toca registro BX*/
{
asm {
  mov ah,0x2c
  int 0x21}
}

void main (void) {
unsigned long contador;
int lugar;

contador=0;
cogesegundo();
_BL=_DH;
do {
cogesegundo();
}while(_DH==_BL);
_BL=_DH;
do {
cogesegundo();
_BH=_DH;
++contador;
}while(_BH<=_BL);
cprintf("Ha sumado : %lu\r\n",contador);
cprintf("Velocidad (IBM PC=100) : %lu\r\n",(contador*100l)/750l);
cprintf("CPU : %s\r\n",procesador[cpu_type()]);
lugar=(_8087<0) ? (-(_8087)) : (_8087);
cprintf("_8087=%d Procesador matematico : %s\r\n",
_8087,procesadormat[lugar]);
};