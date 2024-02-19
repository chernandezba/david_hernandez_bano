#pragma inline
#include "string.h"
#include "conio.h"

int cpu_type(void)
{

unsigned char cpu_num;

asm {
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
return(cpu_num);

}

void main()
{
  unsigned char cpu,name[8];

  cpu=cpu_type();

  if(cpu==0) {strcpy(name,"8088");}
  if(cpu==1) {strcpy(name,"8086");}
  if(cpu==2) {strcpy(name,"NEC V20");}
  if(cpu==3) {strcpy(name,"NEC V30");}
  if(cpu==4) {strcpy(name,"80188");}
  if(cpu==5) {strcpy(name,"80186");}
  if(cpu==6) {strcpy(name,"80286");}
  if(cpu==7) {strcpy(name,"80386");}
  if(cpu==8) {strcpy(name,"80486");}
  if(cpu==9) {strcpy(name,"80486SX");}

 cprintf("Find CPU - Written by Ivan Daunis\r\n");
 cprintf("Processor type is %s\r\n",name);

}

