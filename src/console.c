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
#include "conser.h"
#include "command.h"
#include "mytypes.h"
#include "formats.h"
#include "console.h"
#include "contick.h"

#include <stdlib.h>
#include <string.h>

/*
 *      Define the size of console buffer.
 */

#define CBSIZE                  32

/*
 *      Length of prompt string
 */

#define PROMPT_LEN              sizeof(prompt)

/*
 *      Prompt string
 */

static const char prompt[] = ">>";

/*
 *	    Used to expand TABs
 */

static const char tab_seq[] = "        ";

/*
 *		Erase sequence
 */

static const char erase_seq[] = "\b \b";

/*
 *      Used to store the command's arguments
 */

static char *argv[MAXARGS + 1];

/*
 *      Used to maintain the input char from attached
 *      serial channel
 */

static char console_buffer[CBSIZE];

/*
 *      Console buffer index
 */

static MUInt n;

/*
 *      Output column counter
 */

static MUInt col;

/*
 *      Pointer to console buffer
 */

static char *p;

/*
 *      If CONFIG_CMD_TOUT is defined and command timer elapsed,
 *		command shell is aborted.
 */

static MUInt abort_shell;

/*
 *      Return codes for 'process_in_char' function.
 *      They are negative numbers.
 */

enum
{
    CTRL_C = 1, PARSING
};

/*
 * print_prompt:
 *
 *      Print prompt string on console and initialize
 *      for next command entry.
 */

static
void
print_prompt(void)
{
    n = 0;
    p = console_buffer;
    if (prompt != NULL)
    {
        conser_puts(prompt);
        col = PROMPT_LEN;
        return;
    }
    col = 0;
}

/*
 * delete_char:
 *
 *      Delete one character of console. Check '\t' character.
 */

#if DELETE_CHAR
static
char *
delete_char(char *buffer, char *p, MUInt *colp, MUInt *np)
{
    char *s;

    if (*np == 0)
    {
        conser_putc('\a');
        return p;
    }

    if (*(--p) == '\t')
    {
        while (*colp > PROMPT_LEN) /* delete whole line on console */
        {
            conser_puts(erase_seq);
            --(*colp);
        }
        for (s = buffer; s < p; ++s) /* retype whole line on console */
            if (*s == '\t')
            {
                conser_puts(tab_seq + ((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            }
            else
            {
                ++(*colp);
                conser_putc(*s);
            }
    }
    else
    {
        conser_puts(erase_seq);
        --(*colp);
    }
    --(*np);
    return p;
}
#endif

/*
 * process_in_char:
 *
 *      Every received character from attached
 *      serial channel is parsed on-line.
 *
 *      Returns:
 *
 *          -CTRL_C		- received ^C
 *          -PARSING	- continue
 *          >= 0		- received '\r' or '\n'
 */

static
MInt
process_in_char(char c)
{
    switch (c)
    {
        case '\r':                                  /* Enter */
        case '\n':
            *p = '\0';
            conser_puts("\r\n");
            return p - console_buffer;
        case 0x03:                                  /* ^C - abort */
            return -CTRL_C;
        case 0x15:                                  /* ^U - erase line	*/
#if DELETE_CHAR
            while (col > PROMPT_LEN)
            {
                conser_puts(erase_seq);
                --col;
            }
            p = console_buffer;
            n = 0;
#endif
            return -PARSING;
        case 0x17:                                  /* ^W - erase word  */
#if DELETE_CHAR
            p = delete_char(console_buffer, p, &col, &n);
            while (n > 0 && *p != ' ')
                p = delete_char(console_buffer, p, &col, &n);
#endif
            return -PARSING;
        case 0x08:                                  /* ^H  - backspace	*/
        case 0x7F:                                  /* DEL - backspace	*/
#if DELETE_CHAR
            p = delete_char(console_buffer, p, &col, &n);
#endif
            return -PARSING;
        default:
            /* Must be a normal character then */
            if (n < CBSIZE - 2)
            {
                if (c == '\t')                      /* Expand TABs */
                {
                    conser_puts(tab_seq + (col & 7));
                    col += 8 - (col & 7);
                }
                else                                /* Echo input	*/
                {
                    ++col;
                    conser_putc(c);
                }
                *p++ = c;
                ++n;
            }
            else                                    /* Buffer full */
            {
                conser_putc('\a');
            }
            return -PARSING;
    }
}

/*
 * parse_line:
 *
 *      Parse the received line. Store and prepare all args
 *      into argv parameter. Then return the actual number of args.
 *      Skip any white space (' ' or '\t'). The character '\0' is
 *      the end of line.
 */

static
MInt
parse_line(char *line, char *argv[])
{
    MUInt nargs = 0;

    while (nargs < MAXARGS)
    {
        /* Skip any white space */

        while (*line == ' ' || *line == '\t')
            ++line;

        /* End of line, no more args */

        if (*line == '\0')
        {
            argv[nargs] = NULL;
            return nargs;
        }

        /* Begin of argument string	*/

        argv[nargs++] = line;

        /* Find end of string */

        while (*line && *line != ' ' && *line != '\t')
            ++line;

        /* End of line, no more args */

        if (*line == '\0')
        {
            argv[nargs] = NULL;
            return nargs;
        }

        /* Terminate current arg */

        *line++ = '\0';
    }
#ifdef PRINT_FORMATS
    myprintf(0, "** Too many args (max. %d) **\n", MAXARGS);
#else
    conser_puts("** Too many args **\n");
#endif
    return nargs;
}

/*
 * run_command:
 *
 *      Get and find the actual command. If found it, then
 *		get all arguments and pass it to properly callback
 *		function.
 *
 *      Returns:
 *			0,1	- command executed.
 *			-1  - not executed ( unrecognized or too many args )
 *          If cmd is NULL or "" or longer than CBSIZE-1 it is
 *          considered unrecognized.
 */

static
MInt
run_command(char *cmd)
{
    const CMD_TABLE *cmdtp;
    char *str = cmd;
    MUInt argc;

    /* Empty command */

    if (!cmd || !*cmd)
    {
        return -1;
    }

    if (strlen(cmd) >= CBSIZE)
    {
        conser_puts("## Command too long!\n");
        return -1;
    }

    /* Extract arguments */

    argc = parse_line(str, argv);

    /* Look up command in command table */

    if ((cmdtp = find_cmd(argv[0])) == NULL)
    {
#ifdef PRINT_FORMATS
        myprintf(0, "Unknown command '%s' - try 'help'\n", argv[0]);
#else
        conser_puts("Unknown command - try 'help'\n");
#endif
        return -1;  /* Give up after bad command */
    }

    /* Found - Check max args */

    if (argc > cmdtp->maxargs)
    {
#ifdef PRINT_FORMATS
        myprintf(0, "Usage:\n%s\n", cmdtp->usage);
#endif
        return -1;
    }

    /* OK - Call function to do the command */

    if ((cmdtp->cmd)(cmdtp, argc, argv) != 0)
    {
#ifdef PRINT_FORMATS
        myprintf(0, "Usage:\n%s\n", cmdtp->usage);
#endif
        return -1;
    }

    /* Print prompt */

    print_prompt();

    return 0;
}

/*
 * do_console:
 *
 *      Check if key already pressed and
 *      and send it to command shell process.
 *
 *      Returns:
 *
 *          0 - successfully
 *			1 - abort command shell
 */

static
MUInt
do_console(void)
{
    MInt r;

    if (((r = process_in_char(conser_getc())) >= 0) &&
        (run_command(console_buffer) < 0))
    {
        print_prompt();
        return 0;
    }
    return r == -CTRL_C;
}

/*
 * command_shell:
 *
 *      Entry point to use the command shell. Each received
 *      character from attached serial channel is parsed on-line.
 *      If CONFIG_CMD_TOUT is defined and command timer elapsed,
 *		returns timeout and command shell is aborted.
 */

void
command_shell(void)
{
    if (conser_tstc())
    {
#if CONFIG_CMD_TOUT
        if (abort_shell && is_cmd_timeout())
        {
            exit(0);
        }
#endif
        return;
    }
    else
    {
        abort_shell = 0;
        if (do_console())
        {
            exit(0);
        }
    }
}

/*
 * init_command_shell:
 *
 *      It initialize this module.
 */

void
init_command_shell(void)
{
    conser_init();
    abort_shell = 1;
    set_cmd_timeout(CONFIG_CMD_TIME);
    print_prompt();
}

#define forever         for (;;)
#ifdef __CONSOLE_TST__

void
main(void)
{
    init_command_shell();

    forever
    {
        command_shell();
    }
}
#endif
/* ------------------------------ End of file ------------------------------ */
