#include <string.h>
#include <dos.h>
#include <dir.h>
#include <ctype.h>
#include <stdlib.h>
#include <pcventan.h>
#include <pcutil.h>
#include <pcdisco.h>


/*------------------------------------------------------------------*/
unsigned _stklen=8192;
int unidexe;
char pathori[MAXPATH+1],nombretemp[MAXPATH+1],nombrefichero[MAXPATH+1];
/*------------------------------------------------------------------*/
static void near pcmenu_init(void);
/*-------------------------------------------------------------------------*/
void borratemp(void) {

borrafich(nombretemp);

};
/*------------------------------------------------------------------*/
static void near pcmenu_init(void) {
char *pun;

errorespon();
longitudclave=strlen(clave);
strcpy(nombretemp,_argv[0]);
strcpy(nombrefichero,_argv[0]);
pun=strchr(nombretemp,0);
pun-=3;
memcpy(pun,"TEM",3);
pun=strchr(nombrefichero,0);
pun-=3;
memcpy(pun,"DAT",3);
cambiadisco(getdisk());
unidexe=getdisk();
getcwd(pathori,MAXPATH);
strcpy(diractual,pathori);
pcdisco_init();
pun=_argv[1];
if (_argc!=1) {
  if (strlen(_argv[1])==2 && pun[1]==':')  {
	 strupr(pun);
	  if (isalpha(*pun))
		 if (strchr(unidlogicas,*pun)!=NULL)
			cambiadisco(*pun-'A');
  };
};
borratemp();
writeinit();
pcopcion_init();
}
/*-------------------------------------------------------------------------*/
static void pcmenu_fin(void) {
setdisk(unidexe);
chdir(pathori);
if (hacerven(&ventanas[26],colorf2(SALIDADOS))==-1) ponerror(1);
};
/*-------------------------------------------------------------------------*/
int main (void)  {

atexit(writefin);
atexit(pcmenu_fin);
pcmenu_init();
compruebaconfiguracion();
hacerven(&ventanas[6],colorf2(ARBOLV));
buscararbol(cogedisco());
ponerarbol(0,arbol);
mostrardirectorio(cogedisco());
seleccionaopcion();
return(0);
}