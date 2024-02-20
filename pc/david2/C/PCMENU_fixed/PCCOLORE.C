extern int adaptador;
#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif
#ifndef __PAS__
#define _CType _Cdecl
#else
#define _CType pascal
#endif

#if	!defined(__COLORS)
#define __COLORS
enum COLORS {
	BLACK,			/* dark colors */
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,		/* light colors */
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};
#endif

#define BLINK		128	/* blink bit */
/* TINTA + (FONDO << 4)
NEGRITA : WHITE + (BLACK << 4)
INVERSO : BLACK + (LIGHTGRAY << 4)
NORMAL  : LIGHTGRAY + (BLACK << 4)
*/
int color2[][2]=  {
{  WHITE + (BLACK << 4),BLUE + (YELLOW << 4)},            /* COLORFONDO  */
{  LIGHTGRAY + (BLACK << 4),WHITE + (BLUE << 4)},         /* MARCO       */
{  WHITE + (BLACK << 4),BLACK + (BLUE << 4)},     	/* MENUPRI     */
{  BLACK + (LIGHTGRAY << 4),WHITE + (CYAN << 4)},         /* LOGOTI    */
{  LIGHTGRAY + (BLACK << 4),WHITE + (CYAN << 4)},         /* VENTAN      */
{  WHITE + (BLACK << 4),BLACK + (RED << 4)},              /* MENUINV     */
{  BLACK + (LIGHTGRAY <<4),WHITE + (BLUE << 4)},          /* CAMBIARCONFIGURA */
{  LIGHTGRAY + (BLACK << 4),WHITE + (BLACK << 4)},        /* SALIDADOS   */
{  LIGHTGRAY + (BLACK << 4),RED + (CYAN << 4)},	        /* TITULO */
{  BLACK + (LIGHTGRAY << 4),RED + (CYAN << 4)},	        /* ARBOLV*/
{  LIGHTGRAY + (BLACK << 4),RED + (BLUE << 4)},	        /* SELECCIONADO*/
{  BLACK + (LIGHTGRAY << 4),WHITE + (BROWN << 4)},        /*VENTSEL*/
{  LIGHTGRAY + (BLACK << 4),WHITE + (RED << 4)},          /*FICHSEL*/
{  BLACK + (LIGHTGRAY << 4),BLACK + (CYAN << 4)},         /*PCERRORE*/
{  WHITE + (BLACK << 4),WHITE + (BLUE << 4)},	        /*UNIDSELECCIO*/
{  BLACK + (LIGHTGRAY << 4),BLACK + (CYAN << 4)},	        /*CAMBUNIDA*/
{  WHITE + (BLACK << 4),WHITE + (CYAN << 4)},	        /*MENUTECLAS*/
{  BLACK + (LIGHTGRAY << 4),WHITE + (CYAN << 4)},	        /*INFODISCO*/
{  WHITE + (BLACK << 4),WHITE + (BLUE << 4)},	        /*SELUNI*/
{  BLACK + (LIGHTGRAY << 4),BLUE + (CYAN << 4)},          /*FICHEROSV*/
{  LIGHTGRAY + (BLACK << 4),BLUE + (RED << 4)},		/*DIRSEL*/
{  BLACK + (LIGHTGRAY << 4),YELLOW + (BLUE << 4)},	/*ERRORN*/
{  BLACK + (LIGHTGRAY << 4),WHITE + (BLUE << 4)},	        /*ERRORC*/
{  LIGHTGRAY + (BLACK << 4),BLACK + (CYAN << 4)},		 /*OPSEL*/
{  BLACK + (LIGHTGRAY << 4),CYAN + (WHITE << 4)},		 /*P_P*/
{  LIGHTGRAY + (BLACK << 4),BLUE + (WHITE << 4)},     /*ILUMINA*/
{  BLACK + (LIGHTGRAY << 4),WHITE + (BLUE << 4)},     /*DESILUMINA*/
};
/* TINTA + (FONDO << 4)
NEGRITA : WHITE + (BLACK << 4)
INVERSO : BLACK + (LIGHTGRAY << 4)
NORMAL  : LIGHTGRAY + (BLACK << 4)
PARPADEO : TINTA + (FONDO << 4) + BLINK)
*/
int color[][2]=  {
{  BLACK + (LIGHTGRAY << 4),WHITE},                       /* COLORFONDO2  */
{  BLACK + (LIGHTGRAY << 4),BLACK + (CYAN << 4)},         /* LOGOTIPO    */
{  LIGHTGRAY + (BLACK << 4),WHITE +(BLACK << 4)},         /* SALIDA      */
{  BLACK + (LIGHTGRAY << 4),BLUE + (WHITE << 4)},         /* MENUOPCIO   */
{  WHITE + (BLACK << 4),WHITE + (CYAN << 4)},           /* MENUOPCIO2  */
  };
