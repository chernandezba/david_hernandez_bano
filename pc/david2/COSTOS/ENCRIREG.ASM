.Model Large,C


.Code


Local @@

Public encriptareg

;declaraci¢n para quickbasic: declare sub encriptareg (ptr as any,byval longi as word,byval clave as word)
encriptareg   Proc C
         ARG punt:word,longi:word,clave:word
         ;el primer argumento es la direcci¢n que da el quickbasic
         ;de una variable tipo TYPE
         ;el segundo es la longitud
         ;el tercero es la clave entera (pero s¢lo se usan los 8
         ;primeros bits) para encriptar
         ;utiliza el m‚todo dest=255-(dest xor clave)
         USES SI

         mov si,punt
         mov cx,longi
         mov bx,clave
@@bucle: mov al,[si]
         xor al,bl
         xchg bh,al
         mov al,255
         sub al,bh
         mov [si],al
         inc si
         loop @@bucle
         ret
encriptareg endp

end

