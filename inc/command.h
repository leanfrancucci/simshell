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
 *  \file       file_header.h
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
/* --------------------------------- Module -------------------------------- */
/* ----------------------------- Include files ----------------------------- */
/* ---------------------- External C language linkage ---------------------- */
/* --------------------------------- Macros -------------------------------- */
/* -------------------------------- Constants ------------------------------ */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External variables --------------------------- */
/* -------------------------- Function prototypes -------------------------- */
/* -------------------- External C language linkage end -------------------- */
/* ------------------------------ Module end ------------------------------- */
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "mytypes.h"

/*
 *      Maximum arguments to execute a command
 */

#define MAXARGS             11

/*
 *      Help info for every command. Used by 'help'
 *      command.
 */

#define LONGHELP            1

/*
 *      Abbreviated length for every command
 */

#define ABBREVIATED         1

/*
 *      Include 'echo' command
 */

#define ECHO                1

/*
 *      Include 'help' command
 */

#define HELP                1

#if LONGHELP
#define MK_CMD_TBL_ENTRY(name, lmin, maxargs, cmd, usage, help)  \
    {name, lmin, maxargs, cmd, usage, help}
#else
#define MK_CMD_TBL_ENTRY(name, lmin, maxargs, cmd, usage, help)  \
    {name, lmin, maxargs, cmd, usage}
#endif

typedef struct cmd_tbl_s
{
    char *name;         /* command name					*/
    MInt lmin;          /* minimum abbreviated length	*/
    MInt maxargs;       /* maximum number of arguments	*/
    MInt (*cmd)(const struct cmd_tbl_s *tbl, MInt argc, char *argv[]);
    char *usage;        /* Usage message	(short)		*/
#if LONGHELP
    char *help;         /* Help  message	(long)		*/
#endif
} CMD_TABLE;

const CMD_TABLE *find_cmd(const char *cmd);

#endif
/* ------------------------------ End of file ------------------------------ */
