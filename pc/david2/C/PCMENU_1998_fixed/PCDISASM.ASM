.8086
.model Large,c
;rutinas en asm para pcdisco

public simscdex
.code
simscdex  proc C
;registros modificados AX,BX
			mov  ax,1100h
			mov  bx,0dadah
			push bx
			int  2fh
			;si esta instalado MSCDEX o LOTUS CD/networker retorna en AL FFh
			cmp  al,0ffh 
			jne  short sale
			;si esta instalado MSCDEX o LOTUS CD/networker sacar de la pila
			;un valor y devolverlo en BX
			pop  bx
			;si BX=ADADh MSCDEX esta instalado
			;si BX=DADBh LOTUS CD/networker esta instalado
sale:  	ret
;ret
simscdex endp
			
end