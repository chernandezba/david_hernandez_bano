.MODEL TINY
.DATA
PUBLIC principal,sacaarg,dosh,dosl,inicio,escribir
EXTRN __version:WORD
EXTRN __argc:WORD
EXTRN __argv:WORD
PUBLIC numarg ,puntarg,logo,ayuda,adver,adve2,adve3
PUBLIC unidad,opcion,nombrefich,seclogini,sectres,discoofich,pista,cara,sector
PUBLIC temp,temp2,val_error,sierrorval
cr equ 13
lf equ 10
logo  db 'SECTOR V.2.0 (c) BIT SOFT 27/5/1994',cr,lf,'$'
ayuda db 'SECTOR [UNIDAD] [NOMBRE FICHERO] [SECTOR LOGINI] [SECTRES] [OP1]',cr,lf
		db '                                  [PISTA] [CARA] [SECTOR] [SECTRES] [OP1]',cr,lf
		db 'LA OPCION0=0 SOLO SE PUEDE UTILIZAR CON UNIDADES REMOVIBLES , NO DISCOS DUROS',cr,lf
		db 'OPCION1=0 DE SECTORES A FICHERO',cr,lf
		db 'OPCION1=1 DE FICHERO A SECTORES',cr,lf,'$'
adver db 'LA UNIDAD ESPECIFICADA ES NO REMOVIBLE',cr,lf,'$'
adve2 db 'LA UNIDAD ESPECIFICADA ES REMOTA',cr,lf,'$'
adve3 db 'VERSION DEL MS-DOS INCOMPATIBLE CON OPCION0=0',cr,lf,'$'
adve4 db 'INTRODUZCA PARAMETROS NUMERICOS',cr,lf,'$'
unidad db 0 ; 0:A 1:B 2:C ...
opcion db 0 ; 0 bios 1 dos
numarg db 0 ; igual que __argc de turboc
puntarg dw 0;  igual que __argv de turboc el primer argumento es el nombre del programa
dosh db 0; igual que __osmajor
dosl db 0; igual que __osminor
ref1 equ $
nombrefich db 81 dup (?);nombre del fichero
longnomfich equ $-ref1
seclogini dw 0
sectres dw 0
discoofich db 0; si igual a 0 de sector 0 a fichero , si igual a 1 de fichero a sector
pista db 0
cara db 0
sector db 0
temp dw 0
temp2 dw 0
val_error db 0
.CODE
inicio	proc
			mov dx,OFFSET(logo)
			call escribir
			jmp conti
ponayu:	mov dx,OFFSET(ayuda)
			call escribir
			ret
conti:	mov ah,numarg
			cmp ah,5
			jl ponayu
			pushf
			mov ax,1
			call sacaarg;sale con si=puntero argumento , se entra con al=numarg
			mov bl,[si]
			xor bh,bh
			cmp bl,'a'
			jl mayu
			sub bl,'a'
			jmp conti2
mayu:		sub bl,'A'
conti2:  mov unidad,bl
			mov ah,1
			mov opcion,ah
			mov al,9
			mov ah,44h
			xor bx,bx
			mov bl,unidad
			inc bl
			int 21h
			test dx,4096
			jz conti4
			popf
			mov dx,offset(adve2)
			call escribir
			ret
conti4:	popf
			jnz biosm ; si opcion de leer disco = bios saltar a dosm
			mov ah,dosh
			cmp ah,2
			jg doscorrecto1
			jmp error
doscorrecto1:mov ah,dosl
			cmp ah,0
			jg doscorrecto
error:	mov dx,OFFSET(adve3)
			call escribir
			ret
doscorrecto:mov ah,44h
			mov al,8
			mov bl,unidad
			xor bh,bh
			inc bl
			int 21h
			cmp ax,0
			jz removible
			mov dx,OFFSET(adver)
			call escribir
			ret
removible:mov opcion,0
			jmp dosm
biosm:	xor ah,ah
			mov opcion,0
dosm:	   cld
			mov di,offset(nombrefich)
			mov al,2
			call sacaarg
			mov cx,longnomfich
bucle1:	mov ax,[si]
			mov [di],ax
			dec cx
			jnz short bucle1
			mov ah,opcion
			cmp ah,0
			jz variablesbios
			mov al,3
			call sacaarg
			call val;en si puntero de cadena ; retorna en ax n£mero
			call sierrorval
			mov seclogini,ax
			mov al,4
			call sacaarg
			call val
			call sierrorval
			mov sectres,ax
			mov al,5
			call sacaarg
			mov al,[si]
			cmp al,'1'
			jg  ponayu
			cmp al,'0'
			jl ponayu
			sub al,'0'
			mov discoofich,al
			jmp contprog
variablesbios:mov al,3
			call sacaarg
			call val
			call sierrorval
			mov pista,al
			mov al,4
			call sacaarg
			call val
			call sierrorval
			mov cara,al
			mov al,5
			call sacaarg
			call val
			call sierrorval
			mov sector,al
			mov al,6
			call sacaarg
			call val
			call sierrorval
			mov sectres,ax
contprog:ret
inicio	endp

escribir	proc ; dx:OFFSET texto , finalizando con '$'
		push ax
		mov ah,9h
		int 21h
		pop ax
		ret
escribir endp
val 	proc ; en si puntero de cadena , retorna en ax n£mero en caso de error val_error=1
		push di
		push cx
		push si
		push bx
		xor cx,cx
		xor ax,ax
		mov temp,ax
		mov temp2,ax
		mov val_error,ah
		mov di,si
buc3:	mov ah,[di]
		inc di
		inc cx
		cmp ah,0
		jnz buc3
		cmp ah,'0'
		jl valerror
		cmp ah,'9'
		jg valerror
		dec cx
		mov ax,1
		mov di,cx
		cmp cx,1
		jnz contcalc
		mov ax,1
		jmp short contcalc2
contcalc:dec di
		mov cx,10
		mul cx
		dec di
		jnz contcalc
contcalc2:mov bl,[si]
		cmp bl,0
		jz final
		sub bl,'0'
		xor bh,bh
		mov temp2,ax
		mul bx
		mov bx,ax
		add ax,temp
		mov temp,ax
		mov ax,temp2
		mov bx,10
		div bx
      inc si
		jmp contcalc2
valerror:mov ah,1
			 mov val_error,1
final:mov ax,temp
		pop bx
		pop si
		pop cx
		pop di
		ret
val endp
sierrorval proc
			pushf
			push dx
			mov dh,val_error
			cmp dh,1
			jnz finproc
			add sp,6
			mov cx,offset(adve4)
			call escribir
			ret
finproc: pop dx
			popf
			ret
sierrorval endp
sacaarg proc; sale con si=puntero argumento , se entra con al=numarg
		  push bx
		  push di
		  mov bl,2
		  mul bl
		  xor ah,ah
		  mov di,puntarg
		  add di,ax
		  mov si,word ptr [di]
		  pop di
		  pop bx
		  ret
sacaarg endp
principal proc 
		mov ax,__version
		mov dosh,al
		mov dosl,ah
		mov ax,__argc
		dec ax
		mov numarg,al
		mov ax,word ptr [__argv]
		mov puntarg,ax
		jmp inicio
principal	endp
END