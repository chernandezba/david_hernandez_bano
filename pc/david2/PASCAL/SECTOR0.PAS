program sector0;
{$M 1024,0,655360}

var
  op:string[80];

const erroresbios:array [1..8] of string=(
 'NO HAY ERROR',
 'MARCA DE DIRECCION NO ENCONTRADA',
 'SECTOR NO HALLADO',
 'ERROR DE DMA',
 'CRC ERRONEO',
 'FALLO DEL CONTROLADOR',
 'FALLO EN LA BUSQUEDA',
 'DISCO NO RESPONDE O FUERA DE TIEMPO');

procedure finoperacion;near;
begin
  writeln('Operaci¢n completada');
  end;

procedure copyright;near;
begin
writeln('(c) BIT SOFT 1991-95  DAVID HERNANDEZ BA¥O');
end;

procedure ponerrordisco(_estado:integer); near;
  var
  errorp:integer;
  begin
case _estado of
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
writeln('Error :',erroresbios[errorp]);
end;

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
  xchg al,ah
  xor ah,ah
  end;

procedure leer0; near;
  var
   msg : pointer;
   boot:file;
   unidad:string[3];
    errores:integer;
    estado:integer;
    byteuni:byte;

  begin
    errores:=0;
    copyright;
    unidad:=paramstr(1);
    if length(unidad)=1 then
      unidad:=upcase(unidad[1]);
    if (length(unidad)>1) or ((unidad[1]<'A') or (unidad[1]>'Z')) then
      begin
      writeln('Unidad erronea : ',unidad,#7);
      exit;
      end;
    getmem(msg,512);
    byteuni:=byte(unidad[1])-byte('A');
    writeln('Leyendo sector 0 de la unidad:',unidad[1]);
    repeat
    estado:=disco(2,byteuni,0,0,1,1,msg);
    if estado<>0 then
      begin
    inc(errores,1);
    disco(0,byteuni,0,0,1,1,msg);
    end;
    until ((estado<>0) or (errores<>3));
    if estado<>0 then
      begin
      ponerrordisco(estado);
      writeln(#7);
      halt(1);
      end;
    writeln('Escribiendo fichero:',paramstr(2));
    assign(boot,paramstr(2));
    {$I-}rewrite(boot,1);
    {$I+}if IOResult<>0 then
	begin
	writeln('Error al grabar : ',paramstr(2),#7);
	exit;
	end;
    {$I-}blockwrite(boot,msg^,512);
    close(boot);
    {$I+}if IOResult<>0 then
      begin
      writeln('No hay memoria en la unidad de escritura',#7);
      exit;
      end;
  finoperacion;
  end;
procedure grabar0; near;
  var
   msg : pointer;
   boot:file;
   unidad:string;
    errores:integer;
    estado:integer;
    byteuni:byte;
  begin
  errores:=0;
  copyright;
    unidad:=paramstr(1);
    if length(unidad)=1 then
      unidad:=upcase(unidad[1]);
    if (length(unidad)>1) or ((unidad[1]<'A') or (unidad[1]>'Z')) then
      begin
      writeln('Unidad erronea : ',unidad,#7);
      exit;
      end;
    getmem(msg,512);
    writeln('Leyendo fichero:',paramstr(2));
    assign(boot,paramstr(2));
    {$I-}reset(boot,1);
    {$I+}if IOResult<>0 then
	begin
	writeln('Fichero no encontrado o disco defectuoso : ',paramstr(2),#7);
	exit;
	end;
    if filesize(boot)>512 then
      begin
      writeln('El fichero tiene una extensi¢n m xima de 512 bytes',#7);
      exit;
      end;
    {$I-}blockread(boot,msg^,filesize(boot));
    close(boot);
    {$I+}if IOResult<>0 then
      begin
      writeln('Error al acceder al fichero : ',paramstr(2),#7);
      exit;
      end;
    byteuni:=byte(unidad[1])-byte('A');
    writeln('Escribiendo sector 0 de la unidad:',unidad[1]);
      repeat
    estado:=disco(3,byteuni,0,0,1,1,msg);
    if estado<>0 then
      begin
      disco(0,byteuni,0,0,1,1,msg);
      end;
    until ((estado<>0) or (errores<>3));
    if estado<>0 then
      begin
      ponerrordisco(estado);
      writeln(#7);
      halt(1);
      end;
  finoperacion;
  end;

begin
if paramcount<3 then
  begin
  copyright;
  writeln('USO [UNIDAD] [NOMBRE FICHERO] 0 : DE SECTOR 0 A FICHERO');
  writeln('                              1 : DE FICHERO A SECTOR0');
  writeln;
  exit;
  end;

op:=paramstr(3);
case op[1] of
  '0':leer0;
  '1':grabar0;
    else
  writeln('Tiene que especificar 1 o 0',#7);
  end;
end.