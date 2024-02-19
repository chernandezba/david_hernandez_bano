program sformat;
{$M 1024,0,655360}
{$L A:BOOTSFOR.OBJ}
procedure BOOTSFOR;external;

const
  cabeceras:array [1..18] of byte = (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18);
  sys1:string='IO.SYS';
  sys2:string='MSDOS.SYS';
  sysibm1:string='IBMIO.SYS';
  sysibm2:string='IBMDOS.SYS';

type
  sector0=record {a partir del byte 11}
  bytsector:word;
  sectcluster:byte;
  sectorespresrvcomie:word;
  copiasfat:byte;
  entradasdirprin:word;
  totalsectordisco:word;
  idfat:byte;
  sectorfat:word;
  sectorpista:word;
  caras:word;
  secespresrv:word;
  end;
  parametrosdrive=record
  bpb:byte;
  tipo:byte;
  removible:word;
  maxcilindros:word;
  tipoelem:byte;
  bytesector:word;
  sectorcluster:byte;
  sectoresreserv:word;
  numtablasfat:byte;
  nummaxentradasdirprin:word;
  numerossectores:word;
  idenfat:byte;
  sectoresfat:word;
  sectorespista:word;
  numcabezas:word;
  numsectoresocultos:longint;
  unkown1:longint;
  unkown2:longint;
  unkown3:longint;
  unkown4:longint;
  unkown5:longint;
  end;
var
 etiquetap,etiquetap2,dta,filtrop,clustererroneos,pcabecera,buffer2:pointer;
 sectorelleno,fat,directorio,bufferpathcommand,buffersys1,buffersys2:pointer;
 longpathcommand,longsys1,longsys2,psector0p,mod0,mod1,mod2,l2,l,i,s:longint;
 totalbytdisco,clustererroneol,fechahorapathcommand,fechahorasys1:longint;
 fechahorasys2,bytescluster:longint;
 clustererroneo,bytesfat:integer;
 contador,bytespista,numclustererroneos,estado2,sectorescluster:word;
 clusterporunidad,clusterdisponibles,bytesporsector,handlepathcommand:word;
 handlesys1,handlesys2,handlevol,numparam:word;
 unidadbios,intentos,pista,cara,sector,pistas,pistai,copiafat,estado:byte;
 valorsubfunc8,valorsubfunc17,valorfindfirst,abierto:byte;
 sal,sisubfunc8,siextender,siponsistema,siotravez,sieserror:boolean;
 pathcommand:string[80];
 etiqueta2:string[17];
 etiqueta:string[15];
 nombrefichero,nombrefichero2:string[13];
 opcion,opcion2:string[10];
 unidst,unidadcommand,strotravez:string[2];
 psector0:^sector0;
 paramdisco:parametrosdrive;

 label hazcomosiparamcount1,otravez,sihayerror,pregunta;

procedure reiniciadisco;near;assembler;
asm
  mov ah,$0d
  int $21 {Reinicializar disco}
  end;

procedure sierror;far;
begin

  if exitcode=203 then
    begin
    writeln('Memoria insuficiente'#7);
    erroraddr:=nil;
    end;
  end;


procedure transforma;near;assembler;
asm
  mov ax,ss
  mov es,ax
  mov ds,ax
  cld
  mov al,[si]
  inc al
  mov es:[di],al
  inc di
  inc si
  dec al
  xor ch,ch
  mov cl,al
  repz movsb {traspasa  _nombre a __nombre}
  xor al,al
  mov es:[di],al {le pone un 0 al final del string para convertirlo a cadena ASCIIZ}
  mov ax,ss
  mov ds,ax
  end;

procedure ponopcionovalida;near;
begin
  writeln('Opci¢n no v lida'#7);
  halt;
  end;

procedure ponsubopcionovalida;near;
begin
  writeln('Subopci¢n no v lida'#7);
  halt;
  end;

procedure strupr(str:pointer);near;assembler;
asm
  les di,str
  xor ch,ch
  mov cl,es:[di]
  or cl,cl
  jz @@58
  inc di
@@56:
  mov al,es:[di]
  cmp al,'a'
  jl @@55
  cmp al,'z'
  jg @@55
  sub al,32
  mov es:[di],al
@@55:
  inc di
  loopnz @@56
@@58:
  end;

function cogefechahora(handle:word):longint;near;assembler;
asm
  mov ah,$57
  mov al,0
  mov bx,handle
  int $21
  mov ax,cx
  end;

procedure ponfechahora(handle:word;fechahora:longint);near;assembler;
asm
  lea di,fechahora
  mov ah,$57
  mov al,1
  mov bx,handle
  mov cx,ss:[di]
  mov dx,ss:[di+2]
  int $21
  end;


procedure errorcrearsistema;near;
begin
  writeln('Error al crear archivo de sistema'#7);
  end;

procedure cambiatributos(_nombre:string;atr:word);near;
var
  __nombre:string[81];
begin

asm
  push ds
  lea si,_nombre
  lea di,__nombre
  call transforma
  lea dx,__nombre
  inc dx
  mov ah,$43
  mov al,1
  mov cx,atr
  int $21
  pop ds
  end;
end;
procedure borrar(_nombre:string);near;
var
  __nombre:string[81];

  begin

asm
  push ds
  lea si,_nombre
  lea di,__nombre
  call transforma
  lea dx,__nombre
  inc dx
  mov ah,$41
  int $21
  pop ds
  end;
end;
procedure escribir(handle,bytes:word;buf:pointer);near;assembler;
asm
  mov ah,$40
  mov bx,handle
  mov cx,bytes
  push ds
  lds dx,buf
  int $21
  pop ds
  end;

function crear(_nombre:string;_atr:word):word;near;
var
  __nombre:string[81];
  handle:word;

  begin

asm
  push ds
  lea si,_nombre
  lea di,__nombre
  call transforma
  lea dx,__nombre
  inc dx
  mov ah,$3c
  mov cx,_atr
  int $21
  pop ds
  jnc @@211
  mov ax,$ffff
@@211:
  mov handle,ax
  end;
crear:=handle;
end;

procedure leer(_handle,_leer:word;_buffer:pointer);near;assembler;
asm
  mov ah,$3f
  mov bx,_handle
  mov cx,_leer
  push ds
  lds dx,_buffer
  int $21
  pop ds
  end;


function longitud(_handle:word):longint;near;assembler;
asm
  mov ah,$42
  mov al,2
  mov bx,_handle
  xor cx,cx
  mov dx,cx
  int $21
  push dx
  push ax
  mov ah,$42
  mov al,0
  mov bx,_handle
  xor cx,cx
  mov dx,cx
  int $21
  pop ax
  pop dx
  end;

procedure cerrar(_handle:word);near;assembler;
asm
  mov ah,$3e
  mov bx,_handle
  int $21
  end;


function abrir(_nombre:string;modo:byte):word;near;
var
  __nombre:string[81];
  handle:word;

begin

asm
  push ds
  lea si,_nombre
  lea di,__nombre
  call transforma
  lea dx,__nombre
  inc dx
  mov ah,$3d
  mov al,modo
  int $21 {abre fichero}
  pop ds
  jnc @@43
  mov ax,$ffff
@@43:
  mov handle,ax
  end;
abrir:=handle;
end;

procedure copianombre;near;assembler;
asm
  push ds
  push ax
  mov ax,seg(nombrefichero)
  mov es,ax
  lea di,nombrefichero
  lds si,dta
  add si,30
  mov cx,13
  inc di
  rep movsb
  mov ax,seg(nombrefichero)
  mov es,ax
  lea di,nombrefichero
  xor ch,ch
  mov cl,0
@@39:
  inc di
  mov al,es:[di]
  inc cl
  or al,al
  jnz @@39
  mov ax,seg(nombrefichero)
  mov es,ax
  lea di,nombrefichero
  dec cl
  mov es:[di],cl
  pop ax
  pop ds
  end;


function findfirst(mascara:string;_atributo:word):byte;near;
var
  _mascara:string[81];
  salida:byte;

begin

asm
  push ds
  lea si,mascara
  lea di,_mascara
  call transforma
  lea dx,_mascara
  inc dx
  mov ah,$4e
  mov cx,_atributo
  int $21
  pop ds
  jc @@36
  xor al,al
  jmp @@37
@@36:
  mov al,1
@@37:
  mov salida,al
  call copianombre
  end;
findfirst:=salida;
end;

function findnext:byte;near;assembler;
  asm
    mov ah,$4f
    int $21
    jc @@38
    xor al,al
    jmp @@39
@@38:
    mov al,1
@@39:
    call copianombre
    end;

procedure borrasistema;near;
var
  valorf:byte;
begin
  if handlepathcommand<>$ffff then cerrar(handlepathcommand);
  if handlesys1<>$ffff then cerrar(handlesys1);
  if handlesys2<>$ffff then cerrar(handlesys2);
  reiniciadisco;
  findfirst(unidst+'\*.*',$1+$2+$4);
  cambiatributos(unidst+'\'+nombrefichero,0);
  borrar(unidst+'\'+nombrefichero);
  repeat
  valorf:=findnext;
  cambiatributos(unidst+'\'+nombrefichero,0);
  borrar(unidst+'\'+nombrefichero);
  until valorf=1;
  end;


procedure esperatecla;near;assembler;
asm
  mov ah,$08
  int $21
  end;

procedure ponformatonovalidoparaunidad;near;
begin
  writeln('Formato inv lido para esta unidad'#7);
  end;


function siesdiscoduro(unidad:byte):byte;near;assembler;
asm
  mov ax,$4408
  mov bl,unidad
  int $21
  end;

function obtenerparamdisco(unidad:byte;puntero:pointer):word;near;assembler;

asm
  mov ax,$440d
  mov bl,unidad
  mov cx,$0860
  push ds
  lds dx,puntero
  int $21
  pop ds
  jc @@11
  xor ax,ax
@@11:
end;

procedure rellenamem(p:pointer;w:word;a:byte);near;assembler;
asm
  mov cx,w
  les di,p
  mov al,a
  cld
  rep stosb
  end;

function dosdisco(_comando,_unidad:byte;_secleer,_seclogicinicial:word;
_punt:pointer):word;near;assembler;
{0 leer , 1 escribir}
asm
  push ds
  push bp
  mov al,_unidad
  mov cx,_secleer
  mov dx,_seclogicinicial
  lds bx,_punt
  cmp _comando,0
  jnz @@4
  int $25
  jmp @@5
@@4:
  int $26
@@5:
  jc @@6
  xor ax,ax
  jmp @@7
@@6:
  mov ax,1
@@7:
  add sp,2
  pop bp
  pop ds
  end;

function sierrordosdisco(_comando,_unidad:byte;_secleer,_seclogicinicial:word;
_buffer:pointer):word; near;
  var
  _intentos:byte;
  _estado:word;
  begin
  _intentos:=0;
  _estado:=$ffff;
  repeat
  _estado:=dosdisco(_comando,_unidad,_secleer,_seclogicinicial,_buffer);
  inc(_intentos);
  until (_intentos>3) or (_estado=0);
  sierrordosdisco:=_estado;
end;


procedure discoinservible;near;
begin
writeln(#10#13'Disco inservible'#7);
end;
procedure modificafat (clusterfichero,valor:word;_fat:pointer);near;assembler;
asm
  les di,_fat
  mov ax,clusterfichero
  mov cx,3
  mul cx
  shr ax,1
  add di,ax
  mov ax,clusterfichero
  test ax,1
  jnz @@2
  mov bx,$f000
  and es:[di],bx
  mov ax,valor
  or es:[di],ax
  jmp @@3
@@2:
  mov ax,valor
  mov cl,4
  shl ax,cl
  mov bx,$000f
  and es:[di],bx
  or es:[di],ax
@@3:
end;


function cogevalorpuntero(pp:pointer;ii:integer):integer;near;assembler;
asm
  push ds
  cld
  lds si,pp
  mov ax,ii
  shl ax,1
  add si,ax
  lodsw
  pop ds
  end;
procedure actualizapuntero(pp:pointer;ii:integer;valor:word);near;assembler;
asm
  les di,pp
  mov ax,ii
  shl ax,1
  add di,ax
  mov ax,valor
  cld
  stosw
  end;

procedure inicializacabeceras(_sectorpista:byte;tabla,_p:pointer);near;assembler;
asm
  push ds
  les di,_p
  xor ch,ch
  mov cl,_sectorpista
  mov al,2
  add di,2
  lds si,tabla
  cld
@@8:
  movsb
  stosb
  dec cl
  jz @@10
  add di,2
  jmp @@8
@@10:
  pop ds
end;

procedure arreglacabeceras(_sectorpista:byte;_p:pointer);near;assembler;
asm
  mov cl,_sectorpista
  mov ch,0
  les di,_p
  mov bh,pista
  mov bl,cara
@@1:
  mov es:[di],bh
  inc di
  mov es:[di],bl
  add di,3
  inc ch
  cmp ch,cl
  jl @@1
  end;

procedure mirarsisubfunc8;near;assembler;
asm
  mov ah,8
  mov dl,unidadbios
  int $13
  mov al,false
  jc @@15
  mov valorsubfunc8,bl
  mov al,true
@@15:
  mov sisubfunc8,al
  end;

procedure prepformatat;near;assembler;
asm
  mov ah,$17
  mov al,valorsubfunc17
  mov dl,unidadbios
  int $13
  mov ah,$18
  mov cl,byte ptr paramdisco.sectorespista
  mov ch,byte ptr paramdisco.maxcilindros
  cmp ch,80
  jz @@99
  add ch,2
  jmp @@100
@@99:
  add ch,4
@@100:
  mov dl,unidadbios
  int $13
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

function sierrordisco(_comando,_unidad,_cara,_pista,_sector,_numsectores:byte;
_buffer:pointer):byte; near;
  var
  _intentos:byte;
  _estado:byte;
  begin
  _intentos:=0;
  asm
    mov dl,_unidad
    mov dh,_cara
    mov ch,_pista
    mov cl,_sector
    mov al,_numsectores
    les bx,_buffer
    mov ah,_comando
@@56:
    push dx
    push cx
    push ax
    push es
    push bx
    int $13
    mov _estado,ah
    pop bx
    pop es
    pop ax
    pop cx
    pop dx
    jnc @@55
    push dx
    push cx
    push ax
    push es
    push bx
    mov dl,_unidad
    xor ah,ah
    int $13
    pop bx
    pop es
    pop ax
    pop cx
    pop dx
    inc _intentos
    cmp _intentos,3
    jnz @@56
@@55:
    end;
  sierrordisco:=_estado;
end;

procedure mirasihaydatos;near;
var
  formatea:string[3];
  __error:word;
begin
  __error:=sierrordosdisco(0,unidadbios,1,0,sectorelleno);
  if __error=0 then
    begin
    write('En este disco hay datos , ¨formatear (S/N)? ');
    Readln(formatea);
    strupr(@formatea);
    if (formatea[1]='N') then sieserror:=true;
    end;
  end;


begin

  exitproc:=@sierror;
  etiquetap:=nil;
  etiquetap2:=nil;
  dta:=nil;
  filtrop:=nil;
  clustererroneos:=nil;
  pcabecera:=nil;
  buffer2:=nil;
  sectorelleno:=nil;
  fat:=nil;
  directorio:=nil;
  bufferpathcommand:=nil;
  buffersys1:=nil;
  buffersys2:=nil;
  writeln('SFORMAT V. 1.4. (c) BIT SOFT 25/05/1995');
sihayerror:
  sieserror:=false;
  siponsistema:=false;
  siotravez:=false;
  longpathcommand:=0;
  longsys1:=longpathcommand;
  longsys2:=longsys1;
  numparam:=paramcount;
  psector0p:=longint(@BOOTSFOR);
  inc(psector0p,11);
  psector0:=pointer(psector0p);
  siextender:=false;
  pistai:=0;
  if dta=nil then getmem(dta,256);
  asm
    mov ah,$1a
    push ds
    lds dx,dta
    int $21
    pop ds
    end;
  opcion:=Paramstr(2);
  strupr(@opcion);
  if (numparam<1) then
    begin
    writeln('Uso : SFORMAT [unidad:] [opcion] [subopci¢n]');
    writeln('Opci¢n  :  0 Formatear a 42 pistas 5¬ Doble Densidad');
    writeln('           1 Formatear a 84 pistas 3« Doble Densidad');
    writeln('           2 Formatear a 84 pistas 5¬ Alta  Densidad');
    writeln('           3 Formatear a 84 pistas 3« Alta  Densidad');
    writeln('           S Formatear al formato por defecto y transferir sistema');
    writeln('           E Extender el formato del disco sin formatearlo entero');
    writeln('Subopci¢n: S Formatear y transferir sistema');
    writeln;
    exit;
    end;
  unidst:=Paramstr(1);
  strupr(@unidst);
  if ((unidst[1]>'Z') or (unidst[1]<'A')) then
    begin
    writeln('Unidad erronea'#7);
    exit;
    end;
  unidadbios:=byte(unidst[1])-byte('A');
otravez:
  numclustererroneos:=0;
  paramdisco.bpb:=0;
  if (numparam>=3) then
    begin
    opcion2:=paramstr(3);
    strupr(@opcion2);
    end;
  if (siesdiscoduro(unidadbios+1)=1) then
    begin
    writeln('Este programa solo admite discos removibles , no discos duros'#7);
    exit;
    end;
  if (obtenerparamdisco(unidadbios+1,@paramdisco)<>0) then
    begin
    writeln('Unidad desconodida o error'#7);
    exit;
    end;
hazcomosiparamcount1:
if (numparam=1) then
  begin
  psector0^.bytsector:=paramdisco.bytesector;
  psector0^.sectcluster:=paramdisco.sectorcluster;
  psector0^.copiasfat:=paramdisco.numtablasfat;
  psector0^.entradasdirprin:=paramdisco.nummaxentradasdirprin;
  psector0^.idfat:=paramdisco.idenfat;
  psector0^.sectorfat:=paramdisco.sectoresfat;
  psector0^.sectorpista:=paramdisco.sectorespista;
  if psector0^.sectorpista=36 then
    begin
    writeln('Este programa no soporta unidades de 3« de 2.88MB'#7);
    exit;
    end;
  psector0^.caras:=paramdisco.numcabezas;
  if paramdisco.idenfat=$fd then pistas:=42
    else pistas:=paramdisco.maxcilindros+4;
  psector0^.totalsectordisco:=psector0^.caras*psector0^.sectorpista*pistas;
  if paramdisco.idenfat=$fd then opcion:='0';
  if paramdisco.idenfat=$f9 then opcion:='1';
  if paramdisco.idenfat=$f0 then opcion:='3';
  if paramdisco.sectorespista=15 then opcion:='2';
  end;
  if (numparam>=2) then
  begin
  if (opcion2<>'S') and (numparam=3) then ponsubopcionovalida;
  if ((opcion='S') or (opcion2='S')) then
    begin
    asm
      mov ah,$62
      int $21 {obtener direccion del segmento del psp}
      mov es,bx
      mov di,$2c
      mov ds,es:[di] {direccion de variables de entorno}
      mov si,8 {direccion de COMSPEC=}
      mov al,ds:[5]
      cmp al,0 {mira si PATH est  antes , est  antes si no hay PATH}
      jnz @@88
      mov si,14
@@88:
      mov di,offset(pathcommand)
      mov ax,seg(pathcommand)
      mov es,ax
      mov al,80
      mov es:[di],al
      inc di
      mov cx,80
      cld
      repz movsb {copia a partir de COMSPEC= a variable pathcommand}
      mov ax,seg(pathcommand)
      mov ds,ax
      end;
      unidadcommand:=pathcommand;
    repeat
      if siotravez=false then
	begin
      valorfindfirst:=findfirst(unidadcommand+'\*.*',$1+$2+$4);
      nombrefichero2:=nombrefichero;
      handlepathcommand:=$ffff;
      handlesys1:=$ffff;
      handlesys2:=$ffff;
	end
	else
	begin
	abierto:=1;
	valorfindfirst:=0;
	end;
      estado2:=sierrordosdisco(0,byte(byte(unidadcommand[1])-byte('A')),1,0,
      @BOOTSFOR);
      psector0^.secespresrv:=0;
      asm
	les di,psector0
	add di,509-11
	xor al,al
	mov es:[di],al {toca el byte 509 del sector0 = unidadbios de arranque}
	end;
      if abierto<>1 then
	begin
	abierto:=1;
	handlepathcommand:=abrir(pathcommand,0);
	if nombrefichero2=sys1 then
	  begin
	  handlesys1:=abrir(unidadcommand+'\'+sys1,0);
	  handlesys2:=abrir(unidadcommand+'\'+sys2,0);
	  end;
	if nombrefichero2=sysibm1 then
	  begin
	  handlesys1:=abrir(unidadcommand+'\'+sysibm1,0);
	  handlesys2:=abrir(unidadcommand+'\'+sysibm2,0);
	  end;
	if (handlesys1=$ffff) or (handlesys2=$ffff) or
	  (handlepathcommand=$ffff) then valorfindfirst:=1
	  else valorfindfirst:=0;
	end;
    if (valorfindfirst=1) or (estado2<>0) then
	begin
	writeln('Inserte disco con sistema en unidad ',unidadcommand,' y pulse tecla');
	if handlepathcommand<>$ffff then cerrar(handlepathcommand);
	if handlesys1<>$ffff then cerrar(handlesys1);
	if handlesys2<>$ffff then cerrar(handlesys2);
	abierto:=0;
	esperatecla;
	end;
    until(valorfindfirst<>1);
    if siotravez=false then
      begin
      longpathcommand:=longitud(handlepathcommand);
      longsys1:=longitud(handlesys1);
      longsys2:=longitud(handlesys2);
      fechahorapathcommand:=cogefechahora(handlepathcommand);
      fechahorasys1:=cogefechahora(handlesys1);
      fechahorasys2:=cogefechahora(handlesys2);
      if bufferpathcommand=nil then
	getmem(bufferpathcommand,integer(longpathcommand));
      if buffersys1=nil then getmem(buffersys1,integer(longsys1));
      if buffersys2=nil then getmem(buffersys2,integer(longsys2));
      leer(handlepathcommand,integer(longpathcommand),bufferpathcommand);
      leer(handlesys1,integer(longsys1),buffersys1);
      leer(handlesys2,integer(longsys2),buffersys2);
      reiniciadisco;
      end;
    siponsistema:=true;
    if numparam=2 then
    begin
      numparam:=1;
      goto hazcomosiparamcount1;
      end;
    end;
  if length(opcion)>1 then ponopcionovalida;
  if length(opcion2)>1 then ponsubopcionovalida;
  if (opcion<>'S') and (opcion<>'E') and ((opcion<'0') or (opcion>'3')) then
    ponopcionovalida;
  if opcion='0' then
    begin
    if ((paramdisco.maxcilindros<>40) or (paramdisco.sectorespista<>9))
    and ((paramdisco.maxcilindros<>80) or (paramdisco.sectorespista<>15)) then
      begin
      ponformatonovalidoparaunidad;
      exit;
      end;
    psector0^.bytsector:=512;
    psector0^.sectcluster:=2;
    psector0^.copiasfat:=2;
    psector0^.entradasdirprin:=112;
    psector0^.idfat:=$fd;
    psector0^.sectorfat:=2;
    psector0^.sectorpista:=9;
    pistas:=42;
    psector0^.totalsectordisco:=9*pistas*2;
    psector0^.caras:=2;
    end;
  if opcion='1' then
    begin
    if ((paramdisco.maxcilindros<>80) or (paramdisco.sectorespista<>9))
    and ((paramdisco.maxcilindros<>80)  or (paramdisco.sectorespista<>18)) then
      begin
      ponformatonovalidoparaunidad;
      exit;
      end;
    psector0^.bytsector:=512;
    psector0^.sectcluster:=2;
    psector0^.copiasfat:=2;
    psector0^.entradasdirprin:=112;
    psector0^.idfat:=$fd;
    psector0^.sectorfat:=3;
    psector0^.sectorpista:=9;
    pistas:=84;
    psector0^.totalsectordisco:=pistas*2*9;
    psector0^.caras:=2;
    end;
  if opcion='2' then
    begin
    if ((paramdisco.maxcilindros<>80) and (paramdisco.sectorespista<>15)) then
      begin
      ponformatonovalidoparaunidad;
      exit;
      end;
    psector0^.bytsector:=paramdisco.bytesector;
    psector0^.sectcluster:=paramdisco.sectorcluster;
    psector0^.copiasfat:=paramdisco.numtablasfat;
    psector0^.entradasdirprin:=paramdisco.nummaxentradasdirprin;
    psector0^.idfat:=paramdisco.idenfat;
    psector0^.sectorfat:=paramdisco.sectoresfat;
    psector0^.sectorpista:=paramdisco.sectorespista;
    pistas:=84;
    psector0^.totalsectordisco:=84*psector0^.sectorpista*2;
    psector0^.caras:=2;
    end;
  if opcion='3' then
    begin
    if ((paramdisco.maxcilindros<>80) and (paramdisco.sectorespista<>18)) then
      begin
      ponformatonovalidoparaunidad;
      exit;
      end;
    psector0^.bytsector:=paramdisco.bytesector;
    psector0^.sectcluster:=paramdisco.sectorcluster;
    psector0^.copiasfat:=paramdisco.numtablasfat;
    psector0^.entradasdirprin:=paramdisco.nummaxentradasdirprin;
    psector0^.idfat:=paramdisco.idenfat;
    psector0^.sectorfat:=paramdisco.sectoresfat;
    psector0^.sectorpista:=paramdisco.sectorespista;
    pistas:=84;
    psector0^.totalsectordisco:=84*psector0^.sectorpista*2;
    psector0^.caras:=2;
    end;
  if (opcion='E') then
    begin
    siextender:=true;
    writeln('Inserte disco a extender en la unidad ',unidst,
    '  y pulse tecla');
    esperatecla;
    estado2:=sierrordosdisco(0,unidadbios,1,0,@BOOTSFOR);
    if (estado2<>0) then
      begin
      writeln('Error de disco'#7);
      sieserror:=true;
      goto pregunta;
      end;
    pistas:=(psector0^.totalsectordisco div psector0^.sectorpista) div
    psector0^.caras;
    if (pistas=42) or (pistas=84) then
      begin
      writeln('Disco ya extendido'#7);
      sieserror:=true;
      goto pregunta;
      end;
    if pistas=40 then
      begin
      pistai:=pistas;
      pistas:=42-pistas;
      inc(psector0^.totalsectordisco,psector0^.sectorpista*pistas*
      psector0^.caras);
      end
    else
      begin
      pistai:=pistas;
      pistas:=84-pistas;
      inc(psector0^.totalsectordisco,psector0^.sectorpista*pistas*
      psector0^.caras);
      end;
    if psector0^.idfat=$fd then opcion:='0'  else
    if psector0^.idfat=$f9 then opcion:='1'  else
    if psector0^.idfat=$f0 then opcion:='3'  else
    if psector0^.sectorpista=15 then opcion:='2'
      else
      begin
      writeln('Formato de disco no reconocido'#7);
      sieserror:=true;
      goto pregunta;
      end;
    end;
  end;
  if pcabecera=nil then Getmem(pcabecera,18*4);
  if clustererroneos=nil then Getmem(clustererroneos,5000);
  bytespista:=psector0^.bytsector*psector0^.sectorpista;
  if sectorelleno=nil then Getmem(sectorelleno,bytespista);
  rellenamem(sectorelleno,bytespista,$F6);
  bytesfat:=psector0^.sectorfat*psector0^.bytsector;
  if fat=nil then Getmem(fat,bytesfat);
  if directorio=nil then
    Getmem(directorio,(psector0^.entradasdirprin div 16)*psector0^.bytsector);
  if siextender=false then
    begin
    writeln('Inserte disco a formatear en la unidad ',unidst,
    '  y pulse tecla');
    esperatecla;
    mirasihaydatos;
    if sieserror=true then goto pregunta;
    end;
  writeln('Dando formato a ',integer(psector0^.totalsectordisco)*
  psector0^.bytsector div 1024,' K');
  {$I-}
  if opcion='0' then inicializacabeceras(psector0^.sectorpista,@cabeceras,pcabecera);
  if opcion='1' then inicializacabeceras(psector0^.sectorpista,@cabeceras,pcabecera);
  if opcion='2' then inicializacabeceras(psector0^.sectorpista,@cabeceras,pcabecera);
  if opcion='3' then inicializacabeceras(psector0^.sectorpista,@cabeceras,pcabecera);
  mirarsisubfunc8;
  if sisubfunc8=TRUE then
    begin
    if (opcion='0') and (valorsubfunc8=1) then valorsubfunc17:=1;
    if (opcion='0') and (valorsubfunc8=2) then valorsubfunc17:=2;
    if (opcion='1') and (valorsubfunc8=3) then valorsubfunc17:=4;
    if (opcion='1') and (valorsubfunc8=4) then valorsubfunc17:=5;
    if (opcion='2') then valorsubfunc17:=3;
    if (opcion='3') then valorsubfunc17:=6;
    end;
  if sierrordisco(3,unidadbios,0,0,1,1,@BOOTSFOR)=3 then
   begin
   writeln;
   writeln('Disco protegido contra escritura'#7);
   sieserror:=true;
   goto pregunta;
   end;
  for pista:=pistai to  pistai+pistas-1 do
    for  cara:=0 to (psector0^.caras-1) do
    begin
    if siextender=false then
      write('Formateado el ',(pista * 100) div (pistai+pistas-1),' %'#13)
      else
      write('Formateando pistas adicionales : Pista:',pista,'  Cara:',cara,
      #13);
    arreglacabeceras(psector0^.sectorpista,pcabecera);
    if sisubfunc8=TRUE then prepformatat;
    sierrordisco(5,unidadbios,cara,pista,1,psector0^.sectorpista,
    pcabecera);
    estado:=sierrordisco(4,unidadbios,cara,pista,1,psector0^.sectorpista,
    sectorelleno);
      if (estado<>0) then
	begin
	sector:=1;
	sal:=False;
	while (sal=False) do
	begin
	estado:=sierrordisco(4,unidadbios,cara,pista,sector,1,sectorelleno);
	if (estado<>0) then
	begin
	l:=longint(psector0^.sectcluster);
	l2:=2*l;
	i:=longint(psector0^.sectorespresrvcomie)+
	(longint(psector0^.copiasfat)*longint(psector0^.sectorfat))+
	(longint(psector0^.entradasdirprin) div 16);
	s:=longint(pista)*longint(psector0^.sectorpista)*
	longint(psector0^.caras)+(longint(psector0^.sectorpista)*
	longint(cara))+(longint(sector)-1);
	clustererroneol:=(-i+l2+(s)) div l;
	clustererroneo:=integer(clustererroneol);
	if clustererroneo<2 then
	  begin
	  discoinservible;
	  sieserror:=true;
	  goto pregunta;
	  end;
	if numclustererroneos<>0 then
	  begin
	  if cogevalorpuntero(clustererroneos,numclustererroneos-1)<>clustererroneo then
	  begin
	  actualizapuntero(clustererroneos,numclustererroneos,clustererroneo);
	  inc(numclustererroneos);
	  end;
	end
	else
	begin
	actualizapuntero(clustererroneos,numclustererroneos,clustererroneo);
	inc(numclustererroneos);
	end;
      end;
      inc(sector);
      if (sector>psector0^.sectorpista) then sal:=True;
     end;
  end;
end;
  {$I+}
  writeln('Formateo realizado                                                  ');
  writeln;
  writeln;
  estado:=sierrordisco(3,unidadbios,0,0,1,1,@BOOTSFOR);
  if (estado<>0) then
    begin
    discoinservible;
    sieserror:=true;
    goto pregunta;
    end;
  if siextender=true then
    begin
    estado:=sierrordisco(2,unidadbios,0,0,1+psector0^.sectorespresrvcomie+
  (psector0^.sectorfat*copiafat),psector0^.sectorfat,fat);
    if (estado<>0) then
      begin
      writeln('Error de disco'#7);
      sieserror:=true;
      goto pregunta;
      end;
    end
  else
  begin
  rellenamem(fat,bytesfat,0);
  actualizapuntero(fat,0,(psector0^.idfat) or ($ff shl 8));
  actualizapuntero(fat,1,$ff);
  end;
  if numclustererroneos<>0 then
    for contador:=0 to numclustererroneos-1 do
    modificafat(cogevalorpuntero(clustererroneos,contador),$FF7,fat);
  for copiafat:=0 to psector0^.copiasfat-1 do
    begin
    estado:=sierrordisco(3,unidadbios,0,0,1+psector0^.sectorespresrvcomie+
  (psector0^.sectorfat*copiafat),psector0^.sectorfat,fat);
  if estado<>0 then
    begin
    discoinservible;
    sieserror:=true;
    goto pregunta;
    end;
    end;
  if siextender=false then
    begin
    rellenamem(directorio,(psector0^.entradasdirprin div 16) *
    psector0^.bytsector,0);
  {$I-}
  if (sierrordosdisco(1,unidadbios,psector0^.entradasdirprin div 16,
  psector0^.sectorespresrvcomie+(psector0^.copiasfat*psector0^.sectorfat),
  directorio))<>0 then
    begin
    discoinservible;
    sieserror:=true;
    goto pregunta;
    end;
    end;
  {$I+}
  if siponsistema=true then
    begin
    sectorescluster:=psector0^.sectcluster;
    bytesporsector:=psector0^.bytsector;
    bytescluster:=longint(sectorescluster)*longint(bytesporsector);
    if (longpathcommand mod bytescluster)=0 then mod0:=0 else mod0:=1;
    if (longsys1 mod bytescluster)=0 then mod1:=0 else mod1:=1;
    if (longsys2 mod bytescluster)=0 then mod2:=0 else mod2:=1;
    if (cogevalorpuntero(clustererroneos,0)<=((longsys1 div bytescluster)+
    mod1+(longsys2 div bytescluster)+mod2)+2) and (numclustererroneos<>0) then
      begin
      writeln('No se puede transferir sistema ya que la zona reservada est  defectuosa'#7);
      siponsistema:=false;
      borrasistema;
      end
	else
      begin
    if nombrefichero2=sys1 then
      begin
      findfirst(unidst+'\*.*',$1+$2+$4);
      handlesys1:=crear(unidst+'\'+sys1,0);
      handlesys2:=crear(unidst+'\'+sys2,0);
      end
    else
      begin
      findfirst(unidst+'\*.*',$1+$2+$4);
      handlesys1:=crear(unidst+'\'+sysibm1,0);
      handlesys2:=crear(unidst+'\'+sysibm2,0);
      end;
    handlepathcommand:=crear(unidst+'\COMMAND.COM',0);
    asm
      push ds
      mov dl,unidadbios
      inc dl
      mov ah,$32
      int $21
      pop ds
      end;
    if (handlesys1=$ffff) or (handlesys2=$ffff) or (handlepathcommand=$ffff) then
      begin
      errorcrearsistema;
      siponsistema:=false;
      borrasistema;
      end
    else
      begin
      asm
	mov ah,$32
	mov dl,unidadbios
	inc dl
	push ds
	int $21
	pop ds
	end;
      escribir(handlesys1,longsys1,buffersys1);
      escribir(handlesys2,longsys2,buffersys2);
      escribir(handlepathcommand,longpathcommand,bufferpathcommand);
      ponfechahora(handlepathcommand,fechahorapathcommand);
      ponfechahora(handlesys1,fechahorasys1);
      ponfechahora(handlesys2,fechahorasys2);
      cerrar(handlepathcommand);
      cerrar(handlesys1);
      cerrar(handlesys2);
    if nombrefichero2=sys1 then
      begin
      cambiatributos(unidst+'\'+sys1,1+2+4+32);
      cambiatributos(unidst+'\'+sys2,1+2+4+32);
      end
    else
      begin
      cambiatributos(unidst+'\'+sysibm1,1+2+4+32);
      cambiatributos(unidst+'\'+sysibm2,1+2+4+32);
      end;
      writeln('Sistema transferido');
    end;
    end;
  end;
  asm
    mov dl,unidadbios
    mov ah,$36
    inc dl
    int $21
    mov sectorescluster,ax
    mov clusterdisponibles,bx
    mov bytesporsector,cx
    mov clusterporunidad,dx
    end;
  if (siextender=false) or (siponsistema=false) then
    begin
    sectorescluster:=psector0^.sectcluster;
    bytesporsector:=psector0^.bytsector;
    clusterdisponibles:=(longint(psector0^.totalsectordisco)-
    (longint(psector0^.secespresrv)+longint(psector0^.sectorespresrvcomie)+
    (longint(psector0^.entradasdirprin) div 16)+
    (longint(psector0^.copiasfat)*longint(psector0^.sectorfat)))) div
    longint(psector0^.sectcluster);
    clusterporunidad:=clusterdisponibles;
    dec(clusterdisponibles,numclustererroneos);
    end;
  if siponsistema=true then
    dec(clusterdisponibles,((longpathcommand div bytescluster)+mod0+
    (longsys1 div bytescluster)+mod1+(longsys2 div bytescluster)+mod2));
  totalbytdisco:=longint(clusterporunidad) * longint(sectorescluster) *
  longint(bytesporsector);
  writeln(#10#13,totalbytdisco,' es el n£mero total de bytes del disco');
  if siponsistema=true then
    writeln(longpathcommand+longsys1+longsys2,' es el numero de bytes ocupados por el sistema');
  if numclustererroneos<>0 then
    writeln(longint(numclustererroneos) * longint(psector0^.sectcluster) *
    longint(psector0^.bytsector),' es el n£mero de bytes erroneos marcados en el formateo');
  writeln((longint(clusterdisponibles)*longint(bytesporsector) *
  longint(sectorescluster)),' es el n£mero de bytes disponibles'#10#13);
  if siextender=false then
    begin
    write('Nombre de volumen :');
    Readln(etiqueta);
    etiquetap:=@etiqueta;
    if etiqueta<>'' then
    asm
      les di,etiquetap
      mov al,es:[di]
      cmp al,12
      jl @@35
      mov al,11
      mov es:[di],al
@@35:
      mov al,es:[di]
      xor ah,ah
      add di,ax {busca final de etiqueta}
      inc di
      mov al,0
      mov es:[di],al {le pone valor 0}
      les di,etiquetap
      mov al,es:[di]
      cmp al,9 {mira si etiqueta menor de 9}
      jl @@34
      add di,12
      push ds
      lds si,etiquetap
      add si,11
      mov cx,3
      std
      repz movsb {mueve etiqueta de etiqueta+9 a etiqueta+10 empezando de etiqueta+12 a etiqueta+9}
      pop ds
      les di,etiquetap
      mov al,es:[di]
      inc al
      mov es:[di],al
      add di,9
      mov al,'.'
      mov es:[di],al {pone en etiqueta+9 el caracter '.'}
@@34:
      end;
      etiqueta2:=unidst+etiqueta; {copia en etiqueta2 la unidad+nombre (ejemplo de unidad:'a:')}
      findfirst(unidst+'\*.*',0);
      handlevol:=crear(etiqueta2,8);
      cerrar(handlevol);
end;
pregunta:
  reiniciadisco;
  write('¨Desea formatear otro disco(S/N)?');
  readln(strotravez);
  strupr(@strotravez);
  if (strotravez='S') or (strotravez='SI') then
    begin
    siotravez:=True;
    if siextender=true then opcion:='E';
    if (siponsistema=true) and (opcion2<>'S') then opcion:='S';
    if sieserror=true then goto sihayerror;
    goto otravez;
    end;
  end.