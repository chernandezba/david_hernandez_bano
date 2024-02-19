//Programa para hacer estadisticas de una clase , con edad promedio , notas
//finales ,hombres y mujeres , etc.
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define MAXDATOS 100

unsigned int haydatos,numdatos;
float varianza,mediaarit;
struct CLASE  {
char grupo[5];
int edad;
char sexo;
float califfinal;
char nombre[60];
} *clase;


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
char buffer[80];
double aux;


clrscr();
do  {
  putch('\r');
  clreol();
  cprintf("N£mero de alumnos(M ximo %d,0 salir):",MAXDATOS);
  numdatos=inputnum(wherex(),wherey(),3,"");
  if (!numdatos) return;
  }while(numdatos>MAXDATOS);
if (haydatos) free(clase);
if ((clase=(struct CLASE *)calloc(sizeof(struct CLASE),numdatos))==NULL) nohaymemoria();
haydatos=1;
for (x=0;x<numdatos;++x)  {
  cprintf("\r\nAlumno n£mero %d:",x+1);
  cputs("\r\nNombre : ");
  buffer[0]=61;
  cgets(buffer);
  strcpy(clase[x].nombre,&buffer[2]);
  cputs("\r\nEdad :");
  buffer[0]=3;
  cgets(buffer);
  clase[x].edad=atoi(&buffer[2]);
  cputs("\r\nGrupo :");
  buffer[0]=6;
  cgets(buffer);
  strcpy(clase[x].grupo,&buffer[2]);
  do {
  cputs("\r\nSexo(M/F) :");
  buffer[0]=2;
  cgets(buffer);
  clase[x].sexo=toupper(buffer[2]);
  }while (clase[x].sexo!='M' && clase[x].sexo!='F');
  cputs("\n\rCalificaci¢n final:");
  buffer[0]=5;
  cgets(buffer);
  clase[x].califfinal=atof(&buffer[2]);
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
  cprintf("Dato n£mero %d:\r\n",x+1);
  cprintf("Nombre :%s\r\n",clase[x].nombre);
  cprintf("Edad:%d\r\n",clase[x].edad);
  cprintf("Sexo:%s\r\n",(clase[x].sexo=='M') ? "Masculino" : "Femenino");
  cprintf("Calificaci¢n final:%g\r\n",clase[x].califfinal);
  if (wherey()+4>=info.screenheight)  {
	cputs("Pulse tecla para mas datos ...");
	getch();
	clrscr();
	}
  }
cputs("Pulse tecla ...");
getch();
}

//------------------------------------------------------------------
float mediaaritmetica(void) {
//para la edad promedio
int res;
int x;

for (x=0,res=0;x<numdatos;++x) res+=clase[x].edad;
return (float)res/numdatos;
}
//------------------------------------------------------------------
float mediaaritmetica2(void) {
//para la calificacion final
float res;
int x;

for (x=0,res=0;x<numdatos;++x) res+=clase[x].califfinal;
mediaarit=res/numdatos;
return mediaarit;
}


//------------------------------------------------------------------
void hombresmujeres(int *hombres,int *mujeres)  {
int x;

*hombres=0;
*mujeres=0;
for (x=0;x<numdatos;++x)  {
  if (clase[x].sexo=='M') (*hombres)++;
  if (clase[x].sexo=='F') (*mujeres)++;
  }
}

//------------------------------------------------------------------
float mediana(void)  {
//de la calificaci¢n final
float *datos,aux;
int x,j,i,xx;

//copia las calificaciones finales en datos
if ((datos=(float *)calloc(numdatos,sizeof(float)))==NULL) nohaymemoria();
for (x=0;x<numdatos;++x)  datos[x]=clase[x].califfinal;
xx=wherex();
cputs("ordenando calificaciones finales ...");
//ordena los datos
for (i=0;i<numdatos;i++)
  for (j=0;j<numdatos-i-1;j++) {
	 if (datos[j]>datos[j+1]) {
		aux=datos[j];
		datos[j]=datos[j+1];
	  datos[j+1]=aux;
	  }
	}
i=(numdatos >> 1)-1;
j=i+1;
if (!(numdatos & 1)) aux=(clase[i].califfinal+clase[j].califfinal) / 2;
else aux=clase[(numdatos >> 1)+1].califfinal;
gotoxy(xx,wherey());
clreol();
free(datos);
return aux;
}

//------------------------------------------------------------------
float moda(void)  {
//de la calificaci¢n final
int *repeticiones,x,x1,indmax;
float max;

if ((repeticiones=(int *)calloc(numdatos,sizeof(int)))==NULL) nohaymemoria();
for (x=0;x<numdatos;++x) repeticiones[x]=0;
for (x=0;x<numdatos;++x)  {
  for (x1=x;x1<numdatos;++x1)
	if (clase[x].califfinal==clase[x1].califfinal) repeticiones[x]++;
  }
indmax=0;
max=repeticiones[indmax];
for (x=0;x<numdatos;x++)  {
  if (repeticiones[x]>max)  {
	indmax=x;
	max=repeticiones[x];
	}
  }
free(repeticiones);
return clase[indmax].califfinal;
}
//------------------------------------------------------------------
float amplitud(void)  {
//de la calificaci¢n final
float mayor,menor;
int x;

if (clase[0].califfinal>clase[1].califfinal)  {
  mayor=clase[0].califfinal;
  menor=clase[1].califfinal;
  }
else {
  mayor=clase[1].califfinal;
  menor=clase[0].califfinal;
  }
for (x=2;x<numdatos;++x)  {
  if (clase[x].califfinal>mayor) mayor=clase[x].califfinal;
  if (clase[x].califfinal<menor) menor=clase[x].califfinal;
  }
return mayor-menor;
}

//------------------------------------------------------------------
float desviacionmedia(void)  {
//de la calificaci¢n final
float suma;
int x;
for (x=0,suma=0.0;x<numdatos;++x) suma+=fabs(mediaarit-clase[x].califfinal);
return suma/numdatos;
}


//-----------------------------------------------------------------
float calculavarianza(void) {
//de la calificaci¢n final
float suma;
int x;
for (x=0,suma=0.0;x<numdatos;++x) suma+=pow(mediaarit-clase[x].califfinal,2);
varianza=suma/numdatos;
return varianza;
}
//------------------------------------------------------------------
double desviacionestandard(void)  {
//de la calificaci¢n final
return sqrt(varianza);
}

//------------------------------------------------------------------
void estadisticas(void) {
int hombres,mujeres;


clrscr();
if (!haydatos) {
  cprintf("No se han introducido datos\a\r\n");
  getch();
  return;
  }
gotoxy(29,1);
cputs("ESTADISTICAS DE LA CLASE\r\n\n");
cprintf("\r\nEdad media de la clase:%g\r\n",mediaaritmetica());
hombresmujeres(&hombres,&mujeres);
cprintf("Hombres de la clase:%d\r\n",hombres);
cprintf("Mujeres de la clase:%d\r\n",mujeres);
cprintf("Mediana de la calificaci¢n final:%g\r\n",mediana());
cprintf("Moda de la calificaci¢n final:%g\r\n",moda());
cprintf("Media aritmetica de la calificaci¢n final:%lg\r\n",mediaaritmetica2());
cprintf("Amplitud de la calificaci¢n final:%g\r\n",amplitud());
cprintf("Desviaci¢n media de la calificaci¢n final:%g\r\n",desviacionmedia());
cprintf("Varianza de la calificaci¢n final:%g\r\n",calculavarianza());
cprintf("Desviaci¢n estandard de la calificaci¢n final:%g\r\n",desviacionestandard());
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
cputs("1. INTRODUCIR DATOS DE LA CLASE");
gotoxy(25,7);
cputs("2. LISTA DE DATOS DE LA CLASE");
gotoxy(25,8);
cputs("3. ESTADISTICAS DE LA CLASE");
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
