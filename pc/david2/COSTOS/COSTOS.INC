'$INCLUDE: 'coslib.inc'

TYPE campos
  x   AS INTEGER
  y   AS INTEGER
  longi AS INTEGER
  END TYPE

COMMON SHARED errorfichero AS INTEGER'si hay error al crear fichero de exportar
COMMON SHARED lineaimpresora AS INTEGER'linea actual que tiene la impresora(0 a 66)

DECLARE SUB fancycls (ti%, pa%)
DECLARE SUB printright (l%, c%, cadena$)
DECLARE SUB articulos ()
DECLARE SUB telefonos ()
DECLARE SUB materiales ()
DECLARE SUB tratamientos ()
DECLARE SUB maquinas ()
DECLARE SUB operarios ()
DECLARE SUB taller ()
DECLARE SUB utiles ()
DECLARE SUB modem (numpuerto AS INTEGER, mandato AS STRING)
DECLARE SUB imprime (fichero AS INTEGER, opciones2 AS INTEGER)

'Funciones
DECLARE FUNCTION inputnota$ (fila AS INTEGER, col AS INTEGER, mini AS STRING, maxi AS STRING, cadini$, caden$, longi AS INTEGER)
DECLARE FUNCTION haydomiciliacion% (pagos2 AS pagos)
DECLARE FUNCTION haynotas% (notas2 AS notas)
DECLARE FUNCTION errorimpresora$ ()

