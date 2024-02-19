	.MODEL	LARGE,C

  .CODE

  LOCAL	@@

  PUBLIC handles

  handles	PROC	C

				ARG	num:WORD
				
				MOV	BX,num
				MOV	AH,67H
				INT	21H
				JB		@@fin
				XOR	AX,AX
  @@fin:
				ret
  handles	ENDP

  END