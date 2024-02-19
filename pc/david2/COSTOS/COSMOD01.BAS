DEFINT A-Z

'$INCLUDE: 'costos.inc'

pantarticulos:
DATA 10
DATA 31, 3, "Articulos"
DATA 2, 6, "C¢digo:"
DATA 17, 6, "Cliente:"
DATA 2, 7, "Pieza:"
DATA 2, 8, "Precio por unidad:"
DATA 34, 8, "Precio costos:"
DATA 2, 9, "Tiempo previsto:"
DATA 34, 9, "Tiempo realizado:"
DATA 29,10,"Notas"
DATA 5,14,"[ESC] Salir"

camposarticulosinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 8
DATA 6,10,5
DATA 6,26,21
DATA 7,9,30
DATA 8,21,12
DATA 8,49,12
DATA 9,19,12
DATA 9,52,12
DATA 11, 2, 234
'234=(80-2)*3

pantmaquinas:
DATA 6
DATA 31, 3, "Maquinas"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Precio por hora:"
DATA 30,7,"Precio por maquina:"
DATA 5,8,"[ESC Salir]"

camposmaquinasinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 4
DATA 6,10,4
DATA 6,29,30
DATA 7,19,10
DATA 7,50,8

pantmateriales:
DATA 7
DATA 31, 3, "Materiales"
DATA 2, 6, "C¢digo:"
DATA 16, 6, "Descripcion:"
DATA 2, 7, "Espesor:"
DATA 21, 7, "Ancho:"
DATA 38, 7, "Precio por kilo:"
DATA 5,8,"[ESC Salir]"

camposmaterialesinfo:
'el primer dato es el n£mero de datos(en parejas de 3)
'    y  x  longitud
DATA 5
DATA 6,10,4
DATA 6,29,45
DATA 7,11,9
DATA 7,28,9
DATA 7,55,12

SUB articulos
DIM m AS INTEGER
DIM n AS INTEGER
DIM tot AS INTEGER
DIM ancholin AS INTEGER
DIM modificado AS INTEGER
DIM campos AS INTEGER
DIM caden AS STRING
DIM cadendbl AS DOUBLE
DIM numregistro AS LONG
DIM totalregistros AS LONG
DIM numreg AS LONG'n£mero de registro "no real" usado para ver imprimir el "n£mero de registro actual"
DIM tec AS STRING
DIM salida AS STRING
DIM reg AS LONG
DIM hastafin AS INTEGER
DIM opciones(2) AS INTEGER'para lo que se escoge al imprimir
DIM lstcodimp AS STRING

idxabre
dtabre "articulo.dat", carticulo
totalregistros = dtobtregistros&(carticulo)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE pantarticulos: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE camposarticulosinfo
READ tot
campos = tot
DIM camposarticulos(tot) AS campos
FOR n = 1 TO tot
   READ camposarticulos(n).y, camposarticulos(n).x, camposarticulos(n).longi
   NEXT
GOSUB pantallaarticulos
lineainf
n = 1
dtir carticulo, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistroart
modificado = 0
DO
  GOSUB resaltacampoart
  tec = getkey$
  SELECT CASE tec
  CASE CHR$(9)
    'tab
    GOSUB desresaltacampoart
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampoart
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampoart
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampoart
    CASE CHR$(13)
    IF totalregistros <> 0 THEN
      GOSUB editacampoart
      END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregart
      dtir carticulo, 1, cdtirposterior, numregistro
      numreg = numreg + 1
      IF numregistro = -1 THEN
        numreg = 1
        dtir carticulo, 1, cdtirinicio, numregistro
        END IF
      GOSUB ponregistroart
      END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
    IF totalregistros <> 0 THEN
      GOSUB actualizaregart
      dtir carticulo, 1, cdtiranterior, numregistro
      numreg = numreg - 1
      IF numregistro = -1 THEN
        dtir carticulo, 1, cdtirfin, numregistro
        numreg = totalregistros
        END IF
      GOSUB ponregistroart
      END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
    IF totalregistros <> 0 THEN
      IF numreg > 1 THEN
        n1 = 1
        DO
          dtir carticulo, 1, cdtiranterior, numregistro
          numreg = numreg - 1
          n1 = n1 + 1
          LOOP WHILE n1 <= 8 AND numregistro <> -1
        IF numreg <= 0 THEN numreg = 1
        GOSUB ponregistroart
        END IF
      END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
    IF totalregistros <> 0 THEN
      IF numreg < totalregistros THEN
        n1 = 1
        DO
          dtir carticulo, 1, cdtirposterior, numregistro
          numreg = numreg + 1
          n1 = n1 + 1
          LOOP WHILE n1 <= 8 AND numregistro <> -1
        IF numreg > totalregistros THEN
          numreg = totalregistros
          END IF
        GOSUB ponregistroart
        END IF
      END IF
    CASE CHR$(0) + CHR$(71)
    'home
    IF totalregistros <> 0 THEN
      GOSUB actualizaregart
      dtir carticulo, 1, cdtirinicio, numregistro
      numreg = 1
      GOSUB ponregistroart
      END IF
    CASE CHR$(0) + CHR$(79)
    'end
    IF totalregistros <> 0 THEN
      GOSUB actualizaregart
      dtir carticulo, 1, cdtirfin, numregistro
      numreg = totalregistros
      GOSUB ponregistroart
      END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregart
    numreg = totalregistros + 1
    GOSUB ponblancoarticulos
    articulo(1).cformatec = SPACE$(5)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistroart2
    n = 1
    GOSUB editacampoart
    modificado = 1
    caden = articulo(1).cformatec
    articulo(2) = articulo(1)
    n1 = dtbuscar%(carticulo, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar carticulo, 1
      END IF
      dtir carticulo, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(articulo(1).cformatec, caden) <> 0
        dtir carticulo, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(carticulo)
    GOSUB ponregistroart
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
    IF totalregistros <> 0 THEN
      GOSUB actualizaregart
      salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        dtborrar carticulo, 1, numregistro
        IF numreg >= totalregistros THEN numreg = numreg - 1
        END IF
      totalregistros = dtobtregistros&(carticulo)
      GOSUB pantallaarticulos
      GOSUB ponregistroart
      END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
      IF totalregistros <> 0 THEN
        GOSUB actualizaregart
        salida = confirmacion$("¨ Quiere borrar el archivo de articulos ?", caden)
        IF caden = "S" AND salida <> CHR$(27) THEN
          ponborrar
          dtborrartodo carticulo
          numreg = 0
          numregistro = -1
          GOSUB ponblancoarticulos
          END IF
        totalregistros = dtobtregistros&(carticulo)
        GOSUB pantallaarticulos
        GOSUB ponregistroart
        END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
    IF totalregistros <> 0 THEN
      IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
            CASE 1
            dtir carticulo, 2, cdtirfin, reg
            lstcodimp = articulo(2).cformatec
            dtir carticulo, 1, cdtirinicio, reg
            CASE 2
            dtir carticulo, 2, cdtirfin, reg
            lstcodimp = articulo(2).cformatec
            dtIrA carticulo, 1, numregistro
            CASE 3
            lstcodimp = articulo(1).cformatec
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
                imprime carticulo, opciones(2)
                IF saleimprimir = 1 THEN EXIT DO
                dtir carticulo, 1, cdtirposterior, reg
                LOOP WHILE (strcmp%(articulo(1).cformatec, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
              dtIrA carticulo, 1, numregistro
              END IF
            END IF
        nuevahoja = 0
       GOSUB pantallaarticulos
       GOSUB ponregistroart
       END IF
       END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregart
CLOSE #carticulo
CLOSE #ccosidx
EXIT SUB
pantallaarticulos:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 14, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 10, 1: PRINT caden
LOCATE 10, 1: PRINT "Ã"
LOCATE 10, 80: PRINT "´"
LOCATE 14, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE pantarticulos: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistroart:
IF numregistro = -1 THEN
  GOSUB ponblancoarticulos
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, articulo(1).cliente
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistroart2:
m = n
FOR n = 1 TO campos
   GOSUB poncampoart
   NEXT
n = m
RETURN
resaltacampoart:
poncolor 7, 4
poncampoart:
  LOCATE camposarticulos(n).y, camposarticulos(n).x
  SELECT CASE n
      CASE 1
      PRINT articulo(1).cformatec;
      CASE 2
      PRINT articulo(1).cliente;
      CASE 3
      PRINT articulo(1).pieza;
      CASE 4
      PRINT USING "#######.####"; articulo(1).prunidad;
      CASE 5
      PRINT USING "#######.####"; articulo(1).prcostos;
      CASE 6
      PRINT USING "#######.####"; articulo(1).tprevisto;
      CASE 7
      PRINT USING "#######.####"; articulo(1).treal;
      CASE 8
      ' cuando el campo de notas
      ancholin = 80 - camposarticulos(8).x
      PRINT LEFT$(articulo(1).notas, ancholin);
      LOCATE camposarticulos(8).y + 1, camposarticulos(8).x
      PRINT MID$(articulo(1).notas, 1 + ancholin, ancholin);
      LOCATE camposarticulos(8).y + 2, camposarticulos(8).x
      PRINT MID$(articulo(1).notas, 1 + ancholin + ancholin, ancholin);
      END SELECT
poncolor 3, 10
RETURN
desresaltacampoart:
poncolor 3, 10
GOTO poncampoart
editacampoart:
SELECT CASE n
      CASE 1
      salida = inputstr$(camposarticulos(1).y, camposarticulos(1).x, CHR$(32), CHR$(255), articulo(1).cformatec, caden, camposarticulos(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(camposarticulos(2).y, camposarticulos(2).x, CHR$(32), CHR$(255), articulo(1).cliente, caden, camposarticulos(2).longi, 1, 1)
      CASE 3
      salida = inputstr$(camposarticulos(3).y, camposarticulos(3).x, CHR$(32), CHR$(255), articulo(1).pieza, caden, camposarticulos(3).longi, 1, 1)
      CASE 4
      salida = inputdbl$(camposarticulos(4).y, camposarticulos(4).x, articulo(1).prunidad, cadendbl, 7, 4, 1)
      CASE 5
      salida = inputdbl$(camposarticulos(5).y, camposarticulos(5).x, articulo(1).prcostos, cadendbl, 7, 4, 1)
      CASE 6
      salida = inputdbl$(camposarticulos(6).y, camposarticulos(6).x, articulo(1).tprevisto, cadendbl, 7, 4, 1)
      CASE 7
      salida = inputdbl$(camposarticulos(7).y, camposarticulos(7).x, articulo(1).treal, cadendbl, 7, 4, 1)
      CASE 8
      ' cuando el campo de notas
      salida = inputnota$(camposarticulos(8).y, camposarticulos(8).x, CHR$(32), CHR$(255), articulo(1).notas, caden, camposarticulos(8).longi)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampoart
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      articulo(1).cformatec = caden
      CASE 2
      articulo(1).cliente = caden
      CASE 3
      articulo(1).pieza = caden
      CASE 4
      articulo(1).prunidad = cadendbl
      CASE 5
      articulo(1).prcostos = cadendbl
      CASE 6
      articulo(1).tprevisto = cadendbl
      CASE 7
      articulo(1).treal = cadendbl
      CASE 8
      'cuando el campo de notas
      articulo(1).notas = caden
      END SELECT
  END IF
  RETURN
actualizaregart:
IF modificado = 1 THEN
  dtmodificar carticulo, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancoarticulos:
articulo(1).cformatec = SPACE$(5)
articulo(1).cliente = SPACE$(21)
articulo(1).pieza = SPACE$(30)
articulo(1).prunidad = 0
articulo(1).prcostos = 0
articulo(1).tprevisto = 0
articulo(1).treal = 0
articulo(1).notas = SPACE$(254)
RETURN
END SUB

SUB maquinas
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
dtabre "amaquina.dat", camaquina
totalregistros = dtobtregistros&(camaquina)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE pantmaquinas: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE camposmaquinasinfo
READ tot
campos = tot
DIM camposmaquinas(tot) AS campos
FOR n = 1 TO tot
   READ camposmaquinas(n).y, camposmaquinas(n).x, camposmaquinas(n).longi
   NEXT
GOSUB pantallamaquinas
lineainf
n = 1
dtir camaquina, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistromaq
modificado = 0
DO
  GOSUB resaltacampomaq
  tec = getkey$
  SELECT CASE tec
  CASE CHR$(9)
    'tab
    GOSUB desresaltacampomaq
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampomaq
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampomaq
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampomaq
    CASE CHR$(13)
    IF totalregistros <> 0 THEN
      GOSUB editacampomaq
      END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmaq
      dtir camaquina, 1, cdtirposterior, numregistro
      numreg = numreg + 1
      IF numregistro = -1 THEN
        dtir camaquina, 1, cdtirinicio, numregistro
        numreg = 1
        END IF
      GOSUB ponregistromaq
      END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmaq
      dtir camaquina, 1, cdtiranterior, numregistro
      numreg = numreg - 1
      IF numregistro = -1 THEN
        dtir camaquina, 1, cdtirfin, numregistro
        numreg = totalregistros
        END IF
      GOSUB ponregistromaq
      END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
    IF totalregistros <> 0 THEN
      IF numreg > 1 THEN
        n1 = 1
        DO
          dtir camaquina, 1, cdtiranterior, numregistro
          numreg = numreg - 1
          n1 = n1 + 1
          LOOP WHILE n1 <= 8 AND numregistro <> -1
        IF numreg <= 0 THEN numreg = 1
        GOSUB ponregistromaq
        END IF
      END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
    IF totalregistros <> 0 THEN
      IF numreg < totalregistros THEN
        n1 = 1
        DO
          dtir camaquina, 1, cdtirposterior, numregistro
          numreg = numreg + 1
          n1 = n1 + 1
          LOOP WHILE n1 <= 8 AND numregistro <> -1
        IF numreg > totalregistros THEN
          numreg = totalregistros
          END IF
        GOSUB ponregistromaq
        END IF
        END IF
    CASE CHR$(0) + CHR$(71)
    'home
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmaq
      dtir camaquina, 1, cdtirinicio, numregistro
      numreg = 1
      GOSUB ponregistromaq
      END IF
    CASE CHR$(0) + CHR$(79)
    'end
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmaq
      dtir camaquina, 1, cdtirfin, numregistro
      numreg = totalregistros
      GOSUB ponregistromaq
      END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregmaq
    numreg = totalregistros + 1
    GOSUB ponblancomaquinas
    amaquina(1).codmaquina = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistromaq2
    n = 1
    GOSUB editacampomaq
    modificado = 1
    caden = amaquina(1).codmaquina
    amaquina(2) = amaquina(1)
    n1 = dtbuscar%(camaquina, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar camaquina, 1
      END IF
      dtir camaquina, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(amaquina(1).codmaquina, caden) <> 0
        dtir camaquina, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(camaquina)
    GOSUB ponregistromaq
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmaq
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar camaquina, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(camaquina)
    GOSUB pantallamaquinas
    GOSUB ponregistromaq
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmaq
      salida = confirmacion$("¨ Quiere borrar el archivo de maquinas ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo camaquina
        numreg = 0
        numregistro = -1
        GOSUB ponblancomaquinas
        END IF
      totalregistros = dtobtregistros&(camaquina)
      GOSUB pantallamaquinas
      GOSUB ponregistromaq
    END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir camaquina, 2, cdtirfin, reg
          lstcodimp = amaquina(2).codmaquina
          dtir camaquina, 1, cdtirinicio, reg
          CASE 2
          dtir camaquina, 2, cdtirfin, reg
          lstcodimp = amaquina(2).codmaquina
          dtIrA camaquina, 1, numregistro
          CASE 3
          lstcodimp = amaquina(1).codmaquina
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
              imprime camaquina, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir camaquina, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(amaquina(1).codmaquina, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA camaquina, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallamaquinas
     GOSUB ponregistromaq
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregmaq
CLOSE #camaquina
CLOSE #ccosidx
EXIT SUB
pantallamaquinas:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE pantmaquinas: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistromaq:
IF numregistro = -1 THEN
  GOSUB ponblancomaquinas
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, amaquina(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistromaq2:
m = n
FOR n = 1 TO campos
   GOSUB poncampomaq
   NEXT
n = m
RETURN
resaltacampomaq:
poncolor 7, 4
poncampomaq:
  LOCATE camposmaquinas(n).y, camposmaquinas(n).x
  SELECT CASE n
      CASE 1
      PRINT amaquina(1).codmaquina;
      CASE 2
      PRINT amaquina(1).descripcio;
      CASE 3
      PRINT USING "#######.##"; amaquina(1).preciohora;
      CASE 4
      PRINT USING "########"; amaquina(1).preciomaqu
      END SELECT
poncolor 3, 10
RETURN
desresaltacampomaq:
poncolor 3, 10
GOTO poncampomaq
editacampomaq:
SELECT CASE n
      CASE 1
      salida = inputstr$(camposmaquinas(1).y, camposmaquinas(1).x, CHR$(32), CHR$(255), amaquina(1).codmaquina, caden, camposmaquinas(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(camposmaquinas(2).y, camposmaquinas(2).x, CHR$(32), CHR$(255), amaquina(1).descripcio, caden, camposmaquinas(2).longi, 1, 1)
      CASE 3
      salida = inputdbl$(camposmaquinas(3).y, camposmaquinas(3).x, amaquina(1).preciohora, cadendbl, 7, 2, 1)
      CASE 4
      salida = inputstr$(camposmaquinas(4).y, camposmaquinas(4).x, CHR$(48), CHR$(57), Cvlt$(amaquina(1).preciomaqu), caden, camposmaquinas(4).longi, 1, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampomaq
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      amaquina(1).codmaquina = caden
      CASE 2
      amaquina(1).descripcio = caden
      CASE 3
      amaquina(1).preciohora = cadendbl
      CASE 4
      amaquina(1).preciomaqu = VAL(caden)
      END SELECT
  END IF
  RETURN
actualizaregmaq:
IF modificado = 1 THEN
  dtmodificar camaquina, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancomaquinas:
amaquina(1).codmaquina = SPACE$(4)
amaquina(1).descripcio = SPACE$(30)
amaquina(1).preciohora = 0
amaquina(1).preciomaqu = 0
RETURN
END SUB

SUB materiales
DIM m AS INTEGER
DIM n AS INTEGER
DIM tot AS INTEGER
DIM ancholin AS INTEGER
DIM modificado AS INTEGER
DIM campos AS INTEGER
DIM cadendbl AS DOUBLE
DIM numregistro AS LONG
DIM numreg AS LONG'n£mero de registro "no real" usado para ver imprimir el "n£mero de registro actual"
DIM totalregistros AS LONG
DIM tec AS STRING
DIM salida AS STRING
DIM caden AS STRING
DIM reg AS LONG
DIM hastafin AS INTEGER
DIM opciones(2) AS INTEGER'para lo que se escoge al imprimir
DIM lstcodimp AS STRING


idxabre
dtabre "amateria.dat", camateria
totalregistros = dtobtregistros&(camateria)
CLS
poncolor 1, 4
fancycls 2, 1
RESTORE pantmateriales: READ tot: DIM col(tot) AS INTEGER
DIM fil(tot) AS INTEGER
DIM cadenas(tot) AS STRING
FOR n = 1 TO tot: READ col(n), fil(n), cadenas(n): NEXT
RESTORE camposmaterialesinfo
READ campos
DIM camposmateriales(campos) AS campos
FOR n = 1 TO campos
   READ camposmateriales(n).y, camposmateriales(n).x, camposmateriales(n).longi
   NEXT
GOSUB pantallamateriales
lineainf
n = 1
dtir camateria, 1, cdtirinicio, numregistro
numreg = 1
GOSUB ponregistromat
modificado = 0
DO
  GOSUB resaltacampomat
  tec = getkey$
  SELECT CASE tec
  CASE CHR$(9)
    'tab
    GOSUB desresaltacampomat
    n = n + 1
    IF n > campos THEN n = 1
    GOSUB resaltacampomat
    CASE CHR$(0) + CHR$(15)
    'caps+tab
      GOSUB desresaltacampomat
      n = n - 1
      IF n = 0 THEN n = campos
      GOSUB resaltacampomat
    CASE CHR$(13)
    'enter
  IF totalregistros <> 0 THEN
    GOSUB editacampomat
    END IF
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmat
    dtir camateria, 1, cdtirposterior, numregistro
    numreg = numreg + 1
    IF numregistro = -1 THEN
      dtir camateria, 1, cdtirinicio, numregistro
      numreg = 1
      END IF
    GOSUB ponregistromat
    END IF
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmat
    dtir camateria, 1, cdtiranterior, numregistro
    numreg = numreg - 1
    IF numregistro = -1 THEN
      dtir camateria, 1, cdtirfin, numregistro
      numreg = totalregistros
      END IF
    GOSUB ponregistromat
    END IF
    CASE CHR$(0) + CHR$(73)
    'pgup
  IF totalregistros <> 0 THEN
    IF numreg > 1 THEN
      n1 = 1
      DO
        dtir camateria, 1, cdtiranterior, numregistro
        numreg = numreg - 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg <= 0 THEN numreg = 1
      GOSUB ponregistromat
      END IF
    END IF
    CASE CHR$(0) + CHR$(81)
    'pgdown
  IF totalregistros <> 0 THEN
    IF numreg < totalregistros THEN
      n1 = 1
      DO
        dtir camateria, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        n1 = n1 + 1
        LOOP WHILE n1 <= 8 AND numregistro <> -1
      IF numreg > totalregistros THEN
        numreg = totalregistros
        END IF
      GOSUB ponregistromat
      END IF
    END IF
    CASE CHR$(0) + CHR$(71)
    'home
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmat
    dtir camateria, 1, cdtirinicio, numregistro
    numreg = 1
    GOSUB ponregistromat
    END IF
    CASE CHR$(0) + CHR$(79)
    'end
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmat
    dtir camateria, 1, cdtirfin, numregistro
    numreg = totalregistros
    GOSUB ponregistromat
    END IF
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    GOSUB actualizaregmat
    numreg = totalregistros + 1
    GOSUB ponblancomateriales
    amateria(1).codmateria = SPACE$(4)
    poncolor 2, 1
    LOCATE 4, 3: PRINT SPACE$(47);
    LOCATE 4, 50: PRINT SPACE$(21);
    printright 4, 78, STR$(numreg) + "/" + STR$(numreg)
    GOSUB ponregistromat2
    n = 1
    GOSUB editacampomat
    modificado = 1
    caden = amateria(1).codmateria
    amateria(2) = amateria(1)
    n1 = dtbuscar%(camateria, 2, numregistro, TRUE)
    IF n1 <> TRUE THEN
      'si no existe
      dtinsertar camateria, 1
      END IF
      dtir camateria, 1, cdtirinicio, numregistro
      numreg = 1
      WHILE strcmp%(amateria(1).codmateria, caden) <> 0
        dtir camateria, 1, cdtirposterior, numregistro
        numreg = numreg + 1
        WEND
    totalregistros = dtobtregistros&(camateria)
    GOSUB ponregistromat
  CASE CHR$(0) + CHR$(59)
  'F1: Borrar
  IF totalregistros <> 0 THEN
    GOSUB actualizaregmat
    salida = confirmacion$("¨Est  seguro de querer borrar la ficha?", caden)
    IF caden = "S" AND salida <> CHR$(27) THEN
      dtborrar camateria, 1, numregistro
      IF numreg >= totalregistros THEN numreg = numreg - 1
      END IF
    totalregistros = dtobtregistros&(camateria)
    GOSUB pantallamateriales
    GOSUB ponregistromat
    END IF
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
    IF totalregistros <> 0 THEN
      GOSUB actualizaregmat
      salida = confirmacion$("¨ Quiere borrar el archivo de materiales ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo camateria
        numreg = 0
        numregistro = -1
        GOSUB ponblancomateriales
        END IF
      totalregistros = dtobtregistros&(camateria)
      GOSUB pantallamateriales
      GOSUB ponregistromat
      END IF
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
  IF totalregistros <> 0 THEN
    IF pregsprevsimpri%(opciones(), hastafin) <> 1 THEN
        SELECT CASE opciones(1)
          CASE 1
          dtir camateria, 2, cdtirfin, reg
          lstcodimp = amateria(2).codmateria
          dtir camateria, 1, cdtirinicio, reg
          CASE 2
          dtir camateria, 2, cdtirfin, reg
          lstcodimp = amateria(2).codmateria
          dtIrA camateria, 1, numregistro
          CASE 3
          lstcodimp = amateria(1).codmateria
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
              imprime camateria, opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir camateria, 1, cdtirposterior, reg
              LOOP WHILE (strcmp%(amateria(1).codmateria, lstcodimp) < 0 AND hastafin = 0) OR (reg <> -1 AND hastafin = 1)
            dtIrA camateria, 1, numregistro
            END IF
          END IF
      nuevahoja = 0
     GOSUB pantallamateriales
     GOSUB ponregistromat
     END IF
    END SELECT
  LOOP WHILE tec <> CHR$(27)
GOSUB actualizaregmat
CLOSE #ccosidx
CLOSE #camateria
EXIT SUB
pantallamateriales:
fancycls 2, 1
poncolor 2, 1
box 3, 1, 8, 80
caden = STRING$(79, "Ä")
LOCATE 5, 1: PRINT caden
LOCATE 5, 1: PRINT "Ã"
LOCATE 5, 80: PRINT "´"
LOCATE 8, 60: PRINT CHR$(24) + " " + CHR$(25) + " PgUp  PgDown"
RESTORE pantmateriales: READ tot: FOR m = 1 TO tot:
LOCATE fil(m), col(m): PRINT cadenas(m); : NEXT
RETURN
ponregistromat:
IF numregistro = -1 THEN
  GOSUB ponblancomateriales
  END IF
poncolor 2, 1
LOCATE 4, 3: PRINT SPACE$(47);
center 4, amateria(1).descripcio
LOCATE 4, 50: PRINT SPACE$(21);
printright 4, 78, STR$(numreg) + "/" + STR$(totalregistros)
ponregistromat2:
m = n
FOR n = 1 TO campos
   GOSUB poncampomat
   NEXT
n = m
RETURN
resaltacampomat:
poncolor 7, 4
poncampomat:
  LOCATE camposmateriales(n).y, camposmateriales(n).x
  SELECT CASE n
      CASE 1
      PRINT amateria(1).codmateria;
      CASE 2
      PRINT amateria(1).descripcio;
      CASE 3
      PRINT USING "#####.###"; amateria(1).espesor;
      CASE 4
      PRINT USING "#####.###"; amateria(1).ancho;
      CASE 5
      PRINT USING "#########.##"; amateria(1).preciokilo;
      END SELECT
poncolor 3, 10
RETURN
desresaltacampomat:
poncolor 3, 10
GOTO poncampomat
editacampomat:
SELECT CASE n
      CASE 1
      salida = inputstr$(camposmateriales(1).y, camposmateriales(1).x, CHR$(32), CHR$(255), amateria(1).codmateria, caden, camposmateriales(1).longi, 1, 1)
      CASE 2
      salida = inputstr$(camposmateriales(2).y, camposmateriales(2).x, CHR$(32), CHR$(255), amateria(1).descripcio, caden, camposmateriales(2).longi, 1, 1)
      CASE 3
      salida = inputdbl$(camposmateriales(3).y, camposmateriales(3).x, amateria(1).espesor, cadendbl, 5, 3, 1)
      CASE 4
      salida = inputdbl$(camposmateriales(4).y, camposmateriales(4).x, amateria(1).ancho, cadendbl, 5, 3, 1)
      CASE 5
      salida = inputdbl$(camposmateriales(5).y, camposmateriales(5).x, amateria(1).preciokilo, cadendbl, 9, 2, 1)
      END SELECT
IF ASC(LEFT$(salida, 1)) = 27 THEN
  GOSUB poncampomat
  ELSE
  modificado = 1
  SELECT CASE n
      CASE 1
      amateria(1).codmateria = caden
      CASE 2
      amateria(1).descripcio = caden
      CASE 3
      amateria(1).espesor = cadendbl
      CASE 4
      amateria(1).ancho = cadendbl
      CASE 5
      amateria(1).preciokilo = cadendbl
      END SELECT
  END IF
  RETURN
actualizaregmat:
IF modificado = 1 THEN
  dtmodificar camateria, 1, 2, numregistro
  END IF
modificado = 0
RETURN
ponblancomateriales:
amateria(1).codmateria = SPACE$(4)
amateria(1).descripcio = SPACE$(45)
amateria(1).espesor = 0
amateria(1).ancho = 0
amateria(1).preciokilo = 0
RETURN
END SUB

