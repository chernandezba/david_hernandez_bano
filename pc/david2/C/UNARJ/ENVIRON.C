/* ENVIRON.C, UNARJ, R JUNG, 04/09/91
/* Implementation dependent routines
/* Copyright (c) 1991 by Robert K Jung.  All rights reserved.
/*
/*   This code may be freely used in programs that are NOT archivers.
/*
/* Modification history:
/* Date      Programmer  Description of modification.
/* 04/09/91  R. Jung     Rewrote code.
/*
 */

#include "unarj.h"

#ifdef __TURBOC__

#define SUBS_DEFINED

#include <string.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>

void
case_path(char *name)
{
    strupper(name);
}

int
file_exists(char *name)
{
    return (access(name, 0) == 0);
}

int
fix_path(char *to_name, char *fr_name)
{
    char *p;

    p = fr_name;
    if (p[0] && p[1] == ':')        /* get rid of drive */
        p += 2;
    if (p[0] == '.')
    {
        if (p[1] == '.' && p[2] == '\\')
           p += 2;
        else if (p[1] == '\\')
            p++;
    }
    if (p[0] == '\\')               /* get rid of root dir */
        p++;
    if (to_name != NULL)
    {
        strcpy(to_name, p);
        strupper(p);
    }
    return (int)(p - fr_name);
}

int
set_fmode(char *name, ushort attribute)
{
    if (_chmod(name, 1, attribute) == -1)
        return -1;
    return 0;
}

int
set_ftime(char *name, ulong tstamp)
{
    FILE *fd;
    int code;

    if ((fd = fopen(name, "rb")) == NULL)
	return -1;
    code = setftime(fileno(fd), (struct ftime *) &tstamp);
    fclose(fd);
    return code;
}

#endif

#ifdef _QC

#define SUBS_DEFINED

#include <string.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>

void
case_path(char *name)
{
    strupper(name);
}

int
file_exists(char *name)
{
    return (access(name, 0) == 0);
}

int
fix_path(char *to_name, char *fr_name)
{
    char *p;

    p = fr_name;
    if (p[0] && p[1] == ':')        /* get rid of drive */
        p += 2;
    if (p[0] == '\\')               /* get rid of root dir */
        p++;
    if (to_name != NULL)
    {
        strcpy(to_name, p);
        strupper(p);
    }
    return (int)(p - fr_name);
}

int
set_fmode(char *name, ushort attribute)
{
    return _dos_setfileattr(name, (uint)attribute);
}

int
set_ftime(char *name, ulong tstamp)
{
    FILE *fd;
    int code;
    uint date_stamp, time_stamp;
                             
    date_stamp = (uint)(tstamp >> 16);
    time_stamp = (uint)(tstamp & 0xFFFF);
    if ((fd = fopen(name, "rb")) == NULL)
	return -1;
    code = _dos_setftime(fileno(fd), date_stamp, time_stamp);
    fclose(fd);
    return code;
}

#endif

#ifndef SUBS_DEFINED       /* vanilla version for other compilers */

#include <string.h>

void
case_path(char *name)
{
}

int
file_exists(char *name)
{
    FILE *fd;

    if ((fd = fopen(name, "rb")) == NULL)
	return 0;
    fclose(fd);
    return 1;
}

int
fix_path(char *to_name, char *fr_name)
{
    strcpy(to_name, fr_name);
    return 0;
}

int
set_fmode(char *name, ushort attribute)
{
    return 0;
}

int
set_ftime(char *name, ulong tstamp)
{
    return 0;
}

#endif  /* end of vanilla section */

/* end ENVIRON.C */
