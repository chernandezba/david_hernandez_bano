program sdiskcopy;
{$M 1024,0,655360}

const
  cabeceras:array [1..18] of byte = (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18);

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
 pcabecera,buffer2,sectorelleno,sector0p,sector2,buftemp,bufpathtemp:pointer;
 bufpistas:array [0..84*2] of pointer;
 totalbytdisco,memoriadisponible,espaciodisco_duro:longint;
 bytespista,estado2,longbufpathtemp:word;
 bytesporsector,numparam:word;
 unidadbiosorigen,unidadbiosdestino,estado,valorsubfunc8,
 valorsubfunc17,pistas,pistasleidas,pistasleer,contador,disco_duro:byte;
 sisubfunc8,formateado,leidosector0,asignadamemoria,utilizadisco_duro:boolean;
 siotravez,siabierto:boolean;
 unidorigen,uniddestino,opcion:string[20];
 pathtemp:string[80];
 strsiotravez:string[2];
 filepathtemp:file;
 psector0,psector2:^sector0;
 paramdisco,paramdisco2:parametrosdrive;

 label otravez;


procedure borrar(_nombre:string);near;
var
  __nombre:string[81];

  begin

asm
  push ds
  lea si,_nombre
  lea di,__nombre
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
  lea dx,__nombre
  inc dx
  mov ah,$41
  int $21
  pop ds
  end;
end;




procedure sibreak;far;

begin

  if siabierto=true then
    begin
    close(filepathtemp);
    borrar(pathtemp);
    end;
  halt;
  end;

procedure sierror;far;

begin
  if exitcode=203 then
    begin
    writeln('Memoria insuficiente'#7);
    erroraddr:=nil;
    end;
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

procedure nohaymemoria;near;
begin

writeln('No hay suficiente memoria'#7);
halt;
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
halt;
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

procedure arreglacabeceras(_sectorpista,_pista,_cara:byte;_p:pointer);near;assembler;
asm
  mov cl,_sectorpista
  mov ch,0
  les di,_p
  mov bh,_pista
  mov bl,_cara
@@1:
  mov es:[di],bh
  inc di
  mov es:[di],bl
  add di,3
  inc ch
  cmp ch,cl
  jl @@1
  end;

procedure mirarsisubfunc8(unidbios:byte);near;assembler;
asm
  mov ah,8
  mov dl,unidbios
  int $13
  mov al,false
  jc @@15
  mov valorsubfunc8,bl
  mov al,true
@@15:
  mov sisubfunc8,al
  end;

procedure prepformatat(unidbios,__secpista,__pistas:byte);near;assembler;
asm
  mov ah,$17
  mov al,valorsubfunc17
  mov dl,unidbios
  int $13
  mov ah,$18
  mov cl,__secpista
  mov ch,__pistas
  mov dl,unidbios
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

procedure accededisco(comando,_pistas,_pistai,caras,_secpista,unidadbios:byte;
mensaje:string);near;
var
  pista,sector,cara,punt:byte;
  bufac:pointer;
  sal:boolean;
  begin

{$I-}
  punt:=0;
  for pista:=_pistai to  _pistai+_pistas-1 do
    for  cara:=0 to caras-1 do
    begin
    if (comando=3) and (utilizadisco_duro=true) then
      blockread(filepathtemp,bufpathtemp^,longbufpathtemp);
    if (comando=2) and (asignadamemoria=false) and (utilizadisco_duro=false)
      then
      getmem(bufpistas[punt],_secpista shl 9);
    write(mensaje,((pista * 100) div (pistas-1)),' %'#13);
    if utilizadisco_duro=false then
      estado:=sierrordisco(comando,unidadbios,cara,pista,1,_secpista,
    bufpistas[punt])
      else
      estado:=sierrordisco(comando,unidadbios,cara,pista,1,_secpista,
    bufpathtemp);
    inc(punt);
      if (estado<>0) then
	begin
	sector:=1;
	sal:=False;
	if utilizadisco_duro=false then bufac:=bufpistas[punt] else
	  bufac:=bufpathtemp;
	writeln('Error en pista ',pista,' cara ',cara,#7);
	while (sal=False) do
	begin
	estado:=sierrordisco(comando,unidadbios,cara,pista,sector,1,bufac);
	inc(sector);
	bufac:=pointer(longint(longint(bufac)+512));
      if (sector>_secpista) then sal:=True;
     end;
  end;
  if (comando=2) and (utilizadisco_duro=true) then
    blockwrite(filepathtemp,bufpathtemp^,longbufpathtemp);
end;
{$I+}
end;

procedure formateaunidad(unidadbios,_sectorpista,_caras,_pistas:byte);near;
var
  _pista,_cara:byte;

  begin

  writeln('Formateando durante la copia');
  {$I-}
  for _pista:=0 to  _pistas-1 do
    for  _cara:=0 to _caras-1 do
    begin
    write('Formateado el ',(_pista * 100) div (_pistas-1),' %'#13);
    arreglacabeceras(_sectorpista,_pista,_cara,pcabecera);
    if sisubfunc8=TRUE then prepformatat(unidadbios,_sectorpista,_pistas);
    estado:=sierrordisco(5,unidadbios,_cara,_pista,1,_sectorpista,
    pcabecera);
  end;
  writeln('                                             ');
  {$I+}
end;
procedure mirarsihaydisco(unidbios:byte;_unidst:string);near;
var
  _estado:byte;

  begin

repeat
    _estado:=sierrordisco(2,unidbios,0,0,1,1,buftemp);
  if _estado=$80 then
    begin
    writeln('Asegurese de que hay un disco en la unidad ',_unidst,' y pulse tecla');
    esperatecla;
    end;
  until _estado<>$80;

  end;

function espaciolibre(_uni:byte):longint;near;
var
  sectorescluster,clusterdisponibles,bytessector:word;

begin

  asm
    mov ah,$36
    mov dl,_uni
    int $21
    mov sectorescluster,ax
    mov clusterdisponibles,bx
    mov bytessector,cx
    end;
  espaciolibre:=longint(sectorescluster)*longint(clusterdisponibles)*
  longint(bytessector);
end;

function buscarespacio:byte;near;
var
  _unidad,_contador:byte;
  _espacio,espacio2:longint;
  tempparam:parametrosdrive;

  begin
  espacio2:=0;
  _espacio:=0;
  _unidad:=0;
  for _contador:=0 to 26 do
  begin
    tempparam.bpb:=0;
    if obtenerparamdisco(_contador+1,@tempparam)=0 then
    if siesdiscoduro(_contador+1)=1 then
      begin
      espacio2:=espaciolibre(_contador+1);
      if espacio2>_espacio then
	begin
	_unidad:=_contador;
	_espacio:=espacio2;
	end;
      end;
    end;
  espaciodisco_duro:=_espacio;
  if _unidad=0 then buscarespacio:=255 else buscarespacio:=_unidad;
end;


begin

  exitproc:=@sierror;
  asm
    push ds
    mov ax,seg(sibreak)
    mov dx,offset(sibreak)
    mov ds,ax
    mov ah,$25
    mov al,$23
    int $21 {setvector}
    pop ds
    end;
  siotravez:=false;
  siabierto:=false;
  numparam:=paramcount;
  writeln('SDISKCOPY V. 1.2. (c) BIT SOFT 25/05/1995');
  getmem(sector0p,512);
  getmem(sector2,512);
  getmem(buftemp,512);
  psector0:=pointer(longint(longint(sector0p)+11));
  psector2:=pointer(longint(longint(sector2)+11));
  if (numparam<2) then
    begin
    writeln('Uso : SDISKCOPY [unidad origen] [unidad destino] [opci¢n]');
    writeln('      Opci¢n : R no usar disco duro si falta memoria');
    writeln;
    exit;
    end;
  unidorigen:=Paramstr(1);
  uniddestino:=Paramstr(2);
  opcion:=Paramstr(3);
  strupr(@unidorigen);
  strupr(@uniddestino);
  utilizadisco_duro:=false;
  if numparam=3 then
    begin
    strupr(@opcion);
    if opcion<>'R' then
      begin
      writeln('Opci¢n no v lida'#7);
      exit;
      end
    else utilizadisco_duro:=false;
    end;
  if (unidorigen[1]>'Z') or (unidorigen[1]<'A') or (length(unidorigen)>2)
  or (unidorigen[2]<>':') then
    begin
    writeln('Unidad origen err¢nea'#7);
    exit;
    end;
  if (length(uniddestino)>2) or (uniddestino[1]>'Z') or (uniddestino[1]<'A')
  or (uniddestino[2]<>':')  then
    begin
    writeln('Unidad destino err¢nea'#7);
    exit;
    end;
  unidadbiosorigen:=byte(unidorigen[1])-byte('A');
  unidadbiosdestino:=byte(uniddestino[1])-byte('A');
  if (siesdiscoduro(unidadbiosorigen+1)=1) or
  (siesdiscoduro(unidadbiosdestino+1)=1) then
    begin
    writeln('Este programa solo admite discos removibles , no discos duros'#7);
    exit;
    end;
  paramdisco.bpb:=0;
  paramdisco2.bpb:=0;
  if (obtenerparamdisco(unidadbiosorigen+1,@paramdisco)<>0)
    then
    begin
    writeln('Unidad origen desconocida o error'#7);
    end;
  if (obtenerparamdisco(unidadbiosdestino+1,@paramdisco2)<>0) then
    begin
    writeln('Unidad destino desconocida o error'#7);
    exit;
    end;
  if (paramdisco.sectorespista=36) or (paramdisco2.sectorespista=36) then
    begin
    writeln('Este programa no soporta unidades de 3« de 2.88MB'#7);
    exit;
    end;
    if (paramdisco.maxcilindros>paramdisco2.maxcilindros) or
     (paramdisco.sectorespista>paramdisco2.sectorespista) or
     (paramdisco.numcabezas>paramdisco2.numcabezas) then
       begin
       writeln('Unidad destino y origen son incompatibles'#7);
       exit;
       end;
otravez:
  leidosector0:=false;
  formateado:=false;
  pistasleidas:=0;
  if siotravez=false then
    begin
    disco_duro:=buscarespacio;
    if disco_duro=255 then utilizadisco_duro:=false;
    end;
  {$I-}
  repeat
    writeln('Inserte disco origen en unidad ',unidorigen,' y pulse tecla');
  esperatecla;
  mirarsihaydisco(unidadbiosorigen,unidorigen);
  if leidosector0=false then
    begin
    leidosector0:=true;
  if (sierrordosdisco(0,unidadbiosorigen,1,0,sector0p)<>0) then
    begin
    writeln('Error de disco'#7);
    end;
  if psector0^.totalsectordisco div psector0^.sectorpista div
  psector0^.caras>paramdisco.maxcilindros+4 then
    begin
    writeln('Formato de disquete origen err¢neo'#7);
    exit;
    end;
  if siotravez=false then
    begin
    Getmem(pcabecera,18*4);
    bytespista:=18 * 512;
    Getmem(sectorelleno,bytespista);
    end;
  rellenamem(sectorelleno,bytespista,$F6);
  inicializacabeceras(psector0^.sectorpista,@cabeceras,pcabecera);
  mirarsisubfunc8(unidadbiosdestino);
  if sisubfunc8=TRUE then
    begin
    if (psector0^.idfat=$fd) and (valorsubfunc8=1) then valorsubfunc17:=1;
    if (psector0^.idfat=$fd) and (valorsubfunc8=2) then valorsubfunc17:=2;
    if (psector0^.idfat=$f9) and (valorsubfunc8=3) then valorsubfunc17:=4;
    if (psector0^.idfat=$f9) and (valorsubfunc8=4) then valorsubfunc17:=5;
    if (psector0^.sectorpista=15) then valorsubfunc17:=3;
    if (psector0^.idfat=$f0) then valorsubfunc17:=6;
    end;
  pistas:=(psector0^.totalsectordisco div psector0^.caras) div
  psector0^.sectorpista;
  totalbytdisco:=longint(psector0^.totalsectordisco)*
  longint(psector0^.bytsector);
  if siotravez=false then memoriadisponible:=maxavail;
  if memoriadisponible>=totalbytdisco then utilizadisco_duro:=false
    else if opcion<>'R' then utilizadisco_duro:=true;
  if utilizadisco_duro=true then
    begin
    if espaciodisco_duro>=totalbytdisco then
      begin
    asm
    lea di,pathtemp
    mov al,2
    mov [di],al
    inc di
    mov al,disco_duro
    add al,'A'
    mov [di],al
    mov al,':'
    inc di
    mov [di],al
    end;
    if siotravez=false then
      begin
    pathtemp:=pathtemp+'\SDISKCOP.$$$';
    longbufpathtemp:=psector0^.sectorpista * psector0^.bytsector;
    assign(filepathtemp,pathtemp);
    {$I-}
    rewrite(filepathtemp,1);
    {$I+}
    if IOResult <>0 then
      begin
      writeln('Error al crear archivo temporal'#7);
      exit;
      end;
    siabierto:=true;
    end;
    pistasleer:=pistas;
    if siotravez=false then getmem(bufpathtemp,18 * 512);
    end
     else
      utilizadisco_duro:=false;
    end;
  if utilizadisco_duro=false then
    pistasleer:=memoriadisponible div (psector0^.caras*psector0^.sectorpista*
  psector0^.bytsector);
  if pistasleer>pistas then pistasleer:=pistas;
  end;
  if pistasleer+pistasleidas>pistas then
    pistasleer:=pistas-pistasleidas;
  accededisco(2,pistasleer,pistasleidas,psector0^.caras,psector0^.sectorpista,
  unidadbiosorigen,'Leido el ');
  asignadamemoria:=true;
  {$I+}
  writeln;
  if utilizadisco_duro=true then seek(filepathtemp,0);
  writeln('Inserte disco destino en unidad ',uniddestino,' y pulse tecla');
  esperatecla;
  mirarsihaydisco(unidadbiosdestino,uniddestino);
  {$I-}
  estado:=sierrordisco(2,unidadbiosdestino,0,0,1,1,sector2);
  if ((estado<>0) or (psector2^.caras<>psector0^.caras) or
  (psector0^.sectorpista<>psector2^.sectorpista) or
  (psector0^.totalsectordisco<>psector2^.totalsectordisco)) and
    (formateado=false) then
    begin
    formateaunidad(unidadbiosdestino,psector0^.sectorpista,psector0^.caras,
    pistas);
    formateado:=true;
    end;
  repeat
  estado:=sierrordisco(3,unidadbiosdestino,0,0,1,1,sector0p);
  if estado=3 then
    begin
    writeln('Disco protegido contra escritura'#7);
    writeln('Retire la protecci¢n y pulse tecla');
    esperatecla;
    end;
  until estado<>3;
  accededisco(3,pistasleer,pistasleidas,psector0^.caras,psector0^.sectorpista,
  unidadbiosdestino,'Escrito el ');
  inc (pistasleidas,pistasleer);
  until pistasleidas>=pistas;
  writeln;
  writeln('Disco copiado');
  write(#10#13'¨Quiere copiar otro disco (S/N)?');
  readln(strsiotravez);
  strupr(@strsiotravez);
  if (strsiotravez='S') or (strsiotravez='SI') then
    begin
    if utilizadisco_duro=true then seek(filepathtemp,0);
    siotravez:=true;
    goto otravez;
    end;
  if utilizadisco_duro=true then
    begin
    close(filepathtemp);
    borrar(pathtemp);
    end;
  {$I+}
  end.