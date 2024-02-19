DEFINT A-Z

'Clave por defecto="15051947"
CONST claveini$ = "rvsvrzwt"

'$INCLUDE: 'coslib.inc'

colorsetinfo:
'The following data defines the color schemes available via the main menu.
'
'    scrn  dots  bar  back   title  shdow  choice  curs   cursbk  shdow
DATA 0,    7,    15,  7,     0,     7,     0,      15,    0,      0,    9,      0
DATA 1,    9,    12,  3,     0,     1,     15,     0,     7,      0,    15,     1
DATA 3,    15,   13,  1,     14,    3,     15,     0,     7,      0,    15,     3
DATA 7,    12,   15,  4,     14,    0,     15,     15,    1,      0,    15,     4

'error handler si no encuentra costos.cfg
Errorcfg:
SELECT CASE ERR
    CASE 53
        'crea la configuraci¢n del color y la clave
        colorpref = 1
        puerto = 1
        tipomarcado = "P"
        OPEN "costos.cfg" FOR OUTPUT AS #ccoscfg
        PRINT #ccoscfg, claveini$
        PRINT #ccoscfg, colorpref
        PRINT #ccoscfg, puerto
        PRINT #ccoscfg, tipomarcado
        PRINT #ccoscfg, initcosteon
        CLOSE #ccoscfg
        RESUME
    CASE ELSE
        PRINT "Error  "; ERR; " :";
        ERROR ERR
END SELECT
RESUME NEXT

'Initialize:
'  Read colors in and set up assembly routines
SUB Initialize

WIDTH , 25
teclescatrapada = 0
RESTORE colorsetinfo
FOR ColorSet = 1 TO 4
   FOR x = 1 TO 12
   READ colors(x, ColorSet)
   NEXT x
NEXT ColorSet

END SUB

SUB opencfg
ON ERROR GOTO Errorcfg
OPEN "costos.cfg" FOR INPUT AS #ccoscfg
CLOSE #ccoscfg
OPEN "costos.cfg" FOR INPUT AS #ccoscfg
INPUT #ccoscfg, clave1
INPUT #ccoscfg, colorpref
INPUT #ccoscfg, puerto
INPUT #ccoscfg, tipomarcado
INPUT #ccoscfg, costeon
CLOSE #ccoscfg
ON ERROR GOTO 0 'pone el manejador de errores al del basic
END SUB

