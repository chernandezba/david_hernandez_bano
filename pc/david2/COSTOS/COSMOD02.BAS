DEFINT A-Z

'$INCLUDE: 'costos.inc'

pantoperarios:
DATA 6
DATA 31, 3, "Operarios"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Precio por hora:"
DATA 25,7,"Costo anual:"
DATA 5,8,"[ESC Salir]"

camposoperariosinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 4
DATA 6,10,4
DATA 6,29,30
DATA 7,19,5
DATA 7,38,9

panttratamientos:
DATA 5
DATA 31, 3, "Tratamientos"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Precio por kilo:"
DATA 5,8,"[ESC Salir]"

campostratamientosinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 3
DATA 6,10,4
DATA 6,29,45
DATA 7,19,12


pantutiles:
DATA 5
DATA 31, 3, "Utiles"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Precio:"
DATA 5,8,"[ESC Salir]"

camposutilesinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 3
DATA 6,10,4
DATA 6,29,30
DATA 7,10,7

SUB operarios
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
dtabre "aoperari.dat", caoperari
totalregistros = dtobtregistros&(caoperari)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE pantoperarios: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE camposoperariosinfo
READ tot
campos = tot
DIM camposoperarios(tot) AS campos
FOR n = 1 TO tot
   READ camposoperarios(n).y, camposoperarios(n).x, camposoperarios(n).longi
   NEXT
GOSUB pantallaoperarios
lineainf
n = 1
dtir caoperari, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistroope
modificado = 0
DO
  GOSUB resaltacampoope
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(9)
    'tab
    GOSUB desresaltacampoope
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampoope
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampoope
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampoope
    CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampoope
    END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizaregope
    dtir caoperari, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir caoperari, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizaregope
    dtir caoperari, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir caoperari, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir caoperari, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistroope
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir caoperari, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistroope
      END IF
    END IF
    CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizaregope
    dtir caoperari, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizaregope
    dtir caoperari, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregope
    numreg = totalregistros + 1
    GOSUB ponblancooperarios
    aoperari(1).codoperari = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistroope2
    n = 1
    GOSUB editacampoope
    modificado = 1
    caden = aoperari(1).codoperari
    aoperari(2) = aoperari(1)
    n1 = dtbuscar%(caoperari, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar caoperari, 1
      END IF
      dtir caoperari, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(aoperari(1).codoperari, caden) <> 0
        dtir caoperari, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(caoperari)
    GOSUB ponregistroope
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregope
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar caoperari, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(caoperari)
    GOSUB pantallaoperarios
    GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
    GOSUB actualizaregope
      salida = confirmacion$("¨ Quiere borrar el archivo de operarios ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo caoperari
        numreg = 0
        numregistro = -1
        GOSUB ponblancooperarios
        END IF
      totalregistros = dtobtregistros&(caoperari)
      GOSUB pantallaoperarios
      GOSUB ponregistroope
    END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir caoperari, 2, cdtirfin, reg
          lstcodimp = aoperari(2).codoperari
          dtir caoperari, 1, cdtirinicio, reg
          CASE 2
          dtir caoperari, 2, cdtirfin, reg
          lstcodimp = aoperari(2).codoperari
          dtIrA caoperari, 1, numregistro
          CASE 3
          lstcodimp = aoperari(1).codoperari
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
              imprime caoperari, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir caoperari, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(aoperari(1).codoperari, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA caoperari, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallaoperarios
     GOSUB ponregistroope
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregope
CLOSE #caoperari
CLOSE #ccosidx
EXIT SUB
pantallaoperarios:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE pantoperarios: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistroope:
IF numregistro = -1 THEN
  GOSUB ponblancooperarios
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, aoperari(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistroope2:
m = n
FOR n = 1 TO campos
   GOSUB poncampoope
   NEXT
n = m
RETURN
resaltacampoope:
poncolor 7, 4
poncampoope:
  LOCATE camposoperarios(n).y, camposoperarios(n).x
  SELECT CASE n
      CASE 1
      PRINT aoperari(1).codoperari;
      CASE 2
      PRINT aoperari(1).descripcio;
      CASE 3
      PRINT USING "#####"; aoperari(1).preciohora;
      CASE 4
      PRINT USING "#########"; aoperari(1).costoanual
      END SELECT
poncolor 3, 10
RETURN
desresaltacampoope:
poncolor 3, 10
GOTO poncampoope
editacampoope:
SELECT CASE n
      CASE 1
      salida = inputstr$(camposoperarios(1).y, camposoperarios(1).x, CHR$(32), CHR$(255), aoperari(1).codoperari, caden, camposoperarios(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(camposoperarios(2).y, camposoperarios(2).x, CHR$(32), CHR$(255), aoperari(1).descripcio, caden, camposoperarios(2).longi, 1, 1)
      CASE 3
      salida = inputstr$(camposoperarios(3).y, camposoperarios(3).x, CHR$(48), CHR$(57), Cvlt$(aoperari(1).preciohora), caden, camposoperarios(3).longi, 1, 1)
      CASE 4
      salida = inputstr$(camposoperarios(4).y, camposoperarios(4).x, CHR$(48), CHR$(57), Cvlt$(aoperari(1).costoanual), caden, camposoperarios(4).longi, 1, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampoope
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      aoperari(1).codoperari = caden
      CASE 2
      aoperari(1).descripcio = caden
      CASE 3
      aoperari(1).preciohora = VAL(caden)
      CASE 4
      aoperari(1).costoanual = VAL(caden)
      END SELECT
  END IF
  RETURN
actualizaregope:
IF modificado = 1 THEN
  dtmodificar caoperari, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancooperarios:
aoperari(1).codoperari = SPACE$(4)
aoperari(1).descripcio = SPACE$(30)
aoperari(1).preciohora = 0
aoperari(1).costoanual = 0
RETURN
END SUB

SUB tratamientos
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
dtabre "altrata.dat", caltrata
totalregistros = dtobtregistros&(caltrata)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE panttratamientos: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE campostratamientosinfo
READ tot
campos = tot
DIM campostratamientos(tot) AS campos
FOR n = 1 TO tot
   READ campostratamientos(n).y, campostratamientos(n).x, campostratamientos(n).longi
   NEXT
GOSUB pantallatratamientos
lineainf
n = 1
dtir caltrata, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistrotrat
modificado = 0
DO
  GOSUB resaltacampotrat
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(9)
    'tab
    GOSUB desresaltacampotrat
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampotrat
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampotrat
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampotrat
    CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampotrat
    END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtrat
    dtir caltrata, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir caltrata, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistrotrat
    END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtrat
    dtir caltrata, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir caltrata, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistrotrat
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir caltrata, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistrotrat
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir caltrata, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistrotrat
      END IF
    END IF
    CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtrat
    dtir caltrata, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistrotrat
    END IF
    CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtrat
    dtir caltrata, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistrotrat
    END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregtrat
    numreg = totalregistros + 1
    GOSUB ponblancotratamientos
    altrata(1).codtratami = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistrotrat2
    n = 1
    GOSUB editacampotrat
    modificado = 1
    caden = altrata(1).codtratami
    altrata(2) = altrata(1)
    n1 = dtbuscar%(caltrata, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar caltrata, 1
      END IF
      dtir caltrata, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(altrata(1).codtratami, caden) <> 0
        dtir caltrata, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(caltrata)
    GOSUB ponregistrotrat
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregtrat
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar caltrata, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(caltrata)
    GOSUB pantallatratamientos
    GOSUB ponregistrotrat
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregtrat
      salida = confirmacion$("¨ Quiere borrar el archivo de tratamientos ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo caltrata
        numreg = 0
        numregistro = -1
        GOSUB ponblancotratamientos
        END IF
      totalregistros = dtobtregistros&(caltrata)
      GOSUB pantallatratamientos
      GOSUB ponregistrotrat
      END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir caltrata, 2, cdtirfin, reg
          lstcodimp = altrata(2).codtratami
          dtir caltrata, 1, cdtirinicio, reg
          CASE 2
          dtir caltrata, 2, cdtirfin, reg
          lstcodimp = altrata(2).codtratami
          dtIrA caltrata, 1, numregistro
          CASE 3
          lstcodimp = altrata(1).codtratami
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
              imprime caltrata, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir caltrata, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(altrata(1).codtratami, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA caltrata, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallatratamientos
     GOSUB ponregistrotrat
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregtrat
CLOSE #caltrata
CLOSE #ccosidx
EXIT SUB
pantallatratamientos:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE panttratamientos: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistrotrat:
IF numregistro = -1 THEN
  GOSUB ponblancotratamientos
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, altrata(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistrotrat2:
m = n
FOR n = 1 TO campos
   GOSUB poncampotrat
   NEXT
n = m
RETURN
resaltacampotrat:
poncolor 7, 4
poncampotrat:
  LOCATE campostratamientos(n).y, campostratamientos(n).x
  SELECT CASE n
      CASE 1
      PRINT altrata(1).codtratami;
      CASE 2
      PRINT altrata(1).descripcio;
      CASE 3
      PRINT USING "#########.##"; altrata(1).preciokilo;
      END SELECT
poncolor 3, 10
RETURN
desresaltacampotrat:
poncolor 3, 10
GOTO poncampotrat
editacampotrat:
SELECT CASE n
      CASE 1
      salida = inputstr$(campostratamientos(1).y, campostratamientos(1).x, CHR$(32), CHR$(255), altrata(1).codtratami, caden, campostratamientos(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(campostratamientos(2).y, campostratamientos(2).x, CHR$(32), CHR$(255), altrata(1).descripcio, caden, campostratamientos(2).longi, 1, 1)
      CASE 3
      salida = inputdbl$(campostratamientos(3).y, campostratamientos(3).x, altrata(1).preciokilo, cadendbl, 9, 2, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampotrat
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      altrata(1).codtratami = caden
      CASE 2
      altrata(1).descripcio = caden
      CASE 3
      altrata(1).preciokilo = cadendbl
      END SELECT
  END IF
  RETURN
actualizaregtrat:
IF modificado = 1 THEN
  dtmodificar caltrata, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancotratamientos:
altrata(1).codtratami = SPACE$(4)
altrata(1).descripcio = SPACE$(45)
altrata(1).preciokilo = 0
RETURN
END SUB

SUB utiles
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
dtabre "autiles.dat", cautiles
totalregistros = dtobtregistros&(cautiles)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE pantutiles: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE camposutilesinfo
READ tot
campos = tot
DIM camposutiles(tot) AS campos
FOR n = 1 TO tot
   READ camposutiles(n).y, camposutiles(n).x, camposutiles(n).longi
   NEXT
GOSUB pantallautiles
lineainf
n = 1
dtir cautiles, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistrouti
modificado = 0
DO
  GOSUB resaltacampouti
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(9)
    'tab
    GOSUB desresaltacampouti
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampouti
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampouti
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampouti
    CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampouti
    END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizareguti
    dtir cautiles, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir cautiles, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizareguti
    dtir cautiles, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir cautiles, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir cautiles, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistrouti
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir cautiles, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistrouti
      END IF
    END IF
    CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizareguti
    dtir cautiles, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizareguti
    dtir cautiles, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizareguti
    numreg = totalregistros + 1
    GOSUB ponblancoutiles
    autiles(1).codutil = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistrouti2
    n = 1
    GOSUB editacampouti
    modificado = 1
    caden = autiles(1).codutil
    autiles(2) = autiles(1)
    n1 = dtbuscar%(cautiles, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar cautiles, 1
      END IF
      dtir cautiles, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(autiles(1).codutil, caden) <> 0
        dtir cautiles, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(cautiles)
    GOSUB ponregistrouti
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizareguti
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar cautiles, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(cautiles)
    GOSUB pantallautiles
    GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
      GOSUB actualizareguti
      salida = confirmacion$("¨ Quiere borrar el archivo de utiles ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo cautiles
        numreg = 0
        numregistro = -1
        GOSUB ponblancoutiles
        END IF
      totalregistros = dtobtregistros&(cautiles)
      GOSUB pantallautiles
      GOSUB ponregistrouti
    END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir cautiles, 2, cdtirfin, reg
          lstcodimp = autiles(2).codutil
          dtir cautiles, 1, cdtirinicio, reg
          CASE 2
          dtir cautiles, 2, cdtirfin, reg
          lstcodimp = autiles(2).codutil
          dtIrA cautiles, 1, numregistro
          CASE 3
          lstcodimp = autiles(1).codutil
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
              imprime cautiles, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir cautiles, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(autiles(1).codutil, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA cautiles, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallautiles
     GOSUB ponregistrouti
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizareguti
CLOSE #cautiles
CLOSE #ccosidx
EXIT SUB
pantallautiles:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE pantutiles: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistrouti:
IF numregistro = -1 THEN
  GOSUB ponblancoutiles
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, autiles(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistrouti2:
m = n
FOR n = 1 TO campos
   GOSUB poncampouti
   NEXT
n = m
RETURN
resaltacampouti:
poncolor 7, 4
poncampouti:
  LOCATE camposutiles(n).y, camposutiles(n).x
  SELECT CASE n
      CASE 1
      PRINT autiles(1).codutil;
      CASE 2
      PRINT autiles(1).descripcio;
      CASE 3
      PRINT USING "#######"; autiles(1).precio;
      END SELECT
poncolor 3, 10
RETURN
desresaltacampouti:
poncolor 3, 10
GOTO poncampouti
editacampouti:
SELECT CASE n
      CASE 1
      salida = inputstr$(camposutiles(1).y, camposutiles(1).x, CHR$(32), CHR$(255), autiles(1).codutil, caden, camposutiles(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(camposutiles(2).y, camposutiles(2).x, CHR$(32), CHR$(255), autiles(1).descripcio, caden, camposutiles(2).longi, 1, 1)
      CASE 3
      salida = inputstr$(camposutiles(3).y, camposutiles(3).x, CHR$(48), CHR$(57), Cvlt$(autiles(1).precio), caden, camposutiles(3).longi, 1, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampouti
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      autiles(1).codutil = caden
      CASE 2
      autiles(1).descripcio = caden
      CASE 3
      autiles(1).precio = VAL(caden)
      END SELECT
  END IF
  RETURN
actualizareguti:
IF modificado = 1 THEN
  dtmodificar cautiles, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancoutiles:
autiles(1).codutil = SPACE$(4)
autiles(1).descripcio = SPACE$(30)
autiles(1).precio = 0
RETURN
END SUB

