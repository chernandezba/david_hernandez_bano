/* UNARJ.C, UNARJ, R JUNG, 04/05/91
/* Main Extractor routine
/* Copyright (c) 1991 by Robert K Jung.  All rights reserved.
/*
/*   This code may be freely used in programs that are NOT archivers.
/*
/* Modification history:
/* Date      Programmer  Description of modification.
/* 04/05/91  R. Jung     Rewrote code.
/*
 */

#include "unarj.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/* Global variables */

UCRC   crc;
FILE   *arcfile;
FILE   *outfile;
ushort bitbuf;
long   compsize;
long   origsize;
uchar  subbitbuf;
uchar  header[HEADERSIZE_MAX];
char   arc_name[FNAME_MAX];
int    bitcount;
int    file_type;
int    error_count;

/* Messages */

char M_VERSION [] = "UNARJ 2.00 Copyright (c) 1991 Robert K Jung\n\n";
char M_USAGE   [] = "Usage:  UNARJ archive[.arj] [NUL]\n";

char M_ARCDATE [] = "Archive date      : %s\n";
char M_BADCOMNT[] = "Invalid comment header";
char M_BADHEADR[] = "Bad header";
char M_BADTABLE[] = "Bad Huffman code (%d)";
char M_CANTOPEN[] = "Can't open %s";
char M_CANTREAD[] = "Can't read file or unexpected end of file";
char M_CANTWRIT[] = "Can't write file. Disk full?";
char M_CRCERROR[] = "CRC error!\n";
char M_CRCOK   [] = "CRC OK\n";
char M_DIFFHOST[] = ", Warning! Binary file from a different OS";
char M_DIR     [] = "directory ";
char M_ENCRYPT [] = "File is password encrypted, ";
char M_ERRORCNT[] = "Found %5d error(s)!";
char M_EXTRACT [] = "Extracting %-12s";
char M_FEXISTS [] = "%-12s exists, ";
char M_HEADRCRC[] = "Header CRC error!";
char M_NBRFILES[] = "%5d file(s)\n";
char M_NOMEMORY[] = "Out of memory";
char M_NOTARJ  [] = "%s is not an ARJ archive";
char M_PROCARC [] = "Processing archive: %s\n";
char M_SKIPPED [] = "Skipped %s\n";
char M_SUFFIX  [] = ARJ_SUFFIX;
char M_TESTING [] = "Testing    %-12s";
char M_UNKNMETH[] = "Unknown method: %d, ";
char M_UNKNTYPE[] = "Unknown file type: %d, ";
char M_UNKNVERS[] = "Unknown version: %d, ";
char M_UNSTORE [] = "Unstoring         ";

#define get_crc()	get_longword()
#define fget_crc(f)	fget_longword(f)

#define setup_get(PTR)	(get_ptr = (PTR))
#define get_byte()      ((uchar)(*get_ptr++ & 0xff))

#define BUFFERSIZE      4096

#define ASCII_MASK      0x7F

#define CRCPOLY         0xEDB88320UL

#if CHAR_BIT == 8
#define UPDATE_CRC(crc, c)  \
        crc = crctable[(uchar)crc ^ (uchar)(c)] ^ (crc >> CHAR_BIT)
#else
#define UPDATE_CRC(crc, c)  \
        crc = crctable[((uchar)(crc) ^ (uchar)(c)) & 0xFF] ^ (crc >> CHAR_BIT)
#endif

/* Local variables */

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
static ushort file_mode;
static ulong  time_stamp;
static short  entry_pos;
static ushort host_data;
static uchar  *get_ptr;
static UCRC   file_crc;
static UCRC   header_crc;

static long   first_hdr_pos;
static int    no_output;

static char   *writemode[2]  = { "wb",	"w" };

static UCRC   crctable[UCHAR_MAX + 1];

/* Functions */

static void
make_crctable(void)
{
    uint i, j;
    UCRC r;

    for (i = 0; i <= UCHAR_MAX; i++)
    {
        r = i;
        for (j = CHAR_BIT; j > 0; j--)
        {
            if (r & 1)
                r = (r >> 1) ^ CRCPOLY;
            else
                r >>= 1;
        }
        crctable[i] = r;
    }
}

static void
crc_buf(char *str, uint len)
{
    while (len--)
        UPDATE_CRC(crc, *str++);
}

void
error(char *fmt,...)
{
    va_list args;

    va_start(args, fmt);
    putc('\n', stdout);
    vprintf(fmt, args);
    putc('\n', stdout);
    va_end(args);
    exit(EXIT_FAILURE);
}

static void
strparity(uchar *p)
{
    while (*p)
    {
        FIX_PARITY(*p);
        p++;
    }
}

static FILE *
fopen_msg(char *name, char *mode)
{
    FILE *fd;

    fd = fopen(name, mode);
    if (fd == NULL)
        error(M_CANTOPEN, name);
    return fd;
}

static int
fget_byte(FILE *f)
{
    int c;

    if ((c = getc(f)) == EOF)
        error(M_CANTREAD);
    return c & 0xFF;
}

static uint
fget_word(FILE *f)
{
    uint b0, b1;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    return (b1 << 8) + b0;
}

static ulong
fget_longword(FILE *f)
{
    ulong b0, b1, b2, b3;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    b2 = fget_byte(f);
    b3 = fget_byte(f);
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

static uint
fread_crc(uchar *p, uint n, FILE *f)
{
    n = fread(p, 1, n, f);
    origsize += n;
    crc_buf((char *)p, n);
    return n;
}

void
fwrite_txt_crc(uchar *p, uint n)
{
    uchar c;

    crc_buf((char *)p, n);
    if (no_output)
        return;

    if (file_type == TEXT_TYPE)
    {
        while (n--)
        {
            c = *p++;
            FIX_PARITY(c);
            if (putc(c, outfile) == EOF)
                error(M_CANTWRIT);
        }
    }
    else
    {
        if (fwrite(p, 1, n, outfile) != n)
            error(M_CANTWRIT);
    }
}

void
init_getbits(void)
{
    bitbuf = 0;
    subbitbuf = 0;
    bitcount = 0;
    fillbuf(2 * CHAR_BIT);
}

void
fillbuf(int n)                /* Shift bitbuf n bits left, read n bits */
{
    bitbuf <<= n;
    while (n > bitcount)
    {
	bitbuf |= subbitbuf << (n -= bitcount);
	if (compsize != 0)
	{
	    compsize--;
	    subbitbuf = (uchar) getc(arcfile);
	}
	else
	    subbitbuf = 0;
	bitcount = CHAR_BIT;
    }
    bitbuf |= subbitbuf >> (bitcount -= n);
}

ushort
getbits(int n)
{
    ushort x;

    x = bitbuf >> (2 * CHAR_BIT - n);
    fillbuf(n);
    return x;
}

static int
decode_path(char *name)
{
    for ( ; *name; name++)
    {
        if (*name == ARJ_PATH_CHAR)
            *name = PATH_CHAR;
    }
    return 1;
}

static void
get_date_str(char *str, ulong tstamp)
{
    sprintf(str, "%04u-%02u-%02u %02u:%02u:%02u",
           ts_year(tstamp), ts_month(tstamp), ts_day(tstamp),
	   ts_hour(tstamp), ts_min(tstamp), ts_sec(tstamp));
}

static int
parse_path(char *pathname, char *path, char *entry)
{
    char *cptr, *ptr, *fptr;
    short pos;

    fptr = NULL;
    for (cptr = PATH_SEPARATORS; *cptr; cptr++)
    {
	if ((ptr = strrchr(pathname, *cptr)) != NULL &&
		(fptr == NULL || ptr > fptr))
	    fptr = ptr;
    }
    if (fptr == NULL)
	pos = 0;
    else
	pos = fptr + 1 - pathname;
    if (path != NULL)
    {
       strncpy(path, pathname, pos);
       path[pos] = NULL_CHAR;
    }
    if (entry != NULL)
       strcpy(entry, &pathname[pos]);
    return pos;
}

static void
strncopy(char *to, char *from, int len)
{
    int i;

    for (i = 1; i < len && *from; i++)
        *to++ = *from++;
    *to = NULL_CHAR;
}

void
strupper(char *s)
{
    while (*s)
    {
	*s = (char)toupper(*s);
	s++;
    }
}

void *
malloc_msg(size_t size)
{
    char *p;

    if ((p = (char *)malloc(size)) == NULL)
        error(M_NOMEMORY);
    return (void *)p;
}

static uint
get_word(void)
{
    uint b0, b1;

    b0 = get_byte();
    b1 = get_byte();
    return (b1 << 8) + b0;
}

static ulong
get_longword(void)
{
    ulong b0, b1, b2, b3;

    b0 = get_byte();
    b1 = get_byte();
    b2 = get_byte();
    b3 = get_byte();
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

static long
find_header(FILE *fd, char *name)
{
    long arcpos;
    int c;

    for (arcpos = ftell(fd); arcpos < MAXSFX; arcpos++)
    {
        fseek(fd, arcpos, SEEK_SET);
        c = fget_byte(fd);
        while (arcpos < MAXSFX)
	{
            if (c != HEADER_ID_LO)  /* low order first */
                c = fget_byte(fd);
            else if ((c = fget_byte(fd)) == HEADER_ID_HI)
		break;
	    arcpos++;
	}
        if ((headersize = fget_word(fd)) <= HEADERSIZE_MAX)
        {
            crc = CRC_MASK;
            fread_crc(header, headersize, fd);
            if ((crc ^ CRC_MASK) == fget_crc(fd))
            {
                fseek(fd, arcpos, SEEK_SET);
                return arcpos;
            }
        }
    }
    error(M_NOTARJ, name);
    return 0;
}

static int
read_header(int first, FILE *fd, char *name)
{
    ushort extheadersize, header_id;

    header_id = fget_word(fd);
    if (header_id != HEADER_ID)
    {
	if (first)
            error(M_NOTARJ, name);
	else
	    error(M_BADHEADR);
    }

    headersize = fget_word(fd);
    if (headersize == 0)
	return 0;		/* end of archive */
    if (headersize > HEADERSIZE_MAX)
	error(M_BADHEADR);

    crc = CRC_MASK;
    fread_crc(header, headersize, fd);
    header_crc = fget_crc(fd);
    if ((crc ^ CRC_MASK) != header_crc)
	error(M_HEADRCRC);

    setup_get(header);
    first_hdr_size = get_byte();
    arj_nbr = get_byte();
    arj_x_nbr = get_byte();
    host_os = get_byte();
    arj_flags = get_byte();
    method = get_byte();
    file_type = get_byte();
    (void)get_byte();
    time_stamp = get_longword();
    compsize = get_longword();
    origsize = get_longword();
    file_crc = get_crc();
    entry_pos = get_word();
    file_mode = get_word();
    host_data = get_word();

    hdr_filename = (char *)&header[first_hdr_size];
    strncopy(filename, hdr_filename, sizeof(filename));
    if (host_os != OS)
        strparity((uchar *)filename);
    if ((arj_flags & PATHSYM_FLAG) != 0)
        decode_path(filename);

    hdr_comment = (char *)&header[first_hdr_size + strlen(hdr_filename) + 1];
    strncopy(comment, hdr_comment, sizeof(comment));
    if (host_os != OS)
        strparity((uchar *)comment);

    while ((extheadersize = fget_word(fd)) != 0)
	fseek(fd, extheadersize + 2, SEEK_CUR);

    return 1;			/* success */
}

static void
skip(void)
{
    fseek(arcfile, compsize, SEEK_CUR);
}

static void
unstore(void)
{
    uint n;
    long pos;
    char *buffer;

    buffer = (char *)malloc_msg(BUFFERSIZE);
    pos = ftell(arcfile);
    n = (uint)(BUFFERSIZE - (pos % BUFFERSIZE));
    n = compsize > (long)n ? n : (uint)compsize;
    while (compsize > 0)
    {
	if (fread(buffer, 1, n, arcfile) != n)
            error(M_CANTREAD);
        putc('.', stdout);
	compsize -= n;
        fwrite_txt_crc((uchar *)buffer, n);
        n = compsize > BUFFERSIZE ? BUFFERSIZE : (uint)compsize;
    }
    free(buffer);
}

static int
check_flags(void)
{
    if (arj_x_nbr > ARJ_X_VERSION)
    {
        printf(M_UNKNVERS, arj_x_nbr);
        printf(M_SKIPPED, filename);
	skip();
	return -1;
    }
    if ((arj_flags & GARBLE_FLAG) != 0)
    {
        printf(M_ENCRYPT);
        printf(M_SKIPPED, filename);
	skip();
	return -1;
    }
    if (method < 0 || method > MAXMETHOD || (method == 4 && arj_nbr == 1))
    {
        printf(M_UNKNMETH, method);
        printf(M_SKIPPED, filename);
	skip();
	return -1;
    }
    if (file_type != BINARY_TYPE && file_type != TEXT_TYPE)
    {
        printf(M_UNKNTYPE, file_type);
        printf(M_SKIPPED, filename);
	skip();
	return -1;
    }
    return 0;
}

static int
extract(void)
{
    if (check_flags())
        return 0;

    if (no_output)
        printf(M_TESTING, filename);
    else
    {
        if (file_exists(filename))
        {
            printf(M_FEXISTS, filename);
            printf(M_SKIPPED, filename);
            skip();
            return 0;
        }
        outfile = fopen(filename, writemode[file_type & 1]);
        if (outfile == NULL)
        {
            printf(M_CANTOPEN, filename);
            putchar('\n');
            skip();
            return 0;
        }
        printf(M_EXTRACT, filename);
        if (host_os != OS && file_type == BINARY_TYPE)
            printf(M_DIFFHOST);
    }

    printf("  ");

    crc = CRC_MASK;
    if (file_type == BINARY_TYPE || file_type == TEXT_TYPE)
    {
        if (method == 0)
            unstore();
        else if (method == 1 || method == 2 || method == 3)
            decode();
        else if (method == 4)
            decode_f();
    }

    if (no_output == 0)
    {
        if (file_type == BINARY_TYPE || file_type == TEXT_TYPE)
            fclose(outfile);
        if (host_os == OS)
            set_fmode(filename, file_mode);
        set_ftime(filename, time_stamp);
    }

    if ((crc ^ CRC_MASK) == file_crc)
        printf(M_CRCOK);
    else
    {
        printf(M_CRCERROR);
	error_count++;
    }
    return 1;
}

static void
execute_cmd(void)
{
    int file_count;
    char date_str[22];

    first_hdr_pos = 0;
    time_stamp = 0;
    first_hdr_size = FIRST_HDR_SIZE;

    arcfile = fopen_msg(arc_name, "rb");

    printf(M_PROCARC, arc_name);

    first_hdr_pos = find_header(arcfile, arc_name);
    fseek(arcfile, first_hdr_pos, SEEK_SET);
    if (!read_header(1, arcfile, arc_name))
        error(M_BADCOMNT);
    get_date_str(date_str, time_stamp);
    printf(M_ARCDATE, date_str);

    file_count = 0;
    while (read_header(0, arcfile, arc_name))
    {
        if (extract())
            file_count++;
    }

    printf(M_NBRFILES, file_count);

    fclose(arcfile);
}

int
main(int argc, char *argv[])
{
    int i, j, lastc;

    printf(M_VERSION);

    no_output = 0;
    if (argc == 3 && strcmp(argv[2], "NUL") == 0)
    {
        no_output = 1;
    }
    else if (argc != 2)
    {
        printf(M_USAGE);
        return EXIT_SUCCESS;
    }

    strncopy(arc_name, argv[1], FNAME_MAX);
    case_path(arc_name);
    i = strlen(arc_name);
    j = parse_path(arc_name, NULL, NULL);
    lastc = arc_name[i - 1];
    if (lastc == ARJ_DOT)
        arc_name[i - 1] = NULL_CHAR;
    else if (strchr(&arc_name[j], ARJ_DOT) == NULL)
        strcat(arc_name, M_SUFFIX);

    make_crctable();

    error_count = 0;
    arcfile = NULL;
    outfile = NULL;

    execute_cmd();

    if (error_count > 0)
	error(M_ERRORCNT, error_count);

    return EXIT_SUCCESS;
}

/* end UNARJ.C */
