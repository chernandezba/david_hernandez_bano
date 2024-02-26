//fitxer servidor.h
#ifndef __SERVIDOR_H
#define __SERVIDOR_H


//defines
#ifdef PROVES
    //si esta definit PROVES, es defineix el nom de fitxer de configuracio, el de log
    //i el port per defecte del servidor perque es pugui provar amb un compte d'usuari
    //normal
    #define NOM_FITXER_CFG      "/tmp/proxy_filtre.cfg"    //nom del fitxer de configuracio
    #define NOM_FITXER_LOG      "/tmp/proxy_filtre.log" //nom del fitxer log
#else
    #define NOM_FITXER_CFG      "/etc/proxy_filtre.cfg"    //nom del fitxer de configuracio
    #define NOM_FITXER_LOG      "/var/log/proxy_filtre.log" //nom del fitxer log
#endif
#define NOM_SOCKET_LOCAL    "/tmp/proxy_filtre.socket" //nom del socket local

//codis d'error que retorna la funció processa_peticio

#define PROCESA_ERR_NO_LIN                    -1
#define PROCESA_ERR_METODE_DESCONEGUT         -2 //mètode desconegut, no es ni GET, PUT o HEAD
#define PROCESA_ERR_NO_URI                    -3 //no hi ha URI
#define PROCESA_ERR_PROTOCOL_NO_SOPORTAT      -4 //protocol no soportat, no es http://
#define PROCESA_ERR_NO_URL                    -5 //no hi ha cap URL especificada
#define PROCESA_ERR_VERSIO_HTTP_NO_SUPORTAT   -6 //versió d'http no suportada
#define PROCESA_ERR_FORMAT_VERSIO_HTTP_ERRONI -7 //format versió d'http incorrecte
#define PROCESA_ERR_HEAD_A_PARTIR_HTTP_10     -8 //HEAD només a partir d'HTTP/1.0
//codis resposta HTTP/1.0 que usarà el proxy en les respostes
#define HTTP_PETICIO_ERRONIA        400 //petició al proxy erronia (mètode erroni,protocol erroni)
#define HTTP_PROHIBIT               403 //accés prohibit al client de destí, el client web
#define HTTP_LONGITUD_REQUERIDA     411 //s'ha d'especificar la longitud


#define HTTP_OK                     200 //no n'hi ha hagut error
#define	HTTP_ERROR_INTERN_SERVIDOR  	500 //n'hi ha hagut error intern del servidor
#define HTTP_OPERACIO_NO_IMPLEMENTADA   501 //s'ha especificat un mètode diferent de GET,
//PUT o HEAD
#define HTTP_SERVEI_NO_DISPONIBLE   503 //el retorna quan n'hi ha hagut error de dns

//cadenes per al fitxer log,per a la funcio escriu_fitxer_log
#define URLOK       "URLOK" //ha passat el filtre correctament
#define URLBLOQ     "URLBLOQ"   //url no ha passat el filtre, apareix a la llista
#define WEBBLOQ     "WEBBLOQ"   //plana no ha passat el filtre, la part de la plana en la URL apareix en la llista
#define PARAULABLOQ "PARAULABLOQ"   //pagina html no ha passat el filtre, apareix una paraula
//que està en la llista de paraules

//camps peticio HTTP
#define HTTP_CAMP_CONTENT_LENGTH    "Content-Length: " //especifica longitud peticio
#define HTTP_CAMP_CONTENT_TYPE      "Content-Type: " //especifica la longitud del contingut
#define HTTP_CAMP_CONNECTION        "Connection: " //especifica si es una connexio persistent
#define HTTP_CAMP_PROXY_CONNECTION  "Proxy-Connection: " //especifica si es una connexió persistent, això ho envia Opera, almenys en versió 8 per a Linux
#define HTTP_CAMP_CONTENT_ENCODING  "Accept-Encoding: " //camp que especifica el tipus de codificació que es vol del contingut
#define HTTP_CAMP_KEEP_ALIVE        "Keep-Alive: " //camp que indica que la connexió quedi viva durant uns segons, no admés segons especificació HTTP/1.0, però alguns navegadors o programes ho envien

//camps resposta HTTP
#define HTTP_CAMP_CONTINGUT_RESPOSTA_ENCODING "Content-Encoding: " //si el servidor web ens retorna una linia amb això, significa que el contingut que ens retorna està comprimit amb gzip


//per a la connexió al socket local
#define CONTINGUT_PARAULES          "paraules" //especifica que es passaran paraules per afegir a la llista de prohibides mitjançant el socket local
#define CONTINGUT_URLS              "urls"    //especifica que es passaran urls (sense l'http://) per afegir a la llista de prohibides a traves mitjançant el socket local




#define LONG_MAX_PETICIO_HTTP       2048+7 //longitud maxima peticio http
                                          //(ex:http://www.yahoo.es)

#endif
