//Programa para resolver ecuaciones lineales por matrices,
//determinantes , inversas de matriz
//El programa solo resuelve n ecuaciones de n inc¢gnitas
//Autor : David Hern ndez Ba¤¢
//24-6-96

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define INCOGMAX 20


//----------------------------------------------------------------------
long double matriz[INCOGMAX+1][INCOGMAX+1];
unsigned int maxincog;
long double determinante2(long double *matr,int maxinc);


//---------------------------------------------------------------------
void nohaymemoria(void) {
cputs("\r\nNo queda memoria\a\r\n");
exit(1);
}
//---------------------------------------------------------------------
int inputnum(int x,int y,int longitud,int color,char *texto) {
char buffer[80];
int n;
char tecla;


_setcursortype(_NOCURSOR);
gotoxy(x,y);
cputs(texto);
textattr(color+BLINK); //para que el caracter pr¢ximo aparezca parpadeante
putch('_'); //escribe un caracter
textattr(color); //cambia la tinta a color
n=0;
buffer[n]=0;
do  {
  tecla=getch();
  if (tecla=='\b')  {
	 if (n>0) {
		buffer[n--]=0;
		cputs("\b\b  \b\b");
		textattr(7+BLINK);
		putch('_');
		textattr(color);
		}
	 }
	 else  {
	 if (tecla>='0' && tecla<='9')
		if (n<longitud)  {
		  buffer[n++]=tecla;
		  cprintf("\b%c",tecla);
		  textattr(color+BLINK);
		  putch('_');
		  textattr(color);
		  }
		}
  }while(tecla!=13); //13=ENTER
buffer[n]=0;
cputs("\b ");
n=atoi(buffer);  //covierte la variable tipo cadena buffer a entero ,
//asignando ese entero a n
_setcursortype(_NORMALCURSOR);
return n;
}
//-----------------------------------------------------------
void introducedatos(void)  {
char buffer[21];
int x,y;

cputs("\r\n");
for (y=0;y<maxincog;y++) {
	 for (x=0;x<maxincog;x++) {
	 cprintf("Elemento (%d,%d):",y+1,x+1);
	 buffer[0]=19;
	 cgets(buffer);
	 matriz[y][x]=_atold(&buffer[2]);
	 cputs("\r\n");
	 }
}
}
//---------------------------------------------------------
void centrar(char texto[],int linea)  {
gotoxy(40-(strlen(texto)/2),linea);
cputs(texto);
}

void imprimeformula(unsigned int fila)  {
unsigned int n;

for (n=0;n<maxincog;n++)   {
  cprintf("%Lg%c + ",matriz[fila][n],'A'+n);
  }
cputs("\b\b = ");
}

//--------------------------------------------------------------

long double determinante(long double *matr,int maxinc) {
long double *m;



//guarda la matriz en m y utiliza esta m para hacer la determinante
if ((m=calloc(maxinc,sizeof(long double)*maxinc))==NULL) nohaymemoria();
//esta memoria no hay que liberarla ya que la parte encargada de hacer
//la determinante ya lo hace
//copia matr en m
memcpy(m,matr,sizeof(long double)*maxinc*maxinc);
//esta funci¢n calcula la determinante
return determinante2(m,maxinc);
}

//--------------------------------------------------------------
long double determinante2(long double *matr,int maxinc)  {
unsigned int x,xx,xx0,yy0,t,t1,yy;
int signo;
long double co,*m;

if (maxinc==2) {
	co=(matr[((maxinc)*0)+0]*matr[((maxinc)*1)+1])-
  (matr[((maxinc)*0)+1]*matr[((maxinc)*1)+0]);
  free(matr);
  return co;
  }
if (maxinc==1) {
  co=matr[0];
  free(matr);
  return co;
  }
for (co=0.0,x=0;x<maxinc;++x)  {
  //asigna memoria para copia matr en m sin la fila y columna correspondiente (columna x, fila 0)
  if ((m=calloc(maxinc-1,sizeof(long double)*(maxinc-1)))==NULL) nohaymemoria();
  for (yy=1,yy0=0;yy<maxinc;++yy,++yy0) {
	 for (xx=0,xx0=0;xx<maxinc;++xx) {
		 if (xx!=x) {
			t=((maxinc-1)*yy0)+xx0;
			t1=((maxinc)*yy)+xx;
			m[t]=matr[t1];
		  ++xx0;
		  }
	  }
  }
  //determinante=desarollo por cofactores a partir de la primera columna
  t1=((maxinc*0)+x);
  //si fila+columna es impar signo -1 , sino 1 (fila siempre ser  0)
  signo=((x) & 1) ? -1 : 1;
  co+=matr[t1]*signo*determinante2(m,maxinc-1);
  }
free(matr);
return co;
}
//--------------------------------------------------------------
void transpuesta(long double *mm,int maxinc) {
//hace la transpuesta de la matriz
unsigned int x,y,t,t1;
long double *m2;


m2=calloc(maxinc,sizeof(long double)*maxinc);
if (m2==NULL) nohaymemoria();
for (y=0;y<maxinc;++y)   {
  for (x=0;x<maxinc;++x) {
	 t=(maxinc*x)+y;
	 t1=(maxinc*y)+x;
	 m2[t]=mm[t1];
  }
}
memcpy(mm,m2,sizeof(long double)*(maxinc)*maxinc);
free(m2);
}



//---------------------------------------------------------------
void DETERMINANTES(void) {
int x,y;
char buffer[21];
long double *m;
unsigned int t;

clrscr();
cputs("Nota : El n£mero de filas ser  igual al n£mero de columnas de la matriz\r\n");
do  {
  putch('\r');
  clreol();
  cprintf("Introduzca el n£mero de filas de la matriz(M ximo %d,0 salir):",INCOGMAX);
  maxincog=inputnum(wherex(),wherey(),2,7,"");
  if (!maxincog) return ;
  }while(maxincog>20);
introducedatos();
cputs("Calculando determinante ...");
if ((m=calloc(maxincog,sizeof(long double)*maxincog))==NULL) nohaymemoria();
for (y=0;y<maxincog;++y)  {
  for (x=0;x<maxincog;++x)  {
	 t=(maxincog*y)+x;
	 m[t]=matriz[y][x];
	 }
  }
cprintf("\r\nDeterminante : %Lg",determinante(m,maxincog));
free(m);
cputs("\r\nPulse tecla ...");
getch();
}


//---------------------------------------------------------------
void inversa (long double *mm) {
//mm es una variable donde se copiara la matriz inversa
unsigned int x,ec,y,yy0,xx0,yy,xx,t,t1;
char buffer[80];
long double deter,*cofactores,*m,*soluciones,res;
int signo;


if (maxincog==1) return ;
if ((m=calloc(maxincog,sizeof(long double)*maxincog))==NULL) nohaymemoria();
for (y=0;y<maxincog;++y)  {
  for (x=0;x<maxincog;++x)  {
	 t=(maxincog*y)+x;
	 m[t]=matriz[y][x];
	 }
  }
deter=determinante(m,maxincog);
free(m);
cofactores=calloc(maxincog,sizeof(long double)*(maxincog));
if (cofactores==NULL) nohaymemoria();
  for (y=0;y<maxincog;++y)  {
	 for (x=0;x<maxincog;++x)  {
	 //asigna memoria para copiar la matriz original en memoria
	 m=calloc(maxincog-1,sizeof(long double)*(maxincog-1));
	 if (m==NULL) {
	 nohaymemoria();
	 exit(1);
	 }
  //crea la matriz para los cofactores (es la matriz original pero sin la fila
  //especifica (y) y la columna especifica(x)
  for (yy=0,yy0=0;yy<maxincog;++yy) {
  if (yy!=y) {
	 for (xx=0,xx0=0;xx<maxincog;++xx) {
		 if (xx!=x) {
			t=((maxincog-1)*yy0)+xx0;
			m[t]=matriz[yy][xx];
		  ++xx0;
		  }
	  }
	 ++yy0;
  }
}
	 //para saber el cofactor de dicha fila o columna se elimina dicha fila
	 //o columna y se calcula el determinante de la matriz resultante
	 signo=((x+y) & 1) ? -1 :1 ;
	 t=(maxincog*y)+x;
	 cofactores[t]=signo*determinante(m,maxincog-1);
	 free(m);
	 }
  }
  transpuesta(cofactores,maxincog);
  //A-1 = matriz de cofactores(transpuesta)/determinante
  for (yy0=0;yy0<maxincog;yy0++)  {
	 for (xx0=0;xx0<maxincog;++xx0)  {
		t=(maxincog*yy0)+xx0;
		cofactores[t]/=deter;
		}
	 }
  //ahora cofactores es la inversa de la matriz y la copia a la matriz mm
memcpy(mm,cofactores,sizeof(long double)*maxincog*maxincog);
free(cofactores);
}

//---------------------------------------------------------------
void PONEINVERSA(void)  {
int x,y,t;
char buffer[21];
long double deter,*minversa,*m;
struct text_info info;

clrscr();
gettextinfo(&info);
cputs("Nota : El n£mero de filas ser  igual al n£mero de columnas de la matriz\r\n");
do  {
  putch('\r');
  clreol();
  cprintf("Introduzca el n£mero de filas de la matriz(M ximo %d,0 salir):",INCOGMAX);
  maxincog=inputnum(wherex(),wherey(),2,7,"");
  if (!maxincog) return ;
  }while(maxincog>20);
introducedatos();
if ((minversa=calloc(maxincog,sizeof(long double)*maxincog))==NULL)	nohaymemoria();
cputs("Calculando determinante ...");
if ((m=calloc(maxincog,sizeof(long double)*maxincog))==NULL) nohaymemoria();
for (y=0;y<maxincog;++y)  {
  for (x=0;x<maxincog;++x)  {
	 t=(maxincog*y)+x;
	 m[t]=matriz[y][x];
	 }
  }
deter=determinante(m,maxincog);
free(m);
if (!deter)  {
  cputs("\r\nNo se puede calcular la inversa ya que la determinante es 0\r\n\a");
  getch();
  return;
  }
cprintf("\r\nDeterminante : %Lg\r\n",deter);
cputs("Calculando inversa ....");
inversa(minversa);
cputs("\r\nMatriz inversa:\r\n");
for (y=0;y<maxincog;++y)  {
  for (x=0;x<maxincog;++x)  {
	 t=(maxincog*y)+x;
	 cprintf("ELemento (%d,%d):%Lg\r\n",y+1,x+1,minversa[t]);
	 if (wherey()==info.screenheight)  {
		cputs("Pulse tecla para ver el resto ...");
		getch();
		clrscr();
		}
	 }
  }
cputs("Pulse tecla ...");
getch();
}
//--------------------------------------------------------------
void LINEALES(void) {
unsigned int x,ec,y,yy0,xx0,yy,xx,t,t1;
char buffer[80];
long double deter,*minversa,*m,*soluciones,res;

clrscr();
cputs("Nota:El n£mero de ecuaciones ser  igual al m ximo n£mero de inc¢gnitas"
"\r\n\n");
do  {
  putch('\r');
  clreol();
  cprintf("Introduzca el n£mero m ximo de ecuaciones (M ximo %d,0 salir):",INCOGMAX);
  maxincog=inputnum(wherex(),wherey(),2,7,"");
  if (!maxincog) return;
  }while(maxincog>20);
  cputs("\r\n");
  for (ec=0;ec<maxincog;ec++) {
	 for (x=0;x<maxincog;x++) {
	 cprintf("Ecuaci¢n %u , %c:",ec+1,'A'+x);
	 buffer[0]=19;
	 cgets(buffer);
	 matriz[ec][x]=_atold(&buffer[2]);
	 cputs("\r\n");
	 }
	 imprimeformula(ec);
	 buffer[0]=19;
	 cgets(buffer);
	 matriz[ec][maxincog]=_atold(&buffer[2]);
	 cputs("\r\n");
  }
  cputs(
"Resolviendo matriz a partir del m‚todo A^-1*B donde A es la matriz de los \r\n"
"coeficientes de las variables y B es la matriz de los t‚rminos independientes\r\n"
"la matriz resultante contiene el resultado de las variables\r\n");
  //calcula la soluci¢n a partir del m‚todo A^-1*B donde A es la matriz de
  //los coeficientes de las variables y B es la matriz de los t‚rminos inde
  //pendientes
  if ((m=calloc(maxincog,sizeof(long double)*maxincog))==NULL) nohaymemoria();
  for (y=0;y<maxincog;++y)  {
	 for (x=0;x<maxincog;++x)  {
		t=(maxincog*y)+x;
		m[t]=matriz[y][x];
		}
	 }
  deter=determinante(m,maxincog);
  free(m);
  if (deter==0.0)  {
	 cputs("\r\nNo hay soluci¢n ya que la determinante es 0\r\n\a");
	 getch();
	 return;
	 }
  if ((minversa=calloc(maxincog,sizeof(long double)*(maxincog)))==NULL) nohaymemoria();
  for (y=0;y<maxincog;++y)  {
	 for (x=0;x<maxincog;++x)  {
		t=(maxincog*y)+x;
		minversa[t]=matriz[y][x];
		}
	 }
  inversa(minversa); //calcula la inversa de la matriz
  //crea espacio para la matriz de soluciones
  if ((soluciones=calloc(maxincog,sizeof(long double)))==NULL) nohaymemoria();
  if (maxincog==1)  soluciones[0]=matriz[0][1]/matriz[0][0];
  else {
	 for (yy0=0;yy0<maxincog;++yy0)  {
		 for (xx0=0,res=0.0;xx0<maxincog;++xx0)  {
			t=(maxincog*yy0)+xx0;
		  res+=minversa[t]*matriz[xx0][maxincog];
		soluciones[yy0]=res;
	 }
  }
}
  //en matriz solucion soluciones de las variables
for (yy0=0;yy0<maxincog;++yy0)
 cprintf("%c=%Lg\r\n",'A'+yy0,soluciones[yy0]);
cputs("\r\nPulse tecla");
free(minversa);
free(soluciones);
getch();
}


//---------------------------------------------------------------

void main(void) {
int opcion;

do {
textbackground(BLACK);
clrscr();
textcolor(7);
gotoxy(1,24);
cputs(
"\r\nAutor : David Hern ndez Ba¤¢ (c) 24/6/1996");
centrar("MENU",4);
gotoxy(30,8);
cputs("1. ECUACIONES LINEALES");
gotoxy(30,10);
cputs("2. DETERMINANTES");
gotoxy(30,12);
cputs("3. INVERSA DE UNA MATRIZ");
gotoxy(30,14);
cputs("4. SALIR");
opcion=inputnum(30,16,1,7,"Opci¢n:");
switch (opcion) {
  case 1:LINEALES(); break;
  case 2:DETERMINANTES(); break;
  case 3:PONEINVERSA(); break;
  }
  }while(opcion!=4);
textcolor(7);
textbackground(0);
clrscr();
}