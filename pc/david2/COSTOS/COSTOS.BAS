DEFINT A-Z
'Set default data type to integer for faster operation

'$INCLUDE: 'costos.inc'


'Sub and function declarations
DECLARE SUB encripta (entrada AS STRING, salida AS STRING)
DECLARE FUNCTION Menu% (CurrChoiceX%, maxchoice%, choice$(), itemrow%(), itemcol%(), help$(), barmode%)
DECLARE SUB informacion ()
DECLARE SUB MenuSystem ()

DECLARE FUNCTION clave% () 'si no es correcta devuelve -1
DECLARE FUNCTION inputclave$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER)


    SCREEN 0
    InstReloj
    Initialize          'Initialize program
    IF comenzar = 0 THEN
      'carga la configuraci¢n
      'si no existe , la crea
      opencfg
      encripta clave1, clave2 'desencripta la clave
      IF clave% <> 0 THEN GOTO costosfin
      comenzar = 1
      END IF
    MenuSystem          'This is the main program
costosfin:
    TIMER OFF
    COLOR 7, 0          'Clear screen and end
    CLS

    END

MenuSystemDatosInfo:

  DATA 9

  DATA " Tel‚fono     ","Accesa a List¡n telef¢nico"
  DATA " Art¡culo     ","Accesa a los art¡culos"
  DATA " Material     ","Accesa a los materiales"
  DATA " Tratamientos ","Accesa a los tratamientos"
  DATA " Maquinas     ","Accesa a maquinas"
  DATA " Operarios    ","Accesa a los operarios"
  DATA " Taller       ","Accesa al taller"
  DATA " Utiles       ","Accesa a los £tiles"
  DATA " Costeo       ","Accesa al costeo"

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

'si devuelve -1,clave incorrecta,si devuelve 0 clave correcta
FUNCTION clave%
DIM clavei AS STRING

VIEW PRINT
CLS
fancycls 2, 1
box 10, 10, 12, 60
LOCATE 11, 11
PRINT "Clave : "
sale$ = inputclave$(11, 19, CHR$(32), CHR$(255), "", clavei, 43)
IF clavei <> clave2 OR ASC(MID$(sale$, 1, 1)) = 27 THEN
  clave% = -1
  ELSE clave% = 0
  END IF
END FUNCTION

SUB encripta (entrada AS STRING, salida AS STRING)
DIM i, n AS INTEGER
i = LEN(entrada)
salida = ""
FOR n = 1 TO i
salida = salida + CHR$(255 - (ASC(MID$(entrada, n, 1)) XOR 188))
NEXT n
END SUB

'FancyCls:
'  Clears screen in the right color, and draws nice dots.
SUB fancycls (ti%, pa%)
   
  DIM logo AS STRING * 80
   
  logo = STRING$(80, "°")
  poncolor ti%, pa%
  FOR a = 2 TO 24
    LOCATE a, 1
    PRINT logo;
    NEXT a

END SUB

'fichero es el c¢digo del fichero,de altrata:caltrata,de amateria:camateria,etc.
'opciones2 es el tipo de salto de p gina:si =1 cambio de hoja autom tico,si =2 cambio de hoja manual
SUB imprime (fichero AS INTEGER, opciones2 AS INTEGER)
STATIC temp AS STRING
STATIC longi AS INTEGER

initkeyesc
ON ERROR GOTO errorimpresora
WIDTH LPRINT 138
IF nuevahoja = 1 THEN
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimp
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimp
  SELECT CASE fichero
    CASE carticulo
    temp = "ARTICULOS"
    CASE camateria
    temp = "MATERIALES"
    CASE caltrata
    temp = "TRATAMIENTOS"
    CASE camaquina
    temp = "MAQUINAS"
    CASE caoperari
    temp = "OPERARIOS"
    CASE cataller
    temp = "TALLER"
    CASE cautiles
    temp = "UTILES"
    END SELECT
  LPRINT TAB(14); CHR$(14); temp; CHR$(20)
  IF saleimprimir = 1 THEN GOTO finimp
  LPRINT
  IF saleimprimir = 1 THEN GOTO finimp
  SELECT CASE fichero
    CASE carticulo
    temp = "C¢digo Cliente" + SPACE$(15) + "Pieza" + SPACE$(26) + "Precio/Unidad Precio/Costos Tiempo previsto Tiempo Real"
    CASE camateria
    temp = "Codigo Descripci¢n" + SPACE$(37) + " Espesor   Ancho      Precio/Kilo"
    CASE caltrata
    temp = "C¢digo Descripci¢n" + SPACE$(36) + "Precio/Kilo"
    CASE camaquina
    temp = "C¢digo Descripci¢n" + SPACE$(21) + "Precio/Hora  Precio/Maquina"
    CASE caoperari
    temp = "C¢digo Descripci¢n" + SPACE$(21) + "Precio/Hora  Costo anual"
    CASE cataller
    temp = "C¢digo Descripci¢n" + SPACE$(21) + "Precio/Hora  Precio/Maquina"
    CASE cautiles
    temp = "C¢digo Descripci¢n" + SPACE$(21) + "Precio"
    END SELECT
  IF fichero = carticulo OR fichero = camateria THEN
    LPRINT CHR$(15);
    IF saleimprimir = 1 THEN GOTO finimp
    END IF
  LPRINT temp;
  IF saleimprimir = 1 THEN GOTO finimp
  IF fichero = carticulo OR fichero = camateria THEN
    LPRINT CHR$(18)
    ELSE
    LPRINT
    END IF
  IF saleimprimir = 1 THEN GOTO finimp
  nuevahoja = 0
END IF
IF fichero = carticulo OR fichero = camateria THEN
  LPRINT CHR$(15);
  IF saleimprimir = 1 THEN GOTO finimp
  END IF
SELECT CASE fichero
  CASE carticulo
  LPRINT articulo(1).cformatec; "  "; articulo(1).cliente; " "; articulo(1).pieza; " "; USING "#######.####  #######.####  #######.####    #######.####"; articulo(1).prunidad; articulo(1).prcostos; articulo(1).tprevisto; articulo(1).treal;
  CASE camateria
  LPRINT amateria(1).codmateria; "   "; amateria(1).descripcio; "  "; USING "#####.###  #####.###  #########.##"; amateria(1).espesor; amateria(1).ancho; amateria(1).preciokilo;
  CASE caltrata
  LPRINT altrata(1).codtratami; "   "; altrata(1).descripcio; "  "; USING "#########.##"; altrata(1).preciokilo;
  CASE camaquina
  LPRINT amaquina(1).codmaquina; "   "; amaquina(1).descripcio; "  "; USING "#######.##   ########"; amaquina(1).preciohora; amaquina(1).preciomaqu;
  CASE caoperari
  LPRINT aoperari(1).codoperari; "   "; aoperari(1).descripcio; "    "; USING "#####       #########"; aoperari(1).preciohora; aoperari(1).costoanual;
  CASE cataller
  LPRINT ataller(1).codmaquina; "   "; ataller(1).descripcio; "   "; USING "#######.##  #########"; ataller(1).preciohora; ataller(1).preciomaqu;
  CASE cautiles
  LPRINT autiles(1).codutil; "   "; autiles(1).descripcio; "  "; USING "#######"; autiles(1).precio;
  END SELECT
IF fichero = carticulo OR fichero = camateria THEN
  LPRINT CHR$(18)
  ELSE
  LPRINT
  END IF
IF saleimprimir = 1 THEN GOTO finimp
lineaimpresora = lineaimpresora + 1
IF lineaimpresora = 50 THEN
  LPRINT CHR$(13) + CHR$(12)
  IF saleimprimir = 1 THEN GOTO finimp
  IF tipocambiohoja = 2 THEN
    temi = cambiohoja%
    IF temi = 0 THEN
      GOTO finimp
      END IF
    END IF
  nuevahoja = 1
  lineaimpresora = 0
  END IF
finimp:
donekeyesc
ON ERROR GOTO 0
END SUB

SUB informacion
DIM tec AS STRING

box 10, 30, 15, 60
LOCATE 11, 37
PRINT "Programa de costos"
LOCATE 13, 34
PRINT "(c) Disseny Impr‚s 1997"
tec = getkey$
END SUB

'fila2,columna posici¢n del cursor
'mini caracter m¡nimo
'maxi caracter m ximo
'cadini$ es la cadena inicial
'caden$ es donde se almacenar  la cadena
'longi es la longitud m xima
FUNCTION inputclave$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER)
DIM fila2, col2 AS INTEGER

fila2 = fila
col2 = col + LEN(cadini$)
LOCATE fila2, col2
LOCATE , , 1
curr$ = ""

    finished = FALSE
    DO
        GOSUB inputclaveGetKey

            SELECT CASE kbd$
                CASE CHR$(13)
                    finished = TRUE
                    inputclave$ = kbd$
                CASE CHR$(27)
                    finished = TRUE
                    inputclave$ = kbd$
                CASE CHR$(8)
                    IF curr$ <> "" THEN
                        curr$ = LEFT$(curr$, LEN(curr$) - 1)
                        LOCATE fila2, col2 - 1
                        PRINT " ";
                        col2 = col2 - 1
                        LOCATE fila2, col2
                    END IF
                CASE CHR$(9) 'TAB
                    IF sitab = 1 THEN
                       finished = TRUE
                       inputclave$ = kbd$
                       END IF
                CASE MID$(mini, 1, 1) TO MID$(maxi, 1, 1)
                    IF LEN(caden$) < longi THEN
                        curr$ = curr$ + kbd$
                        GOSUB inputclaveshowtext
                        col2 = col2 + 1
                        LOCATE fila2, col2
                    ELSE
                        BEEP
                    END IF

                CASE ELSE
                    BEEP
            END SELECT

    LOOP UNTIL finished

    caden$ = curr$
    LOCATE , , 0
    EXIT FUNCTION


inputclaveshowtext:
    LOCATE fila2, col2
    PRINT "*";
    RETURN

inputclaveGetKey:
    kbd$ = getkey$
    RETURN


END FUNCTION

'funci¢n exclusiva para las notas
'fila2,columna posici¢n del cursor
'mini caracter m¡nimo
'maxi caracter m ximo
'cadini$ es la cadena inicial
'caden$ es donde se almacenar  la cadena
'longi es la longitud m xima
FUNCTION inputnota$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER)
DIM fila2 AS INTEGER
DIM col2 AS INTEGER
DIM n AS INTEGER
DIM ancholin AS INTEGER
DIM lineatope AS INTEGER
DIM anchostr AS INTEGER
DIM n2 AS INTEGER

ancholin = 80 - col
lineatope = fila + 2
IF LEN(cadini$) < longi THEN
  caden$ = cadini$ + SPACE$(longi - LEN(cadini$))
  ELSE
  caden$ = cadini$
  END IF
IF LEN(cadini$) > longi THEN
  caden$ = LEFT$(caden$, longi)
  END IF
anchostr = longi
GOSUB pon3lineas
n = 1'indice de caden$
fila2 = fila
col2 = col
GOSUB pon3lineas
LOCATE fila2, col2
LOCATE , , 1
finished = FALSE
    DO
        GOSUB inputnotaGetKey

            SELECT CASE kbd$
                CASE CHR$(13)
                    finished = TRUE
                    inputnota$ = kbd$
                CASE CHR$(27)
                    finished = TRUE
                    inputnota$ = kbd$
                CASE CHR$(8)
                    'bs
                    IF n > 1 THEN
                        caden$ = LEFT$(caden$, n - 2) + MID$(caden$, n, anchostr - n + 1) + " "
                        n = n - 1
                        GOSUB pon3lineas
                        col2 = col2 - 1
                        IF col2 < col THEN
                          col2 = col + ancholin - 1
                          fila2 = fila2 - 1
                          END IF
                        LOCATE fila2, col2
                    END IF
                CASE CHR$(0) + CHR$(83)
                        'del
                        caden$ = LEFT$(caden$, n - 1) + MID$(caden$, n + 1, anchostr - n) + " "
                        GOSUB pon3lineas
                        LOCATE fila2, col2
                CASE CHR$(9) 'TAB
                    IF sitab = 1 THEN
                       finished = TRUE
                       inputnota$ = kbd$
                       END IF
                CASE MID$(mini, 1, 1) TO MID$(maxi, 1, 1)
                    IF n <= anchostr THEN
                        caden$ = LEFT$(caden$, n - 1) + LEFT$(kbd$, 1) + MID$(caden$, n, anchostr - n)
                        n = n + 1
                        GOSUB pon3lineas
                        col2 = col2 + 1
                        IF col2 > ancholin + col - 1 THEN
                          col2 = col
                          fila2 = fila2 + 1
                          IF fila2 > lineatope THEN
                            fila2 = fila2 - 1
                            col2 = col + ancholin
                            END IF
                          END IF
                        IF fila2 <= lineatope THEN LOCATE fila2, col2
                    ELSE
                        BEEP
                    END IF
                CASE CHR$(0) + CHR$(75)
                'flecha izquierda
                        IF n > 1 THEN
                        col2 = col2 - 1
                        IF col2 < col THEN
                          col2 = col + ancholin - 1
                          fila2 = fila2 - 1
                          END IF
                        n = n - 1
                        LOCATE fila2, col2
                        END IF
                CASE CHR$(0) + CHR$(77)
                'flecha derecha
                      IF n <= anchostr THEN
                        col2 = col2 + 1
                        n = n + 1
                        IF col2 > ancholin + col - 1 THEN
                          col2 = col
                          fila2 = fila2 + 1
                        END IF
                        LOCATE fila2, col2
                      END IF
                 CASE CHR$(0) + CHR$(72)
                 'flecha arriba
                        IF fila2 > fila THEN
                          fila2 = fila2 - 1
                          n = n - ancholin
                          LOCATE fila2, col2
                          END IF
                 CASE CHR$(0) + CHR$(80)
                 'flecha abajo
                        IF fila2 < lineatope THEN
                          fila2 = fila2 + 1
                          n = n + ancholin
                          LOCATE fila2, col2
                          END IF
                 CASE CHR$(0) + CHR$(79)
                 'end
                        n = n + ((ancholin + col) - col2) - 1
                        col2 = ancholin + col - 1
                        LOCATE fila2, col2
                 CASE CHR$(0) + CHR$(71)
                 'home
                        n = n - (col2 - col)
                        col2 = col
                        LOCATE fila2, col2
                CASE ELSE
                    BEEP
            END SELECT

    LOOP UNTIL finished

    LOCATE , , 0
    EXIT FUNCTION


inputnotaGetKey:
    kbd$ = getkey$
    IF ASC(LEFT$(kbd$, 1)) >= 97 AND ASC(LEFT$(kbd$, 1)) <= 122 THEN
      MID$(kbd$, 1, 1) = CHR$(ASC(LEFT$(kbd$, 1)) - 32)
      END IF
    RETURN

pon3lineas:
'imprime las 3 l¡neas
LOCATE , , 0
FOR n2 = 0 TO 2
   LOCATE n2 + fila, col
   PRINT MID$(caden$, (n2 * ancholin) + 1, ancholin);
   NEXT
LOCATE , , 1
RETURN
END FUNCTION

'Menu:
'  Handles Menu Selection for a single menu (either sub menu, or menu bar)
'  currChoiceX  :  Number of current choice
'  maxChoice    :  Number of choices in the list
'  choice$()    :  Array with the text of the choices
'  itemRow()    :  Array with the row of the choices
'  itemCol()    :  Array with the col of the choices
'  help$()      :  Array with the help text for each choice
'  barMode      :  Boolean:  TRUE = menu bar style, FALSE = drop down style
'
'  Returns the number of the choice that was made by changing currChoiceX
'  and returns the scan code of the key that was pressed to exit
'
FUNCTION Menu (CurrChoiceX, maxchoice, choice$(), itemrow(), itemcol(), help$(), barmode)


    currChoice = CurrChoiceX

    'if in bar mode, color in menu bar, else color box/shadow
    'bar mode means you are currently in the menu bar, not a sub menu
    IF barmode THEN
        poncolor 7, 4
        LOCATE 1, 1
        PRINT SPACE$(80);
    ELSE
        TIMER OFF
        fancycls 2, 1
        poncolor 7, 4
        box itemrow(1) - 1, itemcol(1) - 1, itemrow(maxchoice) + 1, itemcol(1) + LEN(choice$(1)) + 1
      
        poncolor 10, 6
        FOR a = 1 TO maxchoice + 1
            LOCATE itemrow(1) + a - 1, itemcol(1) + LEN(choice$(1)) + 2
            PRINT CHR$(178); CHR$(178);
        NEXT a
        LOCATE itemrow(maxchoice) + 2, itemcol(maxchoice) + 2
        PRINT STRING$(LEN(choice$(maxchoice)) + 2, 178);
        TIMER ON
    END IF
  
    'print the choices
    TIMER OFF
    poncolor 7, 4
    FOR a = 1 TO maxchoice
        LOCATE itemrow(a), itemcol(a)
        PRINT choice$(a);
    NEXT a
    TIMER ON
    finished = FALSE

    WHILE NOT finished
      
        GOSUB MenuShowCursor
        GOSUB MenuGetKey
        GOSUB MenuHideCursor

        SELECT CASE kbd$
            CASE CHR$(0) + "H": GOSUB MenuUp
            CASE CHR$(0) + "P": GOSUB MenuDown
            CASE CHR$(0) + "K": GOSUB MenuLeft
            CASE CHR$(0) + "M": GOSUB MenuRight
            CASE CHR$(13): GOSUB MenuEnter
            CASE CHR$(27): GOSUB MenuEscape
            CASE ELSE:  BEEP
        END SELECT
    WEND

    Menu = currChoice

    EXIT FUNCTION


MenuEnter:
    finished = TRUE
    RETURN

MenuEscape:
    currChoice = 0
    finished = TRUE
    RETURN

MenuUp:
    IF barmode THEN
        BEEP
    ELSE
        currChoice = (currChoice + maxchoice - 2) MOD maxchoice + 1
    END IF
    RETURN

MenuLeft:
    IF barmode THEN
        currChoice = (currChoice + maxchoice - 2) MOD maxchoice + 1
    ELSE
        currChoice = -2
        finished = TRUE
    END IF
    RETURN

MenuRight:
    IF barmode THEN
        currChoice = (currChoice) MOD maxchoice + 1
    ELSE
        currChoice = -3
        finished = TRUE
    END IF
    RETURN

MenuDown:
    IF barmode THEN
        finished = TRUE
    ELSE
        currChoice = (currChoice) MOD maxchoice + 1
    END IF
    RETURN

MenuShowCursor:
    poncolor 8, 9
    LOCATE itemrow(currChoice), itemcol(currChoice)
    PRINT choice$(currChoice);
    PrintHelpLine help$(currChoice)
    RETURN

MenuGetKey:
    kbd$ = ""
    WHILE kbd$ = ""
        kbd$ = INKEY$
    WEND
    RETURN

MenuHideCursor:
    poncolor 7, 4
    LOCATE itemrow(currChoice), itemcol(currChoice)
    PRINT choice$(currChoice);
    RETURN


END FUNCTION

'MenuSystem:
'  Main routine that controls the program.  Uses the MENU function
'  to implement menu system and calls the appropriate function to handle
'  the user's selection
SUB MenuSystem

    DIM choice$(20), menurow(20), menucol(20), help$(20)
    LOCATE , , 0
    choice = 1
    finished = FALSE

    WHILE NOT finished
        GOSUB MenuSystemMain

        subchoice = -1
        WHILE subchoice < 0
            SELECT CASE choice
                CASE 1: GOSUB MenuSystemFile
                CASE 2: GOSUB MenuSystemDatos
                CASE 3: GOSUB MenuSystemUtilidades
                CASE 4: GOSUB MenuSystemColors
            END SELECT
            fancycls 2, 1

            SELECT CASE subchoice
                CASE -2: choice = (choice + 3) MOD 5 + 1
                CASE -3: choice = (choice) MOD 5 + 1
            END SELECT
        WEND
    WEND
    EXIT SUB


MenuSystemMain:
    fancycls 2, 1
    poncolor 7, 4
    box 9, 19, 14, 61
    center 11, "Use las teclas de direcci¢n para el men£"
    center 12, "Presione Entrar para elegir elemento"

    choice$(1) = " Archivo "
    choice$(2) = " Datos "
    choice$(3) = " Utilidades "
    choice$(4) = " Colores "

    menurow(1) = 1: menucol(1) = 2
    menurow(2) = 1: menucol(2) = 11
    menurow(3) = 1: menucol(3) = 20
    menurow(4) = 1: menucol(4) = 36
    
    help$(1) = "Salir o ir a usuario"
    help$(2) = "Selecciona base de datos"
    help$(3) = "Utilidades diversas"
    help$(4) = "Fijar color en pantalla"
    
    DO
        NewChoice = Menu((choice), 4, choice$(), menurow(), menucol(), help$(), TRUE)
    LOOP WHILE NewChoice = 0
    choice = NewChoice
    RETURN

MenuSystemFile:
    choice$(1) = " Acerca de ...   "
    choice$(2) = " Usuario         "
    choice$(3) = " Shell al DOS    "
    choice$(4) = " Salir           "
    FOR i = 1 TO 4
      menurow(i) = i + 2
      menucol(i) = 2
      NEXT
    help$(1) = "Informaci¢n de autor de programa"
    help$(2) = "Ir a usuario"
    help$(3) = "Salir momentaneamente al dos"
    help$(4) = "Abandonar el programa"

    subchoice = Menu(1, 4, choice$(), menurow(), menucol(), help$(), FALSE)

    SELECT CASE subchoice
        CASE 1: informacion
        CASE 2: IF clave% = -1 THEN EXIT SUB
        CASE 3:
        COLOR 7, 0
        CLS
        PRINT "Teclee EXIT para volver a Costos"
        SHELL
        SCREEN 0
        VIEW PRINT
        WIDTH 80, 25
        CASE 4: finished = TRUE
        CASE ELSE
    END SELECT
    RETURN

MenuSystemDatos:
    RESTORE MenuSystemDatosInfo:
    READ n
    FOR i = 1 TO n
      READ choice$(i), help$(i)
      menurow(i) = i + 2
      menucol(i) = 11
      NEXT
    
    subchoice = Menu(1, n, choice$(), menurow(), menucol(), help$(), FALSE)

    SELECT CASE subchoice
        CASE 1: telefonos
        CASE 2: articulos
        CASE 3: materiales
        CASE 4: tratamientos
        CASE 5: maquinas
        CASE 6: operarios
        CASE 7: taller
        CASE 8: utiles
        CASE 9: CHAIN "costeo"
        CASE ELSE
    END SELECT
    RETURN

MenuSystemUtilidades:

    choice$(1) = " Cambiar clave "
    help$(1) = " Cambia la clave de acceso "
    menurow(1) = 3
    menucol(1) = 20


    subchoice = Menu(1, 1, choice$(), menurow(), menucol(), help$(), FALSE)
    SELECT CASE subchoice
        CASE 1
        DIM clavei AS STRING
        box 10, 10, 12, 68
        LOCATE 11, 11
        PRINT "Antigua clave : ";
        sale$ = inputclave$(11, 27, CHR$(32), CHR$(255), "", clavei, 43)
        IF clavei = clave2 AND ASC(MID$(sale$, 1, 1)) = 13 THEN
        LOCATE 11, 11: PRINT SPACE$(56);
        LOCATE 11, 11
        PRINT "Nueva clave : ";
        sale$ = inputclave$(11, 25, CHR$(32), CHR$(255), "", clave2, 43)
        IF ASC(MID$(sale$, 1, 1)) = 13 THEN
          encripta clave2, clave1
          savecfg
          ELSE
          encripta clave1, clave2
          END IF
        END IF
        CASE ELSE
    END SELECT

    RETURN

MenuSystemColors:
    choice$(1) = " Monocrom tico "
    choice$(2) = " Cyan/Azul  "
    choice$(3) = " Azul/Cyan  "
    choice$(4) = " Rojo/Gris  "

    menurow(1) = 3: menucol(1) = 36
    menurow(2) = 4: menucol(2) = 36
    menurow(3) = 5: menucol(3) = 36
    menurow(4) = 6: menucol(4) = 36

    help$(1) = "color para presentaci¢n monocrom tico y LCD"
    help$(2) = "color presentado cyan"
    help$(3) = "color presentado azul"
    help$(4) = "color presentado rojo"

    subchoice = Menu(1, 4, choice$(), menurow(), menucol(), help$(), FALSE)

    SELECT CASE subchoice
        CASE 1 TO 4
            colorpref = subchoice
            savecfg
        CASE ELSE
    END SELECT
    RETURN


END SUB

SUB printright (l%, c%, cadena$)
LOCATE l%, c% - LEN(cadena$)
PRINT cadena$;
END SUB

