program prueba;

uses crt;


type _biosdata=record
  modoactual:byte;
  columnas:word;
  longvideobuffer:word;
  startaddrvideobuf:word;
  cursor1pos:word;
  cursor2pos:word;
  cursor3pos:word;
  cursor4pos:word;
  cursor5pos:word;
  cursor6pos:word;
  cursor7pos:word;
  cursor8pos:word;
  tipocursor:word;
  paginaactual:byte;
  crtdirbase:word;
  reg3x8:byte;
  reg3x9:byte;
  end;

var
  segpantalla:word;
  segpantalla2:word;
  segpantalla3:word;
  puntero:pointer;
  oldmodo:byte;
  i:word;
  biosdata:^_biosdata;

procedure modificapantalla(segm:word;long:word;color:byte);
near;assembler;
{$I-}
asm

  mov si,0
  mov ax,segm
  mov es,ax
  mov ah,color
  mov ch,ah
@@b:
  mov cl,4 {p gina actual}
@@a:
  {mov dx,$3ce
  mov al,8
  out dx,al
  inc dx
  mov al,$ff
  out dx,al}
  mov dx,$3ce
  mov al,4
  out dx,al { selecciona registro de seleccionar pagina}
  inc dx
  mov bh,al
  mov al,cl
  dec al
  out dx,al { selecciona p gina para modo de 256K}
  mov al,bh
  mov al,ah
  and al,1
  push ax
  mov al,es:[si]
  pop ax
  mov es:[si],al
  shr ah,1
  dec cl
  jnz @@a
  mov ah,ch
  inc si
  cmp si,long
  jnz @@b
{$I+}
end;



begin

biosdata:=ptr(seg0040,$49);
segpantalla:=sega000;
segpantalla2:=segb800;
segpantalla3:=segb000;
puntero:=ptr(segpantalla,0);
writeln('Memoria disponible ',memavail);
writeln('Segmento a000 en ',segpantalla);
writeln('Incremento selector ',selectorinc);
writeln('N£mero de columnas :',biosdata^.modoactual);
writeln('Longitud del buffer de video :',biosdata^.longvideobuffer);
readln;
asm
  mov ah,$f
  int $10
  mov oldmodo,al
  mov ax,$0012
  int $10
  {mov dx,$3c0
  mov  al,16
  out  dx,al
  in  al,dx
  mov ah,al
  mov al,16
  out dx,al
  mov al,ah
  and al,2+4+8+16+32+64+128
  or al,1
  inc dx
  out dx,al}
  {mov dx,$3ce
  mov al,5
  out dx,al
  inc dx
  in al,dx
  and al,4+8+16+32+128
  mov ah,2
  or  al,ah
  mov ah,al
  mov al,5
  mov dx,$3ce
  out dx,al
  inc dx
  mov al,ah
  out dx,al
  mov al,2
  mov dx,$3c4
  out dx,al
  inc dx
  mov al,1+2+4+8
  out dx,al}
  end;

for i:=3 to 15 do
begin
  modificapantalla(segpantalla,80*480,i); {80*480 longitud del segmento en modo 12h : 640x480 16c 256K}
  readln;
  end;
{modificapantalla(segpantalla,65535,10,1);}
{modificapantalla(segpantalla,38*1024,16,3);}
{modificapantalla(segpantalla,65535,5,3);}
readln;
asm
  mov ah,0
  mov al,oldmodo
  int $10
  end;
end.