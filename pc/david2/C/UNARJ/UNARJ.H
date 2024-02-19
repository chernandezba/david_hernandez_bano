/* UNARJ.H, UNARJ, R JUNG, 04/05/91
/* Include file
/* Copyright (c) 1990 by Robert K Jung.  All rights reserved.
/*
/*   This code may be freely used in programs that are NOT archivers.
/*
/* Modification history:
/* Date      Programmer  Description of modification.
/* 04/05/91  R. Jung     Rewrote code.
/*
 */

#ifndef _ARH_DEF_
#define _ARH_DEF_

#include <stdio.h>
#include <limits.h>

typedef unsigned char  uchar;	/*  8 bits or more */
typedef unsigned int   uint;	/* 16 - 32 bits or more */
typedef unsigned short ushort;	/* 16 bits or more */
typedef unsigned long  ulong;	/* 32 bits or more */

#define USHRT_BIT   (CHAR_BIT * sizeof(ushort))

/* ********************************************************* */
/* Environment definitions (implementation dependent)        */
/* ********************************************************* */

#ifdef _QC
#define __MSDOS__
#endif

#ifdef __MSDOS__
#define OS		    0
#define WILDSTRING	    "?*"
#define WILD_COMPONENT	    '*'
#define WILD_CHAR	    '?'
#define WILD_ANY	    "*.*"
#define PATH_SEPARATORS     "\\:"
#define PATH_CHAR           '\\'
#define SWITCH_CHARS        "-/"
#define MAXSFX              25000L
#endif

#ifdef __CI
#define PRIME               1
#define OS                  1
#define WILDSTRING          "@+^"
#define WILD_COMPONENT      '@'
#define WILD_CHAR           '+'
#define WILD_ANY            "@@"
#define PATH_SEPARATORS     ">"
#define PATH_CHAR           '>'
#define ARJ_ENVSTR          ".ARJ_SW"
#define LINES_PER_PAGE      24
#define FIX_PARITY(c)       c |= ~ASCII_MASK
#define DEFAULT_DIR         "*>"
#endif

/* Error levels */

#ifndef ERROR_DEFINES

#define ERROR_OK        0       /* success */
#define ERROR_WARN      1       /* minor problem (file not found) */
#define ERROR_FAIL      2       /* fatal error */
#define ERROR_CRC       3       /* CRC error */
#define ERROR_SECURE    4       /* ARJ security invalid or not found */
#define ERROR_WRITE     5       /* disk full */
#define ERROR_OPEN      6       /* can't open file */
#define ERROR_USER      7       /* user specified bad parameters */
#define ERROR_MEMORY    8       /* not enough memory */

#endif

#ifndef MAXSFX		    /* size of self-extracting prefix */
#define MAXSFX		    500000L
#endif
#ifndef FNAME_MAX
#define FNAME_MAX	    512
#endif
#ifndef SWITCH_CHARS
#define SWITCH_CHARS        "-"
#endif
#ifndef FIX_PARITY
#define FIX_PARITY(c)	    c &= ASCII_MASK
#endif
#ifndef ARJ_SUFFIX
#define ARJ_SUFFIX	    ".ARJ"
#endif
#ifndef BAK_SUFFIX
#define BAK_SUFFIX	    ".BAK"
#endif
#ifndef ARJ_TEMP
#define ARJ_TEMP            "ARJTEMP.$??"
#endif
#ifndef ARJ_TEMPLATE
#define ARJ_TEMPLATE        "ARJTEMP.$%02d"     /* related to ARJ_TEMP */
#endif
#ifndef ARJSORT_TEMP
#define ARJSORT_TEMP        "ARJSORT.$$$"
#endif
#ifndef ARJ_ENVSTR
#define ARJ_ENVSTR	    "ARJ_SW"
#endif
#ifndef ARJ_DOT
#define ARJ_DOT 	    '.'
#endif
#ifndef ARCHIVE_SUFFIXES
#define ARCHIVE_SUFFIXES    ".ARJ.ZIP.PAK.ARC.LZH"
#endif
#ifndef LINES_PER_PAGE
#define LINES_PER_PAGE	    24
#endif
#ifndef WILDSTRING
#define WILDSTRING	    "?*"
#endif
#ifndef WILD_COMPONENT
#define WILD_COMPONENT	    '*'
#endif
#ifndef WILD_CHAR
#define WILD_CHAR	    '?'
#endif
#ifndef WILD_ANY
#define WILD_ANY	    "*.*"
#endif
#ifndef NOT_MATCH_ANY
#define NOT_MATCH_ANY       "..."
#endif
#ifndef LIST_CHAR
#define LIST_CHAR	    '!'
#endif
#ifndef DEFAULT_DIR
#define DEFAULT_DIR	    ""
#endif

/* ********************************************************* */
/* end of environmental defines                              */
/* ********************************************************* */

/* ********************************************************* */
/*
/* Structure of archive block (low order byte first):
/*
/*  2  header id (comment and local file) = 0xEA60 or 60000U
/*  2  basic header size (from 'first_hdr_size' thru 'comment' below)
/*	     = first_hdr_size + strlen(filename) + 1 + strlen(comment) + 1
/*	     = 0 if end of archive
/*
/*  1  first_hdr_size (size up to 'extra data')
/*  1  archiver version number
/*  1  minimum archiver version to extract
/*  1  host OS	 (0 = MSDOS, 1 = PRIMOS, 2 = UNIX, 3 = AMIGA, 4 = MACDOS)
/*  1  arj flags (0x01 = GARBLED_FLAG, 0x02 = RESERVED)
/*               (0x04 = VOLUME_FLAG,  0x08 = EXTFILE_FLAG)
/*               (0x10 = PATHSYM_FLAG)
/*  1  method    (0 = stored, 1 = compressed most ... 4 compressed fastest)
/*  1  file type (0 = binary, 1 = text, 2 = comment header)
/*  1  garble password modifier
/*  4  date time stamp modified
/*  4  compressed size
/*  4  original size
/*  4  original file's CRC
/*  2  entryname position in filename
/*  2  file access mode
/*  2  host data
/*  ?  extra data
/*     4 bytes for extended file position
/*
/*  ?  filename (null-terminated)
/*  ?  comment	(null-terminated)
/*
/*  4  basic header CRC
/*
/*  2  1st extended header size (0 if none)
/*  ?  1st extended header
/*  4  1st extended header's CRC
/*  ...
/*  ?  compressed file
/*
/* ********************************************************* */

/* ********************************************************* */
/*							     */
/*     Time stamp format:				     */
/*							     */
/*	31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16      */
/*     |<---- year-1980 --->|<- month ->|<--- day ---->|     */
/*							     */
/*	15 14 13 12 11 10  9  8  7  6  5  4  3	2  1  0      */
/*     |<--- hour --->|<---- minute --->|<- second/2 ->|     */
/*							     */
/* ********************************************************* */

#define CODE_BIT          16

#define NULL_CHAR	'\0'
#define MAXMETHOD	   4

#define ARJ_VERSION        3
#define ARJ_X_VERSION      3    /* decoder version */
#define ARJ_X1_VERSION     1
#define DEFAULT_METHOD	   1
#define DEFAULT_TYPE       0    /* if type_sw is selected */
#define HEADER_ID     0xEA60U
#define HEADER_ID_HI    0xEA
#define HEADER_ID_LO    0x60
#define FIRST_HDR_SIZE	  30
#define FIRST_HDR_SIZE_V  34
#define COMMENT_MAX     2048
#define HEADERSIZE_MAX	 (FIRST_HDR_SIZE + 10 + FNAME_MAX + COMMENT_MAX)
#define BINARY_TYPE	   0	/* This must line up with binary/text strings */
#define TEXT_TYPE	   1
#define COMMENT_TYPE	   2
#define DIR_TYPE           3

#define GARBLE_FLAG     0x01
#define VOLUME_FLAG	0x04
#define EXTFILE_FLAG	0x08
#define PATHSYM_FLAG    0x10

typedef ulong UCRC;	/* CRC-32 */

#define CRC_MASK        0xFFFFFFFFUL

#define ARJ_PATH_CHAR   '/'

/* Timestamp macros */

#define get_tstamp(yy, mm, dd, hr, mn, sc) \
    ( (((ulong)(yy - 1980)) << 25) + ((ulong)mm << 21) + ((ulong)dd << 16) + \
    ((ulong)hr << 11) + (mn << 5) + (sc / 2) )

#define ts_year(ts)  ((uint)((ts >> 25) & 0x7f) + 1980)
#define ts_month(ts) ((uint)(ts >> 21) & 0x0f)	    /* 1..12 means Jan..Dec */
#define ts_day(ts)   ((uint)(ts >> 16) & 0x1f)	    /* 1..31 means 1st..31st */
#define ts_hour(ts)  ((uint)(ts >> 11) & 0x1f)
#define ts_min(ts)   ((uint)(ts >> 5) & 0x3f)
#define ts_sec(ts)   ((uint)((ts & 0x1f) * 2))

/* unarj.c */

extern long origsize;
extern long compsize;

extern UCRC crc;

extern FILE *arcfile;
extern FILE *outfile;

extern ushort bitbuf;

extern uchar subbitbuf;
extern uchar header[HEADERSIZE_MAX];

extern char arc_name[FNAME_MAX];

extern int bitcount;
extern int file_type;
extern int error_count;

void   strupper(char *str);
void   *malloc_msg(size_t size);
void   error(char *fmt, ...);
void   fillbuf(int n);
ushort getbits(int n);
void   fwrite_txt_crc(uchar *p, uint n);
void   init_getbits(void);

/* environ.c */

void   case_path(char *name);
int    file_exists(char *name);
int    fix_path(char *to_name, char *fr_name);
int    set_ftime(char *name, ulong timestamp);
int    set_fmode(char *name, ushort fmode);

/* decode.c */

void    decode(void);
void    decode_f(void);

/* Message strings */

extern char M_VERSION [];
extern char M_USAGE   [];

extern char M_ARCDATE [];
extern char M_BADCOMNT[];
extern char M_BADHEADR[];
extern char M_BADTABLE[];
extern char M_CANTOPEN[];
extern char M_CANTREAD[];
extern char M_CANTWRIT[];
extern char M_CRCERROR[];
extern char M_CRCOK   [];
extern char M_DIFFHOST[];
extern char M_DIR     [];
extern char M_ENCRYPT [];
extern char M_ERRORCNT[];
extern char M_EXTRACT [];
extern char M_FEXISTS [];
extern char M_HEADRCRC[];
extern char M_NBRFILES[];
extern char M_NOMEMORY[];
extern char M_NOTARJ  [];
extern char M_PROCARC [];
extern char M_SKIPPED [];
extern char M_SUFFIX  [];
extern char M_TESTING [];
extern char M_UNKNMETH[];
extern char M_UNKNTYPE[];
extern char M_UNKNVERS[];
extern char M_UNSTORE [];

#endif

/* end UNARJ.H */
