#ifndef _PCEJEC_H
  #ifndef __PCUTIL.H__
  #include <pcutil.h>
  #endif
#define _PCEJEC_H
/*------------------------------------------------------------------*/
//estructura a guardar en pcmenu.tem para ejecutar programas
struct ejecutar {
  char directorio[LONGITUDDIRECTORIO];
  char ejecutable[LONGITUDEJECUTABLE];
  char parametros[LONGITUDPARAMETROS];
  };
#endif