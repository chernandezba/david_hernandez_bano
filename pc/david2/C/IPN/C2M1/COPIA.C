#include <dos.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void main(void)  {
struct date dia;
char param[180];

getdate(&dia);
sprintf(param,"xcopy *.c a: /d:%02d-%02d-%02d",
dia.da_mon,dia.da_day,dia.da_year);
printf("\nCopiando los ficheros *.c de hoy ( %02d/%02d/%02d ) a la unidad a:\n",
dia.da_day,dia.da_mon,dia.da_year);
system (param);
}