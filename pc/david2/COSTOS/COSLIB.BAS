DEFINT A-Z

' $INCLUDE: 'COSLIB.INC'

CONST citemmax = 5
CONST cidxrojo = 1
CONST cidxborrado = 2
CONST cidxx = 1
CONST cidxp = 2
CONST cidxa = 3
CONST cidxba = 4
CONST cidxder = 5
CONST cidxizq = 6
CONST cidxy = 7
CONST cidxh = 8
CONST cidxn = 9
CONST cidxmax = cidxn

CONST cidxstackmax = 2 * 32 + 2

CONST cidxstkramader = 1
CONST cidxstkkeydup = 2

TYPE CabArch
  registros AS LONG
  ultborrado AS LONG
  primerborrado AS LONG
  Longitud AS INTEGER
  raiz AS STRING * ctamidxraiz
  END TYPE

TYPE itemIdx
  control AS INTEGER
  item AS STRING * citemmax
  END TYPE

TYPE ArbolEqui
  item AS itemIdx
  registro AS LONG
  der AS LONG
  izq AS LONG
  END TYPE

TYPE IdxStack
  ref AS LONG
  rama AS INTEGER      '1=der, 0 =izq
  END TYPE

DIM SHARED nombarch(ccosarch) AS STRING
DIM SHARED archivos(ccosarch) AS CabArch

DIM SHARED refs(cidxmax) AS LONG
DIM SHARED items(cidxmax) AS ArbolEqui

DIM SHARED idxarrpila(ccosdbf) AS INTEGER
DIM SHARED idxpila(ccosdbf, cidxstackmax) AS IdxStack

DIM SHARED dtbusitem AS itemIdx

DECLARE SUB idxnewitem (item AS ArbolEqui)
DECLARE SUB idxnewitemref (idx%)
DECLARE SUB grabcab (arch%)
DECLARE SUB dbseek (arch%, rec&)
DECLARE SUB idxinit ()
DECLARE SUB idxgetitems (idx%, item AS itemIdx)
DECLARE SUB idxget (idx%, rec&)
DECLARE SUB idxgetref (idx%, ref%)
DECLARE SUB idxput (idx%, ref&)
DECLARE SUB idxputref (idx%)
DECLARE SUB idxgetfi (arch%, idx%, rec&)
DECLARE FUNCTION idxcmp% (item AS itemIdx, idx%)
DECLARE FUNCTION dbgetultrec& (arch%)
DECLARE SUB idxadd (arch%, idx%, rec&)
DECLARE SUB creacfg ()
DECLARE SUB dbget (arch%, idx%, rec AS LONG)
DECLARE SUB dbput (arch%, idx%, rec&)
DECLARE SUB dbborrar (arch%, rec&)
DECLARE SUB idxputref (idx%)
DECLARE SUB idxrotar (item AS itemIdx, idx%, ret%)
DECLARE SUB idxdividir (item AS itemIdx)
DECLARE SUB idxgetdbitem (arch%, idx%, item AS itemIdx)
DECLARE SUB idxponpila (arch%, pila AS IdxStack)
DECLARE SUB idxgetpila (arch%, pila AS IdxStack)
DECLARE SUB idxputpila (arch%, pila AS IdxStack)
DECLARE SUB idxquitapila (arch%, pila AS IdxStack)
DECLARE SUB idxborrapila (arch%)
DECLARE SUB idxdelstkkey ()

DECLARE FUNCTION idxpilavacia% (arch%)
DECLARE FUNCTION idxgetraiz& (arch%)

DECLARE FUNCTION idxirprimero& (arch%)
DECLARE FUNCTION idxposterior& (arch%)
DECLARE FUNCTION idxirultimo& (arch%)
DECLARE FUNCTION idxanterior& (arch%)
DECLARE FUNCTION idxbuscar% (arch%, item AS itemIdx, rec&)
DECLARE FUNCTION idxfindrec% (arch%, idx%, rec&, doget%)

ErrorDbKill:
  RESUME NEXT

DEFSNG A-Z
SUB dbborrar (arch%, rec&) STATIC
dbseek arch%, rec&
PUT #arch%, , archivos(arch%).primerborrado
archivos(arch%).primerborrado = rec&
archivos(arch%).registros = archivos(arch%).registros - 1
grabcab arch%
END SUB

DEFINT A-Z
SUB dbget (arch%, idx%, rec&) STATIC

DIM lng AS INTEGER

  dbseek arch%, rec&
  lng = archivos(arch%).Longitud
  SELECT CASE arch%
    CASE cpagos:
    GET #arch, , pagos(idx%)
    encriptareg pagos(idx%), lng, clavereg
    CASE ccliente:
    GET #arch, , cliente(idx%)
    encriptareg cliente(idx%), lng, clavereg
    CASE cnotas:
    GET #arch, , notas(idx%)
    encriptareg notas(idx%), lng, clavereg
    CASE carticulo:
    GET #arch, , articulo(idx%)
    encriptareg articulo(idx%), lng, clavereg
    CASE ccoston:
    GET #arch, , coston(idx%)
    encriptareg coston(idx%), lng, clavereg
    CASE camateria:
    GET #arch, , amateria(idx%)
    encriptareg amateria(idx%), lng, clavereg
    CASE ccostom:
    GET #arch, , costom(idx%)
    encriptareg costom(idx%), lng, clavereg
    CASE caltrata:
    GET #arch, , altrata(idx%)
    encriptareg altrata(idx%), lng, clavereg
    CASE ccostr:
    GET #arch, , costr(idx%)
    encriptareg costr(idx%), lng, clavereg
    CASE camaquina:
    GET #arch, , amaquina(idx%)
    encriptareg amaquina(idx%), lng, clavereg
    CASE caoperari:
    GET #arch, , aoperari(idx%)
    encriptareg aoperari(idx%), lng, clavereg
    CASE ccostomec:
    GET #arch, , costomec(idx%)
    encriptareg costomec(idx%), lng, clavereg
    CASE cataller:
    GET #arch, , ataller(idx%)
    encriptareg ataller(idx%), lng, clavereg
    CASE ccostotal:
    GET #arch, , costotal(idx%)
    encriptareg costotal(idx%), lng, clavereg
    CASE cautiles:
    GET #arch, , autiles(idx%)
    encriptareg autiles(idx%), lng, clavereg
    CASE ccostouti:
    GET #arch, , costouti(idx%)
    encriptareg costouti(idx%), lng, clavereg
    END SELECT
END SUB

FUNCTION dbgetultrec& (arch%) STATIC
DIM rec AS LONG
rec = archivos(arch%).primerborrado
archivos(arch%).registros = archivos(arch%).registros + 1
IF rec <> -1 THEN
  dbseek arch%, rec
  GET #arch%, , archivos(arch%).primerborrado
    ELSE
  rec = archivos(arch%).ultborrado
  archivos(arch%).ultborrado = archivos(arch%).ultborrado + 1
  END IF
dbgetultrec& = rec
END FUNCTION

SUB dbput (arch%, idx%, rec&) STATIC
DIM lng AS INTEGER

  dbseek arch%, rec&
  lng = archivos(arch%).Longitud
  SELECT CASE arch%
    CASE cpagos:
    encriptareg pagos(idx%), lng, clavereg
    PUT #arch, , pagos(idx%)
    encriptareg pagos(idx%), lng, clavereg
    CASE ccliente:
    encriptareg cliente(idx%), lng, clavereg
    PUT #arch, , cliente(idx%)
    encriptareg cliente(idx%), lng, clavereg
    CASE cnotas:
    encriptareg notas(idx%), lng, clavereg
    PUT #arch, , notas(idx%)
    encriptareg notas(idx%), lng, clavereg
    CASE carticulo:
    encriptareg articulo(idx%), lng, clavereg
    PUT #arch, , articulo(idx%)
    encriptareg articulo(idx%), lng, clavereg
    CASE ccoston:
    encriptareg coston(idx%), lng, clavereg
    PUT #arch, , coston(idx%)
    encriptareg coston(idx%), lng, clavereg
    CASE camateria:
    encriptareg amateria(idx%), lng, clavereg
    PUT #arch, , amateria(idx%)
    encriptareg amateria(idx%), lng, clavereg
    CASE ccostom:
    encriptareg costom(idx%), lng, clavereg
    PUT #arch, , costom(idx%)
    encriptareg costom(idx%), lng, clavereg
    CASE caltrata:
    encriptareg altrata(idx%), lng, clavereg
    PUT #arch, , altrata(idx%)
    encriptareg altrata(idx%), lng, clavereg
    CASE ccostr:
    encriptareg costr(idx%), lng, clavereg
    PUT #arch, , costr(idx%)
    encriptareg costr(idx%), lng, clavereg
    CASE camaquina:
    encriptareg amaquina(idx%), lng, clavereg
    PUT #arch, , amaquina(idx%)
    encriptareg amaquina(idx%), lng, clavereg
    CASE caoperari:
    encriptareg aoperari(idx%), lng, clavereg
    PUT #arch, , aoperari(idx%)
    encriptareg aoperari(idx%), lng, clavereg
    CASE ccostomec:
    encriptareg costomec(idx%), lng, clavereg
    PUT #arch, , costomec(idx%)
    encriptareg costomec(idx%), lng, clavereg
    CASE cataller:
    encriptareg ataller(idx%), lng, clavereg
    PUT #arch, , ataller(idx%)
    encriptareg ataller(idx%), lng, clavereg
    CASE ccostotal:
    encriptareg costotal(idx%), lng, clavereg
    PUT #arch, , costotal(idx%)
    encriptareg costotal(idx%), lng, clavereg
    CASE cautiles:
    encriptareg autiles(idx%), lng, clavereg
    PUT #arch, , autiles(idx%)
    encriptareg autiles(idx%), lng, clavereg
    CASE ccostouti:
    encriptareg costouti(idx%), lng, clavereg
    PUT #arch, , costouti(idx%)
    encriptareg costouti(idx%), lng, clavereg
    END SELECT
END SUB

SUB dbseek (arch%, rec&) STATIC
SEEK #arch%, 1 + LEN(archivos(1)) + rec& * archivos(arch%).Longitud
END SUB

SUB dtabre (nombre AS STRING, handle AS INTEGER) STATIC
nombarch(handle) = nombre
OPEN nombre FOR BINARY AS #handle LEN = 1
GET #handle, , archivos(handle)
END SUB

DEFSNG A-Z
SUB dtborrar (arch%, idx%, rec&) STATIC
IF rec& <> -1 THEN
  IF idxfindrec%(arch%, idx%, rec&, FALSE) THEN
    idxdelstkkey
    dbborrar arch%, rec&
    dtir arch%, idx%, cdtirposterior, rec&
    IF rec& = -1 THEN dtir arch%, idx%, cdtirfin, rec&
    END IF
  END IF
END SUB

SUB dtborrartodo (arch%) STATIC
idxborrapila arch%
WHILE idxposterior&(arch%) <> -1
  dbborrar ccosidx, refs(cidxx)
  WEND
idxget cidxx, idxgetraiz&(arch%)
idxnewitemref cidxx
idxputref cidxx
grabcab ccosidx
CLOSE #arch%
dtcrea nombarch(arch%), archivos(arch%).Longitud, arch%
END SUB

FUNCTION dtbuscar% (arch%, idx%, rec&, igual%) STATIC

DIM cmp AS INTEGER

idxgetdbitem arch%, idx%, dtbusitem
cmp = idxbuscar%(arch%, dtbusitem, rec&)
IF cmp = 0 OR NOT igual% THEN
  IF cmp > 0 THEN dtir arch%, idx%, cdtirposterior, rec&
  dbget arch%, idx%, rec&
    ELSE
  rec& = -1
  END IF
dtbuscar% = cmp = 0
END FUNCTION

FUNCTION dtcontinuar% (arch%, idx%, rec&) STATIC

dtir arch%, idx%, cdtirposterior, rec&
dtcontinuar% = rec& <> -1 AND idxcmp%(dtbusitem, cidxx) = 0

END FUNCTION

DEFINT A-Z
SUB dtcrea (nombre AS STRING, Longitud AS INTEGER, handle AS INTEGER) STATIC
ON ERROR GOTO ErrorDbKill
KILL nombre
ON ERROR GOTO 0
nombarch(handle) = nombre
OPEN nombre FOR BINARY AS #handle LEN = 1
archivos(handle).registros = 0
archivos(handle).ultborrado = 0
archivos(handle).primerborrado = -1
archivos(handle).Longitud = Longitud
archivos(handle).raiz = STRING$(ctamidxraiz, 0)
grabcab handle
END SUB

SUB dtinsertar (arch%, idx%) STATIC
DIM rec AS LONG
rec = -1
dtmodificar arch%, idx%, idx%, rec
END SUB

DEFSNG A-Z
SUB dtir (arch%, idx%, func%, rec&) STATIC
  SELECT CASE func%
    CASE cdtirinicio
      rec& = idxirprimero&(arch%)
    CASE cdtirfin
      rec& = idxirultimo&(arch%)
    CASE cdtirposterior
      rec& = idxposterior(arch%)
    CASE cdtiranterior
      rec& = idxanterior(arch%)
    END SELECT
  IF rec& <> -1 THEN dbget arch%, idx%, rec&
END SUB

SUB dtIrA (arch%, idx%, rec&) STATIC
DIM x AS INTEGER
x = idxfindrec%(arch%, idx%, rec&, TRUE)
END SUB

SUB dtmodificar (arch%, idx%, idxold%, rec&) STATIC
IF rec& <> -1 THEN
  IF idxfindrec%(arch%, idxold%, rec&, FALSE) THEN idxdelstkkey
    ELSE
  rec& = dbgetultrec(arch%)
  grabcab arch%
  END IF
dbput arch%, idx%, rec&
idxadd arch%, idx%, rec&
END SUB

FUNCTION dtobtregistros& (arch%)
dtobtregistros& = archivos(arch%).registros
END FUNCTION

DEFINT A-Z
SUB grabcab (arch%) STATIC
SEEK #arch%, 1
PUT #arch%, , archivos(arch%)
END SUB

SUB idxabre STATIC
idxinit
dtabre "COSTOS.IDX", ccosidx
END SUB

SUB idxadd (arch%, idx%, rec&) STATIC
DIM item AS itemIdx
DIM x AS LONG
DIM raiz AS LONG
DIM rec2 AS LONG
DIM cmp AS INTEGER
idxgetdbitem arch%, idx%, item
raiz = idxgetraiz&(arch%)
x = raiz
idxget cidxx, x
idxget cidxp, x
idxget cidxa, x
WHILE (x <> -1)
  idxgetref cidxba, cidxa
  idxgetref cidxa, cidxp
  idxgetref cidxp, cidxx
  IF idxcmp%(item, cidxx) < 0 THEN x = items(cidxx).izq ELSE x = items(cidxx).der
  idxget cidxx, x
  idxget cidxizq, items(cidxx).izq
  idxget cidxder, items(cidxx).der
  IF ((items(cidxizq).item.control AND cidxrojo) <> 0) AND ((items(cidxder).item.control AND cidxrojo) <> 0) THEN
    idxdividir item
    x = refs(cidxx)
    END IF
  WEND
x = dbgetultrec(ccosidx)
item.control = item.control OR cidxrojo
items(cidxx).item = item
items(cidxx).registro = rec&
items(cidxx).izq = -1
items(cidxx).der = -1
idxput cidxx, x
IF idxcmp%(item, cidxp) < 0 THEN items(cidxp).izq = x ELSE items(cidxp).der = x
idxputref cidxp
idxdividir item
idxget cidxizq, raiz
idxget cidxder, items(cidxizq).der
items(cidxder).item.control = items(cidxder).item.control AND NOT cidxrojo
idxputref cidxder
cmp = idxfindrec%(arch%, idx%, rec2, FALSE) <> 0
grabcab ccosidx
END SUB

DEFSNG A-Z
FUNCTION idxanterior& (arch%) STATIC
DIM pila AS IdxStack
DIM r AS LONG
DO
  IF idxpilavacia%(arch%) THEN
    idxgetfi arch%, cidxx, r
      ELSE
    idxgetpila arch%, pila
    idxget cidxx, pila.ref
    pila.rama = pila.rama AND NOT cidxstkramader
    idxputpila arch%, pila
    r = items(cidxx).izq
    END IF
  WHILE r <> -1
    idxget cidxx, r
    pila.ref = r
    pila.rama = pila.rama OR cidxstkramader
    idxponpila arch%, pila
    r = items(cidxx).der
    WEND
  IF NOT idxpilavacia(arch%) THEN
    DO WHILE (pila.rama AND cidxstkramader) = 0
      idxquitapila arch%, pila
      IF idxpilavacia(arch%) THEN EXIT DO
      LOOP
    IF (pila.rama AND cidxstkramader) <> 0 THEN
      idxget cidxx, pila.ref
      idxanterior& = items(cidxx).registro
      END IF
    END IF
  IF idxpilavacia(arch%) THEN
    idxanterior& = -1
    EXIT FUNCTION
    END IF
  LOOP WHILE (items(cidxx).item.control AND cidxborrado) <> 0
END FUNCTION

SUB idxborrapila (arch%) STATIC
idxarrpila(arch%) = 0
END SUB

FUNCTION idxbuscar% (arch%, item AS itemIdx, rec&) STATIC
DIM pila AS IdxStack
DIM cmp AS INTEGER
  idxborrapila arch%
  idxgetfi arch%, cidxx, pila.ref
  cmp = 1
  DO WHILE pila.ref <> -1
    idxget cidxx, pila.ref
    cmp = idxcmp%(item, cidxx)
    IF cmp < 0 THEN pila.rama = pila.rama AND NOT cidxstkramader ELSE pila.rama = pila.rama OR cidxstkramader
    IF cmp = 0 THEN pila.rama = pila.rama OR cidxstkkeydup
    idxponpila arch%, pila
    IF cmp = 0 THEN EXIT DO
    IF cmp < 0 THEN pila.ref = items(cidxx).izq ELSE pila.ref = items(cidxx).der
    LOOP
  rec& = items(cidxx).registro
  WHILE cmp = 0 AND (items(cidxx).item.control AND cidxborrado) <> 0
    rec& = idxposterior&(arch%)
    IF rec& = -1 THEN
      idxbuscar% = 1
      EXIT FUNCTION
      END IF
    cmp = idxcmp(item, cidxx)
    WEND
  idxbuscar% = cmp
END FUNCTION

DEFINT A-Z
FUNCTION idxcmp% (item AS itemIdx, idx%) STATIC
idxcmp% = strcmp%(item.item, items(idx%).item.item)
END FUNCTION

SUB idxcrea STATIC
idxinit
dtcrea "COSTOS.IDX", LEN(items(1)), ccosidx
DIM i AS INTEGER
DIM rec AS LONG
idxnewitemref cidxx
FOR i = 1 TO ctamidxraiz STEP 4
  rec = dbgetultrec&(ccosidx)
  idxput cidxx, rec
  MID$(archivos(ccosidx).raiz, i, 4) = MKL$(rec)
  NEXT
grabcab ccosidx
END SUB

DEFSNG A-Z
SUB idxdelstkkey STATIC
items(cidxx).item.control = items(cidxx).item.control OR cidxborrado
idxputref cidxx
END SUB

DEFINT A-Z
SUB idxdividir (item AS itemIdx) STATIC
items(cidxx).item.control = items(cidxx).item.control OR cidxrojo
idxputref cidxx
idxget cidxizq, items(cidxx).izq
items(cidxizq).item.control = items(cidxizq).item.control AND NOT cidxrojo
idxputref cidxizq
idxget cidxder, items(cidxx).der
items(cidxder).item.control = items(cidxder).item.control AND NOT cidxrojo
idxputref cidxder
IF (items(cidxp).item.control AND cidxrojo) <> 0 THEN
  items(cidxa).item.control = items(cidxa).item.control OR cidxrojo
  idxputref cidxa
  IF idxcmp%(item, cidxa) < 0 OR idxcmp%(item, cidxp) < 0 THEN idxrotar item, cidxa, cidxp
  idxrotar item, cidxba, cidxx
  items(cidxx).item.control = items(cidxx).item.control AND NOT cidxrojo
  idxputref cidxx
  END IF
END SUB

DEFSNG A-Z
FUNCTION idxfindrec% (arch%, idx%, rec&, doget%) STATIC
DIM item AS itemIdx
DIM rec2 AS LONG
DIM cmp AS INTEGER
primero = cdtbusprimero
dbget arch%, idx%, rec&
idxgetdbitem arch%, idx%, item
cmp = idxbuscar%(arch%, item, rec2)
WHILE cmp = 0 AND rec& <> rec2
  rec2 = idxposterior&(arch%)
  IF rec2 = -1 THEN
    idxfindrec% = -1
    EXIT FUNCTION
    END IF
  cmp = idxcmp%(item, cidxx)
  WEND
IF doget% AND rec& <> rec2 THEN dbget arch%, idx%, rec2
idxfindrec% = rec& = rec2
END FUNCTION

DEFINT A-Z
SUB idxget (idx%, rec&) STATIC
DIM i AS INTEGER
refs(idx%) = rec&
IF rec& = -1 THEN
  idxnewitemref idx%
    ELSE
  FOR i = 1 TO cidxmax
    IF i <> idx% AND refs(i) = rec& THEN
      items(idx%) = items(i)
      EXIT SUB
      END IF
    NEXT
  dbseek ccosidx, rec&
  GET #ccosidx, , items(idx%)
  END IF
END SUB

SUB idxgetdbitem (arch%, idx%, item AS itemIdx) STATIC
  SELECT CASE arch%
    CASE cpagos: item.item = pagos(idx%).codcliente
    CASE ccliente: item.item = cliente(idx%).codcliente
    CASE cnotas: item.item = notas(idx%).codcliente
    CASE carticulo: item.item = articulo(idx%).cformatec
    CASE ccoston: item.item = coston(idx%).costeon
    CASE camateria: item.item = amateria(idx%).codmateria
    CASE ccostom: item.item = costom(idx%).costeon
    CASE caltrata: item.item = altrata(idx%).codtratami
    CASE ccostr: item.item = costr(idx%).costeon
    CASE camaquina: item.item = amaquina(idx%).codmaquina
    CASE caoperari: item.item = aoperari(idx%).codoperari
    CASE ccostomec: item.item = costomec(idx%).costeon
    CASE cataller: item.item = ataller(idx%).codmaquina
    CASE ccostotal: item.item = costotal(idx%).costeon
    CASE cautiles: item.item = autiles(idx%).codutil
    CASE ccostouti: item.item = costouti(idx%).costeon
    END SELECT
END SUB

DEFSNG A-Z
SUB idxgetfi (arch%, idx%, rec&) STATIC
idxget idx%, idxgetraiz(arch%)
rec& = items(idx%).der
END SUB

SUB idxgetpila (arch%, pila AS IdxStack) STATIC
pila = idxpila(arch%, idxarrpila(arch%))
END SUB

FUNCTION idxgetraiz& (arch%) STATIC
idxgetraiz& = CVL(MID$(archivos(ccosidx).raiz, 4 * (arch% - 1) + 1, 4))
END FUNCTION

DEFINT A-Z
SUB idxgetref (idx%, ref%) STATIC
refs(idx%) = refs(ref%)
items(idx%) = items(ref%)
END SUB

DEFSNG A-Z
SUB idxinit STATIC
DIM i AS INTEGER
FOR i = 1 TO cidxmax
  refs(i) = -1
  idxnewitemref i
  NEXT
FOR i = 1 TO ccosdbf
  idxborrapila i
  NEXT
END SUB

FUNCTION idxirprimero& (arch%) STATIC
idxborrapila arch%
idxirprimero& = idxposterior&(arch%)
END FUNCTION

FUNCTION idxirultimo& (arch%) STATIC
idxborrapila arch%
idxirultimo = idxanterior&(arch%)
END FUNCTION

SUB idxnewitem (item AS ArbolEqui) STATIC
item.item.control = 0
item.item.item = ""
item.registro = -1
item.der = -1
item.izq = -1

END SUB

SUB idxnewitemref (idx%) STATIC
idxnewitem items(idx%)
END SUB

FUNCTION idxpilavacia% (arch%) STATIC
idxpilavacia% = (idxarrpila(arch%) = 0)
END FUNCTION

SUB idxponpila (arch%, pila AS IdxStack) STATIC
idxarrpila(arch%) = idxarrpila(arch%) + 1
idxputpila arch%, pila
END SUB

FUNCTION idxposterior& (arch%) STATIC
DIM pila AS IdxStack
DIM r AS LONG
DO
  IF idxpilavacia%(arch%) THEN
    idxgetfi arch%, cidxx, r
      ELSE
    idxgetpila arch%, pila
    r = pila.ref
    idxget cidxx, r
    r = items(cidxx).der
    END IF
  WHILE r <> -1
    idxget cidxx, r
    pila.ref = r
    pila.rama = pila.rama AND NOT cidxstkramader
    idxponpila arch%, pila
    r = items(cidxx).izq
    WEND
  IF NOT idxpilavacia(arch%) THEN
    DO WHILE (pila.rama AND cidxstkramader) <> 0
      idxquitapila arch%, pila
      IF idxpilavacia(arch%) THEN EXIT DO
      LOOP
    IF (pila.rama AND cidxstkramader) = 0 THEN
      idxget cidxx, pila.ref
      idxposterior& = items(cidxx).registro
      pila.rama = pila.rama OR cidxstkramader
      idxputpila arch%, pila
      END IF
    END IF
  IF idxpilavacia(arch%) THEN
    idxposterior& = -1
    EXIT FUNCTION
    END IF
  LOOP WHILE (items(cidxx).item.control AND cidxborrado) <> 0
END FUNCTION

DEFINT A-Z
SUB idxput (idx%, ref&) STATIC
DIM i AS INTEGER
refs(idx%) = ref&
IF ref& <> -1 THEN
  FOR i = 1 TO cidxmax
    IF i <> idx% AND refs(i) = ref& THEN items(i) = items(idx%)
    NEXT
  dbseek ccosidx, ref&
  PUT #ccosidx, , items(idx%)
  END IF
END SUB

DEFSNG A-Z
SUB idxputpila (arch%, pila AS IdxStack) STATIC
idxpila(arch%, idxarrpila(arch%)) = pila
END SUB

DEFINT A-Z
SUB idxputref (idx%) STATIC
idxput idx%, refs(idx%)
END SUB

DEFSNG A-Z
SUB idxquitapila (arch%, pila AS IdxStack) STATIC
idxarrpila(arch%) = idxarrpila(arch%) - 1
idxgetpila arch%, pila
END SUB

DEFINT A-Z
SUB idxrotar (item AS itemIdx, idx%, ret%) STATIC
idxgetref cidxy, idx%
IF idxcmp%(item, cidxy) < 0 THEN idxget cidxh, items(cidxy).izq ELSE idxget cidxh, items(cidxy).der
IF idxcmp%(item, cidxh) < 0 THEN
  idxget cidxn, items(cidxh).izq
  items(cidxh).izq = items(cidxn).der
  idxputref cidxh
  items(cidxn).der = refs(cidxh)
  idxputref cidxn
    ELSE
  idxget cidxn, items(cidxh).der
  items(cidxh).der = items(cidxn).izq
  idxputref cidxh
  items(cidxn).izq = refs(cidxh)
  idxputref cidxn
  END IF
IF idxcmp%(item, cidxy) < 0 THEN items(cidxy).izq = refs(cidxn) ELSE items(cidxy).der = refs(cidxn)
idxputref cidxy
idxgetref ret%, cidxn
END SUB

FUNCTION strcmp% (str1$, str2$) STATIC
DIM str3 AS STRING
DIM str4 AS STRING

str3 = RTRIM$(str1$)
str4 = RTRIM$(str2$)
IF str3 < str4 THEN
  strcmp% = -1
    ELSE
  IF str3 = str4 THEN strcmp% = 0 ELSE strcmp% = 1
  END IF
END FUNCTION

