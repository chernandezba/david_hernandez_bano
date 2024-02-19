.Model Large,C


.CODE

LOCAL @@

segmono  equ 0b000h
segcolor equ 0b800h

public getventan

getventan proc C
          ARG x:word,y:word,xx:word,yy:word
          USES DI,SI,ES,DS ;el di junto con el es para el buffer,el si junto con el ds para la pantalla
          pushf
          cli
          mov ah,0fh
          int 10h
          mov  bx,segcolor
          mov  cs:@@modo,al
          cmp  al,7
          jne  @@color
          mov  bx,segmono
@@color:  mov  ds,bx
          xor  si,si
          mov  ax,seg(@@buffer)
          mov  es,ax
          lea  di,@@buffer
          mov  dx,x
          or   dx,dx
          jz   @@fin
          dec  dx
          mov  cx,xx
          cmp  cx,dx
          jle  @@fin ;salir si cx <=dx
          sub  cx,dx ;en cx las columnas
          cmp  cx,80
          jg   @@fin ;si columnas > 80 salir
          mov  si,dx
          shl  si,1
          mov  cs:@@columnas,cl
          mov  ax,y
          or   ax,ax
          jz   @@fin
          dec  ax
          mov  dx,yy
          cmp  dx,ax
          jle  @@fin ;salir si dx<=ax
          sub  dx,ax ;en dx las filas
          cmp  dx,25
          jg   @@fin ;si filas>25 salir
          mov  cs:@@filas,dl
          mov  bx,dx ;guardar dx en bx
          mov  dl,160 ;160 columnas por fila(80 caracteres de 2 bytes por caracter(byte,atributo))
          mul  dl ;ax=al*dl
          add  si,ax
          mov  dx,bx ;recuperar el valor de dx
          cld
@@bucle:  mov  ax,cx ;guardar en ax las columnas
          mov  bx,si ;guardar en bx el offset a la primera columna a guardar de la fila actual
          rep  movsw
          mov  cx,ax ;recuperar el valor de cx
          add  bx,160 ;ir a la fila siguiente
          mov  si,bx
          dec  dx
          jnz  @@bucle          
@@fin:    popf
          ret       
          ;para que compile bien la quicklibrary (.qlb) hay que poner las variables aqui
          @@modo    db ? ;para el modo de pantalla actual
          @@buffer  db 80*25*2 dup (?) ; para guardar la pantalla
          @@filas   db 0 ;para las filas guardadas en el buffer
          @@columnas db 0;para las columnas por fila guardadas en el buffer
getventan endp

public    putventan

putventan proc C
          ARG x:word,y:word
          USES DI,SI,ES,DS;el di junto con el es para la pantalla,el si junto con el ds para el buffer        
          pushf
          cli
          mov  bx,segcolor
          cmp  cs:@@modo,7
          jne  @@color2
          mov  bx,segmono
@@color2: mov  es,bx
          lea  si,@@buffer
          mov  ax,seg(@@buffer)
          mov  ds,ax
          xor  cx,cx
          mov  cl,cs:@@columnas ;en cx las columnas
          mov  di,x
          or   di,di
          jz   @@fin2
          dec  di
          shl  di,1
          mov  ax,y
          or   ax,ax
          jz   @@fin2
          dec  ax
          xor  dx,dx
          mov  dl,cs:@@filas ;en dx las filas
          mov  bx,dx ;guardar dx en bx
          mov  dl,160 ;160 columnas por fila(80 caracteres de 2 bytes por caracter(byte,atributo))
          mul  dl ;ax=al*dl
          add  di,ax
          mov  dx,bx ;recuperar el valor de dx
          cld
@@bucle2: mov  ax,cx ;guardar en ax las columnas
          mov  bx,di ;guardar en bx el offset a la primera columna a guardar de la fila actual
          rep  movsw
          mov  cx,ax ;recuperar el valor de cx
          add  bx,160 ;ir a la fila siguiente
          mov  di,bx
          dec  dx
          jnz  @@bucle2
@@fin2:   popf 
          ret       
putventan endp

end