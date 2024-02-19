/* DECODE.C, UNARJ, R JUNG, 04/05/91
/* Decode ARJ archive
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
#include <string.h>

#define THRESHOLD    3
#define DDICSIZ      26624
#define MAXDICBIT   16
#define MATCHBIT     8
#define MAXMATCH   256	    /* not more than UCHAR_MAX + 1 */
#define NC	    (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define NP          (MAXDICBIT + 1)
#define CBIT         9
#define NT          (CODE_BIT + 3)
#define PBIT         5      /* smallest integer such that (1U << PBIT) > NP */
#define TBIT         5      /* smallest integer such that (1U << TBIT) > NT */

#if NT > NP
#define NPT NT
#else
#define NPT NP
#endif

#define CTABLESIZE  4096

#define STRTP          9
#define STOPP         13

#define STRTL          0
#define STOPL          7

/* Local variables */

static short  getlen;
static short  getbuf;

static ushort left[2 * NC - 1];
static ushort right[2 * NC - 1];
static uchar  c_len[NC];
static uchar  pt_len[NPT];

static ushort c_table[CTABLESIZE];
static ushort pt_table[256];
static ushort blocksize;

/* Huffman decode routines */

static void
make_table(int nchar, uchar bitlen[], int tablebits, ushort table[])
{
    ushort count[17], weight[17], start[18], *p;
    uint i, k, len, ch, jutbits, avail, nextcode, mask;

    for (i = 1; i <= 16; i++)
	count[i] = 0;
    for (i = 0; (int)i < nchar; i++)
	count[bitlen[i]]++;

    start[1] = 0;
    for (i = 1; i <= 16; i++)
	start[i + 1] = start[i] + (count[i] << (16 - i));
    if (start[17] != (ushort) (1U << 16))
        error(M_BADTABLE, 0);

    jutbits = 16 - tablebits;
    for (i = 1; (int)i <= tablebits; i++)
    {
	start[i] >>= jutbits;
	weight[i] = 1U << (tablebits - i);
    }
    while (i <= 16)
	weight[i++] = 1U << (16 - i);

    i = start[tablebits + 1] >> jutbits;
    if (i != (ushort) (1U << 16))
    {
	k = 1U << tablebits;
	while (i != k)
	    table[i++] = 0;
    }

    avail = nchar;
    mask = 1U << (15 - tablebits);
    for (ch = 0; (int)ch < nchar; ch++)
    {
	if ((len = bitlen[ch]) == 0)
	    continue;
	nextcode = start[len] + weight[len];
        if ((int)len <= tablebits)
	{
	    for (i = start[len]; i < nextcode; i++)
		table[i] = ch;
	}
	else
	{
	    k = start[len];
	    p = &table[k >> jutbits];
	    i = len - tablebits;
	    while (i != 0)
	    {
		if (*p == 0)
		{
		    right[avail] = left[avail] = 0;
		    *p = avail++;
		}
		if (k & mask)
		    p = &right[*p];
		else
		    p = &left[*p];
		k <<= 1;
		i--;
	    }
	    *p = ch;
	}
	start[len] = nextcode;
    }
}

static void
read_pt_len(short nn, short nbit, short i_special)
{
    short i, c, n;
    ushort mask;

    n = getbits(nbit);
    if (n == 0)
    {
        c = getbits(nbit);
        for (i = 0; i < nn; i++)
            pt_len[i] = 0;
        for (i = 0; i < 256; i++)
            pt_table[i] = c;
    }
    else
    {
        i = 0;
        while (i < n)
        {
            c = bitbuf >> (16 - 3);
            if (c == 7)
            {
		mask = 1U << (16 - 4);
		while (mask & bitbuf)
                {
                    mask >>= 1;
                    c++;
                }
            }
            fillbuf((c < 7) ? 3 : c - 3);
	    pt_len[i++] = (uchar)c;
            if (i == i_special)
            {
                c = getbits(2);
                while (--c >= 0)
                    pt_len[i++] = 0;
            }
        }
        while (i < nn)
            pt_len[i++] = 0;
        make_table(nn, pt_len, 8, pt_table);
    }
}

static void
read_c_len(void)
{
    short i, c, n;
    ushort mask;

    n = getbits(CBIT);
    if (n == 0)
    {
        c = getbits(CBIT);
        for (i = 0; i < NC; i++)
            c_len[i] = 0;
        for (i = 0; i < CTABLESIZE; i++)
            c_table[i] = c;
    }
    else
    {
        i = 0;
        while (i < n)
        {
            c = pt_table[bitbuf >> (16 - 8)];
            if (c >= NT)
            {
		mask = 1U << (16 - 9);
		do
                {
                    if (bitbuf & mask)
                        c = right[c];
                    else
                        c = left[c];
                    mask >>= 1;
                } while (c >= NT);
            }
            fillbuf(pt_len[c]);
            if (c <= 2)
            {
                if (c == 0)
                    c = 1;
                else if (c == 1)
                    c = getbits(4) + 3;
                else
                    c = getbits(CBIT) + 20;
                while (--c >= 0)
                    c_len[i++] = 0;
            }
            else
		c_len[i++] = (uchar)(c - 2);
        }
        while (i < NC)
            c_len[i++] = 0;
        make_table(NC, c_len, 12, c_table);
    }
}

static ushort
decode_c(void)
{
    ushort j, mask;

    if (blocksize == 0)
    {
        blocksize = getbits(16);
        read_pt_len(NT, TBIT, 3);
        read_c_len();
        read_pt_len(NP, PBIT, -1);
    }
    blocksize--;
    j = c_table[bitbuf >> 4];
    if (j >= NC)
    {
        mask = 1U << (16 - 1 - 12);
	do
	{
	    if (bitbuf & mask)
		j = right[j];
	    else
		j = left[j];
	    mask >>= 1;
	} while (j >= NC);
    }
    fillbuf(c_len[j]);
    return j;
}

static ushort
decode_p(void)
{
    ushort j, mask;

    j = pt_table[bitbuf >> (16 - 8)];
    if (j >= NP)
    {
        mask = 1U << (16 - 1 - 8);
	do
	{
	    if (bitbuf & mask)
		j = right[j];
	    else
		j = left[j];
	    mask >>= 1;
	} while (j >= NP);
    }
    fillbuf(pt_len[j]);
    if (j != 0)
    {
        j--;
        j = (1U << j) + getbits(j);
    }
    return j;
}

static void
decode_start(void)
{
    blocksize = 0;
    init_getbits();
}

void
decode(void)
{
    short i;
    short j;
    short c;
    short r;
    uchar *text;
    long count;

    text = (uchar *)malloc_msg(DDICSIZ);

    decode_start();
    count = 0;
    r = 0;

    while (count < origsize)
    {
        if ((c = decode_c()) <= UCHAR_MAX)
        {
	    text[r] = (uchar) c;
            count++;
            if (++r >= DDICSIZ)
            {
                r = 0;
                putc('.', stdout);
                fwrite_txt_crc(text, DDICSIZ);
            }
        }
        else
        {
            j = c - (UCHAR_MAX + 1 - THRESHOLD);
            count += j;
            i = decode_p();
            if ((i = r - i - 1) < 0)
                i += DDICSIZ;
            if (r > i && r < DDICSIZ - MAXMATCH - 1)
            {
                while (--j >= 0)
                    text[r++] = text[i++];
            }
            else
            {
                while (--j >= 0)
                {
                    text[r] = text[i];
                    if (++r >= DDICSIZ)
                    {
                        r = 0;
                        putc('.', stdout);
                        fwrite_txt_crc(text, DDICSIZ);
                    }
                    if (++i >= DDICSIZ)
                        i = 0;
                }
            }
        }
    }
    if (r != 0)
        fwrite_txt_crc(text, r);

exit:
    free(text);
}

/* Macros */

#define GETBIT(c)                       \
{                                       \
    if (getlen <= 0)                    \
    {                                   \
        getbuf |= bitbuf >> getlen;     \
        fillbuf(CODE_BIT-getlen);       \
        getlen = CODE_BIT;              \
    }                                   \
    c = getbuf < 0;                     \
    getbuf <<= 1;                       \
    getlen--;                           \
}

#define GETBITS(c, len)                 \
{                                       \
    if (getlen < (len))                 \
    {                                   \
        getbuf |= bitbuf >> getlen;     \
        fillbuf(CODE_BIT-getlen);       \
        getlen = CODE_BIT;              \
    }                                   \
    c = (ushort)getbuf>>(CODE_BIT-(len));\
    getbuf <<= (len);                   \
    getlen -= (len);                    \
}

static short
decode_ptr(void)
{
    short c;
    short width;
    short plus;
    short pwr;

    plus = 0;
    pwr = 1 << (STRTP);
    for (width = (STRTP); width < (STOPP) ; width++)
    {
        GETBIT(c);
        if (c == 0)
            break;
        plus += pwr;
        pwr <<= 1;
    }
    if (width != 0)
        GETBITS(c, width);
    c += plus;
    return c;
}

static short
decode_len(void)
{
    short c;
    short width;
    short plus;
    short pwr;

    plus = 0;
    pwr = 1 << (STRTL);
    for (width = (STRTL); width < (STOPL) ; width++)
    {
        GETBIT(c);
        if (c == 0)
            break;
        plus += pwr;
        pwr <<= 1;
    }
    if (width != 0)
        GETBITS(c, width);
    c += plus;
    return c;
}

void
decode_f(void)
{
    short i;
    short j;
    short c;
    short r;
    short pos;
    long count;
    uchar *text;

    text = (uchar *)malloc_msg(DDICSIZ);

    init_getbits();
    getlen = getbuf = 0;
    count = 0;
    r = 0;

    while (count < origsize)
    {
        c = decode_len();
        if (c == 0)
        {
            GETBITS(c, CHAR_BIT);
	    text[r] = (uchar)c;
            count++;
            if (++r >= DDICSIZ)
            {
                r = 0;
                putc('.', stdout);
                fwrite_txt_crc(text, DDICSIZ);
            }
        }
        else
        {
            j = c - 1 + THRESHOLD;
            count += j;
            pos = decode_ptr();
            if ((i = r - pos - 1) < 0)
                i += DDICSIZ;
            while (j-- > 0)
            {
                text[r] = text[i];
                if (++r >= DDICSIZ)
                {
                    r = 0;
                    putc('.', stdout);
                    fwrite_txt_crc(text, DDICSIZ);
                }
                if (++i >= DDICSIZ)
                    i = 0;
            }
        }
    }
    if (r != 0)
        fwrite_txt_crc(text, r);

exit:
    free(text);
}

/* end DECODE.C */
