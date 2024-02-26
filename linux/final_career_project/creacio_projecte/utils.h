//fitxer utils.h
//funcions utils
#ifndef __UTILS_H
#define __UTILS_H



//funcions
unsigned long int iptolong(unsigned char ip0,unsigned char ip1,unsigned char ip2,
unsigned char ip3);
void longtoip(unsigned long int ip,char *p0,char *p1,char *p2,char *p3);
void posa_error(int numerr);
void afegeixportaurl(char *purl,unsigned int pport,char *purlambport);
void aminuscules(char *pcad);
void treu_crlf(char *pcadena);

#endif
