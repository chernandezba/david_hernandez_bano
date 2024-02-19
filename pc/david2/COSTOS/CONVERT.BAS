' $INCLUDE: 'COSLIB.INC'

CLS
idxcrea
OPEN "disseny\cliente.txt" FOR INPUT AS #ccosdeli
dtcrea "CLIENTE.DAT", LEN(cliente(1)), ccliente
IF EOF(ccosdeli) THEN GOTO fin1
DO
INPUT #ccosdeli, cliente(1).codcliente, cliente(1).empresa, cliente(1).cif, cliente(1).cargo, cliente(1).nombre, cliente(1).apellidos, cliente(1).direccion, cliente(1).localidad, cliente(1).provincia, cliente(1).pais, cliente(1).postal, cliente(1). _
telefono, cliente(1).telefono2, cliente(1).fax, cliente(1).actividad
dtinsertar ccliente, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccliente)
LOOP WHILE NOT (EOF(ccosdeli))
fin1: CLOSE #ccliente
CLOSE #ccosdeli
CLS
OPEN "disseny\notas.txt" FOR INPUT AS #ccosdeli
dtcrea "notas.dat", LEN(notas(1)), cnotas
IF EOF(ccosdeli) THEN GOTO fin2
DO
INPUT #ccosdeli, notas(1).codcliente, notas(1).notas
dtinsertar cnotas, 1
LOCATE 1, 1: PRINT dtobtregistros&(cnotas)
LOOP WHILE NOT (EOF(ccosdeli))
fin2: CLOSE #ccosdeli
CLOSE #cnotas
CLS
OPEN "disseny\pagos.txt" FOR INPUT AS #ccosdeli
dtcrea "pagos.dat", LEN(pagos(1)), cpagos
IF EOF(ccosdeli) THEN GOTO fin3
DO
INPUT #ccosdeli, pagos(1).codcliente, pagos(1).entidad, pagos(1).direccionb, pagos(1).localidadb, pagos(1).telefonob, pagos(1).provinciab, pagos(1).postale, pagos(1).codigocuen
dtinsertar cpagos, 1
LOCATE 1, 1: PRINT dtobtregistros&(cpagos)
LOOP WHILE NOT (EOF(ccosdeli))
fin3: CLOSE #ccosdeli
CLOSE #cpagos
CLS
OPEN "disseny\articulo.txt" FOR INPUT AS #ccosdeli
dtcrea "articulo.dat", LEN(articulo(1)), carticulo
IF EOF(ccosdeli) THEN GOTO fin4
DO
INPUT #ccosdeli, articulo(1).cformatec, articulo(1).cliente, articulo(1).pieza
INPUT #ccosdeli, articulo(1).prunidad, articulo(1).prcostos, articulo(1).tprevisto
INPUT #ccosdeli, articulo(1).treal
INPUT #ccosdeli, articulo(1).notas
dtinsertar carticulo, 1
LOCATE 1, 1: PRINT dtobtregistros&(carticulo)
LOOP WHILE NOT (EOF(ccosdeli))
fin4: CLOSE #ccosdeli
CLOSE #carticulo
CLS
OPEN "disseny\coston.txt" FOR INPUT AS #ccosdeli
dtcrea "coston.dat", LEN(coston(1)), ccoston
IF EOF(ccosdeli) THEN GOTO fin5
DO
INPUT #ccosdeli, coston(1).costeon
INPUT #ccosdeli, coston(1).fechaentra, coston(1).fechasalid, coston(1).coddisseny, coston(1).cpiezaclie, coston(1).descrpieza
INPUT #ccosdeli, coston(1).piezasanua, coston(1).piezasmesu
INPUT #ccosdeli, coston(1).notas
dtinsertar ccoston, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccoston)
LOOP WHILE NOT (EOF(ccosdeli))
fin5: CLOSE #ccosdeli
CLOSE #ccoston
CLS
OPEN "disseny\amateria.txt" FOR INPUT AS #ccosdeli
dtcrea "amateria.dat", LEN(amateria(1)), camateria
IF EOF(ccosdeli) THEN GOTO fin6
DO
INPUT #ccosdeli, amateria(1).codmateria, amateria(1).descripcio
INPUT #ccosdeli, amateria(1).espesor, amateria(1).ancho, amateria(1).preciokilo
dtinsertar camateria, 1
LOCATE 1, 1: PRINT dtobtregistros&(camateria)
LOOP WHILE NOT (EOF(ccosdeli))
fin6: CLOSE #ccosdeli
CLOSE #camateria
CLS
OPEN "disseny\costom.txt" FOR INPUT AS #ccosdeli
dtcrea "costom.dat", LEN(costom(1)), ccostom
IF EOF(ccosdeli) THEN GOTO fin7
DO
INPUT #ccosdeli, costom(1).costeon, costom(1).codmateria, costom(1).paso
INPUT #ccosdeli, costom(1).pesoneto, costom(1).pesobruto, costom(1).costeneto
INPUT #ccosdeli, costom(1).costebruto
dtinsertar ccostom, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccostom)
LOOP WHILE NOT (EOF(ccosdeli))
fin7: CLOSE #ccosdeli
CLOSE #ccostom
CLS
OPEN "disseny\altrata.txt" FOR INPUT AS #ccosdeli
dtcrea "altrata.dat", LEN(altrata(1)), caltrata
IF EOF(ccosdeli) THEN GOTO fin8
DO
INPUT #ccosdeli, altrata(1).codtratami, altrata(1).descripcio, altrata(1).preciokilo
dtinsertar caltrata, 1
LOCATE 1, 1: PRINT dtobtregistros&(caltrata)
LOOP WHILE NOT (EOF(ccosdeli))
fin8: CLOSE #ccosdeli
CLOSE #caltrata
CLS
OPEN "disseny\costr.txt" FOR INPUT AS #ccosdeli
dtcrea "costr.dat", LEN(costr(1)), ccostr
IF EOF(ccosdeli) THEN GOTO fin9
DO
INPUT #ccosdeli, costr(1).codtratami, costr(1).costeon, costr(1).importepie
dtinsertar ccostr, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccostr)
LOOP WHILE NOT (EOF(ccosdeli))
fin9: CLOSE #ccosdeli
CLOSE #ccostr
CLS
OPEN "disseny\amaquina.txt" FOR INPUT AS #ccosdeli
dtcrea "amaquina.dat", LEN(amaquina(1)), camaquina
IF EOF(ccosdeli) THEN GOTO fin10
DO
INPUT #ccosdeli, amaquina(1).codmaquina, amaquina(1).descripcio
INPUT #ccosdeli, amaquina(1).preciohora, amaquina(1).preciomaqu
dtinsertar camaquina, 1
LOCATE 1, 1: PRINT dtobtregistros&(camaquina)
LOOP WHILE NOT (EOF(ccosdeli))
fin10: CLOSE #ccosdeli
CLOSE #camaquina
CLS
OPEN "disseny\aoperari.txt" FOR INPUT AS #ccosdeli
dtcrea "aoperari.dat", LEN(aoperari(1)), caoperari
IF EOF(ccosdeli) THEN GOTO fin11
DO
INPUT #ccosdeli, aoperari(1).codoperari, aoperari(1).descripcio, aoperari(1).preciohora
INPUT #ccosdeli, aoperari(1).costoanual
dtinsertar caoperari, 1
LOCATE 1, 1: PRINT dtobtregistros&(caoperari)
LOOP WHILE NOT (EOF(ccosdeli))
fin11: CLOSE #ccosdeli
CLOSE #caoperari
CLS
OPEN "disseny\costomec.txt" FOR INPUT AS #ccosdeli
dtcrea "costomec.dat", LEN(costomec(1)), ccostomec
IF EOF(ccosdeli) THEN GOTO fin12
DO
INPUT #ccosdeli, costomec(1).costeon, costomec(1).codmaquina
INPUT #ccosdeli, costomec(1).golpeminut, costomec(1).tiempoprep
INPUT #ccosdeli, costomec(1).poroperari
dtinsertar ccostomec, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccostomec)
LOOP WHILE NOT (EOF(ccosdeli))
fin12: CLOSE #ccosdeli
CLOSE #ccostomec
CLS
OPEN "disseny\ataller.txt" FOR INPUT AS #ccosdeli
dtcrea "ataller.dat", LEN(ataller(1)), cataller
IF EOF(ccosdeli) THEN GOTO fin13
DO
INPUT #ccosdeli, ataller(1).codmaquina, ataller(1).descripcio
INPUT #ccosdeli, ataller(1).preciohora, ataller(1).preciomaqu
dtinsertar cataller, 1
LOCATE 1, 1: PRINT dtobtregistros&(cataller)
LOOP WHILE NOT (EOF(ccosdeli))
fin13: CLOSE #ccosdeli
CLOSE #cataller
CLS
OPEN "disseny\costotal.txt" FOR INPUT AS #ccosdeli
dtcrea "costotal.dat", LEN(costotal(1)), ccostotal
IF EOF(ccosdeli) THEN GOTO fin14
DO
INPUT #ccosdeli, costotal(1).costeon, costotal(1).codmaquina
INPUT #ccosdeli, costotal(1).horas
dtinsertar ccostotal, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccostotal)
LOOP WHILE NOT (EOF(ccosdeli))
fin14: CLOSE #ccosdeli
CLOSE #ccostotal
CLS
OPEN "disseny\autiles.txt" FOR INPUT AS #ccosdeli
dtcrea "autiles.dat", LEN(autiles(1)), cautiles
IF EOF(ccosdeli) THEN GOTO fin15
DO
INPUT #ccosdeli, autiles(1).codutil, autiles(1).descripcio
INPUT #ccosdeli, autiles(1).precio
dtinsertar cautiles, 1
LOCATE 1, 1: PRINT dtobtregistros&(cautiles)
LOOP WHILE NOT (EOF(ccosdeli))
fin15: CLOSE #ccosdeli
CLOSE #cautiles
CLS
OPEN "disseny\costouti.txt" FOR INPUT AS #ccosdeli
dtcrea "costouti.dat", LEN(costouti(1)), ccostouti
IF EOF(ccosdeli) THEN GOTO fin16
DO
INPUT #ccosdeli, costouti(1).costeon, costouti(1).descripcio
INPUT #ccosdeli, costouti(1).importutil, costouti(1).importmant
INPUT #ccosdeli, costouti(1).importcali, costouti(1).totalutile, costouti(1).maquinabil
INPUT #ccosdeli, costouti(1).ncarros, costouti(1).imputilmaq
INPUT #ccosdeli, costouti(1).impmatrmaq
INPUT #ccosdeli, costouti(1).notas
dtinsertar ccostouti, 1
LOCATE 1, 1: PRINT dtobtregistros&(ccostouti)
LOOP WHILE NOT (EOF(ccosdeli))
fin16: CLOSE #ccosdeli
CLOSE #ccostouti
CLOSE #ccosidx

