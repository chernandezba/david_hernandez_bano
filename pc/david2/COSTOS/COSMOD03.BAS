DEFINT A-Z

'$INCLUDE: 'costos.inc'

DECLARE SUB imprimetelefonos (opcion1 AS INTEGER, opcion3 AS STRING, tipocambiohoja AS INTEGER)

panttaller:
DATA 6
DATA 31, 3, "Taller"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Precio por hora:"
DATA 30,7,"Precio por maquina:"
DATA 5,8,"[ESC Salir]"

campostallerinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 4
DATA 6,10,4
DATA 6,29,30
DATA 7,19,10
DATA 7,50,9

panttelefonos:
DATA 26
DATA 31,3,"Clientes/Proveedores"
DATA 2, 6, "C¢digo"
DATA 16, 6, "Empresa"
DATA 55, 6, "C.I.F."
DATA 2, 7, "Cargo"
DATA 24, 7, "Nombre"
DATA 45, 7, "Apellidos"
DATA 2, 8, "Direcci¢n"
DATA 43, 8, "Localidad"
DATA 2, 9, "Provincia"
DATA 38, 9, "Pa¡s"
DATA 64, 9, "C. Postal"
DATA 2,10, "Tel‚fono1"
DATA 29,10, "Tel‚fono2"
DATA 55,10, "FAX"
DATA 2, 11, "Actividad"
DATA 32, 12, "Domiciliaci¢n pagos"
DATA 2, 13, "Entidad"
DATA 2, 14, "Direcci¢n"
DATA 2, 15, "Localidad"
DATA 35, 15, "Tel‚fono"
DATA 2, 16, "Provincia"
DATA 30, 16, "C. Postal"
DATA 2, 17, "C¢digo cuenta"
DATA 39, 18, "Notas"
DATA 5,22,"[ESC] Salir"

campostelefonosinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 23
DATA 6, 9,2
DATA 6, 24,30
DATA 6, 62,15
DATA 7, 8,15
DATA 7, 31,13
DATA 7, 55,19
DATA 8, 12,30
DATA 8, 53,22
DATA 9, 12,17
DATA 9, 43,20
DATA 9, 74,5
DATA 10, 12,15
DATA 10, 39,15
DATA 10, 59,15
DATA 11, 12,30
DATA 13, 10,30
DATA 14, 12, 30
DATA 15, 12, 22
DATA 15, 44, 15
DATA 16, 12, 17
DATA 16, 40, 5
DATA 17, 16, 23
DATA 19, 2, 234
'234=(80-2)*3

errorexportar:
poncolor 1, 4
box 12, 20, 15, 60
LOCATE 12, 40
PRINT "Error";
LOCATE 13, 21
errorfichero = 1
SELECT CASE ERR
  CASE 70
  'acceso denegado o solo lectura
  PRINT "Acceso denegado / Protecci¢n contra escritura";
  CASE 71
  'error de disco
  PRINT "Error al acceder al disco";
  CASE 57
  'error de E/S
  PRINT "Error de lectura/escritura"
  CASE ELSE
  ERROR ERR
  END SELECT
LOCATE 14, 21
PRINT "Pulse tecla";
BEEP
pu$ = getkey$
RESUME NEXT

errorcom:
box 12, 14, 15, 66
SELECT CASE ERR
        CASE 24, 25:
        LOCATE 13, 15: PRINT "Modem no responde: no est  conectado o no hay modem"
        LOCATE 14, 31: PRINT "Pulsa tecla"
        BEEP
        pu$ = getkey$
        RESUME NEXT
        CASE ELSE
        RESUME NEXT
        END SELECT
' Error de impresora
errorimpresora:
    SELECT CASE ERR
        CASE 24, 25
            a$ = errorimpresora$
            IF a$ = CHR$(27) THEN
              saleimprimir = 1
              RESUME NEXT
              END IF
            ponimprimiendo
            IF a$ = "R" THEN RESUME
            IF a$ = "I" THEN RESUME NEXT
        CASE ELSE
        ERROR ERR
    END SELECT

DEFSNG A-Z
FUNCTION haydomiciliacion% (pagos2 AS pagos)
DIM pagos1 AS pagos
DIM comp AS INTEGER

comp = strcmp%(pagos1.entidad, pagos2.entidad) <> 0 OR strcmp%(pagos1.direccionb, pagos2.direccionb) <> 0 OR strcmp%(pagos1.localidadb, pagos2.localidadb) <> 0
comp = comp OR strcmp%(pagos1.telefonob, pagos2.telefonob) <> 0 OR strcmp%(pagos1.provinciab, pagos2.provinciab) <> 0 OR strcmp%(pagos1.postale, pagos2.postale) <> 0
comp = comp OR strcmp%(pagos1.codigocuen, pagos2.codigocuen) <> 0
haydomiciliacion% = comp
END FUNCTION

FUNCTION haynotas% (notas2 AS notas)
DIM notas1 AS notas
DIM comp AS INTEGER

comp = strcmp%(notas1.notas, notas2.notas) <> 0
haynotas% = comp
END FUNCTION

'el 1er par metro opcion1 es si se imprime ficha(1),listin(2) o sobres(3)
'el 3er par metro es lo que se imprime si no hay nombre
'tipocambiohoja:si =1 cambio de hoja autom tico,si =2 cambio de hoja manual
SUB imprimetelefonos (opcion1 AS INTEGER, opcion3 AS STRING, tipocambiohoja AS INTEGER)

STATIC linea2 AS STRING * 78
STATIC barra AS STRING * 1
STATIC temi AS INTEGER
STATIC separador AS STRING * 80
STATIC ancholin AS INTEGER
STATIC tec AS STRING

initkeyesc
ON ERROR GOTO errorimpresora
linea2 = STRING$(78, "Í")
barra = "º"
separador = "Ì" + linea2 + "¹"
SELECT CASE opcion1
  CASE 1
  'Ficha
  WIDTH LPRINT 81
  LPRINT TAB(14); CHR$(14); "FICHERO CLIENTES/PROVEEDORES"; CHR$(20)
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT "É"; linea2; "»"
  IF saleimprimir = 1 THEN GOTO finimptel
  temi = 39 - LEN(RTRIM$(cliente(1).empresa)) / 2
  LPRINT barra; SPACE$(temi); cliente(1).empresa; SPACE$(80 - temi - LEN(cliente(1).empresa) - 2); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT separador
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "C¢digo "; cliente(1).codcliente; "     Empresa "; cliente(1).empresa; " C.I.F. "; cliente(1).cif; "   "; barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Cargo "; cliente(1).cargo; " Nombre "; cliente(1).nombre; " Apellidos "; cliente(1).apellidos; SPACE$(6); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Direcci¢n "; cliente(1).direccion; " Localidad "; cliente(1).localidad; "     "; barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Provincia "; cliente(1).provincia; " Pa¡s "; cliente(1).pais; " C. Postal "; cliente(1).postal; SPACE$(9); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Tel‚fono1 "; cliente(1).telefono; " Tel‚fono2 "; cliente(1).telefono2; " FAX "; cliente(1).fax; SPACE$(7); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Actividad "; cliente(1).actividad; SPACE$(38); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT separador
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Entidad "; pagos(1).entidad; SPACE$(40); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Direcci¢n "; pagos(1).direccionb; SPACE$(38); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Localidad "; pagos(1).localidadb; " Tel‚fono "; pagos(1).telefonob; SPACE$(21); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "Provincia "; pagos(1).provinciab; " C. Postal "; pagos(1).postale; SPACE$(35); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; "C¢digo cuenta "; pagos(1).codigocuen; SPACE$(41); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT separador
  ancholin = 78
  LPRINT barra; LEFT$(notas(1).notas, ancholin); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; MID$(notas(1).notas, 1 + ancholin, ancholin); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT barra; MID$(notas(1).notas, 1 + ancholin + ancholin, ancholin); barra
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT "È"; linea2; "¼"
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT
  lineaimpresora = lineaimpresora + 30
  IF lineaimpresora > 60 THEN
    lineaimpresora = 1
    LPRINT CHR$(13) + CHR$(12)
    IF saleimprimir = 1 THEN GOTO finimptel
    IF tipocambiohoja = 2 THEN
      temi = cambiohoja%
      IF temi = 0 THEN
        GOTO finimptel
        END IF
      END IF
    END IF
  IF saleimprimir = 1 THEN GOTO finimptel
  CASE 2
  'Listin
  WIDTH LPRINT 138
  IF nuevahoja = 1 THEN
    LPRINT
    IF saleimprimir = 1 THEN GOTO finimptel
    LPRINT
    IF saleimprimir = 1 THEN GOTO finimptel
    LPRINT TAB(14); CHR$(14); "LISTIN TELEFONICO"; CHR$(20)
    IF saleimprimir = 1 THEN GOTO finimptel
    LPRINT
    IF saleimprimir = 1 THEN GOTO finimptel
    LPRINT CHR$(15); "C¢digo Empresa"; SPACE$(24); "Cargo"; SPACE$(11); "Nombre"; SPACE$(8); "Apellidos"; SPACE$(11); "Tel‚fono1"; SPACE$(7); "Tel‚fono2"; SPACE$(7); "FAX"
    IF saleimprimir = 1 THEN GOTO finimptel
    nuevahoja = 0
    END IF
  LPRINT CHR$(15); cliente(1).codcliente; "     "; cliente(1).empresa; " "; cliente(1).cargo; " "; cliente(1).nombre; " "; cliente(1).apellidos; " "; cliente(1).telefono; " "; cliente(1).telefono2; " "; cliente(1).fax; CHR$(18)
  IF saleimprimir = 1 THEN GOTO finimptel
  lineaimpresora = lineaimpresora + 1
  IF lineaimpresora = 50 THEN
    LPRINT CHR$(13) + CHR$(12)
    IF saleimprimir = 1 THEN GOTO finimptel
    IF tipocambiohoja = 2 THEN
      temi = cambiohoja%
      IF temi = 0 THEN
        GOTO finimptel
        END IF
      END IF
    nuevahoja = 1
    lineaimpresora = 0
    END IF
  CASE 3
  'sobres
  poncolor 1, 4
  box 10, 5, 13, 76
  center 11, "Prepare el sobre y pulse tecla ... ESC abortar"
  tec = getkey$
  IF tec = CHR$(27) THEN
    saleimprimir = 1
    GOTO finimptel
    END IF
  box 10, 5, 13, 76
  center 11, "Imprimiendo sobre ..."
  LPRINT TAB(45); cliente(1).empresa
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT TAB(45); cliente(1).direccion
  IF saleimprimir = 1 THEN GOTO finimptel
  tec = RTRIM$(cliente(1).localidad)
  LPRINT TAB(45); cliente(1).postal; " "; tec
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT TAB(45); cliente(1).provincia
  IF saleimprimir = 1 THEN GOTO finimptel
  LPRINT TAB(45); "Att. ";
  IF saleimprimir = 1 THEN GOTO finimptel
  IF RTRIM$(cliente(1).nombre) = "" THEN
    LPRINT opcion3
    ELSE
    LPRINT "Sr/a "; RTRIM$(cliente(1).nombre); " "; RTRIM$(cliente(1).apellidos)
    END IF
    IF saleimprimir = 1 THEN GOTO finimptel
    LPRINT CHR$(13) + CHR$(12)
    IF saleimprimir = 1 THEN GOTO finimptel
  END SELECT
finimptel:
donekeyesc
ON ERROR GOTO 0
END SUB

SUB modem (numpuerto AS INTEGER, mandato AS STRING)
DIM cmodem AS INTEGER

cmodem = FREEFILE
ON ERROR GOTO errorcom
OPEN "COM" + Cvit$(numpuerto) + ":9600,n,8,1,lf" FOR OUTPUT AS #cmodem
PRINT #cmodem, mandato
CLOSE #cmodem
ON ERROR GOTO 0
END SUB

DEFINT A-Z
SUB taller
DIM m AS INTEGER
DIM n AS INTEGER
DIM tot AS INTEGER
DIM ancholin AS INTEGER
DIM modificado AS INTEGER
DIM campos AS INTEGER
DIM caden AS STRING
DIM cadendbl AS DOUBLE
DIM numregistro AS LONG
DIM numreg AS LONG'n£mero de registro "no real" usado para ver imprimir el "n£mero de registro actual"
DIM totalregistros AS LONG
DIM tec AS STRING
DIM salida AS STRING
DIM reg AS LONG
DIM hastafin AS INTEGER
DIM opciones(2) AS INTEGER'para lo que se escoge al imprimir
DIM lstcodimp AS STRING


idxabre
dtabre "ataller.dat", cataller
totalregistros = dtobtregistros&(cataller)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE panttaller: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE campostallerinfo
READ tot
campos = tot
DIM campostaller(tot) AS campos
FOR n = 1 TO tot
   READ campostaller(n).y, campostaller(n).x, campostaller(n).longi
   NEXT
GOSUB pantallataller
lineainf
n = 1
dtir cataller, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistrotalle
modificado = 0
DO
  GOSUB resaltacampotalle
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(9)
    'tab
    GOSUB desresaltacampotalle
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampotalle
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampotalle
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampotalle
    CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampotalle
    END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
    dtir cataller, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir cataller, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistrotalle
    END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
    dtir cataller, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir cataller, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistrotalle
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir cataller, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistrotalle
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir cataller, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistrotalle
      END IF
    END IF
    CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
    dtir cataller, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistrotalle
    END IF
    CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
    dtir cataller, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistrotalle
    END IF
  CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregtalle
    numreg = totalregistros + 1
    GOSUB ponblancotaller
    ataller(1).codmaquina = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistrotalle2
    n = 1
    GOSUB editacampotalle
    modificado = 1
    caden = ataller(1).codmaquina
    ataller(2) = ataller(1)
    n1 = dtbuscar%(cataller, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar cataller, 1
      END IF
      dtir cataller, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(ataller(1).codmaquina, caden) <> 0
        dtir cataller, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(cataller)
    GOSUB ponregistrotalle
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar cataller, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(cataller)
    GOSUB pantallataller
    GOSUB ponregistrotalle
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
    GOSUB actualizaregtalle
      salida = confirmacion$("¨ Quiere borrar el archivo de taller ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo cataller
        numreg = 0
        numregistro = -1
        GOSUB ponblancotaller
        END IF
      totalregistros = dtobtregistros&(cataller)
      GOSUB pantallataller
      GOSUB ponregistrotalle
      END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir cataller, 2, cdtirfin, reg
          lstcodimp = ataller(2).codmaquina
          dtir cataller, 1, cdtirinicio, reg
          CASE 2
          dtir cataller, 2, cdtirfin, reg
          lstcodimp = ataller(2).codmaquina
          dtIrA cataller, 1, numregistro
          CASE 3
          lstcodimp = ataller(1).codmaquina
          END SELECT
          poncolor 1, 4
          box 8, 10, 11, 60
          LOCATE 9, 11
          PRINT "Prepare la impresora y pulse tecla,ESC abortar";
          salida = getkey$
          IF salida <> CHR$(27) THEN
            lineaimpresora = 1
            saleimprimir = 0
            ponimprimiendo
            nuevahoja = 1
            DO
              imprime cataller, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir cataller, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(ataller(1).codmaquina, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA cataller, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallataller
     GOSUB ponregistrotalle
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregtalle
CLOSE #cataller
CLOSE #ccosidx
EXIT SUB
pantallataller:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE panttaller: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistrotalle:
IF numregistro = -1 THEN
  GOSUB ponblancotaller
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, ataller(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistrotalle2:
m = n
FOR n = 1 TO campos
   GOSUB poncampotalle
   NEXT
n = m
RETURN
resaltacampotalle:
poncolor 7, 4
poncampotalle:
  LOCATE campostaller(n).y, campostaller(n).x
  SELECT CASE n
      CASE 1
      PRINT ataller(1).codmaquina;
      CASE 2
      PRINT ataller(1).descripcio;
      CASE 3
      PRINT USING "#######.##"; ataller(1).preciohora;
      CASE 4
      PRINT USING "#########"; ataller(1).preciomaqu
      END SELECT
poncolor 3, 10
RETURN
desresaltacampotalle:
poncolor 3, 10
GOTO poncampotalle
editacampotalle:
SELECT CASE n
      CASE 1
      salida = inputstr$(campostaller(1).y, campostaller(1).x, CHR$(32), CHR$(255), ataller(1).codmaquina, caden, campostaller(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(campostaller(2).y, campostaller(2).x, CHR$(32), CHR$(255), ataller(1).descripcio, caden, campostaller(2).longi, 1, 1)
      CASE 3
      salida = inputdbl$(campostaller(3).y, campostaller(3).x, ataller(1).preciohora, cadendbl, 7, 2, 1)
      CASE 4
      salida = inputstr$(campostaller(4).y, campostaller(4).x, CHR$(48), CHR$(57), Cvlt$(ataller(1).preciomaqu), caden, campostaller(4).longi, 1, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampotalle
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      ataller(1).codmaquina = caden
      CASE 2
      ataller(1).descripcio = caden
      CASE 3
      ataller(1).preciohora = cadendbl
      CASE 4
      ataller(1).preciomaqu = VAL(caden)
      END SELECT
  END IF
  RETURN
actualizaregtalle:
IF modificado = 1 THEN
  dtmodificar cataller, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancotaller:
ataller(1).codmaquina = SPACE$(4)
ataller(1).descripcio = SPACE$(30)
ataller(1).preciohora = 0
ataller(1).preciomaqu = 0
RETURN
END SUB

DEFSNG A-Z
SUB telefonos
borramenusystem
STATIC linea AS STRING * 78
STATIC caden AS STRING
STATIC puerto2 AS INTEGER
DIM n AS INTEGER
DIM tot AS INTEGER
DIM editando AS INTEGER
STATIC n1 AS INTEGER
DIM sidomiciliacion AS INTEGER
DIM sinotas AS INTEGER
DIM ancholin AS INTEGER
DIM campos AS INTEGER
DIM canal AS INTEGER
DIM modificadopagos AS INTEGER
DIM modificadonotas AS INTEGER
DIM modificadotelefono AS INTEGER
DIM reg AS LONG
DIM hastafin AS INTEGER
DIM opciones(4) AS INTEGER'para lo que se escoge al imprimir
DIM opcmenu(5) AS STRING
DIM lstcodimp AS STRING
DIM numregistro AS LONG'n£mero de registro real en la base de datos
DIM numreg AS LONG'n£mero de registro "no real" usado para ver imprimir el "n£mero de registro actual"
DIM regs(2) AS LONG'1er indice domiciliacion,2do notas
DIM totalregistros AS LONG
DIM tec AS STRING
DIM tec2 AS STRING
DIM salida AS STRING
STATIC numtelefono AS STRING

idxabre
RESTORE panttelefonos: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE campostelefonosinfo
READ tot
campos = tot
DIM campostelefonos(tot) AS campos
FOR n = 1 TO tot
   READ campostelefonos(n).y, campostelefonos(n).x, campostelefonos(n).longi
   NEXT
poncolor 1, 4
fancycls 2, 1
dtabre "pagos.dat", cpagos
dtabre "cliente.dat", ccliente
dtabre "notas.dat", cnotas
dtir ccliente, 1, cdtirinicio, numregistro
totalregistros = dtobtregistros&(ccliente)
numreg = 1
GOSUB pantallatelefonos
GOSUB lineainftelefonos
GOSUB ponregistrotel
editando = 0
n = 1
modificadonotas = 0
modificadopagos = 0
modificadotelefono = 0
DO
  GOSUB resaltacampotel
  tec = getkey$
  SELECT CASE tec
  CASE CHR$(9)
    'tab
    GOSUB desresaltacampotel
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampotel
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampotel
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampotel
  CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampotel
    END IF
  CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtel
    dtir ccliente, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir ccliente, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistrotel
    END IF
  CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtel
    dtir ccliente, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir ccliente, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistrotel
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir ccliente, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistrotel
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir ccliente, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistrotel
      END IF
    END IF
  CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtel
    dtir ccliente, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistrotel
    END IF
  CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtel
    dtir ccliente, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistrotel
    END IF
  CASE CHR$(0) + CHR$(66)
    'F8 : Configuraci¢n del modem
    poncolor 1, 4
    box 12, 20, 15, 65
    LOCATE 12, 30: PRINT "Configuraci¢n del modem"
    LOCATE 13, 21: PRINT "Elija el puerto del Modem(COM1-COM4):COM";
    DO
    salida = inputstr$(13, 61, "1", "4", Cvit$(puerto), caden, 1, 0, 0)
    LOOP WHILE salida <> CHR$(27) AND LEFT$(caden, 1) = " "
    IF salida <> CHR$(27) THEN
      puerto2 = VAL(caden)
      LOCATE 13, 21: PRINT SPACE$(42);
      IF tipomarcado = "T" THEN
        caden = "1"
        ELSE
        caden = "2"
        END IF
      LOCATE 13, 21: PRINT "Elija el modo de marcado,1=Tonos,2=Pulsos:";
      DO
      salida = inputstr$(13, 63, "1", "2", caden, caden, 1, 0, 0)
      LOOP WHILE salida <> CHR$(27) AND LEFT$(caden, 1) = " "
      IF salida <> CHR$(27) THEN
        puerto = puerto2
        IF LEFT$(caden, 1) = "1" THEN
          tipomarcado = "T"
          ELSE
          tipomarcado = "P"
          END IF
      savecfg
      END IF
    END IF
    GOSUB pantallatelefonos
    GOSUB ponregistrotel
  CASE CHR$(0) + CHR$(65)
    'F7 : Llamar
  IF totalregistros <> 0 THEN
    poncolor 1, 4
    box 12, 15, 14, 65
    LOCATE 13, 16: PRINT "A que n£mero quiere llamar al de telefono1 o 2:";
    salida = inputstr$(13, 63, "1", "2", "1", caden, 1, 0, 0)
    IF salida <> CHR$(27) THEN
      IF caden = "1" THEN
        numtelefono = cliente(1).telefono
        ELSE
        numtelefono = cliente(1).telefono2
        END IF
      LOCATE 13, 16: PRINT SPACE$(48);
      IF LEN(RTRIM$(numtelefono)) = 0 THEN
        box 12, 15, 15, 65
        LOCATE 13, 21
       PRINT "No hay n£mero de telefono en telefono"; caden;
        LOCATE 14, 16
        PRINT SPACE$(48);
        LOCATE 14, 36
        PRINT "Pulsa tecla"
        BEEP
        caden = getkey$
        ELSE
        LOCATE 13, 36: PRINT "Marcando ...";
        modem puerto, "AT X4 D" + tipomarcado + numtelefono
      END IF
    END IF
    GOSUB pantallatelefonos
    GOSUB ponregistrotel
    END IF
  CASE CHR$(0) + CHR$(67)
    'F9: Colgar
    poncolor 1, 4
    box 12, 20, 14, 40
    LOCATE 13, 25: PRINT "Colgando..."
    modem puerto, "ATH" + CHR$(13)
    GOSUB pantallatelefonos
    GOSUB ponregistrotel
  CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregtel
    numreg = totalregistros + 1
    GOSUB ponblanconotaspagos
    cliente(1).codcliente = SPACE$(2)
    notas(1).codcliente = cliente(1).codcliente
    pagos(1).codcliente = notas(1).codcliente
    regs(1) = -1
    regs(2) = -1
    GOSUB ponblancocliente
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistrotel2
    n = 1
    GOSUB editacampotel
    modificadotelefono = 1
    modificadopagos = 0
    modificadonotas = 0
    notas(1).codcliente = cliente(1).codcliente
    pagos(1).codcliente = cliente(1).codcliente
    caden = cliente(1).codcliente
    cliente(2) = cliente(1)
    n1 = dtbuscar%(ccliente, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar ccliente, 1
      END IF
      dtir ccliente, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(cliente(1).codcliente, caden) <> 0
        dtir ccliente, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(ccliente)
    GOSUB ponregistrotel
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtel
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar cpagos, 1, regs(1)
      dtborrar cnotas, 1, regs(2)
      dtborrar ccliente, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(ccliente)
    GOSUB pantallatelefonos
    GOSUB ponregistrotel
    END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    opcmenu(1) = "Ficha"
    opcmenu(2) = "Listin"
    opcmenu(3) = "Sobres"
    opciones(1) = escoge%(opcmenu(), 3, "Imprimir")
    IF opciones(1) <> 0 THEN
      opcmenu(1) = "Todo"
      opcmenu(2) = "Desde"
      opcmenu(3) = "Actual"
      opciones(2) = escoge%(opcmenu(), 3, "Imprimir")
      opciones(3) = -1
      IF opciones(1) = 3 AND opciones(2) <> 0 THEN
        opcmenu(1) = "D. Ventas"
        opcmenu(2) = "D. Compras"
        opcmenu(3) = "Gerencia"
        opcmenu(4) = "D. Tecnico"
        opcmenu(5) = "Direccion"
        opciones(3) = escoge%(opcmenu(), 5, "Imprimir")
        opciones(4) = 1
        IF opciones(3) <> 0 THEN caden = opcmenu(opciones(3))
        END IF
        IF opciones(3) <> 0 THEN
         opcmenu(1) = "Autom tico"
         opcmenu(2) = "Manual"
         IF opciones(1) <> 3 THEN
           opciones(4) = escoge%(opcmenu(), 2, "Cambio de hojas")
           END IF
         IF opciones(4) <> 0 THEN
           hastafin = 0
           IF opciones(2) = 1 OR opciones(2) = 2 THEN hastafin = 1
           SELECT CASE opciones(2)
             CASE 1
               dtir ccliente, 2, cdtirfin, reg
               lstcodimp = cliente(2).codcliente
               dtir ccliente, 1, cdtirinicio, reg
             CASE 2
               dtir ccliente, 2, cdtirfin, reg
               lstcodimp = cliente(2).codcliente
               dtIrA ccliente, 1, numregistro
             CASE 3
               lstcodimp = cliente(1).codcliente
             END SELECT
          poncolor 1, 4
          salida = " "
          IF opciones(1) <> 3 THEN
            box 8, 10, 11, 60
            LOCATE 9, 11
            PRINT "Prepare la impresora y pulse tecla,ESC abortar";
            salida = getkey$
            END IF
          IF salida <> CHR$(27) THEN
            lineaimpresora = 1
            saleimprimir = 0
            ponimprimiendo
            nuevahoja = 1
            DO
              imprimetelefonos opciones(1), caden, opciones(4)
              IF saleimprimir = 1 THEN EXIT DO
              dtir ccliente, 1, cdtirposterior, reg
              pagos(1).codcliente = cliente(1).codcliente
              notas(1).codcliente = cliente(1).codcliente
              IF opciones(1) = 1 THEN
                t% = dtbuscar%(cpagos, 1, regs(1), TRUE)
                t% = dtbuscar%(cnotas, 1, regs(2), TRUE)
                END IF
              LOOP WHILE (strcmp%(cliente(1).codcliente, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA ccliente, 1, numregistro
            END IF
          END IF
        END IF
      END IF
      nuevahoja = 0
     GOSUB pantallatelefonos
     GOSUB ponregistrotel
     END IF
  CASE CHR$(0) + CHR$(64)
    'F6 : Exportar
  IF totalregistros <> 0 THEN
    errorfichero = 0
    canal = FREEFILE
    ON ERROR GOTO errorexportar
    OPEN "TFNO471.txt" FOR APPEND AS #canal
    SELECT CASE n
      CASE 1
      PRINT #canal, cliente(1).codcliente
      CASE 2
      PRINT #canal, cliente(1).empresa
      CASE 3
      PRINT #canal, cliente(1).cif
      CASE 4
      PRINT #canal, cliente(1).cargo
      CASE 5
      PRINT #canal, cliente(1).nombre
      CASE 6
      PRINT #canal, cliente(1).apellidos
      CASE 7
      PRINT #canal, cliente(1).direccion
      CASE 8
      PRINT #canal, cliente(1).localidad
      CASE 9
      PRINT #canal, cliente(1).provincia
      CASE 10
      PRINT #canal, cliente(1).pais
      CASE 11
      PRINT #canal, cliente(1).postal
      CASE 12
      PRINT #canal, cliente(1).telefono
      CASE 13
      PRINT #canal, cliente(1).telefono2
      CASE 14
      PRINT #canal, cliente(1).fax
      CASE 15
      PRINT #canal, cliente(1).actividad
      CASE 16
      'a partir del 16 , domiciliaci¢n pagos
      PRINT #canal, pagos(1).entidad
      CASE 17
      PRINT #canal, pagos(1).direccionb
      CASE 18
      PRINT #canal, pagos(1).localidadb
      CASE 19
      PRINT #canal, pagos(1).telefonob
      CASE 20
      PRINT #canal, pagos(1).provinciab
      CASE 21
      PRINT #canal, pagos(1).postale
      CASE 22
      PRINT #canal, pagos(1).codigocuen
      CASE 23
      ' cuando el campo de notas
      ancholin = 80 - campostelefonos(23).x
      PRINT #canal, LEFT$(notas(1).notas, ancholin)
      PRINT #canal, MID$(notas(1).notas, 1 + ancholin, ancholin)
      PRINT #canal, MID$(notas(1).notas, 1 + ancholin + ancholin, ancholin)
      END SELECT
      CLOSE #canal
      ON ERROR GOTO 0
      IF errorfichero <> 0 THEN
        GOSUB pantallatelefonos
        GOSUB ponregistrotel
        END IF
      END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregtel
      salida = confirmacion$("¨ Quiere borrar el archivo de clientes ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo ccliente
        dtborrartodo cpagos
        dtborrartodo cnotas
        numreg = 0
        numregistro = -1
        cliente(1).codcliente = SPACE$(2)
        notas(1).codcliente = cliente(1).codcliente
        pagos(1).codcliente = notas(1).codcliente
        regs(1) = -1
        regs(2) = -1
        GOSUB ponblancocliente
        GOSUB ponblanconotaspagos
        END IF
      totalregistros = dtobtregistros&(ccliente)
      GOSUB pantallatelefonos
      GOSUB ponregistrotel
      END IF
  END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregtel
CLOSE #cpagos
CLOSE #ccliente
CLOSE #cnotas
CLOSE #ccosidx
EXIT SUB
pantallatelefonos:
poncolor 2, 1
box 3, 1, 22, 80
linea = STRING$(78, "Ä")
LOCATE 5, 2
PRINT linea;
LOCATE 12, 2
PRINT linea;
LOCATE 18, 2
PRINT linea;
linea2$ = "Ã"
LOCATE 5, 1: PRINT linea2$;
LOCATE 12, 1: PRINT linea2$;
LOCATE 18, 1: PRINT linea2$;
linea3$ = "´"
LOCATE 5, 80: PRINT linea3$;
LOCATE 12, 80: PRINT linea3$;
LOCATE 18, 80: PRINT linea3$;
LOCATE 22, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE panttelefonos: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistrotel:
GOSUB ponblanconotaspagos
pagos(1).codcliente = cliente(1).codcliente
t% = dtbuscar%(cpagos, 1, regs(1), TRUE)
notas(1).codcliente = cliente(1).codcliente
t% = dtbuscar%(cnotas, 1, regs(2), TRUE)
IF numregistro = -1 THEN
  GOSUB ponblancocliente
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, cliente(1).empresa
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistrotel2:
n1 = n
FOR n = 1 TO campos
   GOSUB poncampotel
   NEXT
n = n1
RETURN
resaltacampotel:
poncolor 7, 4
poncampotel:
  LOCATE campostelefonos(n).y, campostelefonos(n).x
  SELECT CASE n
      CASE 1
      PRINT cliente(1).codcliente;
      CASE 2
      PRINT cliente(1).empresa;
      CASE 3
      PRINT cliente(1).cif;
      CASE 4
      PRINT cliente(1).cargo;
      CASE 5
      PRINT cliente(1).nombre;
      CASE 6
      PRINT cliente(1).apellidos;
      CASE 7
      PRINT cliente(1).direccion;
      CASE 8
      PRINT cliente(1).localidad;
      CASE 9
      PRINT cliente(1).provincia;
      CASE 10
      PRINT cliente(1).pais;
      CASE 11
      PRINT cliente(1).postal;
      CASE 12
      PRINT cliente(1).telefono;
      CASE 13
      PRINT cliente(1).telefono2;
      CASE 14
      PRINT cliente(1).fax;
      CASE 15
      PRINT cliente(1).actividad;
      CASE 16
      'a partir del 16 , domiciliaci¢n pagos
      PRINT pagos(1).entidad;
      CASE 17
      PRINT pagos(1).direccionb;
      CASE 18
      PRINT pagos(1).localidadb;
      CASE 19
      PRINT pagos(1).telefonob;
      CASE 20
      PRINT pagos(1).provinciab;
      CASE 21
      PRINT pagos(1).postale;
      CASE 22
      PRINT pagos(1).codigocuen;
      CASE 23
      ' cuando el campo de notas
      ancholin = 80 - campostelefonos(23).x
      PRINT LEFT$(notas(1).notas, ancholin);
      LOCATE campostelefonos(23).y + 1, campostelefonos(23).x
      PRINT MID$(notas(1).notas, 1 + ancholin, ancholin);
      LOCATE campostelefonos(23).y + 2, campostelefonos(23).x
      PRINT MID$(notas(1).notas, 1 + ancholin + ancholin, ancholin);
      END SELECT
poncolor 3, 10
RETURN
desresaltacampotel:
poncolor 3, 10
GOTO poncampotel
editacampotel:
SELECT CASE n
      CASE 1
      salida = inputstr$(campostelefonos(1).y, campostelefonos(1).x, CHR$(32), CHR$(255), cliente(1).codcliente, caden, campostelefonos(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(campostelefonos(2).y, campostelefonos(2).x, CHR$(32), CHR$(255), cliente(1).empresa, caden, campostelefonos(2).longi, 1, 1)
      CASE 3
      salida = inputstr$(campostelefonos(3).y, campostelefonos(3).x, CHR$(45), CHR$(91), cliente(1).cif, caden, campostelefonos(3).longi, 1, 1)
      CASE 4
      salida = inputstr$(campostelefonos(4).y, campostelefonos(4).x, CHR$(32), CHR$(255), cliente(1).cargo, caden, campostelefonos(4).longi, 1, 1)
      CASE 5
      salida = inputstr$(campostelefonos(5).y, campostelefonos(5).x, CHR$(32), CHR$(255), cliente(1).nombre, caden, campostelefonos(5).longi, 1, 1)
      CASE 6
      salida = inputstr$(campostelefonos(6).y, campostelefonos(6).x, CHR$(32), CHR$(255), cliente(1).apellidos, caden, campostelefonos(6).longi, 1, 1)
      CASE 7
      salida = inputstr$(campostelefonos(7).y, campostelefonos(7).x, CHR$(32), CHR$(255), cliente(1).direccion, caden, campostelefonos(7).longi, 1, 1)
      CASE 8
      salida = inputstr$(campostelefonos(8).y, campostelefonos(8).x, CHR$(32), CHR$(255), cliente(1).localidad, caden, campostelefonos(8).longi, 1, 1)
      CASE 9
      salida = inputstr$(campostelefonos(9).y, campostelefonos(9).x, CHR$(32), CHR$(255), cliente(1).provincia, caden, campostelefonos(9).longi, 1, 1)
      CASE 10
      salida = inputstr$(campostelefonos(10).y, campostelefonos(10).x, CHR$(32), CHR$(255), cliente(1).pais, caden, campostelefonos(10).longi, 1, 1)
      CASE 11
      salida = inputstr$(campostelefonos(11).y, campostelefonos(11).x, CHR$(48), CHR$(57), cliente(1).postal, caden, campostelefonos(11).longi, 1, 1)
      CASE 12
      salida = inputstr$(campostelefonos(12).y, campostelefonos(12).x, CHR$(40), CHR$(90), cliente(1).telefono, caden, campostelefonos(12).longi, 1, 1)
      CASE 13
      salida = inputstr$(campostelefonos(13).y, campostelefonos(13).x, CHR$(40), CHR$(90), cliente(1).telefono2, caden, campostelefonos(13).longi, 1, 1)
      CASE 14
      salida = inputstr$(campostelefonos(14).y, campostelefonos(14).x, CHR$(40), CHR$(90), cliente(1).fax, caden, campostelefonos(14).longi, 1, 1)
      CASE 15
      salida = inputstr$(campostelefonos(15).y, campostelefonos(15).x, CHR$(32), CHR$(255), cliente(1).actividad, caden, campostelefonos(15).longi, 1, 1)
      CASE 16
      'a partir del 16 , domiciliaci¢n pagos
      salida = inputstr$(campostelefonos(16).y, campostelefonos(16).x, CHR$(32), CHR$(255), pagos(1).entidad, caden, campostelefonos(16).longi, 1, 1)
      CASE 17
      salida = inputstr$(campostelefonos(17).y, campostelefonos(17).x, CHR$(32), CHR$(255), pagos(1).direccionb, caden, campostelefonos(17).longi, 1, 1)
      CASE 18
      salida = inputstr$(campostelefonos(18).y, campostelefonos(18).x, CHR$(32), CHR$(255), pagos(1).localidadb, caden, campostelefonos(18).longi, 1, 1)
      CASE 19
      salida = inputstr$(campostelefonos(19).y, campostelefonos(19).x, CHR$(40), CHR$(90), pagos(1).telefonob, caden, campostelefonos(19).longi, 1, 1)
      CASE 20
      salida = inputstr$(campostelefonos(20).y, campostelefonos(20).x, CHR$(32), CHR$(255), pagos(1).provinciab, caden, campostelefonos(20).longi, 1, 1)
      CASE 21
      salida = inputstr$(campostelefonos(21).y, campostelefonos(21).x, CHR$(48), CHR$(57), pagos(1).postale, caden, campostelefonos(21).longi, 1, 1)
      CASE 22
      salida = inputstr$(campostelefonos(22).y, campostelefonos(22).x, CHR$(32), CHR$(255), pagos(1).codigocuen, caden, campostelefonos(22).longi, 1, 1)
      CASE 23
      ' cuando el campo de notas
      salida = inputnota$(campostelefonos(23).y, campostelefonos(23).x, CHR$(32), CHR$(255), notas(1).notas, caden, campostelefonos(23).longi)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampotel
  ELSE
  IF n >= 1 AND n <= 15 THEN
    modificadotelefono = 1
    END IF
  IF n >= 16 AND n <= 22 THEN
    modificadopagos = 1
    END IF
  IF n = 23 THEN
    modificadonotas = 1
    END IF
  SELECT CASE n
      CASE 1
      cliente(1).codcliente = caden
      modificadopagos = 1
      modificadonotas = 1
      CASE 2
      cliente(1).empresa = caden
      CASE 3
      cliente(1).cif = caden
      CASE 4
      cliente(1).cargo = caden
      CASE 5
      cliente(1).nombre = caden
      CASE 6
      cliente(1).apellidos = caden
      CASE 7
      cliente(1).direccion = caden
      CASE 8
      cliente(1).localidad = caden
      CASE 9
      cliente(1).provincia = caden
      CASE 10
      cliente(1).pais = caden
      CASE 11
      cliente(1).postal = caden
      CASE 12
      cliente(1).telefono = caden
      CASE 13
      cliente(1).telefono2 = caden
      CASE 14
      cliente(1).fax = caden
      CASE 15
      cliente(1).actividad = caden
      CASE 16
      'a partir del 16 , domiciliaci¢n pagos
      pagos(1).entidad = caden
      CASE 17
      pagos(1).direccionb = caden
      CASE 18
      pagos(1).localidadb = caden
      CASE 19
      pagos(1).telefonob = caden
      CASE 20
      pagos(1).provinciab = caden
      CASE 21
      pagos(1).postale = caden
      CASE 22
      pagos(1).codigocuen = caden
      CASE 23
      'cuando el campo de notas
      notas(1).notas = caden
      END SELECT
      pagos(1).codcliente = cliente(1).codcliente
      notas(1).codcliente = cliente(1).codcliente
  END IF
  RETURN
actualizaregtel:
    'borra la ficha de domiciliaci¢n si es necesario
    IF modificadotelefono = 1 THEN dtmodificar ccliente, 1, 2, numregistro
    IF modificadopagos = 1 THEN
      IF NOT (haydomiciliacion%(pagos(1))) THEN
        dtborrar cpagos, 1, regs(1)
        ELSE
        dtmodificar cpagos, 1, 2, regs(1)
        END IF
      END IF
    'borra la ficha de notas si es necesario
    IF modificadonotas = 1 THEN
      IF NOT (haynotas%(notas(1))) THEN
        dtborrar cnotas, 1, regs(2)
        ELSE
        dtmodificar cnotas, 1, 2, regs(2)
        END IF
      END IF
modificadonotas = 0
modificadopagos = 0
modificadotelefono = 0
RETURN
lineainftelefonos:
printlineainf "F1 Bor. F2 Ins. F3 Impr. F4 Bus. F6 Exp.Campo F7 Tel. F8 CON F9 Colgar AF1 B.Tdo"
RETURN
ponblanconotaspagos:
pagos(1).entidad = SPACE$(30)
pagos(1).direccionb = SPACE$(30)
pagos(1).localidadb = SPACE$(30)
pagos(1).telefonob = SPACE$(15)
pagos(1).provinciab = SPACE$(17)
pagos(1).postale = SPACE$(5)
pagos(1).codigocuen = SPACE$(23)
notas(1).notas = SPACE$(234)
RETURN
ponblancocliente:
cliente(1).empresa = SPACE$(30)
cliente(1).cif = SPACE$(15)
cliente(1).cargo = SPACE$(15)
cliente(1).nombre = SPACE$(13)
cliente(1).apellidos = SPACE$(19)
cliente(1).direccion = SPACE$(30)
cliente(1).localidad = SPACE$(22)
cliente(1).provincia = SPACE$(17)
cliente(1).pais = SPACE$(20)
cliente(1).postal = SPACE$(5)
cliente(1).telefono = SPACE$(15)
cliente(1).telefono2 = SPACE$(15)
cliente(1).fax = SPACE$(15)
cliente(1).actividad = SPACE$(15)
RETURN
END SUB

