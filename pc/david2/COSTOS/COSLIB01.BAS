DECLARE FUNCTION pregsprevsimpri% (opciones() AS INTEGER, hastafin AS INTEGER)
DEFINT A-Z

'$INCLUDE: 'coslib.inc'

DIM SHARED colti AS INTEGER
DIM SHARED colpa AS INTEGER

DIM tick AS INTEGER 'para el reloj
DIM pu AS STRING * 1'para el reloj
DIM pu2 AS STRING'para el reloj
DIM yold AS INTEGER'para el reloj
DIM xold AS INTEGER'para el reloj

reloj:
IF (tick) THEN
  pu = ":"
  tick = 0
  ELSE
  pu = " "
  tick = 1
  END IF
xold = POS(0)
yold = CSRLIN
LOCATE 1, 76
pu2 = TIME$
COLOR 7, 0
PRINT LEFT$(pu2, 2); pu; MID$(pu2, 4, 2);
LOCATE yold, xold
COLOR colti, colpa
RETURN
ponsalir:
saleimprimir = 1
RETURN

SUB borramenusystem
VIEW PRINT 1 TO 1
CLS
VIEW PRINT

END SUB

'Box:
'  Draw a box on the screen between the given coordinates.
SUB box (Row1, Col1, Row2, col2) STATIC

    BoxWidth = col2 - Col1 + 1

    LOCATE Row1, Col1
    PRINT "Ú"; STRING$(BoxWidth - 2, "Ä"); "¿";

    FOR a = Row1 + 1 TO Row2 - 1
        LOCATE a, Col1
        PRINT "³"; SPACE$(BoxWidth - 2); "³";
    NEXT a

    LOCATE Row2, Col1
    PRINT "À"; STRING$(BoxWidth - 2, "Ä"); "Ù";

END SUB

'si se pulsa ESC devuelve 0,si no devuelve 1
FUNCTION cambiohoja%
DIM teclapulsada AS STRING

BEEP
center 11, "Prepare otra hoja ... Pulse tecla , ESC abandonar impresion"
teclapulsada = getkey$
IF teclapulsada = CHR$(27) THEN
  saleimprimir = 1
  cambiohoja% = 0
  ELSE
  cambiohoja% = 1
  END IF
IF saleimprimir <> 1 THEN ponimprimiendo
END FUNCTION

'Center:
'  Center text on the given row.
SUB center (row, text$)
    LOCATE row, 41 - LEN(text$) / 2
    PRINT text$;
END SUB

'pregunta si quiere borrar
'devuelve la tecla pulsada (si enter(chr$(13) o escape(chr$(27))
'devuelve en caden lo que se ha escogido "S" o "N"
FUNCTION confirmacion$ (mensaje AS STRING, caden AS STRING)
DIM salida AS STRING
DIM x AS INTEGER
DIM longitud AS INTEGER

longitud = LEN(mensaje)
x = 40 - (longitud / 2)
poncolor 1, 4
box 12, x, 14, x + longitud + 3
LOCATE 13, x + 1
PRINT mensaje;
DO
 salida = inputstr$(13, x + longitud + 1, "N", "S", "N", caden, 1, 1, 0)
 LOOP WHILE salida <> CHR$(27) AND LEFT$(caden, 1) <> "S" AND LEFT$(caden, 1) <> "N"
confirmacion$ = salida
END FUNCTION

'Cvdt$:
'  Convert a double precision number to a string WITHOUT a leading space.
FUNCTION Cvdt$ (x AS DOUBLE)

    Cvdt$ = RIGHT$(STR$(x), LEN(STR$(x)) - 1)

END FUNCTION

'Cvit$:
'  Convert an integer to a string WITHOUT a leading space.
FUNCTION Cvit$ (x AS INTEGER)
    Cvit$ = RIGHT$(STR$(x), LEN(STR$(x)) - 1)
END FUNCTION

FUNCTION Cvit0$ (num%, num0%)

DIM txt AS STRING

txt = Cvit$(num%)

Cvit0$ = STRING$(num0% - LEN(txt), "0") + txt
END FUNCTION

'Cvit$:
'  Convert an long to a string WITHOUT a leading space.
FUNCTION Cvlt$ (x AS LONG)
  Cvlt$ = RIGHT$(STR$(x), LEN(STR$(x)) - 1)
END FUNCTION

SUB donekeyesc

KEY(15) OFF
teclescatrapada = 0

END SUB

DEFSNG A-Z
'Devuelve "R" si se reintenta el error
'Devuelve "I" si se ignora
'Devuelve chr$(27) si se aborta la impresi¢n
FUNCTION errorimpresora$
DIM tec AS STRING

poncolor 1, 4
box 10, 5, 13, 76
center 11, "Error de impresora ..."
center 12, "Presione R para reintentar,I para ignorar , ESC para abortar"
DO
  BEEP
  tec = getkey$
  IF ASC(LEFT$(tec, 1)) >= 97 AND ASC(LEFT$(tec, 1)) <= 122 THEN
    MID$(tec, 1, 1) = CHR$(ASC(LEFT$(tec, 1)) - 32)
    END IF
  LOOP WHILE tec <> "R" AND tec <> "I" AND tec <> CHR$(27)
errorimpresora$ = tec
END FUNCTION

DEFINT A-Z
'Funcion para poder escoger entre varias opciones(opcs)
'Devuelve el indice de la opcion escogida(si es la 1a de la lista,la 2a,...)
'Devuelve 0 si se pulsa ESC
FUNCTION escoge% (opcs() AS STRING, numopcs AS INTEGER, tituloventan AS STRING) STATIC
DIM mayorlong AS INTEGER
DIM n AS INTEGER
DIM x AS INTEGER
DIM y AS INTEGER
DIM tec AS STRING

mayorlong = 0
FOR n = 1 TO numopcs
  IF LEN(opcs(n)) > mayorlong THEN mayorlong = LEN(opcs(n))
  NEXT
IF LEN(tituloventan) > mayorlong THEN mayorlong = LEN(tituloventan) + 2
y = 12 - (numopcs / 2)
x = 40 - (mayorlong / 2)
poncolor 1, 4
box y, x, y + numopcs + 1, x + mayorlong + 1
LOCATE y, x + (mayorlong / 2) - (LEN(tituloventan) / 2) + 1
PRINT tituloventan;
FOR n = 1 TO numopcs
  LOCATE y + n, x + 1
  PRINT opcs(n);
  NEXT
n = 1
GOSUB iluminaopc
DO
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
    GOSUB desiluminaopc
    n = n - 1
    IF n = 0 THEN n = numopcs
    GOSUB iluminaopc
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
    GOSUB desiluminaopc
    n = n + 1
    IF n > numopcs THEN n = 1
    GOSUB iluminaopc
    END SELECT
  LOOP WHILE tec <> CHR$(27) AND tec <> CHR$(13)
IF tec = CHR$(27) THEN
  escoge% = 0
  ELSE
  escoge% = n
  END IF
EXIT FUNCTION
iluminaopc:
poncolor 8, 9
escribeopc:
LOCATE y + n, x + 1
PRINT opcs(n)
RETURN
desiluminaopc:
poncolor 7, 4
GOTO escribeopc
END FUNCTION

FUNCTION getkey$
DIM tec AS STRING

IF teclescatrapada = 1 THEN
  KEY(15) OFF
  END IF
DO
tec = INKEY$
LOOP WHILE tec = ""
IF teclescatrapada = 1 THEN
  KEY(15) ON
  END IF
getkey$ = tec
END FUNCTION

SUB initkeyesc

KEY 15, CHR$(0) + CHR$(1)
ON KEY(15) GOSUB ponsalir
KEY(15) ON
teclescatrapada = 1

END SUB

'funci¢n para leer n£meros en formato double
'fila2,columna posici¢n del cursor
'dblini es el n£mero inicial
'dblfin es donde se almacenar  el n£mero
'longientero es la longitud de la parte entera
'longidecimal es la longitud de la parte de despu‚s del punto
'sitab:si =1 se admite la tecla TAB,si =0 no se admite
'devuelve la tecla que se utiliza para salir,enter(c¢digo 13) o escape (c¢digo 27)
FUNCTION inputdbl$ (fila AS INTEGER, col AS INTEGER, dblini AS DOUBLE, dblfin AS DOUBLE, longientero AS INTEGER, longidecimal AS INTEGER, sitab AS INTEGER)
DIM fila2 AS INTEGER
DIM col2 AS INTEGER
DIM n AS INTEGER
DIM decimal AS INTEGER
DIM punto AS INTEGER
DIM decimales AS INTEGER
DIM cadena AS STRING

cadena = STR$(dblini)
cadena = MID$(cadena, 2, LEN(cadena) - 1)
IF LEN(cadena) = 0 THEN cadena = "0.0"
IF LEFT$(cadena, 1) = "." THEN cadena = "0" + cadena
decimal = 1
n = 1
ancholen = LEN(cadena)
punto = 0
DO
  IF punto = 0 THEN decimal = decimal + 1 ELSE decimales = decimales + 1
  IF MID$(cadena, n, 1) = "." THEN
    punto = 1
    decimales = 0
    END IF
  n = n + 1
LOOP WHILE n <= ancholen
IF punto = 0 THEN
  cadena = cadena + ".0"
  decimales = 1
  decimal = decimal + 1
  END IF
ancholen = LEN(cadena)
IF decimal - 1 < longientero THEN
 cadena = LEFT$(cadena, decimal - 2) + SPACE$(longientero - decimal + 2) + MID$(cadena, decimal - 1, ancholen - decimal + 2)
 END IF
IF decimales < longidecimal THEN
  cadena = LEFT$(cadena, longientero) + "." + MID$(cadena, longientero + 2, decimales) + STRING$(longidecimal - decimales, "0")
  END IF
IF decimales < longidecimal THEN cadena = LEFT$(cadena, longientero + 1) + MID$(cadena, longientero + 2, decimales) + STRING$(longidecimal - decimales, "0")
fila2 = fila
col2 = col
n = 1
ancholen = LEN(cadena)
GOSUB inputdoubleshowtext
LOCATE fila2, col2
LOCATE , , 1

    finished = FALSE
    DO
        GOSUB inputdoubleGetKey

            SELECT CASE kbd$
                CASE CHR$(13)
                    finished = TRUE
                    dblfin = VAL(cadena)
                    inputdbl$ = kbd$
                CASE CHR$(27)
                    finished = TRUE
                    inputdbl$ = kbd$
                CASE CHR$(8)
                    'bs
                    IF n > 1 AND n <= ancholen + 1 THEN
                      IF n < longientero + 1 THEN
                         IF n >= 2 THEN
                           cadena = LEFT$(cadena, n - 2) + MID$(cadena, n, longientero - n + 1) + " " + MID$(cadena, longientero + 1, longidecimal + 1)
                           END IF
                      n = n - 1
                      col2 = col2 - 1
                         END IF
                      IF n > longientero + 2 THEN
                           IF n = longientero + longidecimal + 2 THEN
                           cadena = LEFT$(cadena, longientero + 1) + MID$(cadena, longientero + 2, n - (longientero + 2) - 1) + " "
                           ELSE
                         IF n >= longientero + 3 THEN
                           cadena = LEFT$(cadena, longientero + 1) + MID$(cadena, longientero + 2, n - (longientero + 3)) + MID$(cadena, n, longientero + longidecimal + 2 - n) + " "
                           END IF
                      END IF
                      n = n - 1
                      col2 = col2 - 1
                      END IF
                      GOSUB inputdoubleshowtext
                    END IF
                CASE CHR$(0) + CHR$(83)
                'del
                    IF n <= longientero + longidecimal + 1 THEN
                      IF n <= longientero THEN
                        cadena = LEFT$(cadena, n - 1) + MID$(cadena, n + 1, longientero - n) + " " + MID$(cadena, longientero + 1, longidecimal + 1)
                        END IF
                      IF n >= longientero + 1 THEN
                          cadena = LEFT$(cadena, longientero + 1) + MID$(cadena, longientero + 2, n - (longientero + 2)) + MID$(cadena, n + 1, (longientero + longidecimal + 1) - (n)) + " "
                      END IF
                      GOSUB inputdoubleshowtext
                      END IF
                CASE CHR$(0) + CHR$(75)
                'flecha izquierda
                    IF n > 1 THEN
                      col2 = col2 - 1
                      n = n - 1
                      LOCATE fila2, col2
                      END IF
                CASE CHR$(0) + CHR$(77)
                'flecha derecha
                   IF n <= ancholen THEN
                     col2 = col2 + 1
                     n = n + 1
                     LOCATE fila2, col2
                     END IF
                CASE CHR$(0) + CHR$(71)
                'home
                   n = 1
                   col2 = col
                   LOCATE fila2, col2
                CASE CHR$(0) + CHR$(79)
                'end
                   n = ancholen + 1
                   col2 = col + ancholen
                   LOCATE fila2, col2
                CASE CHR$(9) 'TAB
                    IF sitab = 1 THEN
                       finished = TRUE
                       inputdbl$ = kbd$
                       END IF
                CASE "."
                      BEEP
                      n = longientero + 2
                      col2 = longientero + col + 1
                      LOCATE fila2, col2
                CASE "0" TO "9"
                    IF n <= longientero + longidecimal + 1 THEN
                      IF n <= longientero THEN
                        cadena = LEFT$(cadena, n - 1) + LEFT$(kbd$, 1) + MID$(cadena, n, longientero - n) + MID$(cadena, longientero + 1, longidecimal + 1)
                      ELSE
                      IF n > longientero + 1 THEN
                        cadena = LEFT$(cadena, longientero + 1) + MID$(cadena, longientero + 2, n - longientero - 2) + LEFT$(kbd$, 1) + MID$(cadena, n, longientero + longidecimal - n + 1)
                        END IF
                      END IF
                      n = n + 1
                      col2 = col2 + 1
                      IF MID$(cadena, n, 1) = "." THEN
                        BEEP
                        n = n + 1
                        col2 = col2 + 1
                        END IF
                      GOSUB inputdoubleshowtext
                    END IF
                CASE ELSE
                    BEEP
            END SELECT

    LOOP UNTIL finished

   
    LOCATE , , 0
    EXIT FUNCTION
 

inputdoubleshowtext:
    LOCATE , , 0
    LOCATE fila, col
    PRINT cadena;
    LOCATE fila2, col2
    LOCATE , , 1
    RETURN

inputdoubleGetKey:
    kbd$ = getkey$
    RETURN


END FUNCTION

'fila,columna posici¢n del cursor
'mini caracter m¡nimo
'maxi caracter m ximo
'cadini$ es la cadena inicial
'caden$ es donde se almacenar  la cadena
'longi es la longitud m xima
'amayus:si =1 el texto se convierte a may£sculas,si =0 no se convierte
'sitab:si =1 se admite la tecla TAB,si =0 no se admite
'devuelve la tecla que se utiliza para salir,enter(c¢digo 13) o escape (c¢digo 27)
FUNCTION inputstr$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER, amayus AS INTEGER, sitab AS INTEGER)
DIM fila2 AS INTEGER
DIM col2 AS INTEGER
DIM n AS INTEGER

fila2 = fila
col2 = col
n = 1
IF LEN(cadini$) < longi THEN
  caden$ = cadini$ + SPACE$(longi - LEN(cadini$))
  ELSE
  caden$ = cadini$
  END IF
GOSUB inputstringshowtext
LOCATE fila2, col2
LOCATE , , 1

    finished = FALSE
    DO
        GOSUB inputstringGetKey

            SELECT CASE kbd$
                CASE CHR$(13)
                    finished = TRUE
                    inputstr$ = kbd$
                CASE CHR$(27)
                    finished = TRUE
                    inputstr$ = kbd$
                CASE CHR$(8)
                  'bs
                    IF n > 1 THEN
                      IF longi > 1 THEN
                        IF n <= longi THEN
                          caden$ = MID$(caden$, 1, n - 2) + MID$(caden$, n, longi - n + 1) + " "
                          ELSE
                          caden$ = LEFT$(caden$, longi - 1) + " "
                          END IF
                        END IF
                        IF longi = 1 THEN
                          caden$ = " "
                          END IF
                        n = n - 1
                        col2 = col2 - 1
                        GOSUB inputstringshowtext
                    END IF
                CASE CHR$(0) + CHR$(83)
                'del
                    IF n <= longi THEN
                      IF longi > 1 THEN
                        caden$ = MID$(caden$, 1, n - 1) + MID$(caden$, n + 1, longi - n) + " "
                        END IF
                      IF longi = 1 THEN
                        caden$ = " "
                        END IF
                      GOSUB inputstringshowtext
                      END IF
                CASE CHR$(0) + CHR$(75)
                'flecha izquierda
                    IF n > 1 THEN
                      col2 = col2 - 1
                      n = n - 1
                      LOCATE fila2, col2
                      END IF
                CASE CHR$(0) + CHR$(77)
                'flecha derecha
                   IF n <= longi THEN
                     col2 = col2 + 1
                     n = n + 1
                     LOCATE fila2, col2
                     END IF
                CASE CHR$(0) + CHR$(71)
                'home
                   n = 1
                   col2 = col
                   LOCATE fila2, col2
                CASE CHR$(0) + CHR$(79)
                'end
                   n = longi
                   col2 = col + longi
                   DO
                   IF MID$(caden$, n, 1) <> " " THEN EXIT DO
                   n = n - 1
                   col2 = col2 - 1
                   LOOP WHILE n > 1
                   n = n + 1
                   LOCATE fila2, col2
                CASE CHR$(9) 'TAB
                    IF sitab = 1 THEN
                       finished = TRUE
                       inputstr$ = kbd$
                       END IF
                CASE MID$(mini, 1, 1) TO MID$(maxi, 1, 1)
                    IF n <= longi THEN
                      caden$ = MID$(caden$, 1, n - 1) + LEFT$(kbd$, 1) + MID$(caden$, n, longi - n)
                      n = n + 1
                      col2 = col2 + 1
                      GOSUB inputstringshowtext
                      END IF
                CASE ELSE
                    BEEP
            END SELECT

    LOOP UNTIL finished

    caden$ = caden$
    LOCATE , , 0
    EXIT FUNCTION
  

inputstringshowtext:
    LOCATE , , 0
    LOCATE fila, col
    PRINT caden$;
    LOCATE fila2, col2
    LOCATE , , 1
    RETURN

inputstringGetKey:
    kbd$ = getkey$
    IF amayus = 1 THEN
       IF ASC(LEFT$(kbd$, 1)) >= 97 AND ASC(LEFT$(kbd$, 1)) <= 122 THEN
          MID$(kbd$, 1, 1) = CHR$(ASC(LEFT$(kbd$, 1)) - 32)
          END IF
       END IF
    RETURN

END FUNCTION

DEFSNG A-Z
SUB InstReloj

ON TIMER(1) GOSUB reloj'inicializa el reloj
TIMER ON

END SUB

DEFINT A-Z
'Linea de abajo para:
'articulos ,materiales,tratamientos ,maquinas ,operarios ,taller ,utiles
SUB lineainf
printlineainf "F1 Borrar F2 Insertar F3 Imprimir F4 Buscar Alt-F1 Borrar Todo"
END SUB

SUB ponborrar

poncolor 1, 4
box 10, 20, 13, 40
LOCATE 11, 24
PRINT "Borrando ...";
END SUB

DEFSNG A-Z
SUB poncolor (ti%, pa%)

colti = colors(ti%, colorpref)
colpa = colors(pa%, colorpref)
COLOR colti, colpa

END SUB

DEFINT A-Z
'Pone imprimiendo
SUB ponimprimiendo
poncolor 1, 4
box 10, 5, 13, 76
center 11, "Imprimiendo ... ESC abortar"
END SUB

'esta funcion son las preguntas previas al imprimir como
'imprimir todo,desde o actual
'y si el cambio de hoja es manual o automatico
'devuelve 0 si no se ha pulsado ESC y 1 si se ha pulsado
'en hastafin devuelve 1 si se ha de imprimir hasta el final y 0 si no
'en opciones(1) devuelve 1=todo,2=desde,3=actual
'en opciones(2) devuelve 1=automatico,2=manual
FUNCTION pregsprevsimpri% (opciones() AS INTEGER, hastafin AS INTEGER)
DIM opcmenu(3) AS STRING

opcmenu(1) = "Todo"
opcmenu(2) = "Desde"
opcmenu(3) = "Actual"
opciones(1) = escoge%(opcmenu(), 3, "Imprimir")
pregsprevsimpri% = 1
IF opciones(1) <> 0 THEN
  opcmenu(1) = "Autom tico"
  opcmenu(2) = "Manual"
  opciones(2) = escoge%(opcmenu(), 2, "Cambio de hojas")
  IF opciones(2) <> 0 THEN
    hastafin = 0
    IF opciones(1) = 1 OR opciones(1) = 2 THEN hastafin = 1
    pregsprevsimpri% = 0
  END IF
END IF

END FUNCTION

'PrintHelpLine:
'  Prints help text on the bottom row in the proper color
SUB PrintHelpLine (help$)
    poncolor 5, 4
    LOCATE 25, 1
    PRINT SPACE$(80);
    center 25, help$
END SUB

SUB printlineainf (inf$)
poncolor 1, 4
LOCATE 25, 1
PRINT SPACE$(80);
LOCATE 25, 1
PRINT inf$;
poncolor 2, 1

END SUB

SUB savecfg
OPEN "costos.cfg" FOR OUTPUT AS #ccoscfg
PRINT #ccoscfg, clave1
PRINT #ccoscfg, colorpref
PRINT #ccoscfg, puerto
PRINT #ccoscfg, tipomarcado
PRINT #ccoscfg, costeon
CLOSE #ccoscfg
END SUB

