/**************************************************************************
  uci.cpp - Implementation of command line parsing functions.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-1999 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>

/*-- Project Headers. --*/
#include "core/io/uci.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/*
   Purpose:     Enables testing of this module.
   Explanation: - */
//#define TEST_MODULE

/*
   Purpose:     Static buffers for this module.
   Explanation: These are needed when printing the help parameters. */
static char *buf;
static char buffer[1024];
static char buffer_tmp[1024];

static int
vspf(const char *fmt, ...)
{
    va_list argptr;
    int cnt;

    va_start(argptr, fmt);
    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);

    return (cnt);
}


/**************************************************************************
  Title        : InitUCI

  Purpose      : Initializes command line parser.

  Usage        : y = InitUCI(argc, argv, show_options)

  Input        : argc         - number of command line parameters
                 argv         - command line parameters
                 show_options - if TRUE the help parameters are printed when
                                accessing the functions of this module

  Output       : y - command line parser on success, NULL otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

UCI *
InitUCI(int argc, char **argv, BOOL show_options)
{
    UCI *uci = NULL;

    try {
        uci = new UCI();
        if (uci) {
            uci->args = (int16) argc;
            uci->argv = argv;
            uci->show_options = show_options;
            if (uci->show_options) {
                uci->logfile.open();
                uci->logfile.write("Command line options for %s :\n", argv[0]);
            }

            uci->argument_used = new int16[argc];
            if (!uci->argument_used) {
                delete uci;
                uci = NULL;
            }
            else {
                uci->argument_used[0] = 1;
                for (int i = 1; i < argc; i++)
                    uci->argument_used[i] = 0;
            }
        }
    } catch (std::bad_alloc) {
        DeleteUCI(uci);
        return NULL;
    }

    return (uci);
}


/**************************************************************************
  Title        : DeleteUCI

  Purpose      : Deletes command line parser.

  Usage        : y = DeleteUCI)

  Input        : uci - command line parser

  Output       : y - NULL

  Author(s)    : Juha Ojanpera
  *************************************************************************/

UCI *
DeleteUCI(UCI *uci)
{
    if (uci) {
        if (uci->argument_used)
            delete[] uci->argument_used;
        uci->argument_used = NULL;
        uci->logfile.close();

        delete uci;
    }

    return (NULL);
}


/**************************************************************************
  Title        : ValidateUCI

  Purpose      : Checks whether command line parsing was successful.

  Usage        : ValidateUCI(uci)

  Input        : uci - command line parser

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
ValidateUCI(UCI *uci)
{
    BOOL warnings = FALSE;

    for (int i = 0; i < uci->args; i++)
        if (uci->argument_used[i] == 0) {
            warnings = TRUE;
            uci->logfile.open();
            uci->logfile.write("Warning : argument %s not used\n", uci->argv[i]);
        }

    delete[] uci->argument_used;
    uci->argument_used = NULL;

    return warnings == FALSE;
}


/**************************************************************************
  Title        : GetSwitchAddress

  Purpose      : Finds a string from the command line parameters.

  Usage        : y = GetSwitchAddress(uci, cswitch)

  Input        : uci     - command line parser
                 cswitch - string to be found from the command line options

  Output       : y - address of the string on success, -1 on failure

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int16
GetSwitchAddress(UCI *uci, const char *cswitch)
{
    int16 i, address;

    address = -1;
    for (i = 0; i < uci->args; i++)
        if (strcmp(cswitch, uci->argv[i]) == 0) {
            address = i;
            break;
        }

    return (address);
}


/**************************************************************************
  Title        : SwitchEnabled

  Purpose      : Checks whether a given flag is set on the command line.

  Usage        : y = SwitchEnabled(uci, cswitch, switch_explanation, flag)

  Input        : uci                - command line parser
                 cswitch            - string to be found from the command
                                      line options
                 switch_explanation - (obvious)

  Output       : y    - TRUE on success, FALSE otherwise
                 flag - TRUE if flag was found, unaltered otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
SwitchEnabled(UCI *uci, const char *cswitch, const char *switch_explanation, BOOL *flag)
{
    int16 address;

    if (uci->show_options) {
        buf = buffer;
        vspf("%-42s %s", cswitch, switch_explanation);
        uci->logfile.write("%s\n", buf);

        return (FALSE);
    }

    address = GetSwitchAddress(uci, cswitch);

    if (address != -1) {
        *flag = TRUE;
        uci->argument_used[address - 1] = 1;
        uci->argument_used[address] = 1;
    }

    return ((address != -1));
}


static inline void
ShowCmdLineOption(
    UCI *uci,
    const char *cswitch,
    const char *param_,
    const char *switch_explanation)
{
    buf = buffer_tmp;
    vspf("%s %s", cswitch, param_);
    buf = buffer;
    vspf("%-42s %s", buffer_tmp, switch_explanation);
    uci->logfile.write("%s\n", buf);
}


/**************************************************************************
  Title        : GetSwitchParam

  Purpose      : Reads a value of type 'int16', 'int32', 'char *', or 'float'
                 that is associated with the given string.

  Usage        : y = GetSwitchParam(uci, cswitch, param_, switch_explanation)

  Input        : uci                - command line parser
                 cswitch            - string to be found from the command
                                      line parameters
                 param              - additional textual description of the
                                      command line option
                 switch_explanation - textual description of how 'param' value
                                      is to be interpreted

  Output       : y     - TRUE on success, FALSE otherwise

  Explanation  : The following function can be used to read parameters from
                 command line. The last parameter always specifies the type
                 of data to be read. This can be either a numerical value or
                 text string. If the parameter to be read is not present the
                 last parameter remains unchanged. Typically this function
                 can be used to process command line options of type :

                 -tag <tag_data> - textual description of the option

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
GetSwitchParam(
    UCI *uci,
    const char *cswitch,
    const char *param_,
    const char *switch_explanation,
    int16 *value)
{
    int16 address;

    if (uci->show_options) {
        ShowCmdLineOption(uci, cswitch, param_, switch_explanation);
        return (FALSE);
    }

    address = GetSwitchAddress(uci, cswitch);
    if (address != -1) {
        if (uci->argv[address + 1] != NULL) {
            *value = (int16) atoi(uci->argv[++address]);
            uci->argument_used[address - 1] = 1;
            uci->argument_used[address] = 1;
        }
        else
            address = -1;
    }

    return ((address != -1) ? TRUE : FALSE);
}

BOOL
GetSwitchParam(
    UCI *uci,
    const char *cswitch,
    const char *param_,
    const char *switch_explanation,
    int32 *value)
{
    int16 address;

    if (uci->show_options) {
        ShowCmdLineOption(uci, cswitch, param_, switch_explanation);
        return (FALSE);
    }

    address = GetSwitchAddress(uci, cswitch);
    if (address != -1) {
        if (uci->argv[address + 1] != NULL) {
            *value = (int32) atoi(uci->argv[++address]);
            uci->argument_used[address - 1] = 1;
            uci->argument_used[address] = 1;
        }
        else
            address = -1;
    }

    return ((address != -1) ? TRUE : FALSE);
}

BOOL
GetSwitchString(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    char **text)
{
    int16 address;

    if (uci->show_options) {
        ShowCmdLineOption(uci, cswitch, string_, switch_explanation);
        return (FALSE);
    }

    address = GetSwitchAddress(uci, cswitch);
    if (address != -1) {
        if (uci->argv[address + 1] != NULL) {
            *text = uci->argv[++address];
            uci->argument_used[address - 1] = 1;
            uci->argument_used[address] = 1;
        }
        else
            address = -1;
    }

    return ((address != -1) ? TRUE : FALSE);
}

BOOL
GetSwitchString(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    FLOAT *value)
{
    int16 address;

    if (uci->show_options) {
        ShowCmdLineOption(uci, cswitch, string_, switch_explanation);
        return (FALSE);
    }

    address = GetSwitchAddress(uci, cswitch);
    if (address != -1) {
        if (uci->argv[address + 1] != NULL) {
            *value = atof(uci->argv[++address]);
            uci->argument_used[address - 1] = 1;
            uci->argument_used[address] = 1;
        }
        else
            address = -1;
    }

    return ((address != -1) ? TRUE : FALSE);
}
