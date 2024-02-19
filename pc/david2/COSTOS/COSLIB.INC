CONST TRUE = -1
CONST FALSE = NOT TRUE

'Constantes de ficheros,tambien son los handles
CONST cpagos = 1
CONST ccliente = 2
CONST cnotas = 3
CONST carticulo = 4
CONST ccoston = 5
CONST camateria = 6
CONST ccostom = 7
CONST caltrata = 8
CONST ccostr = 9
CONST camaquina = 10
CONST caoperari = 11
CONST ccostomec = 12
CONST cataller = 13
CONST ccostotal = 14
CONST cautiles = 15
CONST ccostouti = 16

CONST cdbidxmax = 2
CONST cdbcstmax = 1 + 5

CONST ccosdbf = ccostouti + 1
CONST ccosidx = ccosdbf + 1
CONST ccosarch = ccosidx
CONST ccoscfg = ccosarch + 1
CONST ccosdeli = ccoscfg + 1
CONST ccosimpr = ccosdeli + 1

CONST ctamidxraiz = 4 * ccosdbf

CONST cdtirinicio = 1
CONST cdtirfin = 2
CONST cdtirposterior = 3
CONST cdtiranterior = 4

CONST clavereg = 179 'clave con la cual estan encriptados los registros

TYPE cliente
codcliente      AS STRING * 2
empresa         AS STRING * 30
cif             AS STRING * 15
cargo           AS STRING * 15
nombre          AS STRING * 13
apellidos       AS STRING * 19
direccion       AS STRING * 30
localidad       AS STRING * 22
provincia       AS STRING * 17
pais            AS STRING * 20
postal          AS STRING * 5
telefono        AS STRING * 15
telefono2       AS STRING * 15
fax             AS STRING * 15
actividad       AS STRING * 30
END TYPE

TYPE notas
codcliente      AS STRING * 2
notas           AS STRING * 234
END TYPE

TYPE pagos
codcliente      AS STRING * 2
entidad         AS STRING * 30
direccionb      AS STRING * 30
localidadb      AS STRING * 22
telefonob       AS STRING * 15
provinciab      AS STRING * 17
postale         AS STRING * 5
codigocuen      AS STRING * 23
END TYPE

TYPE articulo
cformatec       AS STRING * 5
cliente         AS STRING * 21
pieza           AS STRING * 30
prunidad        AS DOUBLE
prcostos        AS DOUBLE
tprevisto       AS DOUBLE
treal           AS DOUBLE
notas           AS STRING * 254
END TYPE

TYPE coston
costeon         AS STRING * 5
fechaentra      AS STRING * 8
fechasalid      AS STRING * 8
coddisseny      AS STRING * 5
cpiezaclie      AS STRING * 17
descrpieza      AS STRING * 20
piezasanua      AS LONG
piezasmesu      AS LONG
notas           AS STRING * 254
END TYPE

TYPE amateria
codmateria      AS STRING * 4
descripcio      AS STRING * 45
espesor         AS DOUBLE
ancho           AS DOUBLE
preciokilo      AS DOUBLE
END TYPE

TYPE costom
costeon         AS STRING * 5
codmateria      AS STRING * 4
paso            AS INTEGER
pesoneto        AS DOUBLE
pesobruto       AS DOUBLE
costeneto       AS DOUBLE
costebruto      AS DOUBLE
END TYPE

TYPE altrata
codtratami      AS STRING * 4
descripcio      AS STRING * 45
preciokilo      AS DOUBLE
END TYPE

TYPE costr
codtratami      AS STRING * 4
costeon         AS STRING * 5
importepie      AS DOUBLE
END TYPE

TYPE amaquina
codmaquina      AS STRING * 4
descripcio      AS STRING * 30
preciohora      AS DOUBLE
preciomaqu      AS LONG
END TYPE

TYPE aoperari
codoperari      AS STRING * 4
descripcio      AS STRING * 30
preciohora      AS LONG
costoanual      AS LONG
END TYPE

TYPE costomec
costeon         AS STRING * 5
codmaquina      AS STRING * 4
golpeminut      AS LONG
tiempoprep      AS INTEGER
poroperari      AS DOUBLE
END TYPE

TYPE ataller
codmaquina      AS STRING * 4
descripcio      AS STRING * 30
preciohora      AS DOUBLE
preciomaqu      AS LONG
END TYPE

TYPE costotal
costeon         AS STRING * 5
codmaquina      AS STRING * 4
horas           AS DOUBLE
END TYPE

TYPE autiles
codutil         AS STRING * 4
descripcio      AS STRING * 30
precio          AS LONG
END TYPE

TYPE costouti
costeon         AS STRING * 5
descripcio      AS STRING * 45
importutil      AS LONG
importmant      AS LONG
importcali      AS LONG
totalutile      AS LONG
maquinabil      AS INTEGER
ncarros         AS INTEGER
imputilmaq      AS LONG
impmatrmaq      AS LONG
notas           AS STRING * 254
END TYPE

DIM SHARED pagos(cdbidxmax) AS pagos
DIM SHARED cliente(cdbidxmax) AS cliente
DIM SHARED notas(cdbidxmax) AS notas
DIM SHARED articulo(cdbidxmax) AS articulo
DIM SHARED coston(cdbidxmax) AS coston
DIM SHARED amateria(cdbcstmax) AS amateria
DIM SHARED costom(cdbcstmax) AS costom
DIM SHARED altrata(cdbcstmax) AS altrata
DIM SHARED costr(cdbcstmax) AS costr
DIM SHARED amaquina(cdbcstmax) AS amaquina
DIM SHARED aoperari(cdbcstmax) AS aoperari
DIM SHARED costomec(cdbcstmax) AS costomec
DIM SHARED ataller(cdbcstmax) AS ataller
DIM SHARED costotal(cdbcstmax) AS costotal
DIM SHARED autiles(cdbcstmax) AS autiles
DIM SHARED costouti(cdbcstmax) AS costouti

DIM SHARED colors(0 TO 12, 1 TO 4) AS INTEGER             'Different Colors

COMMON comenzar AS INTEGER

COMMON SHARED pagos() AS pagos
COMMON SHARED cliente() AS cliente
COMMON SHARED notas() AS notas
COMMON SHARED articulo() AS articulo
COMMON SHARED coston() AS coston
COMMON SHARED amateria() AS amateria
COMMON SHARED costom() AS costom
COMMON SHARED altrata() AS altrata
COMMON SHARED costr() AS costr
COMMON SHARED amaquina() AS amaquina
COMMON SHARED aoperari() AS aoperari
COMMON SHARED costomec() AS costomec
COMMON SHARED ataller() AS ataller
COMMON SHARED costotal() AS costotal
COMMON SHARED autiles() AS autiles
COMMON SHARED costouti() AS costouti

COMMON SHARED colors() AS INTEGER

COMMON SHARED clave1 AS STRING     'clave encriptada con 255-( xor 188)
COMMON SHARED clave2 AS STRING     'clave desencriptada
COMMON SHARED puerto AS INTEGER'puerto para llamar al modem
COMMON SHARED tipomarcado AS STRING * 1'tipo de marcado para el modem,P=por pulsos,T=por tonos
COMMON SHARED costeon AS INTEGER
COMMON SHARED colorpref AS INTEGER'color Preference
COMMON SHARED teclescatrapada AS INTEGER'si =1 se utiliza ESC en un on key gosub,si =0 no
COMMON SHARED saleimprimir AS INTEGER'si =1 se ha pulsado ESC al darse un error y se ha de salir,si =0 no
COMMON SHARED nuevahoja AS INTEGER'para imprimir

DECLARE SUB dtcrea (nombre AS STRING, longitud AS INTEGER, handle AS INTEGER)
DECLARE SUB dtabre (nombre AS STRING, handle AS INTEGER)
DECLARE SUB idxcrea ()
DECLARE SUB idxabre ()
DECLARE SUB dtinsertar (arch%, idx%)
DECLARE SUB dtir (arch%, idx%, func%, rec&)
DECLARE SUB dtIrA (arch%, idx%, rec&)
DECLARE SUB dtborrar (arch%, idx%, rec&)
DECLARE SUB dtmodificar (arch%, idx%, idxold%, rec&)
DECLARE SUB dtborrartodo (arch%)

DECLARE FUNCTION strcmp% (str1$, str2$)
DECLARE FUNCTION dtobtregistros& (arch%)
DECLARE FUNCTION dtbuscar% (arch%, idx%, rec&, igual%)
DECLARE FUNCTION dtcontinuar% (arch%, idx%, rec&)

DECLARE SUB poncolor (ti%, pa%)
DECLARE SUB InstReloj ()
DECLARE SUB opencfg ()
DECLARE SUB savecfg ()
DECLARE SUB borramenusystem ()
DECLARE SUB lineainf ()
DECLARE SUB initkeyesc ()
DECLARE SUB donekeyesc ()
DECLARE SUB ponimprimiendo ()
DECLARE SUB ponborrar ()
DECLARE SUB Initialize ()
DECLARE SUB center (row%, text$)
DECLARE SUB box (Row1%, Col1%, Row2%, col2%)
DECLARE SUB PrintHelpLine (help$)
DECLARE SUB printlineainf (inf$)
DECLARE SUB encriptareg CDECL (punt AS ANY, BYVAL longi AS INTEGER, BYVAL clave AS INTEGER)
'encriptareg est  incluida en cosasm2.lib y cosasm.qlb y cosasm2.qlb y cosasm.lib

DECLARE FUNCTION getkey$ () 'lee tecla
DECLARE FUNCTION inputstr$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER, amayus AS INTEGER, sitab AS INTEGER)
DECLARE FUNCTION inputdbl$ (fila AS INTEGER, col AS INTEGER, dblini AS DOUBLE, dblfin AS DOUBLE, longientero AS INTEGER, longidecimal AS INTEGER, sitab AS INTEGER)
DECLARE FUNCTION escoge% (opcs() AS STRING, numopcs AS INTEGER, tituloventan AS STRING)
DECLARE FUNCTION confirmacion$ (mensaje AS STRING, caden AS STRING)
DECLARE FUNCTION Cvdt$ (x AS DOUBLE)
DECLARE FUNCTION Cvit$ (x AS INTEGER)
DECLARE FUNCTION Cvlt$ (x AS LONG)
DECLARE FUNCTION Cvit0$ (num%, num0%)
DECLARE FUNCTION errorimpresora$ ()
DECLARE FUNCTION cambiohoja% ()
DECLARE FUNCTION pregsprevsimpri% (opciones() AS INTEGER, hastafin AS INTEGER)

