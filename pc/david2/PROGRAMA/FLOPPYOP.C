//para el controlador NEC PD765
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <bios.h>
#include <ctype.h>
//miscel neo

#define BYTE		   	255 //m scara de 8 bits,en octal es 0377
#define TIMER0				0x40 //direcci¢n del puerto del canal 0 del cron¢metro
#define TIMER_MODE		0x43 //direcci¢n del puerto del canal 3 del cron¢metro
#define IBM_FREQ			1193182L //frecuencia del reloj de IBM para fijar el cron¢metro
#define SQUARE_WAVE		0x36 //modo para generar ondas cuadradas
#define OK					0 //si algo ha ido bien
#define EI0					-11 //si algo va mal
#define STOP_MOTOR		3*HZ //tiempo para pararse el motor en clicks de reloj
#define TRUE				1 //verdadero
#define FALSE				0 //falso
#define esperaintdisco  while (!interrupcion_disco)

//puertos de E/S para el disco flexible

#define DOR					0x3f2 // bits de control de la unidad motora
#define FDC_STATUS		0x3f4 //registro de condici¢n o estado del controlador
									//del disco flexible
#define FDC_DATA			0x3f5 //registro de datos del controlador del disco flexible
#define FDC_DENSITY     0x3f7 //selecciona densidad del diskette
//si se envia 0 a ese puerto especifica diskette introducido de 1200/1440 Kb
//si se envia 1 a ese puerto especifica diskette introducido de 360 Kb
//si se envia 2 a ese puerto especifica diskette introducido de 720 Kb
#define DMA_ADDR			0x004 //puerto de 16 bits bajos de la direcci¢n DMA
#define DMA_TOP			0x081 //puerto de 4 bits altos de la direcci¢n DMA de 20 bits
#define DMA_COUNT			0x005 //puerto del conteo de DMA (conteo=bytes-1)
#define DMA_M2				0x00c //puerto de condici¢n de DMA
#define DMA_M1				0x00b //puerto de condici¢n de DMA
#define DMA_INIT			0x00a	//puerto de inicializaci¢n de DMA

//registros de condici¢n producidos como resultado de la operaci¢n
#define ST0					0x00 //registro de condici¢n 0
#define ST1					0x01 //registro de condici¢n 1
#define ST2					0x02 //registro de condici¢n 2
#define ST3					0x00 //registro de condici¢n 3 (producido por DRIVE SENSE)
#define ST_CYL				0x03 //ranura donde el controlador informa al cilindro
#define ST_HEAD			0x04 //ranura donde el controlador informa a la cabeza
#define ST_SEC				0x05 //ranura donde el controlador informa al sector
#define ST_PCN				0x01 //ranura donde el controlador informa al cilindro presente

//campos contenidos en los puertos de E/S
#define MASTER				0x80 //se usa para ver quien es el amo
#define DIRECTION		   0x40 //¨est  el FDC(controlador del disco) intentando leer o escribir?
#define CTL_BUSY			0x10 //se usa para ver si el controlador est  ocupado
#define CTL_ACCEPTING 	0x80 //FDC da estos bits cuando est  ocioso
#define MOTOR_MASK 		0xF0 //bits que controlan los motores en DOR
#define ENABLE_INT		0x0C //se usa para fijar el puerto DOR
#define ST0_BITS			0xF8 //verificar los 5 bits superiores para buscar la condici¢n
#define ST3_FAULT			0x80 //si se coloca este bit,la unidad se paraliza
#define ST3_WR_PROTECT 	0x40 //fijar si el diskette est  protegida contra escritura
#define ST3_READY			0x20 //cuando la unidad est  lista
#define TRANS_ST0			0x00 //5 bits superiores de ST0 para READ/WRITE
#define SEEK_ST0			0x20 //5 bits superiores de ST0 para SEEK
#define BAD_SECTOR		0x05 //si estos bits se colocan en ST1,recalibrar
#define BAD_CYL			0x1f //si se coloca alguno de estos bits,recalibrar
#define WRITE_PROTECT 	0x02 //el bit se coloca si el disquette est  protegido
									//contra escritura
#define CHANGE				0xc0 //valor producido por FDC despu‚s de la recolocaci¢n

//bytes de comando del controlador del disco flexible
#define FDC_SEEK			0x0f //posicionar cabezal en cilindro
#define FDC_READ			0xe6 //realizar lectura
#define FDC_WRITE			0xc5 //realizar escritura
#define FDC_SENSE			0x08 //ordenar al controlador que se¤ale su estado
#define FDC_RECALIBRATE	0x07 //recalibrar unidad,dirigirla al cilindro 0
#define FDC_SPECIFY 		0x03 //ordenar a la unidad que accepte par metros,configurar controlador (defirle SPEC1,SPEC2(tiempos de carga de cabeza,etc))

//comandos de canales DMA
#define DMA_READ			0x46 //codigo de operaci¢n de lectura DMA
#define DMA_WRITE			0x4a //c¢digo de operaci¢n de escritura DMA

//par metros de la unidad de disco
#define SECTOR_SIZE		512 //tama¤o del sector
int densidad_act=0; //densidad por defecto del disco introducido 1220/1440 Kb
//tiene que ver con FDC_DENSITY
int	  HD525			=	0; //si =1 entonces es una unidad de 5 1/4 de alta densidad
int	  NR_SECTORS	=	9; //sectores por pista,variable(este valor es de un 
								  //disquette de 5 1/4 o de 3 1/2)
#define NR_HEADS			2 //caras
int     GAP				=  0x2a;//tama¤o de espacio entre sectores,variable,si 5 1/4 =2ah,si 3 1/2 =1bh
#define DTL					0xff //determina la extensi¢n de los datos (tama¤o del sector)
#define SPEC1				0xdf //raz¢n de avance y tiempo de descarga de la cabeza (vector 1eh,offset 0)
#define SPEC2				0x02 //tiempo de carga de la cabeza y no dma (vector 1eh,offset 1)

#define HZ					20 //frecuencia de reloj
#define MOTOR_OFF			3*HZ //tiempo para que se pare el motor , en pulsos de reloj
#define MOTOR_START		HZ/2 //tiempo en arrancar el motor

//c¢digos de error
#define ERR_SEEK			 -1 //b£squeda err¢nea
#define ERR_TRANSFER		 -2 //transferencia err¢nea
#define ERR_STATUS		 -3 //error al determinar el estado
#define ERR_RECALIBRATE  -4 //error al recalibrar
#define ERR_WR_PROTECT   -5 //disquette protegido contra escritura
#define ERR_DRIVE        -6 //algo anda mal con la unidad
#define ERR_RESET			 -7 //no puede resetear controlador
#define ERR_GENERAL		 -8 //error general
#define ERR_TIMEOUT		 -9 //fuera de tiempo
#define OVERFLOWDMA      -10 //overflow de dma

//miscel neo2
#define MAX_ERRORS		3 //m ximo de errores en lectura/escritura
#define MAX_RESULTS		8 //m ximo de bytes que produce el controlador
#define NR_DRIVES			2 //m ximo n£mero de unidades
#define DIVISOR			128 //se utiliza para la codificaci¢n del tama¤o del sector
#define MAX_FDC_RETRY	100 //n£mero m ximo de veces que se intenta mandar la
									 //salida al FDC(controlador de disco)
#define DISK_READ			1
#define DISK_WRITE		2
int     NR_BLOCKS=720;      //n£mero de bloques=pistas*caras*sectores por 
									 //pista,variable,seg£n disquette (este valor 
									 //es de un disquette de 5 1/4)


//variables

struct _floppy {
int fl_opcode; //DISK_READ o DISK_WRITE
int fl_curcyl; //cilindro actual
int fl_drive;  //unidad
int fl_cilindro_real; //cilindro real
int fl_cara_real; //cara real
int fl_cylinder; //cilindro(de la cabecera)
int fl_sector; //sector(de la cabecera)
int fl_ultimo_sector; //n£mero del £ltimo sector(cabecera)
int fl_head; //cara(de la cabecera)
int fl_codigolong; //codigo de la longitud del sector(0=128,1=256,2=512,3=1024,..,ff=de 0 a 255)
int fl_count; //n£mero de bytes
char far *fl_address; //donde almacenar
char fl_results[MAX_RESULTS];
char fl_calibration; //CALIBRATED o UNCALIBRATED
} floppy[NR_DRIVES];



#define UNCALIBRATED		0 //la unidad se necesita calibrar en el siguiente uso
#define CALIBRATED		1 //no se necesita calibrar
int motor_status; //la condici¢n del motor actual est  en 4 bits altos
int motor_goal; //la condici¢n del motor deseado est  en 4 bits altos
int motor_prev; //motor que se encendi¢ antes
int need_reset; //=1 si el controlador necesita recalibrarse
int initialized=0; //=1 despu‚s de la transferencia ¢ptima
int steps_per_cyl = 1; //n£mero de pulsos para avanzar por cilindro,variable
							  //seg£n la unidad(su densidad,si es de alta o baja densidad)
char len[]= {-1,0,1,-1,2,-1,-1,3,-1,-1,-1,-1,-1,-1,-1,4};
char interleave[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
unsigned int veces;
unsigned char columnas_pantalla; //para writechar 
unsigned char filas_pantalla; //para writechar
int parar_motor; //para que se pueda apagar el motor
int counter; //contador del reloj
int clicks; //n£mero de pulsos de reloj
int interrupcion_disco; //=1 si se ha hecho interrupci¢n 0eh
int densidad_dd;//densidad de la unidad en modo baja_densidad(tiene que ver con FDC_DENSITY)
int cambiada_densidad=0; //si se ha cambiado la densidad de operar la unidad
int alta_densidad=0; //si la unidad esta en modo de alta densidad
int densidad_unidad_defecto[4]; 
//densidades por defecto de la unidad(tiene que ver con FDC_DENSITY)
//son las densidades maximas de la unidad
//ej:si una unidad es de doble densidad no puede leer discos
//de alta densidad
//se necesita para poder funcionar bien la rutina de acceso
//a la controladora
void interrupt (*oldint0e)(void);
void interrupt (*oldint08)(void);
char *errores_disco[]={
"",//no error
"Error de b£squeda",//err_seek
"Transferencia err¢nea",//err_transfer
"Error al determinar el estado",//err_status
"Error al recalibrar",//err_recalibrate
"Disco protegido contra escritura", //err_wr_protect
"Error de la unidad",//err_drive
"No puedo resetear controlador",//err_reset
"Error general",//err_general
"No hay disco insertado",//err_timeout
"DMA overflow",//overflowdma
};

//declaracion de funciones
void interrupt int08(void);
void interrupt int0e(void);
int do_rdwt(int drive);
void init_clock(unsigned long hz);
int dma_setup(struct _floppy *fp);
void start_motor(struct _floppy * fp);
void stop_motor(void);
int seek(struct _floppy * fp);
int transfer(struct _floppy * fp);
int fdc_results(struct _floppy *fp);
void fdc_out (int val);
int recalibrate(struct _floppy *fp);
int reset(void);



//funciones

void init_clock(unsigned long hz) {
//inicializar el canal 2 del cron¢metro 8253A seg£n hz
unsigned int count,low_byte,high_byte;

count=(unsigned) (IBM_FREQ/hz); 
low_byte=count & BYTE;
high_byte=(count >> 8) & BYTE;
outportb(TIMER_MODE,SQUARE_WAVE);
outportb(TIMER0,low_byte);
outportb(TIMER0,high_byte);
}

//------------------------------------------------------------------
int do_rdwt(int drive)  //leer o escribir sector/es
{
int r,errors;
struct _floppy *fp;

parar_motor=0; //para que no se apague el motor mientras se realiza una operaci¢n de lectura/escritura
errors=0;
fp=&floppy[drive];


//ciclo que permite que se repita una operaci¢n que fall¢
while(errors <= MAX_ERRORS) {
//si ocurren muchos errores cuando inicializado es 0 , probablemente
//signifique que se est  ensayando en la densidad err¢nea.Probar con otra
//incrementar errores aqu¡ porque el ciclo se aborta al hacer un error

errors++;
if (errors==MAX_ERRORS) {

//probar con otra densidad
if (densidad_unidad_defecto[drive]!=0) {
  //si la diskettera no es de alta densidad
  //no podra leer discos de otra densidad
  //entonces el error es de transferencia o
  //es otro error
  break;
  }
  if (cambiada_densidad==1) {
	 //si insiste el error aun cambiada la densidad
	 //salir
	 break;
	 }
  errors=0;
  if (alta_densidad==1) {
	 alta_densidad=0;
	 densidad_act=densidad_dd;
	 cambiada_densidad=1;
	 }
  else {
  alta_densidad=1;
  densidad_act=0;
  cambiada_densidad=1;
  }
  outportb(FDC_DENSITY,densidad_act);
}
if (initialized == 0 && errors == MAX_ERRORS && fp->fl_cilindro_real>0) {
  
  /*if (steps_per_cyl > 2)  {
	 steps_per_cyl=1; //poner steps_per_cyl a 1
	 return ERR_GENERAL;
	 }
  else {
	if (HD525==0) { //si la unidad no es de 5 1/4 de alta densidad no probar otra densidad
	  return ERR_GENERAL; 
	  }
	steps_per_cyl++;
	errors=0;
	}*/
  }
  //verificar si se necesita una recolocaci¢n
  if (need_reset) 
	 if (reset()==ERR_RESET) {
		return ERR_RESET; //si no se puede resetear controlador,salir
		}
  //ahora formar el chip DMA
  if (dma_setup(fp)==OVERFLOWDMA) {
	 r=OVERFLOWDMA;
	 break;
	 }
  //ver si el motor est  andando,si no encenderlo y esperar
  start_motor(fp);
  //si nos dirigimos a un nuevo cilindro,poner el brazo en el cilindro
  r=seek(fp);
  if (r!=OK) continue; //si hay un error , volver a intentarlo
  //realizar la transferencia
  r=transfer(fp);
  if (r==OK) break; //si fu‚ bien,salir del ciclo
  if (r==ERR_WR_PROTECT) break; //no probar m s debido a que no sirve de nada cuando el disco est  protegido contra escritura
  }
//activar el cron¢metro parar_motor para apagar el motor en unos cuantos segundos(counter)
cambiada_densidad=0;
motor_goal=ENABLE_INT; //cuando el cron¢metro llegue a su l¡mite,eliminar
							  //todos los motores
counter=STOP_MOTOR; //tiempo en pulsos de reloj para que se pare el motor
parar_motor=1; //para que se pueda apagar el motor
if (r==OK && fp->fl_cilindro_real>0) initialized=1; 
return (r==OK ? fp->fl_count : r);
}




//------------------------------------------------------------------
int dma_setup(struct _floppy *fp) {

int mode,low_addr,high_addr,top_addr,low_ct,high_ct,top_end;
char far *vir;
unsigned int ct;
unsigned long user_phys;

mode=(fp->fl_opcode == DISK_READ ? DMA_READ : DMA_WRITE);
vir=fp->fl_address;
ct=fp->fl_count;
user_phys=(unsigned long)(((unsigned long)FP_SEG(vir) << 4) + FP_OFF(vir));
low_addr=(int)(user_phys) & BYTE;
high_addr=(int)(user_phys >> 8) & BYTE;
top_addr=(int)(user_phys >> 16) & BYTE;
low_ct=(int)((ct-1)) & BYTE;
high_ct=(int)((ct-1) >> 8) & BYTE;
//verificar si la transferencia requerir  que el contador de direcciones DMA
//pase de un segmento de 64K a otro.Si es afirmativo,no se active,ya que el
//hardware no hace acarreo del bit 15 al 16 de la direcci¢n DMA.Verificar
//tambi‚n si la direcci¢n es 0:0
if (user_phys == 0) {
  puts("Direcci¢n de puntero nula");
  exit(1);
  }
top_end=(int)(((user_phys + ct - 1) >> 16) & BYTE);
if (top_end != top_addr) 
  return OVERFLOWDMA;
//ahora construir los registros DMA
disable();
outportb(DMA_M2,mode); //fijar el modo DMA
outportb(DMA_M1,mode); //volverlo a fijar
outportb(DMA_ADDR,low_addr); //sacar los 8 bits de orden inferior
outportb(DMA_ADDR,high_addr); //sacar los 8 bits siguientes
outportb(DMA_TOP,top_addr); //sacar los 4 bits altos
outportb(DMA_COUNT,low_ct); //sacar los 8 bits bajos del conteo - 1
outportb(DMA_COUNT,high_ct); //sacar los 8 bits altos del conteo - 1
enable();
outportb(DMA_INIT,2); //inicializar DMA
return 0;
}

//------------------------------------------------------------------

void start_motor(struct _floppy * fp) {

int motor_bit,running;

disable();
motor_bit= 1 << (fp->fl_drive + 4); //m scara de bits de esta unidad
motor_goal=motor_bit | ENABLE_INT | fp->fl_drive;
if (motor_status & motor_prev) motor_goal |=motor_prev;
running=motor_status & motor_bit; //distinto de 0 si esta motor est  andando
outportb(DOR,motor_goal);
motor_status=motor_goal;
motor_prev=motor_bit; //registrar el motor arrancado la siguiente vez
enable();
//si el motor ya estaba andando,no hay que esperarlo
if (running) return; //el motor ya estaba andando
//esperar (n pulsos de reloj/4) para que el motor coja velocidad m xima
clicks=0;
#pragma warn -eff
while(clicks<MOTOR_START);
#pragma warn .eff
}

//------------------------------------------------------------------
void stop_motor(void) {

outportb(DOR,ENABLE_INT); //para todos los motores
motor_status=motor_goal;

}

//------------------------------------------------------------------

int seek(struct _floppy * fp) {

//emitir un comando SEEK en la unidad que se indica a menos que el brazo
//ya est‚ colocado en el cilindro correcto

int r;

//si no est  la unidad calibrada,calibrarla
if (fp->fl_calibration == UNCALIBRATED)
	if (recalibrate(fp)!=OK) return (ERR_SEEK);
//¨estamos en el cilindro correcto?
if (fp->fl_curcyl == fp->fl_cilindro_real) return (OK);

//N£mero de cilindro equivocado.Emitir un comando SEEK y esperar la interrupci;on
fdc_out(FDC_SEEK); //iniciar la emisi¢n del comando SEEK
interrupcion_disco=0;
fdc_out((fp->fl_cara_real << 2) | fp->fl_drive);
fdc_out(fp->fl_cilindro_real * steps_per_cyl);
if (need_reset) return (ERR_SEEK); //si el controlador est  paralizado,abortar la b£squeda
#pragma warn -eff
esperaintdisco; //esperar a que termine de procesar el controlador SEEK
#pragma warn .eff
fdc_out(FDC_SENSE); //sondear FDC para hacer que produzca la condici¢n
r=fdc_results(fp); //determinar los bytes de condici¢n del controlador
if ((fp->fl_results[ST0] & ST0_BITS) != SEEK_ST0) r=ERR_SEEK;
if (fp->fl_results[ST1] != fp->fl_cilindro_real * steps_per_cyl) r=ERR_SEEK;
if (r!=OK) //si no ha podido posicionar el brazo
  if (recalibrate(fp) != OK) return (ERR_SEEK); //si no ha podido recalibrar la unidad,retornar
if (r==OK) // si se ha podido hacer SEEK
  fp->fl_curcyl=fp->fl_cilindro_real;
return r;
}

//------------------------------------------------------------------------
int transfer(struct _floppy * fp) {
//leer o escribir un bloque
//antes de llamar aqu¡ , verificar que fp->fl_code sea igual a DISK_READ o DISK_WRITE
int r,s,op;

//si la unidad no est  calibrada,retornar
if (fp->fl_calibration == UNCALIBRATED) return (ERR_TRANSFER);
//si el motor est  apagado,salir
if (((motor_status >> (fp->fl_drive+4)) & 1) == 0) return (ERR_TRANSFER); 
//el comando se emite emitiendo 9 bytes al controlador
op= (fp->fl_opcode == DISK_READ ? FDC_READ : FDC_WRITE);
fdc_out(op);  //enviar el comando de lectura o escritura
interrupcion_disco=0;
fdc_out((fp->fl_cara_real << 2) | fp->fl_drive);
fdc_out(fp->fl_cylinder); //enviar al controlador el cilindro
fdc_out(fp->fl_head); //enviar al controlador la cara
fdc_out(fp->fl_sector); //enviar al controlador el sector
fdc_out(fp->fl_codigolong); //enviar al controlador el tama¤o del sector
fdc_out(fp->fl_ultimo_sector); //indicar al controlador cu n grande es una pista
fdc_out(GAP); //indicar al controlador el interespacio entre sectores
fdc_out(DTL); //indicar al controlador acerca de la extensi¢n de los datos

//bloquear y esperar la interrupci¢n del disco
if (need_reset) return (ERR_TRANSFER); //si el controlador est  paralizado,abortar la operaci¢n
clicks=0;
#pragma warn -eff
while (!interrupcion_disco && clicks<HZ); //mientras el controlador no responde o pase 1 segundo
#pragma warn .eff
if (clicks>=HZ) {
  need_reset=1;
  return ERR_TIMEOUT;
  }
//determinar la condici¢n del controlador y verificar si hay errores
r=fdc_results(fp);
if (r!=OK) return r;
if ((fp->fl_results[ST1] & BAD_SECTOR) || (fp->fl_results[ST2] & BAD_CYL))
  fp->fl_calibration=UNCALIBRATED;
if (fp->fl_results[ST1] & WRITE_PROTECT) return ERR_WR_PROTECT;
if ((fp->fl_results[ST0] & ST0_BITS) != TRANS_ST0) return(ERR_TRANSFER);
if (fp->fl_results[ST1] | fp->fl_results[ST2]) return (ERR_TRANSFER);

//comparar los n£meros reales de los sectores transferidos con el n£mero esperado
s=(fp->fl_results[ST_CYL] - fp->fl_cylinder) * NR_HEADS * fp->fl_ultimo_sector;
s+=(fp->fl_results[ST_HEAD] - fp->fl_head) * fp->fl_ultimo_sector;
s+=(fp->fl_results[ST_SEC] - fp->fl_sector);
if (s*SECTOR_SIZE!= fp->fl_count) return (ERR_TRANSFER);
return(OK);
}

//------------------------------------------------------------------------
int fdc_results(struct _floppy *fp) {

//extraer resultados despu‚s de una operaci¢n

int i,j,status/*,tem*/;

  j=0;
  do {
  status=inportb(FDC_STATUS); 
  }while (!(status & MASTER) && j++<MAX_FDC_RETRY); //mientras no acabe de procesar el comando
  if (j>MAX_FDC_RETRY) return (ERR_STATUS);
  status=inportb(FDC_STATUS); //volver a leerlo
  if ((status & DIRECTION) == 0) return (ERR_STATUS); 
//repetir mediante la extracci¢n de bytes de FDC hasta que se¤ale que ya no hay m s
for (i=0;i<MAX_RESULTS;i++) {
/*  status=inportb(FDC_STATUS); //leerlo
  if ((status & MASTER) == 0) return (ERR_STATUS);  
  status=inportb(FDC_STATUS); //volver a leerlo
  if ((status & DIRECTION) == 0) return (ERR_STATUS); */
  status=inportb(FDC_DATA);
  fp->fl_results[i]=status & BYTE;
  j=0;
  do {
  status=inportb(FDC_STATUS); 
  }while (!(status & MASTER) && j++<MAX_FDC_RETRY); //mientras no acabe de procesar el comando
/*  for (j=0;j<5;j++) tem=j; */
  status=inportb(FDC_STATUS);
  if ((status & DIRECTION) ==0) return (OK);
  }
//FDC est  devolviendo demasiados resultados ya que solo puede ir aqu¡ si se han leido m s de 9 bytes
need_reset=TRUE; //se debe recolocar el chip controlador
return (ERR_STATUS);
}

//---------------------------------------------------------------------
void fdc_out (int val) {
//enviar byte al controlador del disco flexible
//solo se puede escribir este byte cuando el controlador escuche,si no
//escucha obligar al FDC a volverse a inicializar

int retries,r;

if (need_reset==TRUE) return ; //si el controlador necesita hacerse un reset,regresar
retries=MAX_FDC_RETRY;

//se pueden necesitar varios intentos para hacer que el FDC acepte un comando
while (retries-- > 0) {
  r=inportb(FDC_STATUS);
  r&=(MASTER | DIRECTION); //solo observar los bits 2 y 3
  if (r!= CTL_ACCEPTING) continue; //si FDC no escucha,volver a repetir el bucle
  outportb(FDC_DATA,val);
  return ; //FDC ha escuchado el comando,volver
}
//el controlador despu‚s de varios intentos,no escucha,obligarlo a inicializarse
need_reset=TRUE;
}

//-------------------------------------------------------------------------------
int recalibrate(struct _floppy *fp) {
//recalibrar unidad,ponerla en el cilindro 0

int r;

start_motor(fp); //encender el motor,ya que para recalibrarse tiene que estar encendido
fdc_out(FDC_RECALIBRATE); //indicar a la unidad que se recalibre
interrupcion_disco=0;
fdc_out(fp->fl_drive); //indicar que unidad
if (need_reset) return (ERR_SEEK); //no esperar si el controlador est  paralizado
#pragma warn -eff
esperaintdisco; //esperar a que el controlador responda
#pragma warn .eff
//determinar si la recalibraci¢n fu‚ ¢ptima
fdc_out(FDC_SENSE); //decirle al controlador que diga donde estamos
r=fdc_results(fp); //obtener los resultados de FDC_SENSE
fp->fl_curcyl=-1; //obligar un comando SEEK la siguiente vez ya que el controlador no responder¡a
if (r!=OK || (fp->fl_results[ST0] & ST0_BITS) != SEEK_ST0 || 
fp->fl_results[ST_PCN] != 0) {
  //la recalibraci¢n fall¢.La unidad se debe recalibrar y el controlador se debe inicializar
  need_reset=TRUE;
  fp->fl_calibration=UNCALIBRATED;
  return ERR_RECALIBRATE;
  }
  else {
  //la recalibraci¢n fu‚ ¢ptima
  fp->fl_calibration=CALIBRATED;
  return OK;
  }
}

//-------------------------------------------------------------------------
int reset(void) {       
//reinicializar el controlador,despu‚s de que el controlador no responde,etc.

int i,r,status;
struct _floppy *fp;

//deshabilitar interrupciones y cambiar a bajo el bit de recolocaci¢n
need_reset=FALSE;
disable();
motor_status=0;
motor_goal=0;
interrupcion_disco=0;
outportb(DOR,0); //cambiar a bajo el bit de recolocaci¢n
outportb(DOR,ENABLE_INT); //volver a cambiarlo a alto
enable(); //permitir otra vez las interrupciones
#pragma warn -eff
esperaintdisco;  //esperar a que el controlador responda
#pragma warn .eff

//se ha recibido la interrupci¢n de la recolocaci¢n.Continuar la recolocaci¢n
fp=&floppy[0];
fp->fl_results[0]=0;
fdc_out(FDC_SENSE);
r=fdc_results(fp); //determinar resultados
if (r!=OK)  return ERR_RESET;
status=fp->fl_results[0] & BYTE;
if (status != CHANGE) return ERR_RESET;
//la reseteaci¢n fu‚ ¢ptima.Indicar a FDC los par metros de la unidad
fdc_out(FDC_SPECIFY); //especificar algunos par metros de cronometraje
fdc_out(SPEC1); //raz¢n de avance y tiempo de descarga de la cabeza
fdc_out(SPEC2); //tiempo de carga de la cabeza y no dma
//indicar a todas las unidades que no est n calibradas
for (i=0;i<NR_DRIVES;i++) floppy[i].fl_calibration=UNCALIBRATED; 
return 0; //todo ha ido bien
}

//------------------------------------------------------------------------

void interrupt int0e() {
//interrupci¢n que se genera cuando el FDC acaba de procesar un comando
veces++;
interrupcion_disco=1;
outportb(0x20,0x20);
}

//-----------------------------------------------------------------------

void interrupt int08() {
unsigned long far *ticks=MK_FP(0x40,0x6c);
//interrupci¢n del reloj,se genera por cada click del reloj

(*ticks)++; //para actualizar reloj del sistema
++clicks;
if (parar_motor) {
  counter--;
  if (!counter) {
	 stop_motor();
	 parar_motor=0; //para que no se pare constantemente la unidad despues de parada
	 };
  }
outportb(0x20,0x20);
}
//-----------------------------------------------------------------
void ponotravezvectores(void) {
  
delay(2000);
//volver a restaurar vectores
disable(); 
setvect(0x0e,int0e);
setvect(0x08,int08);
enable();
stop_motor();
}
//-----------------------------------------------------------------
void restauravectores(void) {
//restaura los vectores relacionados con el disco,0eh y 08h

disable();
setvect(0x0e,oldint0e);
setvect(0x08,oldint08);
enable();
}
//------------------------------------------------------------------
void pondiscobien(void) {
//poner en orden los datos del bios de las unidades
int unid;
char buffer[512];

for (unid=0;unid<4;unid++) 
  biosdisk(0,unid,0,0,0,1,buffer);
_AH=0x0d; //reinicializar disco
geninterrupt(0x21);
}
//-----------------------------------------------------------------
void al_salir(void) {



restauravectores();
pondiscobien();
}
//------------------------------------------------------------------
void borra_pantalla(void) {
union REGS regs;

regs.h.ah=6; //scroll arriba
regs.h.al=0; //para borrar toda la ventana
regs.h.bh=7; //atributo
regs.h.ch=0; //y
regs.h.cl=0; //x
regs.h.dh=filas_pantalla-1; //yy
regs.h.dl=columnas_pantalla-1; //xx
int86(0x10,&regs,&regs);
regs.h.ah=2; //posicionar cursor
regs.h.bh=0;
regs.x.dx=0;
int86(0x10,&regs,&regs);
}
//------------------------------------------------------------------
void writechar(unsigned char ch) {
union REGS regs,regs2;

regs.h.ah = 9;
regs.h.al = ch;
regs.h.bh = 0;                  /* video page */
regs.h.bl = 7;  /* video attribute */
regs.x.cx = 1;           /* repetition factor */
int86(0x10,&regs,&regs); /* output the char */
regs.h.ah=3;
regs.h.bh=0;
int86(0x10,&regs,&regs);  //lee posicion actual del cursor
if (regs.h.dl==columnas_pantalla-1) {
  regs.h.dl=0;
  regs.h.dh++;
  if (regs.h.dh==filas_pantalla) {
	 regs2.h.ah=6; //scroll arriba
	 regs2.h.al=1;
	 regs2.h.bh=7;
	 regs2.h.ch=0;
	 regs2.h.cl=0;
	 regs2.h.dh=filas_pantalla-1;
	 regs2.h.dl=columnas_pantalla-1;
	 int86(0x10,&regs2,&regs2);
	 regs.h.dh=filas_pantalla-1;
	 }
  }
else regs.h.dl++;
regs.h.ah=2;
regs.h.bh=0;
int86(0x10,&regs,&regs);
}
//--------------------------------------------------------------------
int sibreak(void) {

al_salir();
return 0; //abortar el programa en curso
}

//--------------------------------------------------------------------
int main(void) {
int uni,tipo,status,i,leer,sobreescribe;
char nombre[81],temporal[128];
unsigned int longi;
FILE *handle;
struct text_info info;
struct parametrosdrive {
unsigned char bpb;
unsigned char tipo;
unsigned int removible;
unsigned int maxcilindros;
unsigned char tipoelem;
unsigned int bytesector;
unsigned char sectorcluster;
unsigned int sectoresresrv;
unsigned char numtablasfat;
unsigned int nummaxentradasdirprin;
unsigned int numerossectores;
unsigned char idenfat;
unsigned int sectoresfat;
unsigned int sectorespista;
unsigned int numcabezas;
long numsectoresocultos;
char unknown[64];
} paramdisco[4];
union REGS regs;
struct SREGS sregs;


gettextinfo(&info);
filas_pantalla=info.screenheight;
columnas_pantalla=info.screenwidth;
ctrlbrk(sibreak); //redirecciona a un procedimiento si se pulsa CTRL-C
//o CTRL-BREAK
puts("FLOPPYOP V. 2.0 Lectura/Escritura de discos a traves del controlador");
puts("(c) Bit Soft 17/06/1998");
puts("Cogiendo informaci¢n de las unidades");
for (i=0;i<4;++i) {
  paramdisco[i].bpb=0; //para que coja informaci¢n por defecto
  regs.x.dx=FP_OFF(&paramdisco[i]);
  regs.x.ax=0x440d;
  regs.x.cx=0x0860;
  regs.h.bl=i+1;
  sregs.ds=FP_SEG(&paramdisco[i]);
  intdosx(&regs,&regs,&sregs);
  if (!regs.x.cflag && paramdisco[i].removible!=1) {
	 printf("Unidad %d , %u Kb , %s %s\n",i,paramdisco[i].sectorespista * 
	 paramdisco[i].numcabezas * paramdisco[i].maxcilindros / 2,
	 ((paramdisco[i].maxcilindros==40 && paramdisco[i].sectorespista==9) ||
	 (paramdisco[i].maxcilindros==80 && paramdisco[i].sectorespista==15)) ?
	 "5 1/4" : "3 1/2",(paramdisco[i].sectorespista>=15) ? 
	 "Alta densidad" : "Doble densidad");
	 if (paramdisco[i].maxcilindros==40) 
		densidad_unidad_defecto[i]=1; //360 kb
	 if (paramdisco[i].maxcilindros==80 &&
	 paramdisco[i].sectorespista==9) 
		densidad_unidad_defecto[i]=2; //720 kb
	 if (paramdisco[i].maxcilindros==80 &&
	 paramdisco[i].sectorespista!=9) {
		densidad_unidad_defecto[i]=0; //1200/1440 kb
		alta_densidad=1;
		}
	 }
  else paramdisco[i].removible=1;
  };
putch('\r');
disable();
oldint0e=getvect(0x0e);
oldint08=getvect(0x08);
setvect(0x0e,int0e);
setvect(0x08,int08);
enable();
atexit(al_salir);
puts("Interrupciones 0eh y 08h redirigidas");
veces=0;
motor_prev=0xffff;
//need_reset=1;
motor_status=0;
motor_goal=0;
motor_prev=0xffff;
parar_motor=0;
for (uni=0;uni<NR_DRIVES;++uni) {
  floppy[uni].fl_opcode=DISK_READ;
  floppy[uni].fl_calibration=UNCALIBRATED; //para calibrar la unidad
  floppy[uni].fl_drive=uni;
  floppy[uni].fl_curcyl=255;
  }
otra:
puts("\nUnidad :(0-A,1-B,..):");
scanf("%d",&uni);
if (paramdisco[uni].removible!=1) {
  tipo=((paramdisco[uni].maxcilindros==40 && paramdisco[uni].sectorespista==9) ||
  (paramdisco[uni].maxcilindros==80 && paramdisco[uni].sectorespista==15)) ?
  1 : 2;
  HD525=(paramdisco[uni].maxcilindros==80 && paramdisco[uni].sectorespista==15) ?
  1 : 0;
  alta_densidad=0;
  if (paramdisco[uni].maxcilindros==40) densidad_act=1; //360 kb
  if (paramdisco[uni].maxcilindros==80 &&
  paramdisco[uni].sectorespista==9) densidad_act=2; //720 kb
  if (paramdisco[uni].maxcilindros==80 &&
  paramdisco[uni].sectorespista!=9) {
	 densidad_act=0; //1200/1440 kb
	 alta_densidad=1;
	 }
  if ((paramdisco[uni].maxcilindros==40 && 
  paramdisco[uni].sectorespista==9) || 
  (paramdisco[uni].maxcilindros==80 && 
  paramdisco[uni].sectorespista==15))
	 densidad_dd=1; else densidad_dd=2;
  }
else {
  puts("Par metros no disponibles para esta unidad , especifique :");
  do {
	 puts("Tipo unidad (1=5 1/4 o 2=3 1/2)");
	 scanf("%d",&tipo); } while (tipo!=1 && tipo!=2);
	 do {
		puts("¨Es de alta densidad?(S/N)");
		scanf("%s",&temporal);
		strupr(temporal);
		if (!strcmp(temporal,"N")) break;
		}while (strcmp(temporal,"S"));
  i=strcmp(temporal,"N");
  HD525=(!i) ? 0 : 1;
  densidad_act=(i) ? 0 : (tipo==1) ? 1 : 2;
  if (tipo==1) densidad_dd=1;
  if (tipo==2) densidad_dd=2;
  if (i==0) alta_densidad=0;
  putch('\n');
}
if (tipo==1) GAP=0X2A; else GAP=0X1B;
printf("GAP actual:%d=%02Xh\n",GAP,GAP);
puts("Poner otro Gap(S/N)");
scanf("%s",&temporal);
strupr(temporal);
if (!strcmp(temporal,"S")) {
  puts("\nGAP(en decimal):");
  scanf("%d",&GAP);
  }
do {
  puts("LEER SECTOR O ESCRIBIR SECTOR(LEER=1,ESCRIBIR=2)");
  scanf("%d",&tipo); } while (tipo!=1 && tipo!=2);
if (tipo==1) floppy[uni].fl_opcode=DISK_READ; 
  else floppy[uni].fl_opcode=DISK_WRITE;
puts("Cilindro real:");
scanf("%d",&floppy[uni].fl_cilindro_real);
puts("Cara real:");
scanf("%d",&floppy[uni].fl_cara_real);
puts("Cilindro(de la cabecera):");
scanf("%d",&floppy[uni].fl_cylinder);
puts("Cara(de la cabecera):");
scanf("%d",&floppy[uni].fl_head);
puts("Sector(de la cabecera):");
scanf("%d",&floppy[uni].fl_sector);
printf("%s\t","C¢digos de longitudes");
for (i=0,longi=128;i<8;++i,longi<<=1)
  printf("%u=%u bytes\t",i,longi);
puts("255=De 0 a 255\nC¢digo de bytes por sector");
scanf("%d",&floppy[uni].fl_codigolong);
puts("Bytes a leer(de 0 a 32767)");
scanf("%d",&floppy[uni].fl_count);
puts("N£mero del £ltimo sector de la pista(cabecera)");
scanf("%d",&floppy[uni].fl_ultimo_sector);
if ((floppy[uni].fl_address=malloc(floppy[uni].fl_count))==NULL) {
  puts("Memoria insuficiente\a");
  return(1);
  }
if (floppy[uni].fl_opcode==DISK_WRITE) {
  restauravectores();
  puts("\nNombre del fichero:");
  scanf("%80s",&nombre);
  if ((handle=fopen(nombre,"rb"))==NULL) puts("No puedo abrir fichero");
	else {
	fread ((char near *)floppy[uni].fl_address,1,floppy[uni].fl_count,handle);
	fclose(handle);
	ponotravezvectores();
  }
}
puts(floppy[uni].fl_opcode==DISK_READ ? "\nPulse ENTER para leer disco..."
: "\nPulse ENTER para escribir disco");
gets(temporal);
gets(temporal);
setmem((void *)floppy[uni].fl_address,floppy[uni].fl_count,0);
outportb(FDC_DENSITY,densidad_act);
status=do_rdwt(uni);
if (status==OVERFLOWDMA) {
  if ((floppy[uni].fl_address=malloc(floppy[uni].fl_count))==NULL) {
	 puts("Memoria insuficiente\a");
	 return(1);
	 }
  else {
	 setmem((void *)floppy[uni].fl_address,floppy[uni].fl_count,0);
	 status=do_rdwt(uni);
	 }
  }
printf("\nInterrupci¢n 0x0e llamada %u veces\n",veces);
printf("Codigo de salida:%d\n",status);
if (status<0) printf("Error:%s\n",errores_disco[abs(status)]);
for (i=0;i<=5;++i) printf("ST%d:%d  ",i,floppy[uni].fl_results[i]);
puts("\nPulse ENTER");
gets(temporal);
borra_pantalla();
leer=floppy[uni].fl_count;
for (i=0;i<leer;++i) writechar(floppy[uni].fl_address[i]);
printf("\n%s ","Pulse ENTER");
gets(temporal);
borra_pantalla();
if (floppy[uni].fl_opcode==DISK_READ) {
  puts("Grabar sector a disco(S/N)");
  scanf("%s",&temporal);
  strupr(temporal);
  if (!strcmp(temporal,"S")) {
	 //poner vectores con BIOS
	 restauravectores();
	 puts("\nNombre del fichero:");
	 scanf("%80s",&nombre);
	 sobreescribe=1;
	 if ((handle=fopen(nombre,"rb"))!=NULL) {
		fclose(handle);
		do {
		 puts("Fichero existe,sobreescribir(S/N)");
		 scanf("%s",&temporal);
		 strupr(temporal);
		 if (!strcmp(temporal,"N")) break;
		 }while (strcmp(temporal,"S"));
		sobreescribe=(!strcmp(temporal,"N")) ? 0 : 1;
		}
	 if (sobreescribe==1) {
		if ((handle=fopen(nombre,"wb"))==NULL) puts("No puedo crear fichero");
		else {
		fwrite ((char near *)floppy[uni].fl_address,1,floppy[uni].fl_count,handle);
		fclose(handle);
		}
	 }
	 _AH=0x0d; //reinicializar disco
	 geninterrupt(0x21);
	 ponotravezvectores();
  }
}
steps_per_cyl=1;
initialized=0;
free((char *)floppy[uni].fl_address);
puts("\nOtra operaci¢n (S/N)");
scanf("%s",&temporal);
strupr(temporal);
if (!strcmp(temporal,"S")) goto otra;
return 0;
}