{$M 1024,0,655360}
program verifica;

const
  erroresbios:array [1..9] of string =(
'OK',
'MARCA DE DIRECCION NO ENCONTRADA',
'SECTOR NO ENCONTRADO',
'ERROR DMA',
'ERROR CRC',
'ERROR EN EL CONTROLADOR DE DISCO',
'ERROR DE BUSQUEDA',
'TIEMPO EXCEDIDO',
'DESCONOCIDO');

function disco(_comando,_unidad,_cara,_pista,_sector,_numsectores:byte;
_buffer:pointer):byte; near;
assembler;
asm
  mov dl,_unidad
  mov dh,_cara
  mov ch,_pista
  mov cl,_sector
  mov al,_numsectores
  les bx,_buffer
  mov ah,_comando
  int $13
  mov al,ah
  xor ah,ah
  end;

function toupper(letra:char):char; near;
begin
if (ord(letra)>=97) or (ord(letra)<=122) then toupper:=chr(ord(letra)-32)
  else
  toupper:=letra;
end;

function isalpha(letra:char):boolean; near;
begin
if ((ord(letra)<97) or (ord(letra)>122)) and ((ord(letra)<65)
  or (ord(letra)>90)) then
  isalpha:=false
  else
  isalpha:=true;
end;


function teclado(code:shortint):word; near;
assembler;
asm
  mov ah,code
  int $16
  end;

function tecla:word; near;
var
  key,modifiers,tec:word;

begin
tec:=65535;
while (tec = 0) do tec:=teclado(1);
key:=teclado(0);
modifiers:= teclado(2);
if (key and $ff<>0) then key:=key and $ff;
tecla:=key;
end;


var
  cara,pista,sector,error,intentos,pistai,sectori,caras,sectorf,pistaf,
  errorp,unid,icara:byte;
  unidad:string[2];
  datos:string[255];
  datos2:string[255];
  datos3:string[10];
  puntero:pointer;

begin

puntero:=@datos;
writeln('VERIFICADOR DE DISCOS V.1.0');
writeln('(c) Bit Soft 25/07/1994');
pistaf:=0;
pistai:=3;
while (pistai>pistaf) do
  begin
  write('Desde pista:');
  Readln(pistai);
  write('Hasta pista:');
  Readln(pistaf);
  end;
sectorf:=0;
sectori:=3;
while (sectori>sectorf) do
  begin
  write('Desde sector:');
  Readln(sectori);
  write('Hasta sector:');
  Readln(sectorf);
  end;
Write('Numero de caras:');
Readln(caras);
icara:=0;
if (caras=1) then
  begin
  write('Que cara?:');
  Readln(icara);
  end;
unidad:='¤:';
while (unidad[2]<>':') or (unidad[1]>'B') or (unidad[1]<'A') do
  begin
  write('Unidad (letra seguida de dos puntos):');
  Readln(unidad);
  if isalpha(unidad[1])=FALSE then unidad[1]:=toupper(unidad[1]);
  if (unidad[1]>'B') or (unidad[1]<'A') then
  writeln(#7'Solamente se pueden verificar unidad A: o B:');
  end;
unid:=byte(unidad[1]);
dec(unid,byte('A'));
write('Introduzca el disco en la unidad ',unidad,' y pulse tecla ');
writeln('ESC salir');
if (tecla=27) then
begin
  writeln('ESC PULSADO , FIN PROGRAMA');
  halt(1);
  end;
writeln('Verificando');
intentos:=0;
error:=1;
while (error<>0) and (intentos<>3) do
begin
  error:=disco(4,unid,0,0,1,1,puntero);
  if error<>0 then
    begin
    inc(intentos);
    disco(0,unid,0,0,0,0,puntero);
    end;
  end;
if error<>0 then
  begin
  writeln(#7'Error , no hay disco en la unidad o disco defectuoso');
  halt(1);
  end;
for pista:=pistai to pistaf do
  for cara:=icara to icara+caras-1 do
    for sector:=sectori to sectorf do
	 begin
	 write(#13'Pista ',pista,' cara ',cara,' sector ',sector);
	 intentos:=0;
	 error:=disco(4,unid,cara,pista,sector,1,puntero);
	 while (error<>0) and (intentos<>3) do
	 begin
	 if error<>0 then
	     begin
	     inc(intentos);
	     disco(0,unid,0,0,0,0,puntero);
	     end;
	 error:=disco(4,unid,cara,pista,sector,1,puntero);
	 end;
	 if error <> 0 then
	   begin
		 case error of
		 $00:errorp:=1;
		 $02:errorp:=2;
		 $04:errorp:=3;
		 $08:errorp:=4;
		 $10:errorp:=5;
		 $20:errorp:=6;
		 $40:errorp:=7;
		 $80:errorp:=8;
		 else errorp:=9;
		 end;
	       write('     Error :',erroresbios[errorp]);
	       if tecla=27 then
		 begin
		 writeln(#10#13'ESC PULSADO , FIN PROGRAMA');
		 halt(1);
		 end;
	       write(#13'                                                                 ');
	       end;
    end;
writeln('');
end.
