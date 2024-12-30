#ifndef __PCDISCO.H__
#define __PCDISCO.H__

/*------------------------------------------------------------------*/
#ifndef __PCUTIL.H__
typedef enum boolean BOOLEAN;
enum boolean {FALSE=0,TRUE};
#endif
/*------------------------------------------------------------------*/
struct sector0 {
char msdos[8];
unsigned int bytsector;
unsigned char sectcluster;
unsigned int secreservcomie;
unsigned char copiasfat;
unsigned int entradasdirprin,totalsec;
unsigned char idfat;
unsigned char sectorfat;
unsigned int  sectorpista,numcaras,sectoresespreserv;
unsigned char bitsfat;
unsigned long grantotalsectores;
};
/*------------------------------------------------------------------*/
struct ffblk_ex {
   //lo primero es lo mismo que la estructura ffblk para el findfirst
	char     ff_reserved[21];
	char		ff_attrib;
	unsigned	int ff_ftime;
	unsigned	int ff_fdate;
	unsigned long	ff_fsize;
	char		ff_name[13];
	//a partir de aqui solo lo utilizan las rutinas de pcdisco.c
	//y solo son para discos no comprimidos,ni cd-roms,ni unidades
	//de red
	unsigned int ff_clusini;
	unsigned char entradarel;
	unsigned char sectorrel;
	unsigned int ff_entrada;
	char	 	*ff_bufferdirencontrado;
	char 		ff_filtro[13];
	unsigned int ff_cluster_ac;
	unsigned long sector_ac;
	};
/*------------------------------------------------------------------*/
struct _unidad {
char diractual_disco[MAXPATH+1];
unsigned int cluster_in_dir_actual;
char *subdirectorio;
struct atribs {
//atributos de la unidad
//si valen 1 es que se cumple,si valen 0 es que no se cumplen
unsigned int disco_duro:1;
unsigned int particion_grande:1;
unsigned int unidad_red:1;
unsigned int unidad_comprimida:1; //si es unidad comprimida con STACKER,DOUBLESPACE,...
unsigned int cd_rom:1;
unsigned int noaccesodirecto:1; //si =1 no se accede al disco
//directamente (con int 25h/26h) si no que se accede indirectamente
} atribs;
};
/*------------------------------------------------------------------*/
int cogerinfodisco(struct sector0 *,int);
int leesectorabs(int,unsigned int,unsigned long,char *);
int leefat (char *,struct sector0 *,int);
int leecluster(int,unsigned int ,struct sector0 *,char *);
unsigned int cogeproxclusterfichero(int ,unsigned int ,struct sector0 *,struct ffblk_ex *);
int cambiadir (char *);
int buscaprimero (char *,struct ffblk_ex *,char,BOOLEAN);
int buscasiguiente(struct ffblk_ex *,BOOLEAN);
void pcdisco_init(void);
int cambiadisco(int);
int cogedisco (void);
void cogerdiractual(char *,int);
int buscadir(struct ffblk_ex *aa);
void errordisco(int errornum);
unsigned char simscdex(void);/*si esta instalado MSCDEX o LOTUS CD/networker 
retorna  FFh , y si retorna FFh si BX=ADADh MSCDEX esta instalado
BX=DADBh LOTUS CD/networker esta instalado*/
/*------------------------------------------------------------------*/
extern int totalunidades;
/*------------------------------------------------------------------*/
extern unsigned char unidadesdisco;
/*------------------------------------------------------------------*/
extern char unidlogicas[];
/*------------------------------------------------------------------*/
extern struct _unidad *unidad;
/*------------------------------------------------------------------*/
#endif