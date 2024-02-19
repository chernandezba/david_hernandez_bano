// 14 . Programa de las Torres de Hanoi

#include <conio.h> //para kbhit,getch,cprintf,gotoxy,cputs,clrscr,putch
//clreol,wherex,wherey,_setcursortype,textattr
#include <stdlib.h> //para atoi
#include <string.h> //para strlen,strupr,strcmp
#include <time.h> //para time,time_t
#define maxdiscos 15
#define anchoanilla 20



struct discos {
  char discos[maxdiscos];  // el indice 0 es el disco de m s abajo
  int numdiscos; // es donde est  el disco m s alto
  } disco[3];

unsigned int nummov; // n£mero de movimientos hechos
unsigned int pulsadoescape; // si se pulsa ESC en inputnum
unsigned char discos;
time_t tiemporeferencia;


//procedimiento que se llama cuando lo has resuelvo
void lo_has_resuelto(void)  {

gotoxy(1,22);
cprintf("­­­ Has acabado el juego !!! Pulsa tecla para salir\a");
getch();

}


//imprime el tiempo pasado
void reloj(void)
{
time_t diferencia;
unsigned char horas,minutos,segundos;

diferencia=time(NULL)-tiemporeferencia;
if (!diferencia)
  horas=minutos=segundos=0;
  else {
  segundos=diferencia % 60;
  diferencia/=60;
  minutos=diferencia % 60;
  diferencia/=60;
  horas=diferencia % 24;
  }
gotoxy(70,1);
cprintf("%02u:%02u:%02u",horas,minutos,segundos);
}

char esperatecla(void)  {
//mientras no se pulsa tecla llama al procedimiento reloj
while(!kbhit()) reloj();
//devuelve tecla pulsada
return getch();
}

int inputnum(int x,int y,int longitud,char *texto,char sireloj) {
char buffer[80];
int n;
char tecla;


pulsadoescape=0;
gotoxy(x,y);
cprintf("%s",texto);
textattr(7+BLINK); //para que el caracter pr¢ximo aparezca parpadeante
putch('_'); //escribe un caracter
textattr(7); //cambia la tinta a blanca
n=0;
buffer[n]=0;
do  {
  //guarda la posici¢n del cursor de en X y en Y ya que el procedimiento
  //reloj altera esta posici¢n
  x=wherex();
  y=wherey();
  if (sireloj==1) while(!kbhit()) reloj();
  //restaura la antigua posici¢n del cursor
  gotoxy(x,y);
  tecla=getch();
  if (tecla=='\b')  {
	 if (n>0) {
		buffer[n--]=0;
		cprintf("\b\b  \b\b");
		textattr(7+BLINK);
		putch('_');
		textattr(7);
		}
	 }
	 else  {
	 if (tecla>='0' && tecla<='9')
		if (n<longitud)  {
		  buffer[n++]=tecla;
		  cprintf("\b%c",tecla);
		  textattr(7+BLINK);
		  putch('_');
		  textattr(7);
		  }
		}
  }while(tecla!=13 && tecla!=27); //13=ENTER , 27=ESCAPE
buffer[n]=0;
cprintf("\b ");
if (tecla==27) pulsadoescape=1;
n=atoi(buffer);  //covierte la variable tipo cadena buffer a entero ,
//asignando ese entero a n
return n;
}

void actualizanm(void) // para actualizar el n£mero movimientos
{
gotoxy(1,1);
cprintf("N§ Movimientos      : %u",nummov);
}


void muevedesdehasta(unsigned char desde,unsigned char hasta) {
int i,ancho;
//ancho contiene el ancho del disco a mover

if (disco[desde-1].numdiscos) {
  gotoxy((anchoanilla*(desde-1))+1,18-disco[desde-1].numdiscos);
  for (i=0;i<maxdiscos;i++) putch(' ');
  ancho=disco[desde-1].discos[disco[desde-1].numdiscos-1];
  disco[hasta-1].discos[disco[hasta-1].numdiscos]=ancho;
  disco[desde-1].numdiscos--;
  disco[hasta-1].numdiscos++;
  gotoxy((anchoanilla*(hasta-1))+1,18-disco[hasta-1].numdiscos);
  for (i=0;i<ancho;i++) putch('*');
  }
}


//escribe en pantalla de donde se mueve hasta donde se mueve
void escribemuevedesdehasta(char desde,char hasta)  {
gotoxy(1,22);
cprintf("Mover de disco : %1u    a disco : %1u",desde,hasta);
}


//procedimiento para resolver el juego
void towers(int n,char frompeg,char topeg,char auxpeg)  {


// si es solo un disco,efectuar movimiento y regresar
if (n==1) {
 escribemuevedesdehasta(frompeg,topeg);
 muevedesdehasta(frompeg,topeg);
 ++nummov;
 actualizanm();
 reloj();
 return ;
 }
// mover los n-1 discos de arriba de 1 a 2 usando el 3 como auxiliar
towers(n-1,frompeg,auxpeg,topeg);
// mover el disco restante de 1 a 3
escribemuevedesdehasta(frompeg,topeg);
 muevedesdehasta(frompeg,topeg);
 ++nummov;
 actualizanm();
 reloj();
// mover n-1 discos de 2 hacia 3 empleando el 1 como auxiliar
towers(n-1,auxpeg,topeg,frompeg);
}


int acabado(void) //mira si se ha acabado , si se ha acabado devuelve 1
// si no devuelve 0
{


if ((disco[1].numdiscos==discos) || (disco[2].numdiscos==discos))
//si se han trasladado todos los discos al disco 2 o el 3
  return 1;
  else
  return 0;
}



void centrar(int y,char *texto)  {
gotoxy(40-(strlen(texto)/2),y);
cputs(texto); //imprime el contenido de la variable tipo cadena texto
}

void main(void)  {
int n,n1,dedisco,adisco,sepuedemover;
char buffer[80];

_setcursortype(_NOCURSOR); // hace desaparecer el cursor
for (n=0;n<=2;n++) disco[n].numdiscos=0; // incializa estructura disco
clrscr();
centrar(1,"TORRES DE HANOI");
gotoxy(anchoanilla-19,18);
cprintf("DISCO 1");
gotoxy((anchoanilla*2)-19,18);
cprintf("DISCO 2");
gotoxy((anchoanilla*3)-19,18);
cprintf("DISCO 3");
gotoxy(1,22);
do {
discos=inputnum(1,22,2,"N£mero de discos (M ximo 15,0 o ESC o ENTER SALIR):",0);
gotoxy(1,22);
clreol();} while(discos && discos>15);
if (!discos) return ;
for (n=discos,n1=0;n>0;n--,n1++) disco[0].discos[n1]=n;
disco[0].numdiscos=discos;
for (n=0;n<disco[0].numdiscos;n++)  {
  gotoxy(1,17-n);
  for (n1=0;n1<disco[0].discos[n];n1++) cprintf("*");
 }
gotoxy(1,2);
cprintf("Movimientos m¡nimos : %u",(1<<discos)-1);
nummov=0;
actualizanm();
gotoxy(1,22);
buffer[0]=2;
do {
  _setcursortype(_NORMALCURSOR); //para que aparezca el cursor
  cprintf("¨Quieres que el computador lo resuelva (S/N)?");
  cgets(buffer); // coge lo introducido en el teclado y lo almacena
  //en buffer a partir del tercer caracter,buffer[2]
  strupr(&buffer[2]); //convierte la cadena introducida a may£sculas
  gotoxy(1,22);
  clreol();
  _setcursortype(_NOCURSOR); //para que desaparezca el cursor
  }while(strcmp(&buffer[2],"S") && strcmp(&buffer[2],"N") && !pulsadoescape);
if (!pulsadoescape) {
  if (!strcmp(&buffer[2],"S")) {
	 tiemporeferencia=time(NULL); //devuelve los segundos transcurridos desde
	 //1970
	 _setcursortype(_NOCURSOR); //esconde el cursor
	 towers(discos,1,3,2); //lo resuelve
	 gotoxy(1,22);
	 clreol();
	 cprintf("­­­ Ya lo he resuelto !!! Pulsa tecla para salir\a\a");
	 getch();
	 }
  else {
  tiemporeferencia=time(NULL);
  do  {
  otra:
	do
	dedisco=inputnum(1,22,1,"Mover de disco(ESC SALIR):",1);
	while (dedisco>3 || dedisco<1 && !pulsadoescape);
	if (pulsadoescape) break; // se sale del programa con enter
	n=wherex(); //devuelve la posici¢n actual en x del cursor
	do
	adisco=inputnum(n+4,22,1,"a disco(ESC SALIR):",1);
	while (adisco>3 || adisco<1 && !pulsadoescape);
	if (pulsadoescape) break; // se sale del programa con enter
	gotoxy(1,22);
	clreol(); //borra a partir del cursor hasta el final de linea
	gotoxy(1,22);
	//comprueba si se puede mover de dedisco a adisco
	sepuedemover=1; // de momento la jugada es v lida
	if (disco[dedisco-1].discos[disco[dedisco-1].numdiscos-1]>
		 disco[adisco-1].discos[disco[adisco-1].numdiscos-1] &&
		 disco[adisco-1].numdiscos)  {
		 cprintf("No se puede mover donde en la punta hayan menos discos\a");
		 esperatecla();
		 sepuedemover=0;
		 }
	if (!sepuedemover) {
	  gotoxy(1,22);
	  clreol();
	  }
	else {
	  nummov++;
	  muevedesdehasta(dedisco,adisco);
	  actualizanm();
	  if (acabado()==1) {
		 lo_has_resuelto();
		 break;
		 }
	  }
	}while (1);
  }
}
_setcursortype(_NORMALCURSOR); // vuelve a visualizar el cursor
clrscr(); // borra toda la pantalla
}