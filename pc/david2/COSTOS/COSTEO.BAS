DEFINT A-Z

'$INCLUDE: 'coslib.inc'
'$INCLUDE: 'cosasm.inc'

CONST initcosteon = 100

CONST ccstmaxcol = 5
CONST ccstmaxfil = 100
CONST ccsttotcol = ccstmaxcol + 1
CONST ccscmaterial = 1
CONST ccsctratamiento = 2
CONST ccscmaquina = 3
CONST ccsccostotal = 4
CONST ccsccostoutil = 5
CONST ccstsubcam = ccsccostoutil
CONST cdbcstsubmax = cdbcstmax - 1

CONST cstlngcol1 = 16 + 2
CONST cstlngcol2 = 15 + 7
CONST cstlngcol3 = 14 + 2
CONST cstlngcol4 = 13 + 5
CONST cstlngcol5 = 31

CONST ccstmskcte = "T"
CONST ccstmskeditar = "E"

CONST ccstmsktexto = "X"        ' "X"+texto$

CONST ccstmskcampo = "C"        ' "C"+chr$(campo)+chr$(idx)
CONST ccstmskcampoRedraw = "W"  ' "W"+chr$(campo)+chr$(idx)

CONST ccstmskceldaant = "TX"

CONST ccamCliente = "CLI"
CONST ccamMaterial = "MAT"
CONST ccamTratamiento = "TRA"
CONST ccamMaquina = "MAQ"
CONST ccamTaller = "TAL"
CONST ccamCostoUtil = "CTL"

CONST ccamCostoN = "CLI00"
CONST ccamFechaEntra = "CLI01"
CONST ccamFechaSalid = "CLI02"
CONST ccamCliCodigo = "CLI03"
CONST ccamCliDescripcion = "CLI04"
CONST ccamCPiezaClie = "CLI05"
CONST ccamDescrPieza = "CLI06"
CONST ccamPiezasAnua = "CLI07"
CONST ccamPiezasMesu = "CLI08"
CONST ccamCodDisseny = "CLI09"

CONST ccamMatCodigo = "MAT00"
CONST ccamMatDescripcion = "MAT01"
CONST ccamMatEspesor = "MAT02"
CONST ccamMatAncho = "MAT03"
CONST ccamMatPrecioKilo = "MAT04"
CONST ccamMatPaso = "MAT05"
CONST ccamMatPesoNeto = "MAT06"
CONST ccamMatPesoBruto = "MAT07"
CONST ccamMatCosteNeto = "MAT08"
CONST ccamMatCosteBruto = "MAT09"
CONST ccamMatTotalMLote = "MAT10"
CONST ccamMatTotalMAnual = "MAT11"

CONST ccamTraCodigo = "TRA00"
CONST ccamTraDescripcio = "TRA01"
CONST ccamTraPrecioKilo = "TRA02"
CONST ccamTraImportePie = "TRA03"
CONST ccamTraImporteLote = "TRA04"
CONST ccamTraImporteAnual = "TRA05"

CONST ccamMaqCodigo = "MAQ00"
CONST ccamMaqDescripcio = "MAQ01"
CONST ccamMaqGolpeMinut = "MAQ02"
CONST ccamMaqTiempoPrep = "MAQ03"
CONST ccamMaqPorOperari = "MAQ04"
CONST ccamMaqImportePieza = "MAQ05"
CONST ccamMaqImporteLote = "MAQ06"
CONST ccamMaqImporteAnual = "MAQ07"
CONST ccamMaqTHorasLote = "MAQ08"
CONST ccamMaqTHorasAnual = "MAQ09"

CONST ccamTalCodigo = "TAL00"
CONST ccamTalDescripcio = "TAL01"
CONST ccamTalHoras = "TAL02"
CONST ccamTalImporte = "TAL03"

CONST ccstprintseparar = 1
CONST ccstprintinv = 2
CONST ccstprintvar = 4
CONST ccstprintnocabe = 8

TYPE cstsubcamcontroltype
  filault AS INTEGER
  regsborrado AS STRING * cdbcstsubmax
  cam AS INTEGER
  altura AS INTEGER
  modificado AS INTEGER
  END TYPE

DIM SHARED cstcontrol(ccstmaxfil, ccstmaxcol) AS STRING
DIM SHARED cstcolumna(ccsttotcol) AS INTEGER
DIM SHARED cstsubcamcontrol(ccstsubcam) AS cstsubcamcontroltype
DIM SHARED cstsubcamregs(ccstsubcam, cdbcstsubmax) AS LONG

DIM SHARED cstultfil AS INTEGER
DIM SHARED cstpricolpan AS INTEGER
DIM SHARED cstprifilpan AS INTEGER
DIM SHARED cstfilact AS INTEGER
DIM SHARED cstcolact AS INTEGER

DECLARE SUB costeo ()
DECLARE SUB vaciarceldas (fili%, filf%)
DECLARE SUB printcelda (y%, fil%, col%, flags%)
DECLARE SUB printhoja ()
DECLARE SUB poncelda (fil%, col%, control$)
DECLARE SUB insfilas (fil%, num%)
DECLARE SUB escrcelda (fil%, col%, flags%)
DECLARE SUB inputcelda (modi%)
DECLARE SUB ponceldacursor ()
DECLARE SUB primeracelda ()
DECLARE SUB ayudaceldaact (modi%)
DECLARE SUB cstinscampo (cstarch%, rec&)
DECLARE SUB delfilas (fil%, num%)
DECLARE SUB lprinthoja (opccmbhoja%)

DECLARE FUNCTION cmbrect% ()
DECLARE FUNCTION getdate$ (caden$)
DECLARE FUNCTION getstrcelda$ (fil%, col%, flags%)
DECLARE FUNCTION esceldacursoract% ()
DECLARE FUNCTION editarhoja% ()
DECLARE FUNCTION inputdate$ (fila AS INTEGER, col AS INTEGER, cadini$, caden$, sitab AS INTEGER)
DECLARE FUNCTION cnummiles$ (num&)
DECLARE FUNCTION cnummilesd$ (num#)
DECLARE FUNCTION obtidxarch% (control$)

Initialize
opencfg

'IF comenzar <> 0 THEN
  n = handles%(30)
  IF n = 0 THEN
    InstReloj
    costeo
      ELSE
    PRINT "No puedo abrir los ficheros de costos", CHR$(7)
    tec$ = getkey$
    END IF
  'END IF

'CHAIN "costos"

END

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
SUB ayudaceldaact (modi%) STATIC

CONST bxy = 3
CONST bxx = 10
CONST bxy2 = 20
CONST bxalto = bxy2 - bxy - 1
CONST bxxrec = bxx + 1

DIM control AS STRING
DIM idx AS INTEGER
DIM ccam AS INTEGER
DIM bxx2 AS INTEGER
DIM bxancho AS INTEGER
DIM y AS INTEGER
DIM numregistropripan AS LONG
DIM numregistro AS LONG
DIM recnum AS LONG
DIM rec AS LONG
DIM num2 AS LONG
DIM alto AS INTEGER
DIM tec AS STRING
DIM codigo AS STRING
DIM arch AS INTEGER
DIM i AS INTEGER

control = cstcontrol(cstfilact, cstcolact)
idx = VAL(RIGHT$(control, 1))
arch = cdbcstmax
i = obtidxarch%(control)
SELECT CASE i
  CASE 0
    ccam = ccliente
    bxx2 = bxx + 2 + 1 + 30 + 2
    codigo = RTRIM$(coston(1).coddisseny)
    arch = 2
  CASE ccscmaterial
    ccam = camateria
    bxx2 = bxx + 4 + 1 + 45 + 2
    codigo = RTRIM$(costom(idx).codmateria)
  CASE ccsctratamiento
    ccam = caltrata
    bxx2 = bxx + 4 + 1 + 45 + 2
    codigo = RTRIM$(costr(idx).codtratami)
  CASE ccscmaquina
    ccam = camaquina
    bxx2 = bxx + 4 + 1 + 30 + 2
    codigo = RTRIM$(costomec(idx).codmaquina)
  CASE ccsccostotal
    ccam = cataller
    bxx2 = bxx + 4 + 1 + 30 + 2
    codigo = RTRIM$(costotal(idx).codmaquina)
  END SELECT
bxancho = bxx2 - bxx - 1
PrintHelpLine "Escoja un c¢digo"
poncolor 2, 1
box bxy, bxx, bxy2, bxx2
GOSUB buscarcodigoayuda
DO
esperateclaayu:
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(0) + CHR$(71)
      GOSUB ponayudaact
      GOSUB irayudaprimera
      GOSUB ponayudaactinv
    CASE CHR$(0) + CHR$(72)
      GOSUB ponayudaact
      GOSUB irayudaanterior
      IF numregistro = -1 THEN
        GOSUB irayudaultima
          ELSE
        IF recnum < 1 THEN
          recnum = 1
          dtIrA ccam, arch, numregistropripan
          dtir ccam, arch, cdtiranterior, numregistropripan
          GOSUB ponhojaayuda
          END IF
        END IF
      GOSUB ponayudaactinv
    CASE CHR$(0) + CHR$(73)
      GOSUB ponayudaact
      y = bxalto - recnum + 1
      dtIrA ccam, 2, numregistropripan
      FOR alto = 1 TO bxalto
        dtir ccam, arch, cdtiranterior, numregistropripan
        IF numregistropan = -1 THEN
          GOSUB irayudaprimera
          GOTO esperateclaayu
          END IF
        IF alto = y THEN numregistro = numregistropripan
        NEXT
      GOSUB ponhojaayuda
    CASE CHR$(0) + CHR$(79)
      GOSUB ponayudaact
      GOSUB irayudaultima
      GOSUB ponayudaactinv
    CASE CHR$(0) + CHR$(80)
      GOSUB ponayudaact
      GOSUB irayudaposterior
      IF numregistro = -1 THEN
        GOSUB irayudaprimera
          ELSE
        IF recnum > bxalto THEN
          dtIrA ccam, arch, numregistropripan
          dtir ccam, arch, cdtirposterior, numregistropripan
          recnum = recnum - 1
          GOSUB ponhojaayuda
          END IF
        END IF
      GOSUB ponayudaactinv
    CASE CHR$(0) + CHR$(81)
      GOSUB ponayudaact
      dtIrA ccam, arch, numregistropripan
      FOR alto = 1 TO bxalto
        dtir ccam, arch, cdtirposterior, numregistropripan
        IF numregistropripan = -1 THEN
          GOSUB irayudaultima
          GOTO esperateclaayu
          END IF
        NEXT
      numregistro = numregistropripan
      FOR alto = 1 TO recnum - 1
        dtir ccam, arch, cdtirposterior, numregistro
        NEXT
      GOSUB ponhojaayuda
    CASE CHR$(13)
      SELECT CASE ccam
        CASE ccliente
          MID$(coston(idx).coddisseny, 1, 2) = cliente(2).codcliente
          cliente(1) = cliente(2)
        CASE camateria
          costom(idx).codmateria = amateria(arch).codmateria
          amateria(idx) = amateria(arch)
        CASE caltrata
          costr(idx).codtratami = altrata(arch).codtratami
          altrata(idx) = altrata(arch)
        CASE camaquina
          costomec(idx).codmaquina = amaquina(arch).codmaquina
          amaquina(idx) = amaquina(arch)
        CASE cataller
          costotal(idx).codmaquina = ataller(arch).codmaquina
          ataller(idx) = ataller(arch)
        END SELECT
      IF i > 0 THEN cstsubcamcontrol(i).modificado = TRUE ELSE modi% = TRUE
      EXIT DO
    CASE " " TO CHR$(255)
      codigo = UCASE$(tec)
      GOSUB buscarcodigoayuda
    END SELECT
  LOOP UNTIL tec = CHR$(27)
printhoja
EXIT SUB
irayudaprimera:
recnum = 1
dtir ccam, arch, cdtirinicio, numregistro
numregistropripan = numregistro
GOSUB ponhojaayuda
RETURN
ponhojaayuda:
rec = recnum
recnum = 1
num2 = numregistro
dtIrA ccam, arch, numregistropripan
GOSUB colorayudanormal
FOR y = bxy + 1 TO bxy2 - 1
  IF numregistro <> -1 THEN
    GOSUB printayuday
    GOSUB irayudaposterior
      ELSE
    LOCATE y, bxxrec
    PRINT SPACE$(bxancho)
    END IF
  NEXT
recnum = rec
numregistro = num2
dtIrA ccam, arch, numregistro
GOSUB ponayudaactinv
RETURN
irayudaposterior:
recnum = recnum + 1
dtir ccam, arch, cdtirposterior, numregistro
RETURN
irayudaanterior:
dtir ccam, arch, cdtiranterior, numregistro
recnum = recnum - 1
RETURN
colorayudanormal:
poncolor 3, 10
RETURN
printayuday:
GOSUB getayudarec
LOCATE y, bxxrec
PRINT control
GOTO colorayudanormal
getayudarec:
SELECT CASE ccam
  CASE ccliente
    control = cliente(arch).codcliente + " " + cliente(arch).empresa
  CASE camateria
    control = amateria(arch).codmateria + " " + amateria(arch).descripcio
  CASE caltrata
    control = altrata(arch).codtratami + " " + altrata(arch).descripcio
  CASE camaquina
    control = amaquina(arch).codmaquina + " " + amaquina(arch).descripcio
  CASE cataller
    control = ataller(arch).codmaquina + " " + ataller(arch).descripcio
  END SELECT
RETURN
ponayudaactinv:
poncolor 7, 4
ponayudaact:
y = recnum + bxy
GOSUB printayuday
RETURN
irayudaultima:
dtir ccam, arch, cdtirfin, numregistro
dtir ccam, arch, cdtirfin, numregistropripan
recnum = 1
FOR alto = 1 TO bxalto - 1
  dtir ccam, arch, cdtiranterior, numregistropripan
  IF numregistropripan = -1 THEN EXIT FOR
  recnum = recnum + 1
  NEXT
GOSUB ponhojaayuda
RETURN
buscarcodigoayuda:
SELECT CASE ccam
  CASE ccliente
    cliente(arch).codcliente = codigo
  CASE camateria
    amateria(arch).codmateria = codigo
  CASE caltrata
    altrata(arch).codtratami = codigo
  CASE camaquina
    amaquina(arch).codmaquina = codigo
  CASE cataller
    ataller(arch).codmaquina = codigo
  END SELECT
num2 = dtbuscar%(ccam, arch, numregistro, FALSE)
recnum = 1
FOR alto = 1 TO bxalto / 2
  dtir ccam, arch, cdtiranterior, numregistropripan
  IF numregistropripan = -1 THEN
    dtir ccam, arch, cdtirinicio, numregistropripan
    dtIrA ccam, arch, numregistro
    GOTO ponhojaayuda
    END IF
  recnum = recnum + 1
  NEXT
recnum = (bxalto / 2) + 1
GOSUB ponhojaayuda
RETURN
END SUB

FUNCTION cmbrect%

DIM si AS INTEGER

si = FALSE
IF cstfilact < cstprifilpan THEN
  si = TRUE
  cstprifilpan = cstfilact - 1
  END IF
WHILE cstfilact > cstprifilpan + 20
  si = TRUE
  cstprifilpan = cstprifilpan + 21
  WEND
IF cstcolact < cstpricolpan THEN
  si = TRUE
  cstpricolpan = cstcolact
  END IF
cmbrect% = si

END FUNCTION

DEFINT A-Z
FUNCTION cnummiles$ (num&)

DIM txt AS STRING
DIM i AS INTEGER

txt = Cvlt$(num&)
FOR i = LEN(txt) - 3 TO 1 STEP -3
  txt = LEFT$(txt, i) + "." + MID$(txt, i + 1)
  NEXT
cnummiles$ = txt
END FUNCTION

DEFSNG A-Z
FUNCTION cnummilesd$ (num#)

DIM txt AS STRING
DIM i AS INTEGER
DIM ini AS INTEGER

txt = Cvdt$(num#)
ini = INSTR(txt, ".")
IF ini < 1 THEN
  ini = LEN(txt)
    ELSE
  MID$(txt, ini, 1) = ","
  ini = ini - 1
  IF ini = 0 THEN
    cnummilesd$ = "0" + txt
    EXIT FUNCTION
    END IF
  END IF
FOR i = ini - 3 TO 1 STEP -3
  txt = LEFT$(txt, i) + "." + MID$(txt, i + 1)
  NEXT
cnummilesd$ = txt
END FUNCTION

SUB costeo

DIM numregistro AS LONG
DIM numreg2 AS LONG
DIM tec AS STRING
DIM caden AS STRING
DIM salida AS STRING
DIM primero AS INTEGER
DIM i AS INTEGER
DIM j AS INTEGER
DIM rec AS LONG
DIM cam AS INTEGER
DIM cmp AS INTEGER
DIM opciones(2) AS INTEGER
DIM lstcodimp AS STRING
DIM reg AS LONG
DIM modificado AS INTEGER

cstcolumna(1) = 2
cstcolumna(2) = 2 + cstlngcol1
cstcolumna(3) = 2 + cstlngcol1 + cstlngcol2
cstcolumna(4) = 2 + cstlngcol1 + cstlngcol2 + cstlngcol3
cstcolumna(5) = 2 + cstlngcol1 + cstlngcol2 + cstlngcol3 + cstlngcol4
cstcolumna(6) = 2 + cstlngcol1 + cstlngcol2 + cstlngcol3 + cstlngcol4 + cstlngcol5

cstsubcamcontrol(1).cam = ccostom
cstsubcamcontrol(2).cam = ccostr
cstsubcamcontrol(3).cam = ccostomec
cstsubcamcontrol(4).cam = ccostotal
cstsubcamcontrol(5).cam = ccostouti

cstsubcamcontrol(1).altura = 6
cstsubcamcontrol(2).altura = 4
cstsubcamcontrol(3).altura = 6
cstsubcamcontrol(4).altura = 4

idxabre
dtabre "cliente.dat", ccliente
dtabre "articulo.dat", carticulo
dtabre "coston.dat", ccoston
dtabre "amateria.dat", camateria
dtabre "costom.dat", ccostom
dtabre "altrata.dat", caltrata
dtabre "costr.dat", ccostr
dtabre "amaquina.dat", camaquina
dtabre "aoperari.dat", caoperari
dtabre "costomec.dat", ccostomec
dtabre "ataller.dat", cataller
dtabre "costotal.dat", ccostotal
dtabre "autiles.dat", cautiles
dtabre "costouti.dat", ccostouti
dtir ccoston, 1, cdtirinicio, numregistro
borramenusystem
GOSUB lineainfcosteo
GOSUB ponregistrocst
primero = TRUE
DO
  IF primero THEN
    primero = FALSE
    numreg2 = numregistro
    IF numregistro = -1 THEN GOSUB editarhojacosteo
    END IF
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(0) + CHR$(80)
    'flecha abajo
    dtir ccoston, 1, cdtirposterior, numregistro
    IF numregistro = -1 THEN dtir ccoston, 1, cdtirinicio, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(72)
    'flecha arriba
    dtir ccoston, 1, cdtiranterior, numregistro
    IF numregistro = -1 THEN dtir ccoston, 1, cdtirfin, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(73)
    'pgup
    n1 = 1
    DO
      dtir ccoston, 1, cdtiranterior, numregistro
      n1 = n1 + 1
      LOOP WHILE n1 <= 8 AND numregistro <> -1
    IF numregistro = -1 THEN dtir ccoston, 1, cdtirinicio, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(81)
    'pgdown
    n1 = 1
    DO
      dtir ccoston, 1, cdtirposterior, numregistro
      n1 = n1 + 1
      LOOP WHILE n1 <= 8 AND numregistro <> -1
    IF numregistro = -1 THEN dtir ccoston, 1, cdtirfin, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(71)
    'home
    dtir ccoston, 1, cdtirinicio, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(79)
    'end
    dtir ccoston, 1, cdtirfin, numregistro
    GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(60)
    'F2: Insertar
    numreg2 = numregistro
    numregistro = -1
    GOSUB ponregistrocst
    GOSUB editarhojacosteo
    CASE CHR$(0) + CHR$(61)
    'F3: Imprimir
    IF numregistro <> -1 THEN
      IF pregsprevsimpri%(opciones(), hastafin%) <> 1 THEN
          SELECT CASE opciones(1)
            CASE 1
              dtir ccoston, 2, cdtirfin, reg
              lstcodimp = coston(2).costeon
              dtir ccoston, 1, cdtirinicio, reg
            CASE 2
              dtir ccoston, 2, cdtirfin, reg
              lstcodimp = coston(2).costeon
              dtIrA ccoston, 1, numregistro
            CASE 3
              lstcodimp = coston(1).costeon
            END SELECT
          poncolor 1, 4
          salida = " "
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
              GOSUB dohoja
              lprinthoja opciones(2)
              IF saleimprimir = 1 THEN EXIT DO
              dtir ccoston, 1, cdtirposterior, reg
              LOOP WHILE reg <> -1 AND strcmp%(coston(1).costeon, lstcodimp) < 0
            dtIrA ccoston, 1, numregistro
            GOSUB dohoja
            END IF
          END IF
      END IF
    nuevahoja = 0
    printhoja
    CASE CHR$(0) + CHR$(59)
    'F1: Borrar
      salida = confirmacion$("¨Est  seguro de querer borrar la hoja de costeo?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        dtborrar ccoston, 1, numregistro
        FOR i = 1 TO ccstsubcam
          cam = cstsubcamcontrol(i).cam
          FOR j = 1 TO cdbcstsubmax
            dtborrar cam, j, cstsubcamregs(i, j)
            NEXT
          NEXT
        IF numregistro = -1 THEN costeon = initcosteon
        END IF
      GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(104)
      ' Alt-F1:Borrar todo
      salida = confirmacion$("¨ Quiere borrar el archivo de costeo ?", caden)
      IF caden = "S" AND salida <> CHR$(27) THEN
        ponborrar
        dtborrartodo ccoston
        dtborrartodo ccostom
        dtborrartodo ccostr
        dtborrartodo ccostomec
        dtborrartodo ccostotal
        dtborrartodo ccostouti
        numregistro = -1
        costeon = initcosteon
        END IF
      GOSUB ponregistrocst
    CASE CHR$(0) + CHR$(64)
      numreg2 = numregistro
      GOSUB editarhojacosteo
    END SELECT
  LOOP WHILE tec <> CHR$(27)
CLOSE #ccliente
CLOSE #carticulo
CLOSE #ccoston
CLOSE #camateria
CLOSE #ccostom
CLOSE #caltrata
CLOSE #ccostr
CLOSE #camaquina
CLOSE #caoperari
CLOSE #ccostomec
CLOSE #cataller
CLOSE #ccostotal
CLOSE #cautiles
CLOSE #ccostouti
CLOSE #ccosidx
savecfg
EXIT SUB
lineainfcosteo:
printlineainf "F1 Borrar F2 Insertar F3 Imprimir F4 Buscar F6 Editar Alt-F1 B.Todo"
RETURN
ponregistrocst:
GOSUB dohoja
printhoja
RETURN
dohoja:
cstultfil = 0
cstpricolpan = 1
cstprifilpan = 1
vaciarceldas 1, ccstmaxfil
IF numregistro = -1 THEN
  cliente(1).codcliente = ""
  coston(1).costeon = "O" + Cvit0$(costeon, 4)
  coston(1).fechaentra = ""
  coston(1).fechasalid = ""
  coston(1).coddisseny = ""
  coston(1).cpiezaclie = ""
  coston(1).descrpieza = ""
  coston(1).piezasanua = 0
  coston(1).piezasmesu = 0
  END IF
insfilas 1, 4
poncelda 1, 1, ccstmskcte + ccstmsktexto + "Costo N."
poncelda 1, 2, ccstmskcte + ccstmsktexto + "Fecha Entrada"
poncelda 1, 3, ccstmskcte + ccstmsktexto + "Fecha Salida"
poncelda 1, 4, ccstmskcte + ccstmsktexto + "C.Cliente"
poncelda 1, 5, ccstmskcte + ccstmsktexto + "Descripci¢n"
poncelda 2, 1, ccstmskcte + ccstmskcampo + ccamCostoN + "1"
poncelda 2, 2, ccstmskeditar + ccstmskcampo + ccamFechaEntra + "1"
poncelda 2, 3, ccstmskeditar + ccstmskcampo + ccamFechaSalid + "1"
poncelda 2, 4, ccstmskeditar + ccstmskcampoRedraw + ccamCliCodigo + "1"
poncelda 2, 5, ccstmskcte + ccstmskcampo + ccamCliDescripcion + "1"
poncelda 3, 1, ccstmskcte + ccstmsktexto + "C.Pieza Cliente"
poncelda 3, 2, ccstmskcte + ccstmsktexto + "Descripci¢n"
poncelda 3, 3, ccstmskcte + ccstmsktexto + "L. A. Piezas"
poncelda 3, 4, ccstmskcte + ccstmsktexto + "L.M.Piezas"
poncelda 3, 5, ccstmskcte + ccstmsktexto + "De Alta S/N"
poncelda 4, 1, ccstmskeditar + ccstmskcampo + ccamCPiezaClie + "1"
poncelda 4, 2, ccstmskeditar + ccstmskcampo + ccamDescrPieza + "1"
poncelda 4, 3, ccstmskeditar + ccstmskcampoRedraw + ccamPiezasAnua + "1"
poncelda 4, 4, ccstmskeditar + ccstmskcampoRedraw + ccamPiezasMesu + "1"
poncelda 4, 5, ccstmskeditar + ccstmskcampoRedraw + ccamCodDisseny + "1"
salida = coston(1).costeon
FOR i = 1 TO ccstsubcam
  cstsubcamcontrol(i).filault = 5
  cstsubcamcontrol(i).regsborrado = SPACE$(cdbcstsubmax)
  cstsubcamcontrol(i).modificado = FALSE
  NEXT
FOR j = 1 TO cdbcstsubmax
  amateria(j).codmateria = ""
  altrata(j).codtratami = ""
  amaquina(j).codmaquina = ""
  NEXT
FOR i = 1 TO ccstsubcam
  GOSUB poncosteon
  cam = cstsubcamcontrol(i).cam
  cmp = dtbuscar%(cam, cdbcstmax, rec, TRUE)
  FOR j = 1 TO cdbcstsubmax
    IF cmp THEN
      cstinscampo i, rec
      cmp = dtcontinuar%(cam, cdbcstmax, rec)
        ELSE
      cstsubcamregs(i, j) = -1
      END IF
    NEXT
  NEXT
primeracelda
poncosteon:
costom(cdbcstmax).costeon = salida
costr(cdbcstmax).costeon = salida
costomec(cdbcstmax).costeon = salida
costotal(cdbcstmax).costeon = salida
costouti(cdbcstmax).costeon = salida
RETURN
editarhojacosteo:
modificado = editarhoja%
IF modificado THEN
  IF numregistro = -1 THEN costeon = costeon + 1
  dtmodificar ccoston, 1, 2, numregistro
  END IF
FOR i = 1 TO ccstsubcam
  modificado = modificado OR cstsubcamcontrol(i).modificado
  IF cstsubcamcontrol(i).modificado THEN
    tec = cstsubcamcontrol(i).regsborrado
    cam = cstsubcamcontrol(i).cam
    FOR j = 1 TO cdbcstsubmax
      rec = cstsubcamregs(i, j)
      SELECT CASE MID$(tec, j, 1)
        CASE "N"
          dtmodificar cam, j, cdbcstmax, rec
        CASE "S"
          dtborrar cam, j, rec
        END SELECT
      NEXT
    END IF
  NEXT
IF NOT modificado THEN
  IF numregistro = -1 AND numreg2 <> -1 THEN
    numregistro = numreg2
    dtIrA ccoston, 1, numreg2
    END IF
  END IF
GOSUB ponregistrocst
GOTO lineainfcosteo
END SUB

SUB cstinscampo (cstarch%, rec&)

DIM control AS cstsubcamcontroltype
DIM altura AS INTEGER
DIM i AS INTEGER
DIM i1 AS INTEGER
DIM j AS INTEGER
DIM idx AS STRING * 1
DIM fila AS INTEGER

control = cstsubcamcontrol(cstarch%)
fila = cstsubcamcontrol(cstarch%).filault
altura = control.altura
IF INSTR(control.regsborrado, "N") < 1 THEN
  fila = fila + 1
  altura = altura + 1
  END IF
cstfilact = fila + 1
cstcolact = 1
cstpricolpan = 1
FOR i = cstarch% TO ccstsubcam
  cstsubcamcontrol(i).filault = cstsubcamcontrol(i).filault + altura
  NEXT
insfilas fila, altura
j = INSTR(control.regsborrado, " ")
i1 = INSTR(control.regsborrado, "S")
IF j = 0 THEN
  j = i1
    ELSEIF i1 > 0 AND i1 < j THEN j = i1
  END IF
MID$(cstsubcamcontrol(cstarch%).regsborrado, j, 1) = "N"
cstsubcamregs(cstarch%, j) = rec&
idx = Cvit$(j)
SELECT CASE cstarch%
  CASE ccscmaterial
    costom(j) = costom(cdbcstmax)
    IF rec& = -1 THEN
      costom(j).codmateria = ""
      costom(j).paso = 0
      costom(j).pesoneto = 0
      costom(j).pesobruto = 0
      costom(j).costeneto = 0
      costom(j).costebruto = 0
      END IF
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "C.Material"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Descripci¢n"
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampoRedraw + ccamMatCodigo + idx
    poncelda fila, 2, ccstmskcte + ccstmskcampo + ccamMatDescripcion + idx
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Espesor"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Ancho"
    poncelda fila, 3, ccstmskcte + ccstmsktexto + "Precio Kilo"
    poncelda fila, 4, ccstmskcte + ccstmsktexto + "Paso"
    poncelda fila, 5, ccstmskcte + ccstmsktexto + "Peso Neto Kg"
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmskcampo + ccamMatEspesor + idx
    poncelda fila, 2, ccstmskcte + ccstmskcampo + ccamMatAncho + idx
    poncelda fila, 3, ccstmskcte + ccstmskcampo + ccamMatPrecioKilo + idx
    poncelda fila, 4, ccstmskeditar + ccstmskcampo + ccamMatPaso + idx
    poncelda fila, 5, ccstmskeditar + ccstmskcampo + ccamMatPesoNeto + idx
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Peso Bruto"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Coste Neto"
    poncelda fila, 3, ccstmskcte + ccstmsktexto + "Coste Bruto"
    poncelda fila, 4, ccstmskcte + ccstmsktexto + "Total M.Lote"
    poncelda fila, 5, ccstmskcte + ccstmsktexto + "Total M.Anual"
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampoRedraw + ccamMatPesoBruto + idx
    poncelda fila, 2, ccstmskeditar + ccstmskcampo + ccamMatCosteNeto + idx
    poncelda fila, 3, ccstmskeditar + ccstmskcampo + ccamMatCosteBruto + idx
    poncelda fila, 4, ccstmskcte + ccstmskcampo + ccamMatTotalMLote + idx
    poncelda fila, 5, ccstmskcte + ccstmskcampo + ccamMatTotalMAnual + idx
  CASE ccsctratamiento
    costr(j) = costr(cdbcstmax)
    IF rec& = -1 THEN
      costr(j).codtratami = ""
      costr(j).importepie = 0
      END IF
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "C.Tratamiento"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Descripci¢n"
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampoRedraw + ccamTraCodigo + idx
    poncelda fila, 2, ccstmskcte + ccstmskcampo + ccamTraDescripcio + idx
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Precio Kilo"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Importe Pieza"
    poncelda fila, 3, ccstmskcte + ccstmsktexto + "Importe Lote"
    poncelda fila, 4, ccstmskcte + ccstmsktexto + "Importe Anual"
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmskcampo + ccamTraPrecioKilo + idx
    poncelda fila, 2, ccstmskeditar + ccstmskcampo + ccamTraImportePie + idx
    poncelda fila, 3, ccstmskcte + ccstmskcampo + ccamTraImporteLote + idx
    poncelda fila, 4, ccstmskcte + ccstmskcampo + ccamTraImporteAnual + idx
  CASE ccscmaquina
    costomec(j) = costomec(cdbcstmax)
    IF rec& = -1 THEN
      costomec(j).codmaquina = ""
      costomec(j).golpeminut = 0
      costomec(j).tiempoprep = 0
      costomec(j).poroperari = 0
      END IF
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "C.Maquina"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Descripci¢n"
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampoRedraw + ccamMaqCodigo + idx
    poncelda fila, 2, ccstmskcte + ccstmskcampo + ccamMaqDescripcio + idx
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Golpes Minuto"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "T. Preparacion"
    poncelda fila, 3, ccstmskcte + ccstmsktexto + "% de Operario"
    poncelda fila, 4, ccstmskcte + ccstmsktexto + "Importe Pieza"
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampo + ccamMaqGolpeMinut + idx
    poncelda fila, 2, ccstmskeditar + ccstmskcampo + ccamMaqTiempoPrep + idx
    poncelda fila, 3, ccstmskeditar + ccstmskcampo + ccamMaqPorOperari + idx
    poncelda fila, 4, ccstmskeditar + ccstmskcampo + ccamMaqImportePieza + idx
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Importe Lote"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Importe Anual"
    poncelda fila, 3, ccstmskcte + ccstmsktexto + "T.Horas Lote"
    poncelda fila, 4, ccstmskcte + ccstmsktexto + "T.Horas Anual"
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampo + ccamMaqImporteLote + idx
    poncelda fila, 2, ccstmskeditar + ccstmskcampo + ccamMaqImporteAnual + idx
    poncelda fila, 3, ccstmskeditar + ccstmskcampo + ccamMaqTHorasLote + idx
    poncelda fila, 4, ccstmskeditar + ccstmskcampo + ccamMaqTHorasAnual + idx
  CASE ccsccostotal
    costotal(j) = costotal(cdbcstmax)
    IF rec& = -1 THEN
      costotal(j).codmaquina = ""
      costotal(j).horas = 0
      END IF
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "C.Maquina"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Descripci¢n"
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampoRedraw + ccamTalCodigo + idx
    poncelda fila, 2, ccstmskcte + ccstmskcampo + ccamTalDescripcio + idx
    poncelda fila, 3, ccstmskceldaant
    poncelda fila, 4, ccstmskceldaant
    fila = fila + 1
    poncelda fila, 1, ccstmskcte + ccstmsktexto + "Tiempo Horas"
    poncelda fila, 2, ccstmskcte + ccstmsktexto + "Importe"
    fila = fila + 1
    poncelda fila, 1, ccstmskeditar + ccstmskcampo + ccamTalHoras + idx
    poncelda fila, 2, ccstmskeditar + ccstmskcampo + ccamTalImporte + idx
  END SELECT
END SUB

SUB delfilas (fil%, num%)

DIM f AS INTEGER
DIM f2 AS INTEGER
DIM f3 AS INTEGER
DIM c AS INTEGER

f3 = cstultfil
cstultfil = cstultfil - num%
FOR f = fil% TO cstultfil
  f2 = f + num%
  FOR c = 1 TO ccstmaxcol
    cstcontrol(f, c) = cstcontrol(f2, c)
    NEXT
  NEXT
vaciarceldas cstultfil + 1, f3
IF cstfilact > cstultfil THEN cstfilact = cstultfil
END SUB

FUNCTION editarhoja% STATIC

DIM tec AS STRING
DIM modificado AS INTEGER
DIM opcmenu(ccstsubcam) AS STRING
DIM i AS INTEGER
DIM fil AS INTEGER
DIM col AS INTEGER
DIM caden AS STRING
DIM salida AS STRING

modificado = FALSE
GOSUB lineainfeditarcosteo
DO
iresperartecla:
  tec = getkey$
  SELECT CASE tec
    CASE CHR$(0) + CHR$(59)
      ayudaceldaact modificado
      GOSUB lineainfeditarcosteo
    CASE CHR$(0) + CHR$(60)
      opcmenu(1) = "Material"
      opcmenu(2) = "Tratamiento"
      opcmenu(3) = "Maquina"
      opcmenu(4) = "Total"
      i = escoge%(opcmenu(), 4, "Archivos")
      IF i <> 0 THEN cstinscampo i, -1
      printhoja
    CASE CHR$(0) + CHR$(61)
      fil = cstfilact
      col = cstcolact
      IF MID$(cstcontrol(fil, col), 3, 3) = ccamCliente THEN GOTO iresperartecla
      DO
        tec = cstcontrol(fil, col)
        IF MID$(tec, 2, 1) <> ccstmsktexto AND MID$(tec, 6, 2) = "00" THEN EXIT DO
        col = col - 1
        IF col = 0 THEN
          col = ccstmaxcol
          fil = fil - 1
          END IF
        LOOP
      salida = confirmacion$("¨ Quiere borrar " + getstrcelda(fil, col, 0) + " " + getstrcelda(fil, col + 1, 0) + " ?", caden)
      IF salida <> CHR$(27) AND caden = "S" THEN
        i = obtidxarch%(tec)
        MID$(cstsubcamcontrol(i).regsborrado, VAL(RIGHT$(tec, 1)), 1) = "S"
        col = cstsubcamcontrol(i).altura
        IF INSTR(cstsubcamcontrol(i).regsborrado, "N") < 1 THEN
          col = col + 1
          fil = fil - 1
          END IF
        delfilas fil - 1, col
        cstsubcamcontrol(i).modificado = TRUE
        FOR fil = i TO ccstsubcam
          cstsubcamcontrol(fil).filault = cstsubcamcontrol(fil).filault - col
          NEXT
        END IF
      fil = 1
      WHILE NOT esceldacursoract%
        cstfilact = cstfilact + fil
        IF cstfilact >= cstultfil THEN fil = -1
        WEND
      printhoja
    CASE CHR$(0) + CHR$(71)
      GOSUB desmarcaitem
      primeracelda
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(72)
      GOSUB desmarcaitem
      DO
        cstfilact = cstfilact - 1
        IF cstfilact < 1 THEN cstfilact = cstultfil
        LOOP UNTIL esceldacursoract%
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(73)
      GOSUB desmarcaitem
      cstfilact = cstfilact - 21
      IF cstfilact < 1 THEN cstfilact = 1
      GOSUB buscarcelda
    CASE CHR$(0) + CHR$(79)
      GOSUB desmarcaitem
      cstfilact = cstultfil
      cstcolact = ccstmaxcol
      DO UNTIL esceldacursoract%
        cstcolact = cstcolact - 1
        LOOP
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(75)
      GOSUB desmarcaitem
      DO
        cstcolact = cstcolact - 1
        IF cstcolact < 1 THEN cstcolact = ccstmaxcol
        LOOP UNTIL esceldacursoract%
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(77)
      GOSUB desmarcaitem
      DO
        cstcolact = cstcolact + 1
        IF cstcolact > ccstmaxcol THEN cstcolact = 1
        LOOP UNTIL esceldacursoract%
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(80)
      GOSUB desmarcaitem
      DO
        cstfilact = cstfilact + 1
        IF cstfilact > cstultfil THEN cstfilact = 1
      LOOP UNTIL esceldacursoract%
      GOSUB marcaitem
    CASE CHR$(0) + CHR$(81)
      GOSUB desmarcaitem
      cstfilact = cstfilact + 21
      IF cstfilact > cstultfil THEN cstfilact = cstultfil
      GOSUB buscarcelda
    CASE CHR$(13), " "
      IF LEFT$(cstcontrol(cstfilact, cstcolact), 1) = ccstmskeditar THEN inputcelda modificado
    END SELECT
  LOOP WHILE tec <> CHR$(27)
editarhoja% = modificado
EXIT FUNCTION
lineainfeditarcosteo:
printlineainf "F1 Ayuda F2 Insertar campo F3 Borrar campo"
RETURN
desmarcaitem:
escrcelda cstfilact, cstcolact, ccstprintseparar
RETURN
marcaitem:
ponceldacursor
RETURN
buscarcelda:
fil = 1
IF NOT esceldacursoract% THEN
  cstcolact = ccstmaxcol
  DO
    cstcolact = cstcolact - 1
    IF cstcolact < 1 THEN
      cstcolact = ccstmaxcol
      cstfilact = cstfilact + fil
      IF cstfilact >= cstultfil THEN fil = -1
      END IF
    LOOP UNTIL esceldacursoract%
  END IF
GOTO marcaitem
END FUNCTION

FUNCTION esceldacursoract% STATIC

DIM control AS STRING

control = cstcontrol(cstfilact, cstcolact)
esceldacursoract% = control <> "" AND MID$(control, 2, 1) <> ccstmsktexto
END FUNCTION

SUB escrcelda (fil%, col%, flags%) STATIC
printcelda 3 + fil% - cstprifilpan, fil%, col%, flags%
END SUB

DEFINT A-Z
FUNCTION getdate$ (caden$)
IF RTRIM$(caden$) <> "" THEN
  getdate$ = RIGHT$(caden$, 2) + "/" + MID$(caden$, 5, 2) + "/" + LEFT$(caden$, 4)
    ELSE
  getdate$ = ""
  END IF
END FUNCTION

DEFSNG A-Z
FUNCTION getstrcelda$ (fil%, col%, flags%) STATIC

CONST opcnonum = 0
CONST opcnuml = 1
CONST opcnumd = 2

CONST ningunaunidad = 0
CONST unidadprecio = 1
CONST unidadpeso = 2
CONST unidadgolpes = 3
CONST unidadpreciopeso = 4
CONST unidadtiempo = 5
CONST unidadporcentaje = 6

DIM control AS STRING
DIM idx AS INTEGER
DIM texto AS STRING
DIM rec AS LONG
DIM separar AS INTEGER
DIM i AS INTEGER
DIM cam AS STRING
DIM opc AS INTEGER
DIM numl AS LONG
DIM numd AS DOUBLE
DIM unidad AS INTEGER

control = cstcontrol(fil%, col%)
texto = ""
IF MID$(control, 2, 1) = ccstmsktexto THEN
  texto = MID$(control, 3)
    ELSE
  idx = VAL(RIGHT$(control, 1))
  separar = (flags% AND ccstprintseparar) <> 0
  cam = MID$(control, 3, 5)
  SELECT CASE cam
    CASE ccamCliDescripcion
      texto = MID$(coston(idx).coddisseny, 1, 2)
      i = RTRIM$(texto) <> ""
      IF i THEN
        i = strcmp%(texto, cliente(1).codcliente) = 0
        IF NOT i THEN
          cliente(1).codcliente = texto
          i = dtbuscar%(ccliente, 1, rec, TRUE)
          END IF
        END IF
      IF NOT i THEN cliente(1).empresa = ""
    CASE ccamMatDescripcion TO ccamMatPrecioKilo
      texto = costom(idx).codmateria
      i = RTRIM$(texto) <> ""
      IF i THEN
        i = strcmp%(texto, amateria(idx).codmateria) = 0
        IF NOT i THEN
          amateria(idx).codmateria = texto
          i = dtbuscar%(camateria, idx, rec, TRUE)
          END IF
        END IF
      IF NOT i THEN
        amateria(idx).descripcio = ""
        amateria(idx).espesor = 0
        amateria(idx).ancho = 0
        amateria(idx).preciokilo = 0
        END IF
    CASE ccamTraDescripcio TO ccamTraPrecioKilo
      texto = costr(idx).codtratami
      i = RTRIM$(texto) <> ""
      IF i THEN
        i = strcmp%(texto, altrata(idx).codtratami) = 0
        IF NOT i THEN
          altrata(idx).codtratami = texto
          i = dtbuscar%(caltrata, idx, rec, TRUE)
          END IF
        END IF
      IF NOT i THEN
        altrata(idx).descripcio = ""
        altrata(idx).preciokilo = 0
        END IF
    CASE ccamMaqDescripcio
      texto = costomec(idx).codmaquina
      i = RTRIM$(texto) <> ""
      IF i THEN
        i = strcmp%(texto, amaquina(idx).codmaquina) = 0
        IF NOT i THEN
          amaquina(idx).codmaquina = texto
          i = dtbuscar%(camaquina, idx, rec, TRUE)
          END IF
        END IF
      IF NOT i THEN amaquina(idx).descripcio = ""
    CASE ccamTalDescripcio
      texto = costotal(idx).codmaquina
      i = RTRIM$(texto) <> ""
      IF i THEN
        i = strcmp%(texto, ataller(idx).codmaquina) = 0
        IF NOT i THEN
          ataller(idx).codmaquina = texto
          i = dtbuscar%(cataller, idx, rec, TRUE)
          END IF
        END IF
      IF NOT i THEN ataller(idx).descripcio = ""
    END SELECT
  opc = opcnonum
  unidad = ningunaunidad
  SELECT CASE cam
    CASE ccamCostoN
      texto = coston(idx).costeon
    CASE ccamFechaEntra
      texto = getdate$(coston(idx).fechaentra)
    CASE ccamFechaSalid
      texto = getdate$(coston(idx).fechasalid)
    CASE ccamCliCodigo
      texto = MID$(coston(idx).coddisseny, 1, 2)
    CASE ccamCliDescripcion
      texto = cliente(1).empresa
    CASE ccamCPiezaClie
      texto = coston(idx).cpiezaclie
    CASE ccamDescrPieza
      texto = coston(idx).descrpieza
    CASE ccamPiezasAnua
      opc = opcnuml
      numl = coston(idx).piezasanua
    CASE ccamPiezasMesu
      opc = opcnuml
      numl = coston(idx).piezasmesu
    CASE ccamCodDisseny
      texto = coston(1).coddisseny
   CASE ccamMatCodigo
      texto = costom(idx).codmateria
    CASE ccamMatDescripcion
      texto = amateria(idx).descripcio
    CASE ccamMatEspesor
      opc = opcnumd
      numd = amateria(idx).espesor
    CASE ccamMatAncho
      opc = opcnumd
      numd = amateria(idx).ancho
    CASE ccamMatPrecioKilo
      opc = opcnumd
      numd = amateria(idx).preciokilo
    CASE ccamMatPaso
      texto = Cvit$(costom(idx).paso)
      IF separar THEN texto = texto + " m/m"
    CASE ccamMatPesoNeto
      opc = opcnumd
      numd = costom(idx).pesoneto
    CASE ccamMatPesoBruto
      opc = opcnumd
      numd = costom(idx).pesobruto
    CASE ccamMatCosteNeto
      opc = opcnumd
      unidad = unidadprecio
      numd = costom(idx).costeneto
    CASE ccamMatCosteBruto
      opc = opcnumd
      unidad = unidadprecio
      numd = costom(idx).costebruto
    CASE ccamMatTotalMLote
      opc = opcnumd
      unidad = unidadpeso
      numd = coston(1).piezasmesu * costom(idx).pesobruto
    CASE ccamMatTotalMAnual
      opc = opcnumd
      unidad = unidadpeso
      numd = coston(1).piezasanua * costom(idx).pesobruto
    CASE ccamTraCodigo
      texto = costr(idx).codtratami
    CASE ccamTraDescripcio
      texto = altrata(idx).descripcio
    CASE ccamTraPrecioKilo
      opc = opcnumd
      unidad = unidadpreciopeso
      numd = altrata(idx).preciokilo
    CASE ccamTraImportePie
      opc = opcnumd
      unidad = unidadprecio
      numd = costr(idx).importepie
    CASE ccamMaqCodigo
      texto = costomec(idx).codmaquina
    CASE ccamMaqDescripcio
      texto = amaquina(idx).descripcio
    CASE ccamMaqGolpeMinut
      opc = opcnuml
      unidad = unidadgolpes
      numl = costomec(idx).golpeminut
    CASE ccamMaqTiempoPrep
      unidad = unidadtiempo
      texto = Cvit$(costomec(idx).tiempoprep)
    CASE ccamMaqPorOperari
      opc = opcnumd
      numd = costomec(idx).poroperari
      unidad = unidadporcentaje
    CASE ccamTalCodigo
      texto = costotal(idx).codmaquina
    CASE ccamTalDescripcio
      texto = ataller(idx).descripcio
    CASE ccamTalHoras
      opc = opcnumd
      unidad = unidadtiempo
      numd = costotal(idx).horas
    END SELECT
  SELECT CASE opc
    CASE opcnuml
      IF separar THEN
        texto = cnummiles$(numl)
          ELSE
        texto = Cvlt$(numl)
        END IF
    CASE opcnumd
      IF separar THEN
        texto = cnummilesd$(numd)
          ELSE
        texto = Cvdt$(numd)
        END IF
    END SELECT
  SELECT CASE unidad
    CASE unidadprecio
      IF separar THEN texto = texto + " Pts"
    CASE unidadpeso
      IF separar THEN texto = texto + " Kg"
    CASE unidadgolpes
      IF separar THEN texto = texto + " Golpes"
    CASE unidadpreciopeso
      IF separar THEN texto = texto + " Pts Kg"
    CASE unidadtiempo
      IF separar THEN texto = texto + " Horas"
    CASE unidadporcentaje
      IF separar THEN texto = texto + "%"
    END SELECT
  END IF
getstrcelda$ = RTRIM$(texto)
END FUNCTION

SUB inputcelda (modi%) STATIC
DIM tec AS STRING
DIM control AS STRING
DIM i AS INTEGER
DIM cadini AS STRING
DIM caden AS STRING
DIM x AS INTEGER
DIM y AS INTEGER
DIM esesc AS INTEGER
DIM idx AS INTEGER
DIM cadinidbl AS DOUBLE

control = cstcontrol(cstfilact, cstcolact)
x = cstcolumna(cstcolact) - cstcolumna(cstpricolpan) + 2
y = 3 + cstfilact - cstprifilpan
cadini = getstrcelda(cstfilact, cstcolact, FALSE)
cadinidbl = VAL(cadini)
printcelda y, cstfilact, cstcolact, 0
idx = VAL(RIGHT$(control, 1))
SELECT CASE MID$(control, 3, 5)
  CASE ccamFechaEntra
    esesc = inputdate$(y, x, cadini, coston(idx).fechaentra, FALSE) = CHR$(27)
  CASE ccamFechaSalid
    esesc = inputdate$(y, x, cadini, coston(idx).fechasalid, FALSE) = CHR$(27)
  CASE ccamCliCodigo
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 2, 1, 0) = CHR$(27)
    IF NOT esesc THEN MID$(coston(idx).coddisseny, 1, 2) = caden
  CASE ccamCPiezaClie
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 17, 1, 0) = CHR$(27)
    IF NOT esesc THEN coston(idx).cpiezaclie = caden
  CASE ccamDescrPieza
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 20, 0, 0) = CHR$(27)
    IF NOT esesc THEN coston(idx).descrpieza = caden
  CASE ccamPiezasAnua
    esesc = inputstr$(y, x, "0", "9", cadini, caden, 8, 0, 0) = CHR$(27)
    IF NOT esesc THEN coston(idx).piezasanua = VAL(caden)
  CASE ccamPiezasMesu
    esesc = inputstr$(y, x, "0", "9", cadini, caden, 8, 0, 0) = CHR$(27)
    IF NOT esesc THEN coston(idx).piezasmesu = VAL(caden)
  CASE ccamCodDisseny
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 5, 1, 0) = CHR$(27)
    IF NOT esesc THEN coston(idx).coddisseny = caden
  CASE ccamMatCodigo
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 5, 1, 0) = CHR$(27)
    IF NOT esesc THEN costom(idx).codmateria = caden
  CASE ccamMatPaso
    esesc = inputstr$(y, x, "0", "9", cadini, caden, 8, 0, 0) = CHR$(27)
    IF NOT esesc THEN costom(idx).paso = VAL(caden)
  CASE ccamMatPesoNeto
    esesc = inputdbl$(y, x, cadinidbl, costom(idx).pesoneto, 7, 5, 0) = CHR$(27)
  CASE ccamMatPesoBruto
    esesc = inputdbl$(y, x, cadinidbl, costom(idx).pesobruto, 7, 5, 0) = CHR$(27)
  CASE ccamMatCosteNeto
    esesc = inputdbl$(y, x, cadinidbl, costom(idx).costeneto, 7, 5, 0) = CHR$(27)
  CASE ccamMatCosteBruto
    esesc = inputdbl$(y, x, cadinidbl, costom(idx).costebruto, 7, 5, 0) = CHR$(27)
  CASE ccamTraCodigo
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 5, 1, 0) = CHR$(27)
    IF NOT esesc THEN costr(idx).codtratami = caden
  CASE ccamTraImportePie
    esesc = inputdbl$(y, x, cadinidbl, costr(idx).importepie, 7, 5, 0) = CHR$(27)
  CASE ccamMaqCodigo
    esesc = inputstr$(y, x, " ", CHR$(255), cadini, caden, 5, 1, 0) = CHR$(27)
    IF NOT esesc THEN costomec(idx).codmaquina = caden
  CASE ccamMaqGolpeMinut
    esesc = inputstr$(y, x, "0", "9", cadini, caden, 5, 0, 0) = CHR$(27)
    IF NOT esesc THEN costomec(idx).golpeminut = VAL(caden)
  CASE ccamMaqTiempoPrep
    esesc = inputstr$(y, x, "0", "9", cadini, caden, 2, 0, 0) = CHR$(27)
    IF NOT esesc THEN costomec(idx).tiempoprep = VAL(caden)
  CASE ccamMaqPorOperari
    esesc = inputdbl$(y, x, cadinidbl, costomec(idx).poroperari, 5, 2, 0) = CHR$(27)
  CASE ccamTalHoras
    esesc = inputdbl$(y, x, cadinidbl, costotal(idx).horas, 9, 2, 0) = CHR$(27)
  CASE ELSE
    esscr = TRUE
  END SELECT
i = obtidxarch%(control)
IF i > 0 THEN cstsubcamcontrol(i).modificado = cstsubcamcontrol(i).modificado OR NOT esesc ELSE modi% = modi% OR NOT esesc
IF esesc OR MID$(control, 2, 1) = ccstmskcampo THEN
  escrcelda cstfilact, cstcolact, ccstprintseparar OR ccstprintinv
    ELSE
  printhoja
  END IF
END SUB

DEFINT A-Z
FUNCTION inputdate$ (fila AS INTEGER, col AS INTEGER, cadini$, caden$, sitab AS INTEGER)
DIM tec AS STRING
DIM i AS INTEGER
DIM i2 AS INTEGER
DIM num(8) AS INTEGER
DIM nums AS INTEGER
DIM txt AS STRING * 10
DIM datini AS STRING
datini = cadini$
DO
  tec = inputstr$(fila, col, "/", "9", datini, txt, 10, FALSE, sitab)
  datini = txt
  IF tec = CHR$(27) THEN GOTO inpdatfin
  nums = 1
  i = 1
  DO
    i2 = INSTR(i, txt, "/")
    IF i2 < 1 THEN i2 = 255
    num(nums) = VAL(MID$(txt, i, i2 - i))
    i = i2 + 1
    nums = nums + 1
    LOOP UNTIL i2 = 255
  LOOP UNTIL nums = 4 AND num(1) >= 1 AND num(1) <= 31 AND num(2) >= 1 AND num(2) <= 12 AND num(3) < 10000
  datini = Cvit$(num(3))
  caden$ = MID$(DATE$, 7, 4 - LEN(datini)) + datini + Cvit0$(num(2), 2) + Cvit0$(num(1), 2)
inpdatfin:
inputdate$ = tec
END FUNCTION

DEFSNG A-Z
SUB insfilas (fil%, num%) STATIC
DIM i AS INTEGER
DIM col AS INTEGER
FOR i = cstultfil TO fil% STEP -1
  FOR col = 1 TO ccstmaxcol
    cstcontrol(i + num%, col) = cstcontrol(i, col)
    cstcontrol(i, col) = ""
    NEXT
  NEXT
cstultfil = cstultfil + num%
END SUB

DEFINT A-Z
SUB lprinthoja (opccmbhoja%) STATIC

CONST lbitsmodogr = 1920
CONST lbitsmodogrL = 1920&
CONST lleancho = 137
CONST lbitsalt = 3
CONST lbitsimpanc = lbitsmodogr - 2
CONST lnumcarl = 137
CONST lnumfil = 2

DIM lgrbits(lbitsalt) AS STRING * lbitsmodogr
DIM lgrposstr(lnumfil, ccstmaxcol) AS INTEGER
DIM lgrstr(lnumfil, ccstmaxcol) AS STRING
DIM lbmgcdesc AS STRING * 2
DIM lgrmodo AS STRING * 4
DIM lbitsimpcar AS INTEGER
DIM lcar13 AS STRING * 1
DIM lesc3 AS STRING * 2
DIM lcar255 AS STRING * 1

DIM i AS INTEGER
DIM j AS INTEGER
DIM h AS INTEGER
DIM n AS INTEGER
DIM fil AS INTEGER
DIM col AS INTEGER
DIM col2 AS INTEGER
DIM ancho AS INTEGER
DIM lx AS INTEGER
DIM li AS INTEGER
DIM car AS INTEGER
DIM la AS INTEGER

lbmgcdesc = CHR$(27) + "Z"
lgrmodo = lbmgcdesc + MKI$(lbitsmodogr)
lbitsimpcar = 128
lcar13 = CHR$(13)
lcar255 = CHR$(255)
lesc3 = CHR$(27) + "3"
initkeyesc
ON ERROR GOTO errorimpresora
OPEN "PRN" FOR OUTPUT AS ccosimpr
PRINT #ccosimpr, CHR$(15)
IF saleimprimir = 1 THEN GOTO ldone
GOSUB linitbuff
GOSUB limpbuff
IF saleimprimir = 1 THEN GOTO ldone
li = 1
FOR fil = 1 TO cstultfil
  lx = lbitsmodogr * 4 / lleancho
  col = 1
  DO
    col2 = col
    DO
      col2 = col2 + 1
      IF col2 > ccstmaxcol THEN EXIT DO
      LOOP WHILE cstcontrol(fil, col2) = ccstmskceldaant
    GOSUB calcla
    IF cstcontrol(fil, col) <> "" THEN
      GOSUB limpLnV
      lgrposstr(li, col) = lx + (lbitsmodogr / (2 * lleancho))
      lgrstr(li, col) = getstrcelda$(fil, col, ccstprintseparar)
      lx = lx + 1
      la = la - 1
      GOSUB limpLnH
        ELSE
      EXIT DO
      END IF
    col = col2
    lx = lx + la
    LOOP UNTIL col > ccstmaxcol
  IF col > 1 THEN GOSUB limpLnV
  IF fil > 1 THEN
    FOR col2 = col TO ccstmaxcol
      IF cstcontrol(fil - 1, col2) = "" THEN EXIT FOR
      NEXT
    IF col <> col2 THEN
      GOSUB calcla
      GOSUB limpLnH
      lx = lx + la
      END IF
    END IF
  IF saleimprimir = 1 THEN GOTO ldone
  li = li + 1
  IF li = 3 THEN
    GOSUB limpbuff
    li = 1
    END IF
  NEXT
PRINT #ccosimpr, lgrmodo; STRING$(lbitsmodogr, 128); CHR$(18); CHR$(12)
IF opccmbhoja% <> 1 THEN i = cambiohoja%
ldone:
CLOSE #ccosimpr
donekeyesc
ON ERROR GOTO 0
EXIT SUB
limpbuff:
h = 3
FOR i = 1 TO lbitsalt
  PRINT #ccosimpr, lesc3; CHR$(3 * h); lgrmodo; lgrbits(i)
  IF saleimprimir = 1 THEN RETURN
  h = 8 - h
  IF i < lbitsalt THEN
    PRINT #ccosimpr, lesc3; CHR$(3 * h);
    FOR j = 1 TO ccstmaxcol
      n = lgrposstr(i, j)
      IF n = 0 THEN EXIT FOR
      PRINT #ccosimpr, lbmgcdesc; MKI$(n); STRING$(n, 0); lgrstr(i, j); lcar13;
      IF saleimprimir = 1 THEN RETURN
      NEXT
    PRINT #ccosimpr,
    h = 12 - h
    IF h > 8 THEN h = 8
    END IF
  NEXT
linitbuff:
lgrbits(1) = lcar255 + STRING$(lbitsimpanc, lbitsimpcar) + lcar255
FOR i = 2 TO lbitsalt
  lgrbits(i) = lcar255 + STRING$(lbitsimpanc, 0) + lcar255
  NEXT
IF lbitsimpcar <> 0 THEN
  lbitsimpcar = 0
    ELSE
  FOR i = 1 TO lnumfil
    FOR j = 1 TO ccstmaxcol
      lgrposstr(i, j) = 0
      lgrstr(i, j) = ""
      NEXT
    NEXT
  END IF
RETURN
limpLnV:
i = li
SELECT CASE li
  CASE 1
    car = 240
  CASE 2
    car = 15
    i = 3
  END SELECT
MID$(lgrbits(i), lx, 1) = lcar255
'MID$(lgrbits(2), lx, 1) = CHR$(car OR ASC(MID$(lgrbits(2), lx, 1)))
strpnor lgrbits(2), lx, 1, car
RETURN
limpLnH:
IF li = 1 THEN car = 128 ELSE car = 8
'FOR i = lx TO lx + la - 1
  'MID$(lgrbits(li), i, 1) = CHR$(car OR ASC(MID$(lgrbits(li), i, 1)))
  'NEXT
strpnor lgrbits(li), lx, la, car
RETURN
calcla:
la = lbitsmodogrL * (cstcolumna(col2) - cstcolumna(col) - col2 + col) / lleancho
RETURN
END SUB

FUNCTION obtidxarch% (control$)

  DIM i AS INTEGER
 
  SELECT CASE MID$(control$, 3, 3)
    CASE ccamMaterial
      i = ccscmaterial
    CASE ccamTratamiento
      i = ccsctratamiento
    CASE ccamMaquina
      i = ccscmaquina
    CASE ccamTaller
      i = ccsccostotal
    CASE ccamCostoUtil
      i = ccsccostoutil
    CASE ELSE
      i = 0
    END SELECT
  obtidxarch% = i
END FUNCTION

DEFSNG A-Z
SUB poncelda (fil%, col%, control$) STATIC
cstcontrol(fil%, col%) = control$
END SUB

SUB ponceldacursor

DIM si AS INTEGER
DIM flags AS INTEGER

si = cmbrect%
IF NOT si THEN
  flags = ccstprintseparar OR ccstprintinv OR ccstprintvar
  escrcelda cstfilact, cstcolact, flags
  si = (flags AND ccstprintnocabe) <> 0
  IF si THEN
    flags = cstcolumna(cstcolact + 1)
    DO
      cstpricolpan = cstpricolpan + 1
      LOOP UNTIL flags - cstcolumna(cstpricolpan) < 78
    END IF
  END IF
IF si THEN printhoja
END SUB

SUB primeracelda
cstfilact = 2
cstcolact = 1
END SUB

SUB printcelda (y%, fil%, col%, flags%) STATIC
DIM txt AS STRING
DIM ancho AS INTEGER
DIM x AS INTEGER
DIM l AS INTEGER
DIM col2 AS INTEGER
DIM colp AS INTEGER
DIM control AS STRING
DIM nocabe AS INTEGER
DIM ti AS INTEGER
DIM pa AS INTEGER

control = cstcontrol(fil%, col%)
IF control <> "" AND control <> ccstmskceldaant THEN
  x = cstcolumna(col%) - cstcolumna(cstpricolpan) + 2
  colp = cstcolumna(col%)
  txt = getstrcelda$(fil%, col%, flags%)
  l = LEN(txt)
  SELECT CASE MID$(control, 2, 1)
    CASE ccstmsktexto
      ti = 7
      pa = 4
    CASE ELSE
      ti = 2
      pa = 1
    END SELECT
  IF (flags% AND ccstprintinv) <> 0 THEN poncolor 11, 12 ELSE poncolor ti, pa
  col2 = col%
  DO
    col2 = col2 + 1
    ancho = cstcolumna(col2) - colp - 1
    IF col2 > ccstmaxcol THEN EXIT DO
    LOOP WHILE cstcontrol(fil%, col2) = ccstmskceldaant
  IF l < ancho THEN txt = txt + SPACE$(ancho - l)
  LOCATE y%, x
  nocabe = x + ancho > 80
  IF nocabe THEN
    ancho = 80 - x
    IF (flags% AND ccstprintvar) <> 0 THEN flags% = flags OR ccstprintnocabe
    END IF
  PRINT LEFT$(txt, ancho);
  IF NOT nocabe THEN
    poncolor ti, pa
    PRINT "³"
    END IF
  END IF
END SUB

SUB printhoja STATIC

DIM y AS INTEGER
DIM fil AS INTEGER
DIM col AS INTEGER
DIM si AS INTEGER

poncolor 2, 1
box 2, 1, 24, 80
y = 3
si = cmbrect%
FOR fil = cstprifilpan TO cstultfil
  FOR col = cstpricolpan TO ccstmaxcol
    printcelda y, fil, col, ccstprintseparar
    NEXT
  y = y + 1
  IF y > 23 THEN EXIT FOR
  NEXT
escrcelda cstfilact, cstcolact, ccstprintseparar OR ccstprintinv
END SUB

SUB vaciarceldas (fili%, filf%) STATIC

DIM fil AS INTEGER
DIM col AS INTEGER

FOR fil = fili% TO filf%
  FOR col = 1 TO ccstmaxcol
    cstcontrol(fil, col) = ""
    NEXT
  NEXT
END SUB

