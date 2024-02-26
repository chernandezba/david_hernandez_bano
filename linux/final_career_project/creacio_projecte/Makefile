# Fitxer Makefile per compilar el projecte
# per executar, usar la comanda 
# make


# nom del compilador
CC      = gcc
#per poder fer debug
#DEBUG	= -g3 -ggdb3 -gstabs+
#si no volem debug,quitem # de la seguent linia i afegim # a l'inici de la linia anterior
DEBUG	=
# opcions del compilador
#si esta definit PROVES, es defineix el nom de fitxer de configuracio, el de log
#i el port per defecte del servidor perque es pugui provar amb un compte d'usuari
#normal
#afegint -DPROVES a CFLAGS es per fer proves, usa el fitxer de configuracio
#de /tmp/proxy_filtre.cfg i el de log a /tmp/proxy_filtre.log
CFLAGS      = -O -Wall -D_THREAD_SAFE -D_GNU_SOURCE $(DEBUG)
#-O s'ha possat per optimitzar el codi per velocitat
#-D_THREAD_SAFE indica que usi les version THREAD_SAFE(funcions segures per a l'ús
#de fils) de les funcions
#-D_GNU_SOURCE s'ha possat per poder usar la funcio getline
# llibreries necessitades per compilar el servidor
LIBS_SERVIDOR        = -lpthread
# objectes del servidor
OBJECTES_SERVIDOR = servidor.o llista.o llista_amb_valor.o configuracio.o utils.o linia.o filtre.o
# nom del servidor
NOM_SERVIDOR = proxy_filtre
# objectes del programa auxiliar per afegir entrades al filtre del servidor
OBJECTES_AUXILIAR_AFEGIR_AL_FILTRE = afegir.o utils.o
# nom del programa auxiliar per afegir entrades al filtre
NOM_AUXILIAR_AFEGIR_FILTRE = afegir
# objectes del programa auxiliar que mostra historial de URLs accedides correctament i les bloquejades
OBJECTES_AUXILIAR_HISTORIAL = historial.o utils.o llista_amb_valor.o
# nom del programa auxilar per mostrar historial
NOM_AUXILIAR_HISTORIAL = historial

# llista de tots els programes
PROGRAMES = $(NOM_SERVIDOR) $(NOM_AUXILIAR_AFEGIR_FILTRE) $(NOM_AUXILIAR_HISTORIAL)

# per compilar-ho tot
all: servidor auxiliar_afegir_filtre auxiliar_historial

#una petita ajuda
help:
	@echo 'Opcions disponibles    :';
	@echo 'servidor               : compila servidor proxy';
	@echo 'auxiliar_afegir_filtre : compila el programa auxiliar per afegir paraules o urls';
	@echo "auxiliar_historial     : compila el programa auxiliar per fer l'historial";
	@echo 'clean                  : esborra els fitxers objectes i els executables del directori actual'
	@echo 'all                    : compila tot'
	
# per compilar solament el servidor, la linia de $(CC) s'ha de deixar amb tabulador a l'inici
servidor: $(OBJECTES_SERVIDOR)
	$(CC) $(CFLAGS) $(OBJECTES_SERVIDOR) $(LIBS_SERVIDOR) $(DEBUG) -o $(NOM_SERVIDOR)

      
# per compilar nomes el programa auxiliar per afegir entrades al filtre, la linia de $(CC) s'ha de deixar amb tabulador a l'inici
auxiliar_afegir_filtre: $(OBJECTES_AUXILIAR_AFEGIR_AL_FILTRE)   
	$(CC) $(CFLAGS) $(OBJECTES_AUXILIAR_AFEGIR_AL_FILTRE) -o $(NOM_AUXILIAR_AFEGIR_FILTRE)
    

# per compilar solament el programa auxiliar que mostra l'historial, la linia de $(CC) s'ha de deixar amb tabulador a l'inici
auxiliar_historial: $(OBJECTES_AUXILIAR_HISTORIAL)  
	$(CC) $(CFLAGS) $(OBJECTES_AUXILIAR_HISTORIAL) -o $(NOM_AUXILIAR_HISTORIAL)

# per esborrar fitxers objecte i executables, la linia de $(CC) s'ha de deixar amb tabulador a l'inici
clean:
	rm -f *.o *.a $(PROGRAMES)
