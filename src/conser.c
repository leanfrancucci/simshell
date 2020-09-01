/* --------------------------------------------------------------------------
 *
 *                       Simple Shell for Embedded Systems
 *                       ---------------------------------
 *
 *                       Suitable for tiny embedded systems
 *
 *                      Copyright (c) 2020 Leandro Francucci
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Site: https://vortexmakes.com/
 * e-mail: lf@vortexmakes.com
 *  ---------------------------------------------------------------------------
 */

/**
 *  \file       file_header.c
 *  \brief      Brief description and purpose of this file.
 *
 *  Long description and purpose of this file.
 */

/* -------------------------- Development history -------------------------- */
/*
 *  2016.12.15  LeFr  v2.4.05  ---
 */

/* -------------------------------- Authors -------------------------------- */
/*
 *  LeFr  Leandro Francucci  lf@vortexmakes.com
 */

/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
#include "mytypes.h"
#include "console.h"

#ifdef DOS_PLATFORM
#include <stdio.h>
#include <conio.h>
#include <process.h>
#else
#include "gsqueue.h"
#include "qdata.h"
#include "serial.h"
#endif

void
conser_init(void)
{
#ifdef DOS_PLATFORM
    system("cls");
#endif
}

/*
 * conser_tstc:
 *
 *      Checks to see if a key is currently available
 *
 *      Returns:
 *
 *          0 - on success
 *          1 - key is not available
 */

MUInt
conser_tstc(void)
{
#ifdef DOS_PLATFORM
    return 0;
#else
    if (is_empty_gsqueue(COM1_QUEUE) == EMPTY_QUEUE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#endif
}

void
conser_putc(const char c)
{
#ifdef DOS_PLATFORM
    putc(c, stdout);
#else
    put_char(COM1CH, c);
#endif
}

void
conser_puts(const char *s)
{
#ifdef DOS_PLATFORM
    while (*s)
        conser_putc(*s++);
#else
    put_string(COM1CH, s);
#endif
}

/*
 * conser_getc:
 *
 *      Blocking function that return one character
 *      from attached serial line.
 */

MUInt
conser_getc(void)
{
#ifdef DOS_PLATFORM
    return getch();
#else
    unsigned char c;

    if (get_char(COM1CH, &c) == EMPTY_QUEUE)
    {
        return 0xFF;
    }
    else
    {
        return c;
    }
#endif
}
/* ------------------------------ End of file ------------------------------ */
