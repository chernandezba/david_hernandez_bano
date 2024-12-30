#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <pcventan.h>
#include <pcutil.h>
#include <pcdisco.h>


void preparanuevoarbol(void);
/*--------------------------------------------------------------------*/
void cambunidproc(void){
char buf_unid[80];
unsigned char _x,_xrel,xsal,anc,anc2;
int teclapul;
extern int ventanaactual; 
//para después poder activar la ventana anterior al actualizar el nombre del volumen de la unidad actual

teclapul=0;
if (hacerven(&ventanas[12],colorf2(CAMBUNIDA))==-1)
  ponerror(1);
sprintf(buf_unid,"UNIDADES:%s",unidlogicas);
centrar(2,0,buf_unid);
anc=((ventana_ancho()-strlen(buf_unid)+1) /2);
anc2=strlen("UNIDADES:");
xsal=anc+anc2;//posici¢n dentro de la ventana de la primera letra de la unidad
_x=xsal+(unsigned char)(strchr(unidlogicas,'A')-
(char *)&unidlogicas); //posici¢n de la letra de la unidad actual
_xrel=_x-xsal;
writeatrib(_x,2,1,colorf2(SELUNI));
teclapul=0;
while (teclapul!=ENTER && teclapul!=ESC) {
  teclapul=tecla();
  if (bufteclado==0x4D00) //flecha derecha del teclado num‚rico
         {
          writeatrib(_x,2,1,colorf2(CAMBUNIDA));
          if (_xrel!=totalunidades) {
          _x++;
          _xrel++;
          }
          else {
          _x=xsal;
          _xrel=0;        };
        };
  if (bufteclado==0X4B00)  //fecha izquierda del teclado num‚rico
          {
          writeatrib(_x,2,1,colorf2(CAMBUNIDA));
          if (_xrel) {
                 _x--;
                 _xrel--;}
                 else {
                _x=xsal;
                _xrel=0;
                };
			 };
  writeatrib(_x,2,1,colorf2(SELUNI));
  }
cerrarven(&ventanas[12]);
if (teclapul!=ESC) {
		  cambiadisco(unidlogicas[_xrel]-'A');
		  cambiadir("\\");
		  strcpy(diractual,"\\");
		  preparanuevoarbol();
		  buscararbol(cogedisco());
		  arbolptr=arbol;
		  hacerven(&ventanas[6],colorf2(ARBOLV));
		  ponerarbol(0,arbol);
		  poner_etiqueta_disco();
		  mostrardirectorio(cogedisco());
		  escribedirectorio(diractual);
		  cambiarven(&ventanas[6]);
		  sel_dir(colorf2(DIRSEL));
		  seleccionaventana(&ventanas[8],colorf2(MARCO));
		  seleccionaventana(&ventanas[6],colorf2(VENTSEL));
		  ventanaactual=6;
		  }
};

/*-------------------------------------------------------------------------*/
void poninfodisco(int uni) {
char info[80];
struct dfree libred;
unsigned long libredi,capacidad;
struct sector0 capacid;
int er;

if (hacerven(&ventanas[16],colorf2(INFODISCO))==-1)
  ponerror(1);
centrar(2,0,"LEYENDO INFORMACION");
getdfree(uni+1,&libred);
cerrarven(&ventanas[16]);
if (libred.df_sclus==0xffff) {
  pon_mensaje("ERROR DE DISCO","ERROR LEYENDO EL DISCO",1);
  return ;
  }
if (unidad[uni].atribs.unidad_red==0 && unidad[uni].atribs.unidad_comprimida==0
&& unidad[uni].atribs.cd_rom==0) {
//si no es una unidad comprimida,de red o es un cd-rom
  if ((er=cogerinfodisco(&capacid,uni))!=0) {
	  errordisco(er);
	  return;
	  }
  }
if (hacerven(&ventanas[13],colorf2(INFODISCO))==-1) {
			 ponerror(1);
			 return;
			 }
centrar(2,0,"ESPACIO LIBRE");
libredi =  (unsigned long) libred.df_avail *(unsigned long) libred.df_bsec *
(unsigned long) libred.df_sclus;
sprintf(info,"%lu",libredi);
centrar(3,0,info);
centrar(4,0,"CAPACIDAD DISCO");
if (unidad[uni].atribs.unidad_red==0 && unidad[uni].atribs.unidad_comprimida==0
&& unidad[uni].atribs.cd_rom==0) {
 capacidad=(unsigned long)libred.df_total *
 (unsigned long)libred.df_bsec;
  capacidad=(unsigned long)((capacid.totalsec) ?
  (unsigned long)capacid.totalsec : capacid.grantotalsectores);
  capacidad-=(unsigned long)((((unsigned long)capacid.sectorfat*
  capacid.copiasfat)+(unsigned long)capacid.sectoresespreserv+
  (unsigned long)capacid.secreservcomie)*
  (unsigned long)capacid.bytsector)+(capacid.entradasdirprin*
  (unsigned long)32);
  }
else {
 capacidad=(unsigned long)libred.df_total *
 (unsigned long)libred.df_bsec*(unsigned long)libred.df_sclus;
 }
if (capacidad > 5l*1024l*1024l) //si es un disco de mas de 5 Mb
//imprimir su capacidad en MB
sprintf(info,"%u MB",(unsigned int)((capacidad >> 10)>> 10)); // >> 10 =/1024
//si no imprimirla en Kb
else sprintf(info,"%u KB",(unsigned int)(capacidad >> 10));
centrar(5,0,info);
if (unidad[uni].atribs.unidad_red==0 && unidad[uni].atribs.unidad_comprimida==0
&& unidad[uni].atribs.cd_rom==0) {
  centrar(6,0,"BYTES POR CLUSTER");
  sprintf(info,"%u",(unsigned int)capacid.sectcluster*(unsigned int)
  capacid.bytsector);
  centrar(7,0,info);
  centrar(8,0,"SECTORES POR PISTA");
  sprintf(info,"%u",(unsigned int)capacid.sectorpista);
  centrar(9,0,info);
  centrar(10,0,"NUMERO DE CARAS");
  sprintf(info,"%u",(unsigned int)capacid.numcaras);
  centrar(11,0,info);
  centrar(12,0,"TIPO FAT");
  sprintf(info,"%uBIT",capacid.bitsfat);
  centrar(13,0,info);
  centrar(14,0,"SISTEMA DISCO");
  centrar(15,0,capacid.msdos);
  centrar(18,0,"PULSE TECLA");
  }
else centrar (7,0,"PULSE TECLA");
tecla();
cerrarven(&ventanas[13]);
}
/*------------------------------------------------------------------------*/
void pfun_arbol(void) {

if (hacerven(&ventanas[17],colorf2(ARBOLV))==-1)
  ponerror(1);
ponerarbol(0,arbol);
tecla();
borra_tec_buffer();
cerrarven(&ventanas[17]);
}
/*------------------------------------------------------------------------*/
