program prueba;

uses crt;

type _vesa_mode_info=record
  flags1:word;
  flagswindowa:byte;
  flagswindowb:byte;
  window_granularity:word; {tama¤o del segmento}
  kb_window:word;
  segment_windowa:word;
  segment_windowb:word;
  bank_switch:pointer; {equivalente a funci¢n 4fh subfunci¢n 5h}
  bytes_scan_line:word; {lo que viene ahora es opcional para los modos VESA , necesitado para los modos OEM}
  ancho:word;
  alto:word;
  ancho_caracter:byte;
  alto_caracter:byte;
  numero_planos_memoria:byte;
  bits_pixel:byte;
  bancos:byte;
  tipo_modelo_memoria:byte; {0=texto , 1=graficos , 2=graficos hercules ,3=arquitectura de 4-planos }
  {,4=arquitectura de pixeles empacados , 5=256 color(unchained)}
  longitud_banco_kb:byte;
  null:string[255];
  end;

var
  segpantalla:word;
  segpantalla2:word;
  segpantalla3:word;
  puntero:pointer;
  oldmodo,c:byte;
  i,i2:word;
  vesamodeinfo:_vesa_mode_info;
  bancoact:word;
const
  modosmem:array [0..5] of string=(
  'texto','graficos','graficos hercules','arquitectura de 4-planos',
  'arquitectura de pixeles empacados','256 color(unchained)');
  modo=$103;

function pow(base:word;exponente:word):word;near;assembler;
asm
  mov ax,base
  mov cx,exponente
@@mul:
  mul ax
  loopnz @@mul
  end;


procedure putpixel256(x:word;y:word;color:byte);near;
{$I-}
var
  banco:word;
  offsetc:word;
  offset1:longint;
begin
offset1:=(longint(y)*longint(vesamodeinfo.bytes_scan_line))+longint(x);
banco:=word(offset1 div longint(4096));
if banco=0 then offsetc:=x else
offsetc:=word(offset1 mod (longint(banco)*longint(4096)));
asm
  mov bx,vesamodeinfo.segment_windowa
  mov es,bx
  mov ax,bancoact
  cmp ax,banco
  je @escribe
  mov bx,0
  mov dx,banco
  call dword ptr vesamodeinfo.bank_switch
@escribe:
  mov di,offsetc
  mov al,color
  stosb
  end;
bancoact:=banco;
{$I+}
end;


begin
asm
  mov ah,$f
  int $10
  mov oldmodo,al
  push ds
  pop es
  lea di,vesamodeinfo
  mov bx,modo
  mov cx,bx
  mov dx,bx
  mov ax,$4f01
  int $10
  mov bancoact,0
  mov bh,0
  mov bl,0
  mov ax,$4f05
  mov dx,0
  int $10
  end;
segpantalla:=sega000;
segpantalla2:=segb800;
segpantalla3:=segb000;
puntero:=ptr(segpantalla,0);
writeln('Memoria disponible ',memavail);
writeln('Segmento a000 en ',segpantalla);
writeln('Incremento selector ',selectorinc);
writeln('Modo vesa , ancho:',vesamodeinfo.ancho,' alto:',vesamodeinfo.alto,
' colores:',pow(2,vesamodeinfo.bits_pixel));
writeln('Tipo memoria :',modosmem[vesamodeinfo.tipo_modelo_memoria]);
readln;
asm
  mov ax,$4f02
  mov bx,modo
  int $10
  end;
c:=0;
for i:=vesamodeinfo.alto-1downto 0 do
begin
  for i2:=0 to vesamodeinfo.ancho-1 do
	 putpixel256(i2,i,c);
if c=255 then c:=0 else c:=c+1;
end;
readln;
asm
  mov ah,0
  mov al,oldmodo
  int $10
  end;
end.