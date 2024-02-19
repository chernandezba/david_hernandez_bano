//Programa para calcular estadisticas , media , mediana , desviaci¢n media ,etc.
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define MAXDATOS 100

unsigned int haydatos,numdatos;
double *datos,mediaarit,varianza;


//---------------------------------------------------------------------
void nohaymemoria() {
cputs("No hay memoria\r\n\a");
exit(1);
}
//---------------------------------------------------------------------
int inputnum(int x,int y,int longitud,char *texto) {
char buffer[80];
int n;
char tecla;


_setcursortype(_NOCURSOR);
gotoxy(x,y);
cputs(texto);
textattr(7+BLINK); //para que el caracter pr¢ximo aparezca parpadeante
putch('_'); //escribe un caracter
textattr(7); //cambia la tinta a color
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
  }while(tecla!=13); //13=ENTER
buffer[n]=0;
cputs("\b ");
n=atoi(buffer);  //covierte la variable tipo cadena buffer a entero ,
//asignando ese entero a n
_setcursortype(_NORMALCURSOR);
return n;
}




//------------------------------------------------------------------

void introducevalores(void) {
int x,i,j;
char buffer[19];
double aux;


clrscr();
gotoxy(1,35);
cputs("INTRODUCIR VALORES");
gotoxy(1,3);
do  {
  putch('\r');
  clreol();
  cprintf("N£mero de datos(M ximo %d,0 salir):",MAXDATOS);
  numdatos=inputnum(wherex(),wherey(),3,"");
  if (!numdatos) return;
  }while(numdatos>MAXDATOS);
if (haydatos) free(datos);
if ((datos=(double *)calloc(numdatos,sizeof(double)))==NULL)  nohaymemoria();
haydatos=1;
for (x=0;x<numdatos;++x)  {
  cprintf("\r\nDato n£mero %d:",x+1);
  buffer[0]=sizeof(buffer)-2;
  cgets(buffer);
  datos[x]=atof(&buffer[2]);
  }
cputs("ordenando valores ...");
for (i=0;i<numdatos;i++)
  for (j=0;j<numdatos-i-1;j++) {
	 if (datos[j]>datos[j+1]) {
		aux=datos[j];
		datos[j]=datos[j+1];
	  datos[j+1]=aux;
	  }
	}
}
//------------------------------------------------------------------
void listavalores(void)  {
int x;
struct text_info info;

clrscr();
if (!haydatos) {
  cprintf("No se han introducido datos\a\r\n");
  getch();
  return;
  }
gotoxy(1,35);
cputs("LISTA DE VALORES");
gotoxy(1,3);
gettextinfo(&info);
for (x=0;x<numdatos;++x)  {
  cprintf("Dato n£mero %d:%lg\r\n",x+1,datos[x]);
  if (wherey()==info.screenheight)  {
	cputs("Pulse tecla para mas datos ...");
	getch();
	clrscr();
	}
  }
cputs("Pulse tecla ...");
getch();
}

//------------------------------------------------------------------
double mediana(void)  {
int i,j;
i=(numdatos >> 1)-1;
j=i+1;
if (!(numdatos & 1)) return (datos[i]+datos[j]) / 2;
else return datos[((numdatos >> 1)+1)-1];
}

//------------------------------------------------------------------
double moda(void)  {
//los datos estan ordenados
int *repeticiones,x,x1,indmax,max;

if ((repeticiones=(int *)calloc(numdatos,sizeof(int)))==NULL) nohaymemoria();
for (x=0;x<numdatos;++x) repeticiones[x]=0;
for (x=0;x<numdatos;)  {
  x1=x;
  while(datos[x1]==datos[x]) {
	++x;
	repeticiones[x1]++;
	}
  }
indmax=0;
max=repeticiones[indmax];
for (x=0;x<numdatos;x++)  {
  if (repeticiones[x]>max)  {
	indmax=x;
	max=datos[x];
	}
  }
return max;
}

//------------------------------------------------------------------
double mediaaritmetica(void) {
double res;
int x;

for (x=0,res=0.0;x<numdatos;++x) res+=datos[x];
mediaarit=res/numdatos;
return mediaarit;
}



//------------------------------------------------------------------
double amplitud(void)  {
double mayor,menor;
int x;

if (datos[0]>datos[1])  {
  mayor=datos[0];
  menor=datos[1];
  }
else {
  mayor=datos[1];
  menor=datos[0];
  }
for (x=2;x<numdatos;++x)  {
  if (datos[x]>mayor) mayor=datos[x];
  if (datos[x]<menor) menor=datos[x];
  }
return mayor-menor;
}

//------------------------------------------------------------------
double desviacionmedia(void)  {
double suma;
int x;
for (x=0,suma=0.0;x<numdatos;++x) suma+=fabs(mediaarit-datos[x]);
return suma/numdatos;
}


//-----------------------------------------------------------------
double calculavarianza(void) {
double suma;
int x;
for (x=0,suma=0.0;x<numdatos;++x) suma+=pow(mediaarit-datos[x],2);
varianza=suma/numdatos;
return varianza;
}
//------------------------------------------------------------------
double desviacionestandard(void)  {
return sqrt(varianza);
}
//------------------------------------------------------------------
void estadisticas(void) {


clrscr();
if (!haydatos) {
  cprintf("No se han introducido datos\a\r\n");
  getch();
  return;
  }
gotoxy(35,1);
cputs("ESTADISTICAS");
cprintf("\r\nMediana:%lg\r\n",mediana());
cprintf("Moda:%lg\r\n",moda());
cprintf("Media aritmetica:%lg\r\n",mediaaritmetica());
cprintf("Amplitud:%lg\r\n",amplitud());
cprintf("Desviaci¢n media:%lg\r\n",desviacionmedia());
cprintf("Varianza:%lg\r\n",calculavarianza());
cprintf("Desviaci¢n estandard:%lg\r\n",desviacionestandard());
cputs("Pulse tecla\r\n");
getch();
}

//------------------------------------------------------------------

void main(void)  {
int opcion;

haydatos=0;
do {
clrscr();
gotoxy(35,1);
cputs("MENU");
gotoxy(25,6);
cputs("1. INTRODUCIR VALORES");
gotoxy(25,7);
cputs("2. LISTA DE VALORES");
gotoxy(25,8);
cputs("3. ESTADISTICAS");
gotoxy(25,9);
cputs("4. SALIR");
opcion=inputnum(25,11,1,"OPCION:");
switch(opcion) {
	case 1:introducevalores();
		   break;
	case 2:listavalores();
		   break;
	case 3:estadisticas();
		   break;
	}
}while(opcion!=4);
clrscr();
}
