color 7
close
option base 1
on error goto errores
totalclientes=0
totalasientos=0
mestabla=1
semanatabla=1
dim asiento1$(1500)
dim cliente$(21)
dim meses$(13)
restore nombremeses
for n=1 to 12
read a$
meses$(n)=a$
next n
GOSUB MENUCENTRAL
OPEN "ASIENTO1" FOR APPEND AS #1
CLOSE #1
OPEN "ASIENTO1" FOR INPUT AS #1
N=0
IF LOF(1)<>0 THEN
  DO WHILE (EOF(1)=0)
  INPUT #1,B$
  ASIENTO1$(N+1)=B$
  N=N+1
  LOOP
  END IF
totalasientos=n
CLOSE #1
OPEN "CLIENTES" FOR APPEND AS #1
CLOSE#1
OPEN "CLIENTES" FOR INPUT AS #1
N=0
IF LOF(1)<>0 THEN
  DO WHILE (EOF(1)=0)
  INPUT #1,B$
  CLIENTE$(N+1)=B$
  N=N+1
  LOOP
  END IF
totalclientes=n
CLOSE #1
menu:
cls
locate 2,30
?"MENU CENTRAL"
LOCATE 6,25
?TAB (25);"1 - INSERTAR TIEMPO"
?TAB (25);"2 - CREACION CLIENTES"
?TAB (25);"3 - MODIFICAR CLIENTES"
?TAB (25);"4 - MODIFICAR ASIENTOS"
?TAB (25);"5 - LISTAR ASIENTOS"
?TAB (25);"6 - HOJA MES"
?TAB (25);"7 - IR A MENU"
?TAB (25);"8 - ANULAR ASIENTOS"
?TAB (25);"9 - ANULAR CLIENTE"
?TAB (25);"10- SALIR"
OPCIO:
cadena$=""
do while cadena$=""
LOCATE 19,30:? SPC(51):y=19:x=30:longitud=2:gosub dialnum
loop
OPCION=VAL(cadena$)
IF OPCION>10 or opcion<1 THEN GOTO OPCIO
cls
on opcion goto insertar,creaclientes,modificarclientes,modificarasientos,imprimirasientos,hojames,irrun,irborrarasientos,irborrarcliente,irsalir
irsalir:
end
irrun:
run
rem APARTADO DE INSERTAR ASIENTOS
insertar:
restore nombremeses
for n=1 to 12
locate 5+n,10
print using("##");n;
print " ";meses$(n)
next n
Mes=0
DO WHILE (Mes<1 or Mes>12)
locate 12,30:? spc(51):locate 12,30:? "MES ( NUMERICO ):":y=12:x=47:longitud=2:gosub dialnum:Mes=VAL(cadena$)
LOOP
cls
gosub clientes
otroasiento:
gosub oldasiento
for n=0 to 3
locate (n*2)+14,17
?spc(18);
next n
asiento1$(totalasientos+1)=string$(13," ")
LOCATE 10,10:?"INSERTAR"
gosub pantallatiempo
LOCATE 14,17
? TOTALasientos+1
idia:
locate 16,17
?spc(18):x=17:y=16:longitud=2:gosub dialnum:a$=cadena$
if a$="" then dia=val(mid$(date$,4,2)) else dia=val(mid$(a$,1,2))
if dia=0 then goto saleasientos
if dia<1 or dia>31 then goto idia
locate 16,17:entrada$=str$(dia):gosub convierte:dia1$=entrada$
? entrada$;:? "/";:entrada$=str$(mes):gosub convierte
? entrada$:mes1$=entrada$
mid$(asiento1$(totalasientos+1),1,2)=mes1$
mid$(asiento1$(totalasientos+1),3,2)=dia1$
icliente:
x=17:y=18:locate 18,17:? spc (18):longitud=2:gosub dialnum:a$=cadena$
entrada$=str$(val(a$)):gosub convierte
for n=1 to totalclientes
  if entrada$=mid$(cliente$(n),1,2) then goto conti
  next n
locate 18,17
?"NO EXISTE"
beep
gosub esperatecla
goto icliente
conti:
mid$(asiento1$(totalasientos+1),5,2)=entrada$
cadena$=""
do while cadena$=""
locate 20,17:? spc (18):x=17:y=20:longitud=5:gosub dialnup:a$=cadena$
loop
mid$(asiento1$(totalasientos+1),7,5)=mid$(str$(val(a$)),2,5)
totalasientos=totalasientos+1
if totalasientos>=1500 then
  locate 22,1
  ?"NO PUEDO INSERTAR MAS ASIENTOS"
  ?"BORRE ASIENTOS"
  gosub esperatecla
  goto saleasientos
  end if
goto otroasiento
saleasientos:
gosub poneordenando
for n=1 to totalasientos-1
 for m=n+1 to totalasientos
   if val(mid$(asiento1$(n),5,2))>val(mid$(asiento1$(m),5,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   if val(mid$(asiento1$(n),3,2))>val(mid$(asiento1$(m),3,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   if val(mid$(asiento1$(n),1,2))>val(mid$(asiento1$(m),1,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   next m
 next n
if modificar=1 then
  open "ASIENTO1" FOR OUTPUT AS #1
  gosub ponegrabando
  FOR N=1 TO TOTALASIENTOS
  PRINT #1,ASIENTO1$(N)
  NEXT N
  CLOSE#1
END IF
goto menu
rem MUESTRA LOS CLIENTES CREADOS EN PANTALLA
clientes:
locate 1,50:? "Co.":locate 1,56:?"DESCRIPCION"
n=1
do while n<=totalclientes
locate n+1,50:?mid$(cliente$(n),1,2):locate n+1,56:?mid$(cliente$(n),3,22)
n=n+1
loop
return
convierte:
if len(entrada$)=2 then entrada$="0"+mid$(entrada$,2,1) else entrada$=mid$(entrada$,2,2)
return
menucentral:
cls:locate 2,30:? "MENU CENTRAL"
opcionerronea:
locate 12,30:?spc(51):locate 12,30:?"PRUEBAS (SI/NO):":
y=12:x=46:longitud=2:gosub dialalfa:a$=cadena$
if a$<>"s" and a$<>"si" and a$<>"n" and a$<>"no" and a$<>"S" and a$<>"SI" and a$<>"N" and a$<>"NO" then goto opcionerronea
IF A$="S" OR A$="SI" OR A$="si" or a$="s" THEN MODIFICAR=0 ELSE MODIFICAR=1
RETURN
rem APARTADO DE INSERTAR CLIENTES
creaclientes:
locate 1,10:? "INSERTAR CLIENTES"
gosub pantallaclientes
otrocliente:
cliente$(totalclientes+1)=string$(24," ")
for n=0 to 3
locate (N*2)+14,25:?spc(56);
next n
gosub clientes
locate 14,25:? totalclientes
encontrado=1
do while encontrado=1
locate 16,25:?spc(8)
x=25:y=16:longitud=2:gosub dialnum:a$=cadena$
entrada$=str$(val(a$))
gosub convierte
if entrada$="00" then encontrado=0
for n=1 to totalclientes
if entrada$=mid$(cliente$(n),1,2) then
locate 16,25
?"EXISTE"
beep
gosub esperatecla
encontrado=1
goto conti2
end if
next n
encontrado=0
conti2:
loop
if entrada$="00" then goto saleclientes
mid$(cliente$(totalclientes+1),1,2)=entrada$
locate 18,25:?spc(56)
x=25:y=18:longitud=22:gosub dialalfa:a$=cadena$
mid$(cliente$(totalclientes+1),3,22)=a$
totalclientes=totalclientes+1
if totalclientes>20 then
  locate 14,25
  print "NO CABEN MAS CLIENTES"
  beep
  gosub esperatecla
  goto saleclientes
  end if
goto otrocliente
saleclientes:
gosub poneordenando
for n=1 to totalclientes-1
  for m=n+1 to totalclientes
  if val(mid$(cliente$(n),1,2))>val(mid$(cliente$(m),1,2)) then
  a$=cliente$(n)
  b$=cliente$(m)
  cliente$(n)=b$
  cliente$(m)=a$
  end if
  next m
 next n
if modificar=1 then
  gosub ponegrabando
  open "CLIENTES" for output as #1
  for n=1 to totalclientes
  print #1,cliente$(n)
  next n
  close #1
  end if
goto menu
poneordenando:
locate 24,30:color 31:? "ORDENANDO ...";:COLOR 7:return
ponegrabando:
color 31:locate 24,30:? "GRABANDO DATOS...";:color 7:return
rem PANTALLA DE ASIENTOS
pantallatiempo:
locate 14,1
? "N.ASIENTO      :"
locate 16,1
? "DIA ( 0 SALIR ):"
locate 18,1
? "CODIGO CLIENTE :"
locate 20,1
? "TOTAL HORAS    :"
return
rem PANTALLA DE CLIENTES
pantallaclientes:
locate 14,1
? "N. CLIENTE             :"
?
? "CODIGO     ( 0 SALIR ) :"
?
? "DESCRIPCION            :":return
rem RUTINA DE ESPERAR TECLA
esperatecla:
tecla$=""
  do while tecla$=""
  tecla$=inkey$
  loop
return
rem APARTADO DE MODIFICAR CLIENTES
modificarclientes:
locate 1,10:? "MODIFICAR CLIENTES"
gosub pantallaclientes
otrabusqueda:
locate 18,25:? spc(22)
locate 16,25:?spc(22)
gosub clientes
x=25:y=16:longitud=2:gosub dialalfa:a$=cadena$
if a$="" then goto saleclientes
entrada$=str$(val(a$)):gosub convierte
if entrada$="00" then goto saleclientes
porcodigo:
for n=1 to totalclientes
if mid$(cliente$(n),1,2)=entrada$ then
  puntero=n
  goto encontradadescripcion
  end if
next n
locate 16,25
?spc(56)
locate 16,25
? "NO ENCONTRADO"
beep
gosub esperatecla
goto otrabusqueda
encontradadescripcion:
locate 18,25:?mid$(cliente$(puntero),3,22)
locate 16,25:?mid$(cliente$(puntero),1,2)
locate 19,1
? "NUEVA DESCRIPCION      :"
locate 19,25:?spc(22):x=25:y=19:longitud=22:gosub dialalfa:a$=cadena$
if a$="" then a$=mid$(cliente$(puntero),3,22)
antiguocliente$=cliente$(puntero)
cliente$(puntero)=string$(24," ")
mid$(cliente$(puntero),1,2)=mid$(antiguocliente$,1,2)
mid$(cliente$(puntero),3,22)=a$
locate 19,1:?spc(47)
locate 17,1:?spc(47)
goto otrabusqueda
borrarcliente:
aborrar=0
locate 23,1
? "ANULAR CLIENTE (S/N):"
a$="e"
do while a$<>"s" and a$<>"si" and a$<>"SI" and a$<>"S" and a$<>"n" and a$<>"no" and a$<>"N" and a$<>"NO"
  locate 23,22:?spc(59):x=22:y=23:longitud=2:gosub dialalfa:a$=cadena$
  loop
if a$="s" or a$="si" or a$="SI" or a$="S" then
  aborrar=1
  for n1=puntero to totalclientes-1
  cliente$(n1)=cliente$(n1+1)
  next n1
  totalclientes=totalclientes-1
  end if
locate 23,1:?spc(79):return
rem APARTADO DE MODIFICAR ASIENTOS
modificarasientos:
locate 1,30:?"MODIFICAR ASIENTOS"
gosub pantallatiempo
gosub clientes
numeroasiento:
locate 13,17:?spc(20):longitud=3:x=17:y=14:gosub dialnum:a$=cadena$
if val(a$)=0 then goto saleasientos
if val(a$)>totalasientos or val(a$)<1 then goto numeroasiento
puntero=val(a$)
gosub modificaasiento
goto numeroasiento
muestradatos:
locate 15,17:?spc(20):locate 16,17:?mid$(asiento1$(puntero),3,2);"/";mid$(asiento1$(puntero),1,2)
locate 17,17:?spc(20):locate 18,17:?mid$(asiento1$(puntero),5,2)
locate 19,17:?spc(20):locate 20,17:?mid$(asiento1$(puntero),7,5)
return
modificaasiento:
gosub muestradatos
locate 8,1:?"NUEVO DIA     :           "
locate 10,1:?"NUEVO CLIENTE :           "
locate 12,1:?"NUEVO TIEMPO  :           "
nuevodia2:
locate 8,16:?spc(20):longitud=2:x=16:y=8:gosub dialnum:a$=cadena$
if a$="0" then return
if a$="" then goto nuevodia2
nuevodia=val(mid$(a$,1,2))
if nuevodia<1 or nuevodia>31 then goto nuevodia2
locate 8,16:entrada$=str$(nuevodia):gosub convierte:nuevodia$=entrada$
? entrada$;:? "/";mid$(asiento1$(puntero),1,2)
mid$(asiento1$(puntero),3,2)=nuevodia$
nuevocliente:
locate 10,16:? spc (18):longitud=2:x=16:y=10:gosub dialnum:a$=cadena$
if a$="" then a$=mid$(asiento1$(puntero),5,2)
entrada$=str$(val(a$)):gosub convierte
for n=1 to totalclientes
  if entrada$=mid$(cliente$(n),1,2) then goto conti3
  next n
locate 10,17
?"NO EXISTE"
beep
gosub esperatecla
goto nuevocliente
conti3:
mid$(asiento1$(puntero),5,2)=entrada$
locate 12,16:?entrada$
a$=string$(6," ")
x=16:y=12:longitud=5:gosub dialnup:a$=cadena$
if a$="" then a$=mid$(asiento1$(puntero),7,5)
antiguoasiento$=asiento1$(puntero)
asiento1$(puntero)=string$(11," ")
mid$(asiento1$(puntero),1,6)=mid$(antiguoasiento$,1,6)
mid$(asiento1$(puntero),7,5)=mid$(str$(val(a$)),2,5)
locate 12,16:?mid$(asiento1$(puntero),7,5)
return
rem APARTADO DE IMPRIMIR ASIENTOS
imprimirasientos:
if totalasientos=0 then goto menu
locate 10,30:?"POR PANTALLA O IMPRESORA":locate 11,30:?"(P/I ENTER SALIR):"
cadena$="q"
do while cadena$<>"p" and cadena$<>"i" and cadena$<>"P" and cadena$<>"I" and cadena$<>""
x=49:y=11:longitud=1:gosub dialalfa
loop
if cadena$="" then goto menu
if cadena$="p" or cadena$="P" then
impresora=0
else
impresora=1
end if
pidemes:
restore nombremeses
for n=1 to 12
locate 6+n,5
print using("##");n;
print " ";meses$(n)
next n
locate 12,30:?"MES ( 13 TODOS , 0 SALIR) :"
locate 12,57
?spc(5);
x=57:y=12:longitud=2:gosub dialnum
if val(cadena$)>13 then goto pidemes
if val(cadena$)=0 then goto menu
entrada$=str$(val(cadena$)):gosub convierte
mes$=entrada$
mes=val(entrada$)
horas=0
totalhoras=0
for z=1 to totalasientos
  if mid$(asiento1$(z),1,2)=mes$ or mes=13 then
  puntero=z
  goto siasiento3
  end if
  next z
locate 13,30:
print "ASIENTOS DEL MES DE ";meses$(mes);" NO ENCONTRADOS";
beep
gosub esperatecla
locate 13,30:?spc(49);
goto pidemes
siasiento3:
cls
if impresora=1  then
  locate 12,20:?"PREPARE IMPRESORA Y PULSE TECLA"
  a$=""
  do while a$=""
  a$=inkey$
  loop
  cls
  locate 12,30:?"IMPRIMIENDO..."
  open "LPT1:" for output as #1
  width #1,132
  print #1,chr$(15)
  end if
if totalasientos=1 then goto menu
if mes=13 then
  mesi=1
  mesf=12
  else
  mesi=mes
  mesf=mes
  end if
puntero=1:antpuntero=1
totalhoras=0.0#
asumar=0.0#
for m=mesi to mesf
  puntero=1
  do while val(mid$(asiento1$(puntero),1,2))<>m and puntero<=totalasientos
  puntero=puntero+1
  loop
  if val(mid$(asiento1$(puntero),1,2))=m then encontrado=1 else encontrado=0
  if impresora=1 and encontrado=1 then
  print #1,tab(40);"LISTADO CLIENTES , MES : ";meses$(m)
  print #1,
  end if
  if impresora=0 and encontrado=1 then
  print tab(30);"MES : ";meses$(m)
  print:print
  end if
  clienterel=1
  if encontrado=1 and puntero<=totalasientos then
for n=1 to totalclientes
  puntero=1
  do while (val(mid$(asiento1$(puntero),1,2))<>m or val(mid$(asiento1$(puntero),5,2))<>n) and puntero<=totalasientos
  puntero=puntero+1
  loop
  if val(mid$(asiento1$(puntero),1,2))=m then encontrado=1 else encontrado=0
  columna=0
  fila=0
  horas=0.0#
  if val(mid$(asiento1$(puntero),5,2))=n then
  if impresora=0 then locate 3,1
      if impresora=0 and encontrado=1 and puntero<=totalasientos then
      print:print mid$(cliente$(n),1,2);" ";mid$(cliente$(n),3,22)
      end if
      if impresora=1 and encontrado=1 and puntero<=totalasientos then
      print #1,:print #1,mid$(cliente$(n),1,2);" ";mid$(cliente$(n),3,22)
      end if
      do while (m=val(mid$(asiento1$(puntero),1,2)) and puntero<=totalasientos) and encontrado=1
      if val(mid$(asiento1$(puntero),5,2))=n then
      asumar=val(mid$(asiento1$(puntero),7,5))
        if impresora=0 then
        print "N.";
        print using "####";puntero;
        print " Dia  ";mid$(asiento1$(puntero),3,2);" ";"Horas ";
        print using "##.##";asumar;
        print " ";
        horas=horas+asumar
        totalhoras=totalhoras+asumar
        columna=columna+1
        if columna=3 then
          columna=0
          fila=fila+1
          locate fila+5,1
          end if
        end if
      if impresora=1 then
        print #1,"N.";
        print #1,using "####";puntero;
        print #1," Dia ";mid$(asiento1$(puntero),3,2);" ";"Horas ";
        print #1,using "##.##";asumar;
        print #1," ";
        horas=horas+asumar
        totalhoras=totalhoras+asumar
        columna=columna+1
        if columna=5 then
          print #1,
          columna=0
          fila=fila+1
          end if
        end if
      if fila=16 and impresora=0 then
        print tab(35);"PULSE TECLA PARA CONTINUAR"
        beep
        gosub esperatecla
        fila=1
        end if
      end if
      puntero=puntero+1
      loop
    if encontrado=1 and impresora=0 then
   print :
   print tab(30);"TOTAL HORAS :";
   print using "###.##";horas
   print tab(30);"PULSE TECLA":beep:gosub esperatecla
   locate 3,1
   for nn=3 to 21:?spc(79):next nn
   end if
   if encontrado=1 and impresora=1 then
   for tt=1 to 6-fila
   print #1,
   next tt
   print #1,tab(80);"TOTAL HORAS :";
   print #1,using "###.##";horas
   print #1,
   clienterel=clienterel+1
   fila=1
   columna=1
   if clienterel=7 then
   beep
   print #1,chr$(12)
   print tab(35);"CAMBIE PAPEL Y PULSE TECLA"
   gosub esperatecla
   print #1,tab(40);"LISTADO CLIENTES , MES : ";meses$(m)
   print #1,
   clienterel=1
   end if
   end if
end if
  next n
if impresora=0 then
   print :print "PULSE TECLA":beep:gosub esperatecla
   cls
   end if
if impresora=1 then
   beep
   print #1,chr$(12)
   print tab(35);"CAMBIE PAPEL Y PULSE TECLA"
   gosub esperatecla
   end if
end if
next m
if impresora=0 then
print
locate 12,30
print "SUMA TOTALES DE HORAS :";
print using "###.##";totalhoras
gosub esperatecla
end if
if impresora=1 then
print #1,
print #1,
print #1,tab(80);"SUMA TOTALES DE HORAS :";
print #1,using "###.##";totalhoras
print #1,chr$(18);chr$(12)
close #1
end if
goto menu
rem RUTINA DE MANEJO DE ERRORES
errores:
beep
if err=27 then
  ?"FIN DE PAPEL , CAMBIE EL PAPEL Y PULSE TECLA"
  gosub esperatecla
  resume
  end if
if err=57 then
  ?"ERROR DE ENTRADA/SALIDA , REINTENTAR , IGNORAR O ABANDONAR(R/I/A) ";
  otratecla2:
  tecla$=""
  do while tecla$=""
  tecla$=inkey$
  loop
  if tecla$="r" or tecla$="R" then print tecla$:resume
  if tecla$="i" or tecla$="I" then print tecla$:resume next
  if tecla$="a" or tecla$="A" then print tecla$:close:stop
  goto otratecla2
  else
  ?"ERROR :";ERR;" EN LINEA:";ERL;" EN DIRECCION:";ERADR
  close
  stop
  end if
rem APARTADO DE IMPRIMIR LA HOJA DEL MES
hojames:
if totalasientos=0 then goto menu
pidemes2:
restore nombremeses
for n=1 to 12
locate 6+n,5
print using("##");n;
print " ";meses$(n)
next n
locate 12,30:?"MES A IMPRIMIR HOJA ( 0 SALIR ):"
locate 12,63
?spc(10);
x=63:y=12:longitud=2:gosub dialnum
if val(cadena$)>12 then goto pidemes2
entrada$=str$(val(cadena$)):gosub convierte
if entrada$="00" then goto menu
mes$=entrada$
mes=val(entrada$)
horas=0
totalhoras=0
for z=1 to totalasientos
  if mid$(asiento1$(z),1,2)=mes$ or mes=0 then
  puntero=z
  goto siasiento4
  end if
  next z
locate 13,30:
print "ASIENTOS DEL MES DE ";meses$(mes);" NO ENCONTRADOS"
beep
gosub esperatecla
locate 13,30:?spc(49);
goto pidemes2
siasiento4:
cls
locate 12,20:?"PREPARE IMPRESORA Y PULSE TECLA"
gosub esperatecla
cls
locate 12,30:?"IMPRIMIENDO..."
open "LPT1:" for output as #1
width #1,132
print #1,chr$(15)
puntero=1:antpuntero=1
totalhoras=0.0#
asumar=0.0#
m=mes
puntero=1
do while val(mid$(asiento1$(puntero),1,2))<>m and puntero<=totalasientos
puntero=puntero+1
loop
if val(mid$(asiento1$(puntero),1,2))=m then encontrado=1 else encontrado=0
print #1,tab(40);"HOJA MES : ";meses$(m)
print #1,
clienterel=1
if encontrado=1 and puntero<=totalasientos then
for n=1 to totalclientes
  puntero=1
  do while (val(mid$(asiento1$(puntero),1,2))<>m or val(mid$(asiento1$(puntero),5,2))<>n) and puntero<=totalasientos
  puntero=puntero+1
  loop
  if val(mid$(asiento1$(puntero),1,2))=m then encontrado=1 else encontrado=0
  columna=0
  fila=0
  horas=0.0#
  if val(mid$(asiento1$(puntero),5,2))=n then
      if encontrado=1 and puntero<=totalasientos then
      print #1,:print #1,mid$(cliente$(n),1,2);" ";mid$(cliente$(n),3,22)
      end if
      do while (m=val(mid$(asiento1$(puntero),1,2)) and puntero<=totalasientos) and encontrado=1
      if val(mid$(asiento1$(puntero),5,2))=n then
      asumar=val(mid$(asiento1$(puntero),7,5))
        print #1," Dia ";mid$(asiento1$(puntero),3,2);" ";"Horas ";
        print #1,using "##.##";asumar;
        print #1," ";
        horas=horas+asumar
        totalhoras=totalhoras+asumar
        columna=columna+1
        if columna=6 then
          print #1,
          columna=0
          fila=fila+1
          end if
      end if
      puntero=puntero+1
      loop
   if encontrado=1 then
   for tt=1 to 6-fila
   print #1,
   next tt
   print #1,tab(80);"TOTAL HORAS :";
   print #1,using "###.##";horas
   print #1,
   clienterel=clienterel+1
   fila=1
   columna=1
   if clienterel=7 then
   beep
   print #1,chr$(12)
   print tab(35);"CAMBIE PAPEL Y PULSE TECLA"
   gosub esperatecla
   print #1,tab(40);"HOJA MES : ";meses$(m)
   print #1,
   clienterel=1
   end if
   end if
end if
  next n
end if
print #1,
print #1,
print #1,chr$(18);chr$(27);chr$(69);
print #1,tab(20);"SUMA DE TOTALES DE HORAS .........  ";
print #1,using "###.##";totalhoras;
print #1,chr$(27);chr$(70);chr$(12)
close #1
goto menu
rem MESES DEL ANYO
nombremeses:
data ENERO,FEBRERO,MARZO,ABRIL,MAYO,JUNIO,JULIO
data AGOSTO,SEPTIEMBRE,OCTUBRE,NOVIEMBRE,DICIEMBRE
rem APARTADO DE BORRAR ASIENTOS
irborrarasientos:
locate 1,30:?"BORRAR ASIENTOS"
locate 12,1:?"MES ( 0 SALIR , 13 TODOS ) :"
borrarasientos2:
locate 12,29:?spc(52)
longitud=2:x=29:y=12:gosub dialnum
if val(cadena$)>13 or val(cadena$)<1 then goto borrarasientos2
entrada$=str$(val(cadena$)):gosub convierte
if entrada$="00" then goto menu
if entrada$="13" then
  totalasientos=0
  goto saleasientos
  end if
n=1
cls
color 16+15
locate 12,30:? "BORRANDO ASIENTOS"
color 7
for n=1 to totalasientos
  if mid$(asiento1$(n),1,2)=entrada$ then
    for m=n to totalasientos-1
      asiento1$(m)=asiento1$(m+1)
      next m
    totalasientos=totalasientos-1
    end if
next n
goto saleasientos

rem rutinas del dial
rem en todas entrada x,y,longitud
rem          salida lontotal,cadena$
rem dialalfa:dial que admite los codigos 32-255
rem dialnum:dial que admite los codigos 48-57 (teclas numericas)
rem dialnup:dial que admite los codigos 48-57 y "."
rem todas ademas admiten el 8 y 13

dialalfa:
codigoi=32:codigom=255:punto=1
goto dial
dialnum:
codigoi=48:codigom=57:punto=0:goto dial
dialnup:
codigoi=48:codigom=57:punto=1:goto dial
dial:
cadena$="": lontotal=0:teclapunto=0
bucle1:
locate y,x:color 16+7:?"_ ":color 7
bucle2:
tecla$=inkey$
 if len(tecla$)=0 then goto bucle2
 codtec=asc(tecla$)
 if codtec=13 then locate y,x:?" ";:return
 if codtec=8 then goto teclaborr
 if lontotal=longitud then goto bucle2
  if tecla$="." then
   if teclapunto or punto=0 then goto bucle2
   teclapunto=1:goto sitecla
  end if
 if codtec<codigoi or codtec>codigom then beep:goto bucle2
sitecla:
 locate y,x:? tecla$
 x=x+1:lontotal=lontotal+1:cadena$=cadena$+tecla$
 goto bucle1
teclaborr:
 if lontotal=0 then goto bucle2
 if mid$(cadena$,lontotal,1)="." then teclapunto=0
 x=x-1:lontotal=lontotal-1:cadena$=mid$(cadena$,1,lontotal):goto bucle1
rem APARTADO DE ANULAR CLIENTES
irborrarcliente:
cls
locate 1,20:?"ANULAR CLIENTE"
gosub clientes:gosub pantallaclientes
locate 16,25:?spc(22)
locate 18,25:?spc(22)
x=25:y=16:longitud=2:gosub dialnum
if val(cadena$)=0 then
  gosub poneordenando
  for n=1 to totalasientos-1
  for m=n+1 to totalasientos
   if val(mid$(asiento1$(n),5,2))>val(mid$(asiento1$(m),5,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   if val(mid$(asiento1$(n),3,2))>val(mid$(asiento1$(m),3,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   if val(mid$(asiento1$(n),1,2))>val(mid$(asiento1$(m),1,2)) then
   a$=asiento1$(n)
   b$=asiento1$(m)
   asiento1$(n)=b$
   asiento1$(m)=a$
   end if
   next m
 next n
if modificar=1 then
  open "ASIENTO1" FOR OUTPUT AS #1
  gosub ponegrabando
  FOR N=1 TO TOTALASIENTOS
  PRINT #1,ASIENTO1$(N)
  NEXT N
  CLOSE#1
  END IF
  goto saleclientes
  end if
entrada$=str$(val(cadena$)):gosub convierte
for n=1 to totalclientes
  if mid$(cliente$(n),1,2)=entrada$ then
  puntero=n
  locate 16,25:?mid$(cliente$(puntero),1,2)
  locate 18,25:?mid$(cliente$(puntero),3,22)
  oldcliente$=cliente$(puntero)
  gosub borrarcliente
  if aborrar=1 then gosub borrarasientosrelacionados
  goto irborrarcliente
  end if
  next n
locate 16,25:?"NO ENCONTRADO":beep:gosub esperatecla:goto irborrarcliente
oldasiento:
locate 1,1
? "ULTIMO ASIENTO"
if totalasientos<>0 then
locate 2,1
? "N.ASIENTO      :";totalasientos;
locate 4,1
? "DIA ( 0 SALIR ):";mid$(asiento1$(totalasientos),3,2);"/";mid$(asiento1$(totalasientos),1,2);
locate 6,1
? "CODIGO CLIENTE :";mid$(asiento1$(totalasientos),5,2);
locate 8,1
? "TOTAL HORAS    :";mid$(asiento1$(totalasientos),7,5);
end if
return
borrarasientosrelacionados:
locate 23,1:color 16+15:?"BORRANDO ASIENTOS RELACIONADOS"
color 7
nn=1
buc111:
if mid$(asiento1$(nn),5,2)=mid$(oldcliente$,1,2) then
  for mm=nn to totalasientos-1
  asiento1$(mm)=asiento1$(mm+1)
  next mm
  totalasientos=totalasientos-1:nn=nn-1
end if
nn=nn+1
if nn<=totalasientos then goto buc111
locate 23,1:?spc(29);
return