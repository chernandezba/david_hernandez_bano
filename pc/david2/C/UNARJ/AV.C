/* AV.C, ARJ, R JUNG, 03/13/91
/* Display the contents of an ARJ archive
/* Copyright (c) 1990 by Robert K Jung.  All rights reserved.
/*
/* License policy:
/*   You are free to use this code for any purpose as long as you make
/*   mention of its original author.
/*
/* Description:
/*   This program can be compiled under Turbo C 2.0, Turbo C++ 1.0 and
/*   QuickC 2.5.  Use option -DUNIX for UNIX SysV R3.2.
/*
/* Modification history:
/* Date      Programmer  Description of modification.
/* 03/13/91  R. Jung     Added empty directory support.
/* 03/07/91  R. Jung     Improved find_header() to quickly discern non-ARJ
/*                       archives.
/* 02/28/91  R. Jung     Added display of internal archive date-time.
/*                       Removed special case of zero files.
/* 02/21/91  R. Jung     Fixed UCHAR_MAX define.  Fixed get_date_str() for
/*                       years over 1999.  Modification of initialization
/*                       in list_arc().
/* 01/28/91  R. Jung     Fixed ANSI version of strncopy() by testing *from.
/* 01/26/91  A. Dunkel   Changes for AT&T UNIX.
/* 01/10/91  R. Jung     Fixed description of header.
/* 01/03/91  R. Jung     Fixed description of time stamp.
/* 12/09/90  R. Jung     Increased comment size to 2048.  Added length check
/*                       to comment processing.  Corrected copyright display.
/*                       Added more string length checking.
/* 11/15/90  R. Jung     Initial coding.
/*
*/

#include <stdio.h>
#include <limits.h>

#ifdef UNIX
#ifndef UCHAR_MAX
#define UCHAR_MAX       CHAR_MAX
#endif
#define EXIT_FAILURE	(1)
#define EXIT_SUCCESS	(0)
#include <unistd.h>
#include <varargs.h>
#else
#include <stdlib.h>
#include <stdarg.h>
#endif

#include <ctype.h>
#include <string.h>

typedef unsigned char  uchar;   /*  8 bits or more */
typedef unsigned int   uint;    /* 16 - 32 bits or more */
typedef unsigned short ushort;  /* 16 bits or more */
typedef unsigned long  ulong;   /* 32 bits or more */

typedef ulong UCRC;

#define FA_RDONLY	0x01		/* Read only attribute */
#define FA_HIDDEN	0x02		/* Hidden file */
#define FA_SYSTEM	0x04		/* System file */
#define FA_LABEL	0x08		/* Volume label */
#define FA_DIREC	0x10		/* Directory */
#define FA_ARCH		0x20		/* Archive */

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
/*  1  arj flags (0x01 = GARBLED_FLAG) indicates passworded file
/*               (0x02 = RESERVED)
/*               (0x04 = VOLUME_FLAG)  indicates continued file to next volume
/*               (0x08 = EXTFILE_FLAG) indicates file starting position field
/*               (0x10 = PATHSYM_FLAG) indicates path translated
/*  1  method    (0 = stored, 1 = compressed most ... 4 compressed fastest)
/*  1  file type (0 = binary, 1 = text, 2 = comment header)
/*  1  reserved
/*  4  date time stamp modified
/*  4  compressed size
/*  4  original size
/*  4  original file's CRC
/*  2  entryname position in filename
/*  2  file access mode
/*  2  host data (currently not used)
/*  ?  extra data
/*     4 bytes for extended file position when used
/*     (this is present when EXTFILE_FLAG is set)
/*
/*  ?  filename (null-terminated)
/*  ?  comment	(null-terminated)
/*
/*  4  basic header CRC
/*
/*  2  1st extended header size (0 if none)
/*  ?  1st extended header (currently not used)
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

#define NULL_CHAR       '\0'
#define PATH_CHAR       '\\'
#define FNAME_MAX       512
#define MAXSFX        25000
#define OS                0
#define HOST_OS_NAMES  { "MS-DOS","PRIMOS","UNIX","AMIGA","MAC-OS",NULL }

#ifdef UNIX
#define HEADER_ID      60000
#else
#define HEADER_ID      60000U
#endif
#define FIRST_HDR_SIZE    30
#define COMMENT_MAX     2048
#define HEADERSIZE_MAX   (FIRST_HDR_SIZE + 10 + FNAME_MAX + COMMENT_MAX)
#define BINARY_TYPE        0 /* This must line up with binary/text strings */
#define TEXT_TYPE          1
#define COMMENT_TYPE       2
#define DIR_TYPE           3
#define GARBLE_FLAG     0x01
#define VOLUME_FLAG     0x04
#define EXTFILE_FLAG    0x08
#define PATHSYM_FLAG    0x10

#ifdef UNIX
#define CRC_MASK        0xFFFFFFFFL
#else
#define CRC_MASK        0xFFFFFFFFUL
#endif
#define ASCII_MASK      0x7F
#define ARJ_PATH_CHAR   '/'
#ifdef UNIX
#define CRCPOLY         0xEDB88320L
#else
#define CRCPOLY         0xEDB88320UL
#endif

#define UPDATE_CRC(crc, c)  \
        crc = crctable[(uchar)crc ^ (uchar)(c)] ^ (crc >> CHAR_BIT)
#define FIX_PARITY(c)   c &= ASCII_MASK
#define get_tstamp(yy, mm, dd, hr, mn, sc) \
    ( (((ulong)(yy - 1980)) << 25) + ((ulong)mm << 21) + ((ulong)dd << 16) + \
    ((ulong)hr << 11) + (mn << 5) + (sc / 2) )

#define ts_year(ts)  ((uint)((ts >> 25) & 0x7f) + 1980)
#define ts_month(ts) ((uint)(ts >> 21) & 0x0f)      /* 1..12 means Jan..Dec */
#define ts_day(ts)   ((uint)(ts >> 16) & 0x1f)      /* 1..31 means 1st..31st */
#define ts_hour(ts)  ((uint)(ts >> 11) & 0x1f)
#define ts_min(ts)   ((uint)(ts >> 5) & 0x3f)
#define ts_sec(ts)   ((uint)((ts & 0x1f) * 2))

#define get_crc()       get_longword()
#define fget_crc(f)     fget_longword(f)

#define setup_get(PTR)  (get_ptr = (PTR))
#define get_byte()      ((uchar)(*get_ptr++ & 0xff))
#define setup_put(PTR)  (put_ptr = (PTR))

static UCRC   crc;
static FILE   *arcfile;
static long   compsize;
static long   origsize;
static uchar  header[HEADERSIZE_MAX];
static char   arc_name[FNAME_MAX];
static char   filename[FNAME_MAX];
static char   comment[COMMENT_MAX];
static char   *hdr_filename;
static char   *hdr_comment;
static ushort headersize;
static uchar  first_hdr_size;
static uchar  arj_nbr;
static uchar  arj_x_nbr;
static uchar  host_os;
static uchar  arj_flags;
static short  method;
static short  file_type;
static ushort file_mode;
static ulong  time_stamp;
static ulong  arj_time_stamp;
static short  entry_pos;
static ushort host_data;
static uchar  *get_ptr;
static UCRC   file_crc;
static UCRC   header_crc;
static long   torigsize;
static long   tcompsize;

static UCRC   crctable[UCHAR_MAX + 1];

#ifdef UNIX
void error(va_alist)
va_dcl
{
    va_list ap;
    char    *fmt, *s;

    va_start(ap);
    fmt = (char *) va_arg(ap, char *);
    s = (char *) va_arg(ap, char *);

    fputc('\n', stdout);
    fprintf(stdout, fmt, s);
    fputc('\n', stdout);
    va_end(ap);
    exit(EXIT_FAILURE);
}
#else
void
error(char *fmt,...)
{
    va_list args;

    va_start(args, fmt);
    fputc('\n', stdout);
    vfprintf(stdout, fmt, args);
    fputc('\n', stdout);
    va_end(args);
    exit(EXIT_FAILURE);
}
#endif

#ifdef UNIX
void
make_crctable()
#else
void
make_crctable(void)
#endif
{
    uint i, j;
    UCRC r;

    for (i = 0; i <= UCHAR_MAX; i++)
    {
        r = i;
        for (j = 0; j < CHAR_BIT; j++)
        {
            if (r & 1)
                r = (r >> 1) ^ (CRCPOLY);
            else
                r >>= 1;
        }
        crctable[i] = r;
    }
}

#ifdef UNIX
void
crc_buf(str, len)
char   *str;
uint   len;
#else
void
crc_buf(char *str, uint len)
#endif
{
    while (len--)
        UPDATE_CRC(crc, *str++);
}

#ifdef UNIX
void
strparity(p)
uchar   *p;
#else
void
strparity(uchar *p)
#endif
{
    while (*p)
    {
        FIX_PARITY(*p);
        p++;
    }
}

#ifdef UNIX
void
get_date_str(str, tstamp)
char   *str;
ulong  tstamp;
#else
void
get_date_str(char *str, ulong tstamp)
#endif
{
    sprintf(str, "%02u-%02u-%02u %02u:%02u:%02u",
           ts_year(tstamp) % 100, ts_month(tstamp), ts_day(tstamp),
           ts_hour(tstamp), ts_min(tstamp), ts_sec(tstamp));
}

#ifdef UNIX
int
parse_path(pathname)
char   *pathname;
#else
int
parse_path(char *pathname)
#endif
{
    char *cptr, *ptr, *fptr;
    short pos;

    fptr = NULL;
    for (cptr = ":\\"; *cptr; cptr++)
    {
        if ((ptr = strrchr(pathname, *cptr)) != NULL &&
                (fptr == NULL || ptr > fptr))
            fptr = ptr;
    }
    if (fptr == NULL)
        pos = 0;
    else
        pos = fptr + 1 - pathname;
    return pos;
}

#ifdef UNIX
int
decode_path(name)
char	*name;
#else
int
decode_path(char *name)
#endif
{
    for ( ; *name; name++)
    {
        if (*name == ARJ_PATH_CHAR)
            *name = PATH_CHAR;
    }
    return 1;
}

#ifdef UNIX
int
fget_byte(f)
FILE	*f;
#else
int
fget_byte(FILE *f)
#endif
{
    int c;

    if ((c = fgetc(f)) == EOF)
        error("Can't read file or unexpected end of file");
    return c & 0xFF;
}

#ifdef UNIX
uint
fget_word(f)
FILE   *f;
#else
uint
fget_word(FILE *f)
#endif
{
    uint b0, b1;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    return (b1 << 8) + b0;
}

#ifdef UNIX
ulong
fget_longword(f)
FILE  *f;
#else
ulong
fget_longword(FILE *f)
#endif
{
    ulong b0, b1, b2, b3;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    b2 = fget_byte(f);
    b3 = fget_byte(f);
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

#ifdef UNIX
uint
fread_crc(p, n, f)
uchar   *p;
uint	n;
FILE	*f;
#else
uint
fread_crc(uchar *p, uint n, FILE *f)
#endif
{
    n = fread(p, 1, n, f);
    origsize += n;
    crc_buf((char *)p, n);
    return n;
}

#ifdef UNIX
void
get_mode_str(str, mode)
char	*str;
ushort	mode;
#else
void
get_mode_str(char *str, ushort mode)
#endif
{
    strcpy(str, "---W");
    if (mode & FA_ARCH)
        str[0] = 'A';
    if (mode & FA_SYSTEM)
        str[1] = 'S';
    if (mode & FA_HIDDEN)
        str[2] = 'H';
    if (mode & FA_RDONLY)
        str[3] = 'R';
}

#ifdef UNIX
static uint
get_word()
#else
static uint
get_word(void)
#endif
{
    uint b0, b1;

    b0 = get_byte();
    b1 = get_byte();
    return (b1 << 8) + b0;
}

#ifdef UNIX
static ulong
get_longword()
#else
static ulong
get_longword(void)
#endif
{
    ulong b0, b1, b2, b3;

    b0 = get_byte();
    b1 = get_byte();
    b2 = get_byte();
    b3 = get_byte();
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

#ifdef UNIX
static void
strncopy(to, from, len)
char	*to, *from;
int	len;
#else
static void
strncopy(char *to, char *from, int len)
#endif
{
    int i;

    for (i = 1; i < len && *from; i++)
        *to++ = *from++;
    *to = NULL_CHAR;
}

#ifdef UNIX
static long
find_header()
#else
static long
find_header(void)
#endif
{
    long arcpos;
    uchar c;

    for (arcpos = 0; arcpos < MAXSFX; arcpos++)
    {
        fseek(arcfile, arcpos, SEEK_SET);
        c = (uchar)fget_byte(arcfile);
        while (arcpos < MAXSFX)
        {
            if (c != (uchar)HEADER_ID)      /* low order byte first */
                c = (uchar)fget_byte(arcfile);
            else if ((c = (uchar)fget_byte(arcfile)) == ((ushort)HEADER_ID >> 8))
                break;
            arcpos++;
        }
        if ((headersize = fget_word(arcfile)) > 0 && headersize <= HEADERSIZE_MAX)
        {
            crc = CRC_MASK;
            fread_crc(header, headersize, arcfile);
            if ((crc ^ CRC_MASK) == fget_crc(arcfile))
            {
                fseek(arcfile, arcpos, SEEK_SET);
                break;
            }
        }
    }
    return arcpos;
}

#ifdef UNIX
static int
read_header()
#else
static int
read_header(void)
#endif
{
    ushort extheadersize;

    if (fget_word(arcfile) != HEADER_ID)
        error("Bad header");

    if ((headersize = fget_word(arcfile)) == 0)
        return 0;               /* end of archive */
    if (headersize > HEADERSIZE_MAX)
        error("Bad header");

    crc = CRC_MASK;
    fread_crc(header, headersize, arcfile);
    header_crc = fget_crc(arcfile);
    if ((crc ^ CRC_MASK) != header_crc)
        error("Header CRC error");

    setup_get(header);
    first_hdr_size = get_byte();
    arj_nbr = get_byte();
    arj_x_nbr = get_byte();
    host_os = get_byte();
    arj_flags = get_byte();
    method = get_byte();
    file_type = get_byte();
    (void) get_byte();
    time_stamp = get_longword();
    compsize = get_longword();
    origsize = get_longword();
    file_crc = get_crc();
    entry_pos = get_word();
    file_mode = get_word();
    host_data = get_word();

    hdr_filename = (char *)&header[first_hdr_size];
    strncopy(filename, hdr_filename, FNAME_MAX);
    if (host_os != OS)
        strparity((uchar *)filename);
    if ((arj_flags & PATHSYM_FLAG) != 0)
        decode_path(filename);

    hdr_comment = (char *)&header[first_hdr_size + strlen(hdr_filename) + 1];
    strncopy(comment, hdr_comment, sizeof(comment));
    if (host_os != OS)
        strparity((uchar *)comment);

    while ((extheadersize = fget_word(arcfile)) != 0)
        fseek(arcfile, extheadersize + 2, SEEK_CUR);

    return 1;                   /* success */
}

#ifdef UNIX
static void
skip()
#else
static void
skip(void)
#endif
{
    fseek(arcfile, compsize, SEEK_CUR);
}

#ifdef UNIX
uint
ratio(a, b)         /* [(1000a + [b/2]) / b] */
long   a, b;
#else
uint
ratio(long a, long b)         /* [(1000a + [b/2]) / b] */
#endif
{
   int i;

   for (i = 0; i < 3; i++)
       if (a <= LONG_MAX / 10)
           a *= 10;
       else
           b /= 10;
   if ((long) (a + (b >> 1)) < a)
   {
       a >>= 1;
       b >>= 1;
   }
   if (b == 0)
       return 0;
   return (uint) ((a + (b >> 1)) / b);
}

#ifdef UNIX
static void
list_start()
#else
static void
list_start(void)
#endif
{
    printf("Filename     ");
    printf("  Original Compressed Ratio DateTime modified CRC-32   Attr TPMGVX\n");
    printf("------------ ---------- ---------- ----- ----------------- -------- ---- ------\n");
}

#ifdef UNIX
static void
list_arc(count)
int  count;
#else
static void
list_arc(int count)
#endif
{
    uint r;
    int garble_mode, path_mode, volume_mode, extfil_mode, ftype;
    char date_str[20], fmode_str[10];
    static char mode[4] = { 'B', 'T', '?', 'D' };
    static char pthf[2] = { ' ', '+' };
    static char pwdf[2] = { ' ', 'G' };  /* plain, encrypted */
    static char volf[2] = { ' ', 'V' };
    static char extf[2] = { ' ', 'X' };

    if (count == 0)
        list_start();

    garble_mode = ((arj_flags & GARBLE_FLAG) != 0);
    path_mode = (entry_pos > 0);
    volume_mode = ((arj_flags & VOLUME_FLAG) != 0);
    extfil_mode = ((arj_flags & EXTFILE_FLAG) != 0);
    r = ratio(compsize, origsize);
    torigsize += origsize;
    tcompsize += compsize;
    ftype = file_type;
    if (ftype != BINARY_TYPE && ftype != TEXT_TYPE && ftype != DIR_TYPE)
        ftype = 3;
    get_date_str(date_str, time_stamp);
    strcpy(fmode_str, "    ");
    if (host_os == OS)
        get_mode_str(fmode_str, file_mode);
    if (strlen(&filename[entry_pos]) > 12)
        printf("%-12s\n             ", &filename[entry_pos]);
    else
        printf("%-12s ", &filename[entry_pos]);
    printf("%10ld %10ld %u.%03u %s %08lX %4s %c%c%u%c\n",
        origsize, compsize, r / 1000, r % 1000, date_str, file_crc,
        fmode_str, mode[ftype], pthf[path_mode], method, pwdf[garble_mode],
        volf[volume_mode], extf[extfil_mode]);
}

#ifdef UNIX
void
execute_cmd()
#else
void
execute_cmd(void)
#endif
{
    int count;
    uint r;
    char date_str[22];

    torigsize = 0;
    tcompsize = 0;

    if ((arcfile = fopen(arc_name, "rb")) == NULL)
        error("Can't open: %s", arc_name);

    find_header();
    if (!read_header())
        error("Bad archive header");
    arj_time_stamp = time_stamp;

    count = 0;
    while (read_header())
    {
        list_arc(count);
        ++count;
        skip();
    }

    get_date_str(date_str, arj_time_stamp);
    printf("------------ ---------- ---------- ----- -----------------\n");
    r = ratio(tcompsize, torigsize);
    printf(" %5d files %10ld %10ld %u.%03u %s\n",
        count, torigsize, tcompsize, r / 1000, r % 1000, date_str);
    fclose(arcfile);
}

#ifdef UNIX
int
main(argc, argv)
int  argc;
char *argv[];
#else
int
main(int argc, char *argv[])
#endif
{
    int i, j, lastc;

    printf("AV - ARJ file viewer - Rev 1.3, Copyright (c) 1990,91 Robert K Jung\n");

    if (argc != 2)
        error("Usage:  AV archive_name[.ARJ]");

    make_crctable();

    strncopy(arc_name, argv[1], FNAME_MAX);
    i = strlen(arc_name);
    j = parse_path(arc_name);
    lastc = arc_name[i - 1];
    if (lastc == '.')
        arc_name[i - 1] = NULL_CHAR;
    else if (strrchr(&arc_name[j], '.') == NULL)
        strcat(arc_name, ".ARJ");

    execute_cmd();

    return EXIT_SUCCESS;
}

/* end AV.C */
