#include <dos.h>
#include <stdlib.h>
#include <dir.h>
#include <mem.h>
#include <io.h>
#include <string.h>
#include <bios.h>
#include <pcventan.h>
#include <pcutil.h>
#include <pcdisco.h>
#include <errno.h>

#define BUFFERDIRECTORIO 1024

char directorio_buffer[BUFFERDIRECTORIO],fat[512],fatemer[512],
*bufferdirencontrado,
unidlogicas[26];
static char *erroresdisco[]={ //errores posibles en la lectura absoluta de disco
"Orden err¢nea",
"Marca de direcci¢n err¢nea",
"Sector no hallado",
"Fallo en el acceso al DMA",
"Error CRC,no se ley¢ bien el sector",
"Fallo del controlador",
"Error al intentar localizar la pista",
"No hubo conexi¢n con el dispositivo",
"Error desconocido"
};
unsigned char se0[512];
unsigned int oldtemp2,oldclusterac,oldentrada; // ROOTdir
unsigned char unidadesdisco; // _entradasdirprin , sectorcluster
static int unidactual;
int leido,leidosub,totalunidades;
static int setupdisco; //para saber si se ha mirado si la partici¢n es de mas de 32 Mb
int leidafat; //para saber si se ha leido el sector necesario de la fat
//(para cogeproxcluster fichero) as¡ tambi‚n se sabe si se ha cambiado 
//de unidad ya que entonces se leer  el sector de la fat necesario de la 
//nueva unidad
BOOLEAN leeboot;
static struct sector0 sector0actual;
struct _unidad *unidad;
//struct ffblk_ex *activoffblk_ex;
struct bios_info {
unsigned int haydrives:1; // si =1 se dispone de unidades , si =0 no se dispone
unsigned int fpu:1; // si se tiene coprocesador matem tico
unsigned int ramsize:2;
unsigned int modevideo:2;
// modo inicial : 0 = no usado , 1=40x25, 2=80x25 , 3=80x25 monocromo
unsigned int drives:2; // 0=1 unidad , 1=2 unidades , 2=3 unidades , 3=4 unidades
unsigned int dma:1; // si se tiene DMA
unsigned int comports:3; // n£mero puertos serie
unsigned int gameio:1; // si se tiene salida para joystick
unsigned int serialprint:1; // si se tiene la impresora puesta en serie
unsigned int paralports:2; // n£mero de puertos paralelos
} biosinfo;
/*------------------------------------------------------------------------*/
const unsigned int ls=BUFFERDIRECTORIO/512;
const unsigned char defaultbitsfat=12;
/*------------------------------------------------------------------------*/
static BOOLEAN near pascal busca(char *,char *,struct ffblk_ex *);
static void near pascal finbusca(struct ffblk_ex *);
static unsigned int near pascal calculaROOTdir(struct sector0 *);
static int near pascal buscarunidlogicas(char *bul);
static void   buscabiosinfo(void);
static int near pascal _absread(int drive, unsigned int nsects,
unsigned long lsect, void *buffer); //substituci¢n de absread de C para
//que pueda leer las particiones de mas de 32 Mb
/*------------------------------------------------------------------------*/
static unsigned int near pascal calculaROOTdir(struct sector0 *b){
return(b->secreservcomie+(b->copiasfat*b->sectorfat));
}
/*------------------------------------------------------------------------*/
static int near pascal _absread(int drive, unsigned int nsects,
unsigned long lsect, void *buffer) {
char info[11]; //se usa si la unidad > 32 Mb
unsigned long *temp;
char * far *temp2;
union REGS regs;
struct SREGS sregs;
//si no hay error devuelve 0 y si lo hay devuelve el c¢digo de error

if (setupdisco && unidad[drive].atribs.particion_grande==TRUE) 
//si la particion es mayor de 32 Mb 
  {
  temp=(unsigned long *)&info;
  *temp=lsect;
  ++temp;
  *(unsigned int *)temp=nsects;
  temp2=(char * far *)&info[6];
  *temp2=buffer;
  regs.x.bx=FP_OFF(&info);
  sregs.ds=FP_SEG(&info);
  regs.h.al=drive; //drive en al
  regs.x.cx=65535U;
  }
else { //para unidades <= 32 Mb o si no se ha ejecutado pcdisco_init
  regs.x.bx=FP_OFF(buffer);
  sregs.ds=FP_SEG(buffer);
  regs.x.dx=(unsigned int)lsect;
  regs.x.cx=nsects;
  regs.h.al=drive; //drive en al
  }
int86x(0x25,&regs,&regs,&sregs); //lectura absoluta de disco
if (regs.x.flags & 1) //si el carry est  alzado
  return regs.h.ah;//si hay error devuelve el c¢digo de error
else return 0; //si no,devuelve 0
}
/*-------------------------------------------------------------------------*/
void errordisco(int errornum) {
//cuando hay error en alguna rutina relacionada con :
//_absread,cogerinfodisco,buscaprimero,buscasiguiente
int i; //el indice para los mensajes de error

switch (errornum) {
	case 0x1:i=0;
				break;
	case 0x2:i=1;
				break;
	case 0x4:i=2;
				break;
	case 0x8:i=3;
				break;
	case 0x10:i=4;
				 break;
	case 0x20:i=5;
				 break;
	case 0x40:i=6;
				 break;
	case 0x80:i=7;
				 break;
	default:  i=8;
				 break;
				 }
pon_mensaje("ERROR DISCO",erroresdisco[i],0);
}
/*-------------------------------------------------------------------------*/
void pcdisco_init(void) {
register n,drivedos,resulioctl;
auto BOOLEAN cdromcont; //si esta instalado MSCDEX que servira para comprobar si
//determinada unidad es un CDROM
struct sector0 test;
auto unsigned int tipocontcdrom; //tipo de controlador de software de CDROM
//si es ADADh=MSCDEX,si es DADBh LOTUS CD/networker
char unidscdrom[255]; //para el numero de unidades cdrom
struct SREGS sregs;
union REGS regs;
BOOLEAN dvrdblspace; //si esta instalado dblspace,dvrspace


cdromcont=FALSE;
buscarunidlogicas(unidlogicas);
tipocontcdrom=simscdex();
if (tipocontcdrom==0xff) {
  tipocontcdrom=_BX;
  cdromcont=TRUE;
  setmem(unidscdrom,254,0);
  regs.x.ax=0x150d;
  regs.x.bx=FP_OFF(unidscdrom);
  sregs.es=FP_SEG(unidscdrom);
  int86x(0x2f,&regs,&regs,&sregs);
  for(n=0;n<=253 && unidscdrom[n]!=0;n++) unidscdrom[n]+='A';
  };
regs.x.ax=0x4a11;
regs.x.bx=0;
int86(0x2f,&regs,&regs);
dvrdblspace=FALSE;
if (regs.x.ax==0) dvrdblspace=TRUE;
leeboot=TRUE;
if ((unidad=calloc(unidlogicas[totalunidades-1]-'A'+1,
  sizeof(struct _unidad)))==NULL)
  ponerror(1);
setmem(unidad,(unidlogicas[totalunidades-1]-'A'+1)*sizeof(struct _unidad),0);
leidosub=0;
oldtemp2=0xFFFF;
setupdisco=0; 
leidafat=0;
for (n=0;n<totalunidades;n++) {
  drivedos=unidlogicas[n]-'A';
  if (cdromcont==TRUE && strchr(unidscdrom,unidlogicas[n])) 
	 //si es una unidad CDROM
	 unidad[drivedos].atribs.cd_rom=1;
  else {
	 if (dvrdblspace==TRUE) { 
	 //mirar si esa unidad esta comprimida
		regs.x.ax=0x4a11;
		regs.x.bx=3;
		regs.h.cl=drivedos;
		int86x(0x2f,&regs,&regs,&sregs);
		if (regs.h.cl!=0xff) unidad[drivedos].atribs.unidad_comprimida=1;
		}
	 if (unidad[drivedos].atribs.unidad_comprimida!=1)  {
		//si no es unidad comprimida
		regs.x.ax=0x4409;
		regs.h.bl=drivedos+1;
		intdos(&regs,&regs);
		if (regs.x.cflag==0 && (regs.x.dx & 0x8000/*bit 15*/)) {
		  //es una unidad de red o remota
		  unidad[drivedos].atribs.unidad_red=1;
		  }
		else {
		  resulioctl=ioctl(drivedos+1,8,0,0);
		  if (!resulioctl) {
			 if ((unidad[drivedos].subdirectorio=malloc(2048))==NULL) ponerror(1);
			 unidad[drivedos].atribs.disco_duro=FALSE;
			 }
		  else {
			 unidad[drivedos].atribs.disco_duro=TRUE;
			 cogerinfodisco(&test,drivedos);
			 if ((unidad[drivedos].subdirectorio=
			 malloc(test.sectcluster*test.bytsector))==NULL) ponerror(1);
			 if (!test.totalsec)
				if (test.grantotalsectores>65535l) 
				  unidad[drivedos].atribs.particion_grande=TRUE;
			 };
		  };
		}
	 }
  }
setupdisco=1; //para saber si se ha mirado si la particion es de mas de 32 Mb
ajventana(&ventanas[12],strlen("CAMBIAR UNIDAD")+totalunidades+2);
};
/*------------------------------------------------------------------------*/
int leesectorabs(int unid,unsigned int secleer,unsigned long seclogicinic,
char *punteroleer) {
unsigned long a_leer=((unsigned long)secleer << 9),_leido,quedan;
int er;
//si no hay error devuelve 0 y si lo hay devuelve el c¢digo de error

punteroleer=calculadireccion(punteroleer,0l); //para hacer que punteroleer
//tenga una direcci¢n equivalente con un segmento distinto y con un offset
//lo m s cercano a cero
for(_leido=0l,quedan=a_leer;_leido<a_leer;_leido+=((unsigned long)
secleer << 9),quedan-=_leido,punteroleer=calculadireccion(punteroleer,
(unsigned long)secleer << 9)) {
  if (quedan+(unsigned long)FP_OFF(punteroleer)>65535l)
	 secleer=((65535U-FP_OFF(punteroleer))+1) >> 9 ; /* >> 9 = / 512 */
  if ((er=_absread(unid,secleer,seclogicinic,punteroleer))!=0)
	 return (er);
  };
return 0;
}
/*-----------------------------------------------------------------------*/
int leefat (char *f,struct sector0 *s,int disko) {
//si no hay error devuelve 0 y si lo hay devuelve el c¢digo de error
int er;
if ((er=leesectorabs(disko,s->sectorfat,(unsigned long)
  s->secreservcomie,f))!=0)
  return er;
return 0;
}
/*-----------------------------------------------------------------------*/
unsigned int cogeproxclusterfichero(int un,unsigned int clusterfichero,
struct sector0 *u,struct ffblk_ex *sss) {
/* si es el £ltimo cluster devuelve 0xFFF si es defectuoso 0xFF7 */
/* si es una fat de 16 BITS devuelve 0xFFFF o 0xFFF7*/
unsigned long temp1;
unsigned int bytemer,temp3,temp2;
unsigned char _bitsfat=u->bitsfat,_ss *pbytemer,_ds *pfat=
(unsigned char _ds *)&fat,_ds *pfatemer=(unsigned char _ds *)&fatemer,
_ds *upfat=(unsigned char _ds *)&fat;

pbytemer=(unsigned char _ss *)&bytemer;
if (_bitsfat==12) temp1=(unsigned long)((unsigned long)clusterfichero*3) >> 1; /* *513/342 *3/2 */
if (_bitsfat==16) temp1=((unsigned long)clusterfichero << 1);/* *512/256 */
temp2=(unsigned int)((unsigned long)temp1 >> 9); /* /512 */
temp1-=((unsigned long)temp2 << 9);
if (oldtemp2!=temp2 || !leidafat) {
  leesectorabs(un,1,(unsigned long)u->secreservcomie+(unsigned long)temp2,
  fat);
  leidafat=1;
  oldtemp2=temp2;
  }
if (_bitsfat==12)  {
  if (temp1==511) {
	if (oldtemp2!=temp2)  //lee los 8 bits que quedan de los 12 bits 
	//que est  en el sector siguiente
	  leesectorabs(un,1,(unsigned long)u->secreservcomie+(unsigned long)
	  temp2+1L,fatemer);
	pbytemer[1]=*(pfat+(unsigned int)temp1);
	pbytemer[0]=*pfatemer;
 }
 else bytemer=*(unsigned int *)(upfat+(unsigned int)temp1);
  if ((sss->ff_cluster_ac & 1)==0) temp3=bytemer & 0x0fff;
	else temp3=(bytemer >> 4);
}  else{
  bytemer=*(unsigned int *)(upfat+(unsigned int)temp1);
  temp3=bytemer;};
oldtemp2=temp2;
return((unsigned int)temp3);
};
/*--------------------------------------------------------------------*/
int leecluster(int disko,unsigned int cluster,struct sector0 *s00,
char *metdat) {
//si no hay error devuelve 0 y si lo hay devuelve el c¢digo de error
int er;

if ((er=leesectorabs(disko,s00->sectcluster,((unsigned long)cluster-2)*
(unsigned long)s00->sectcluster+calculaROOTdir(s00)+
 ((unsigned long)s00->entradasdirprin >> 4),metdat)) /* >>4=*32/512 */
  !=0) return er;
return 0;
}
/*-------------------------------------------------------------------------*/
int cogerinfodisco (struct sector0 *in,int disk){
//si no hay error devuelve 0 y si lo hay devuelve el c¢digo de error
unsigned char _ds *se0p=(unsigned char _ds *)&se0;
unsigned char bits;
int er;

if ((er=leesectorabs(disk,1,0L,(char *)se0p))!=0) return er;
memcpy(in->msdos,&se0p[3],30-3);
/*
in->bytsector=*(unsigned int *)(se0p+11);
memcpy(in->msdos,&se0p[3],8);
in->sectcluster=*(unsigned char *)(se0p+13);
in->secreservcomie=*(unsigned int *) (se0p+14);
in->copiasfat=*(unsigned int *) (se0p+16);
in->entradasdirprin=*(unsigned int *) (se0p+17);
in->totalsec=*(unsigned int *) (se0p+19);
in->sectorfat=*(unsigned int *) (se0p+22);
in->sectorpista=*(unsigned int *) (se0p+24);
in->numcaras=*(unsigned int *) (se0p+26);
in->sectoresespreserv=*(unsigned int *) (se0p+28);
*/
in->sectorpista>>=8;
in->numcaras>>=8;
in->sectoresespreserv>>=8;
in->grantotalsectores=*(unsigned long *)(se0p+32);
//ver si in->totalsec no es 0
if (in->totalsec) {
 //si no lo es
 if ((in->totalsec / in->sectcluster)>(1 << 12) -1)
	//si tiene m s de 4095 clusters la fat es de 16 bits
	bits=16;
	else bits=12;
	}
 else {
 //si in->totalsec es 0 entonces en in->grantotalsectores
 //est  el n£mero de sectores
 if ((in->grantotalsectores / in->sectcluster)>(1 << 12)-1)
  //si tiene m s de 4095 clusters la fat es de 16 bits
  bits=16;
  else bits=12;
  }
in->bitsfat=bits;
return 0;
}
/*-------------------------------------------------------------------------*/
static BOOLEAN near pascal busca(char *filt,char *buffer,
struct ffblk_ex *sss) {
unsigned i1;
BOOLEAN encontrado=FALSE,coincidenombre,coincideext=FALSE;
char nombrefiltro[MAXFILE],extfiltro[MAXEXT],nombre[MAXFILE],
ext[MAXEXT],*a,*b,_ss *puntero,_ss *puntero2,byte;


if (buffer[2]==0 && buffer[4]==0 && buffer[6]==0 && buffer[8]==0 &&
buffer[10]==0) //si es una entrada de nombre largo como las de Windows 95
  //la ignora y devuelve como si no fuese el nombre que se busca
  encontrado=FALSE;
else {
  fnsplit(filt,NULL,NULL,nombrefiltro,extfiltro);
  if (*extfiltro) strcpy(extfiltro,extfiltro+1);
  memcpy(nombre,buffer,8);
  if (*buffer=='\x0') return FALSE;
  nombre[8]='\x0';
  if ((a=strchr(nombre,' '))!=NULL) *a='\x0';
  a=strchr(nombrefiltro,'*');
  b=strchr(nombrefiltro,'?');
  if (a==NULL && b==NULL)
	 coincidenombre=(strcmp(nombrefiltro,nombre)!=0) ? FALSE : TRUE;
  else {
	 for (i1=0,coincidenombre=TRUE,puntero=(char _ss *) &nombrefiltro,
	 puntero2=(char _ss *) &nombre;i1<8;i1++,puntero++,puntero2++) {
		byte=*puntero;
		if (byte=='*') break;
		if (byte=='?') continue;
		if (byte!=*puntero2) {
			coincidenombre=FALSE;
			break;
			};
		if (byte=='\x0' && *puntero2=='\x0') {
		  coincidenombre=TRUE;
		  break;
		  };
	 };
  };
  if (coincidenombre==TRUE) {
	 if (*(buffer+8)==' ' && !(*extfiltro)) coincideext=TRUE;
	 else {
		memcpy(ext,(buffer+8),3);
		ext[3]='\x0';
		if ((a=strchr(ext,' '))!=NULL) *a='\x0';
		a=strchr(extfiltro,'*');
		b=strchr(extfiltro,'?');
		if (a==NULL && b==NULL && extfiltro[0]!='\x0')
		  coincideext=(strcmp(extfiltro,ext)!=0) ? FALSE : TRUE;
		if (a!=NULL || b!=NULL) {
		  for (i1=0,puntero=(char _ss *) &extfiltro,puntero2=(char _ss *) &ext,
		  coincideext=TRUE;i1<8;i1++,puntero++,puntero2++) {
			 byte=*puntero;
			 if (byte=='*') break;
			 if (byte=='?') continue;
			 if (*puntero2!=byte) {
				 coincideext=FALSE;
				 break;
				 };
		  if (byte=='\x0' && *puntero2=='\x0') {
			 coincideext=TRUE;
			 break;
			 };
		  };
		};
	 };
  };
  if (coincideext==TRUE && coincidenombre==TRUE) encontrado=TRUE;
};
sss->ff_entrada++;
sss->entradarel++;
sss->ff_bufferdirencontrado=buffer;
return encontrado;
};
/*------------------------------------------------------------------------*/
static void near pascal finbusca(struct ffblk_ex *ss) {
char *aa,*bb,*pbufferdirencontrado=ss->ff_bufferdirencontrado,
*pname=ss->ff_name;

if (*pbufferdirencontrado==0) return;
ss->ff_attrib=*(pbufferdirencontrado+11);
/*ss->ff_ftime=*(unsigned int *) (pbufferdirencontrado+22);
ss->ff_fdate=*(unsigned int *) (pbufferdirencontrado+24);*/
memcpy(&ss->ff_ftime,(pbufferdirencontrado+22),4);
ss->ff_fsize=*(unsigned long *)(pbufferdirencontrado+28);
ss->ff_clusini=*(unsigned int *)(pbufferdirencontrado+26);
memcpy(pname,pbufferdirencontrado,8);
if (*pname!='.') {
  pname[8]='\x0';
  if ((aa=memchr(pname,' ',8))!=NULL) *aa='\x0';
  if (*(pbufferdirencontrado+8)!=' ') { // si el fichero tiene extensi¢n
	 strcat(pname,".");
	 pname[12]='\x0';
	 bb=strchr(pname,'.')+1;
	 memcpy(bb,(pbufferdirencontrado+8),3);
	 if ((aa=memchr(bb,' ',3))!=NULL)
		*aa=0;
	 if (aa==bb) *bb=0;
	 }
  }
else if ((aa=memchr(pname,' ',11))!=NULL) *aa=0;
ss->ff_bufferdirencontrado+=32;
};
/*------------------------------------------------------------------------*/
int buscaprimero (char *filtro,struct ffblk_ex *ss,char atributo,
BOOLEAN proteccion) {
/* si proteccion =FALSE se coge tambien nombre de borrados*/
//si hay m s ficheros o lo ha encontrado devuelve 0
//si no hay m s devuelve -1
//y si hay error leyendo el disco devuelve el c¢digo de error

int un=cogedisco(),res;

if (unidad[un].atribs.unidad_red==1 || unidad[un].atribs.unidad_comprimida==1
  || unidad[un].atribs.cd_rom==1) {
	 //si es una unidad de red,esta comprimida o es un cd-rom
	 //utilizar findfirst del ms-dos
	 res=findfirst(filtro,(struct ffblk *) ss,atributo);
	 if (res==-1 && _doserrno==ENMFILE/*no mas ficheros*/) return -1;
	 if (res==-1) return _doserrno;
	 return res;
	 }
 if (leeboot==TRUE) {
 cogerinfodisco (&sector0actual,un);
 leeboot=FALSE;
 }
 //_entradasdirprin=sector0actual.entradasdirprin;
 //sectorcluster=sector0actual.sectcluster;
 ss->ff_cluster_ac=unidad[un].cluster_in_dir_actual;
 ss->ff_entrada=0;
 ss->entradarel=0;
 ss->sectorrel=0;
 ss->sector_ac=calculaROOTdir(&sector0actual); //ROOTdir=activoffblk_ex=ss;
 strcpy(ss->ff_filtro,filtro);
 if (!unidad[un].cluster_in_dir_actual)  {
 leido=0;
 leidosub=0;
 ss->ff_bufferdirencontrado=(char far *)&directorio_buffer;}
 else  {
 ss->ff_bufferdirencontrado=unidad[un].subdirectorio;
 leidosub=0;
 }
 do res=buscasiguiente(ss,proteccion);
 while (((ss->ff_attrib & atributo)==0 && res==0) ||
 ss->ff_name[0]!=0 && (ss->ff_name[0]=='\xe5' && proteccion==TRUE));
 return (res);
}
/*------------------------------------------------------------------------*/
int buscasiguiente(struct ffblk_ex *ss,BOOLEAN proteccion) {
/* si proteccion =FALSE se coge tambien nombre de borrados*/
//si hay m s ficheros devuelve 0
//si no hay m s devuelve -1
//y si hay error de disco devuelve el c¢digo de error

BOOLEAN siencontrado;
int diskac=cogedisco(),er;

if (unidad[diskac].atribs.unidad_red==1 || unidad[diskac].atribs.unidad_comprimida==1
  || unidad[diskac].atribs.cd_rom==1) {
	 //si es una unidad de red,esta comprimida o es un cd-rom
	 //utilizar findfirst del ms-dos
	 er=findnext((struct ffblk *) ss);
	 if (er==-1 && _doserrno==ENMFILE/*no hay mas ficheros*/) return -1;
	 if (er==-1) return _doserrno;
	 return er;
	 }
//activoffblk_ex=ss;
siencontrado=FALSE;
if (!unidad[diskac].cluster_in_dir_actual) {
  for (;siencontrado==FALSE && ss->ff_entrada!=
	 sector0actual.entradasdirprin;){
	 if (ss->entradarel==16) {
		ss->entradarel=0;
		ss->sectorrel++;
		};
		if (ss->sectorrel==ls) {
		 ss->sector_ac+=ls;
		 ss->sectorrel=0;
		 leido=0;
		 ss->ff_bufferdirencontrado=(char far *)&directorio_buffer;};
		if (!leido) {
		 if ((er=leesectorabs(diskac,ls,ss->sector_ac,
		 (char *)&directorio_buffer))!=0)
			return er;
		 leido=1;
		 };
		siencontrado=busca(ss->ff_filtro,ss->ff_bufferdirencontrado,ss);
		if (siencontrado==TRUE && *ss->ff_bufferdirencontrado=='\xe5' &&
		proteccion==TRUE) siencontrado=FALSE;
		if (*ss->ff_bufferdirencontrado=='\x0') {
		 siencontrado=FALSE;
		 break;}
		if (siencontrado==FALSE) ss->ff_bufferdirencontrado+=32;
		};
	 }
 else {
 //  ultclusterleido=ss->ff_cluster_ac;
 do{
 if (ss->entradarel==16) {
	ss->entradarel=0;
	ss->sectorrel++;
	};
 if (ss->sectorrel==sector0actual.sectcluster) {
	  leidosub=0;
	  ss->ff_bufferdirencontrado=unidad[diskac].subdirectorio;
	  ss->sectorrel=0;
	  ss->entradarel=0;
	  ss->ff_cluster_ac=cogeproxclusterfichero(diskac,ss->ff_cluster_ac,
	  &sector0actual,ss);
	  };
 if ((ss->ff_cluster_ac!=0xFFF && sector0actual.bitsfat==12) ||
	(ss->ff_cluster_ac!=0xFFFF && sector0actual.bitsfat==16)) {
	if (!leidosub) {
	  if ((er=leecluster(diskac,ss->ff_cluster_ac,&sector0actual,
	  unidad[diskac].subdirectorio))!=0)
		 return(er);
	  leidosub=1;
	  };
  siencontrado=busca(ss->ff_filtro,ss->ff_bufferdirencontrado,ss);
  };
  if ((ss->ff_cluster_ac==0xFFF && sector0actual.bitsfat==12) ||
	 (ss->ff_cluster_ac==0xFFFF && sector0actual.bitsfat==16)) {
		siencontrado=FALSE;
		break;}
  if (siencontrado==TRUE && *ss->ff_bufferdirencontrado=='\xe5' &&
	  proteccion==TRUE) siencontrado=FALSE;
  if (*ss->ff_bufferdirencontrado==0) {
	 siencontrado=FALSE;
	 break; }
  if (siencontrado==FALSE) ss->ff_bufferdirencontrado+=32;
  }while (siencontrado==FALSE);
 };
 if (siencontrado==TRUE) finbusca(ss);
 return (siencontrado==TRUE) ? 0 : -1;
};
/*------------------------------------------------------------------------*/
int cambiadir (char *dir) { 
//en realidad no cambia el directorio , sino que actualiza las variables de 
//unidactual.diractual y la de clusini
//pero si es una unidad comprimida,de red,o un cdrom llama a la funcion DOS
//chdir
int diskac,c0,c1,er;
struct ffblk_ex bb;
char drive[MAXDRIVE],__dir[MAXDIR],file[MAXFILE],ext[MAXEXT],*punterodir;
char temp[MAXPATH+1];
//si no hay error devuelve 0
//y si lo hay devuelve el c¢digo de error

strupr(dir);
leidosub=0;
fnsplit(dir,drive,__dir,file,ext);
c0=strcmp(dir,"\\");
c1=strcmp(dir,"..");
if (drive[0]!='\x0') {
  diskac=drive[0]-'A';
  leido=0;
  } else diskac=cogedisco();
if (unidad[diskac].atribs.unidad_red==1 || unidad[diskac].atribs.unidad_comprimida==1
  || unidad[diskac].atribs.cd_rom==1) {
  //si es una unidad de red,comprimida o es un cdrom
  //utiliza la funcion del DOS normal
  er=chdir(dir);
  getcwd(temp,MAXPATH);
  strcpy(unidad[diskac].diractual_disco,&temp[2]); //para quitar [unidad]: que devuelve getcwd
  if (c0==0) unidad[diskac].diractual_disco[0]=0;
  return er;
  }
if (c0==0) {
  unidad[diskac].cluster_in_dir_actual=0;
  unidad[diskac].diractual_disco[0]='\x0';
  };
if (c1==0) {
  if ((er=buscaprimero("*.*",&bb,FA_DIREC,TRUE))!=0) return er;
  if ((er=buscasiguiente(&bb,TRUE))!=0) return(er);
  unidad[diskac].cluster_in_dir_actual=bb.ff_clusini;
  punterodir=strchr(unidad[diskac].diractual_disco,0);
  while(*punterodir--!='\\');
  *(punterodir+1)=0;
  };
if (c0!=0 && c1!=0) {
  if ((er=buscaprimero(dir,&bb,FA_DIREC,TRUE))!=0) return(er);
  unidad[diskac].cluster_in_dir_actual=bb.ff_clusini;
  strcat(unidad[diskac].diractual_disco,"\\");
  strcat(unidad[diskac].diractual_disco,bb.ff_name);
  };
return(0);
};
/*------------------------------------------------------------------------*/
int cambiadisco(int disk) {
int res=setdisk(disk);

unidactual=getdisk();
leido=0;
leidosub=0;
leidafat=0;
leeboot=TRUE;
return (res);
};
/*-----------------------------------------------------------------------*/
int cogedisco (void) {
return(unidactual);
};
/*-----------------------------------------------------------------------*/
void cogerdiractual(char *dir,int numbytes) {
int diskac=cogedisco();

if (unidad[diskac].diractual_disco[0]=='\x0') strcpy(dir,"\\");
else memcpy(dir,unidad[diskac].diractual_disco,numbytes);
dir[numbytes]=0;
};
/*------------------------------------------------------------------------*/
int buscadir(struct ffblk_ex *aa) {
  int d=0;

//si hay m s directorios devuelve 0
//si no hay m s directorios devuelve -1
//si hay error leyendo el disco devuelve el c¢digo de error

if ((aa->ff_attrib & FA_DIREC)!=0 && aa->ff_name[0]!='.') return 0;
do {
 d=buscasiguiente(aa,TRUE);
  if (((aa->ff_attrib & FA_DIREC)==0) || aa->ff_name[0]=='.') {
	 if (aa->ff_name[0]=='.' && d==0) d=buscasiguiente(aa,TRUE);
	 if (aa->ff_name[0]=='.' && d==0) d=buscasiguiente(aa,TRUE);
	}
  }while(d==0 && ((aa->ff_attrib & FA_DIREC)==0));
  return d;
  }
/*------------------------------------------------------------------------*/
static void buscabiosinfo(void) {
int ax2;
ax2=biosequip();
*(unsigned int *)&biosinfo=ax2;
}
/*------------------------------------------------------------------------*/
static near int pascal buscarunidlogicas(char *bul){
register diski,antdisk=cogedisco();
for (diski=totalunidades=0;diski<26;++diski)
  { cambiadisco(diski);
	 if (diski==cogedisco())
		if (!(ioctl(diski+1,9,0,0) & 4096)) { //4096,bit 12
		totalunidades++;
		*bul++=diski+'A';
	 }
  }
*bul=0;
cambiadisco(antdisk);
buscabiosinfo(); // guarda el resultado en la estructura biosinfo
if (biosinfo.drives==0 && biosinfo.haydrives) { // si solo hay una unidad
  strcpy(unidlogicas+1,unidlogicas+2); //quitar de unidlogicas la letra B
  //ya que la funci¢n que cambia de unidad no verifica si existe la unidad B
  totalunidades--;
  }
return(totalunidades);
}