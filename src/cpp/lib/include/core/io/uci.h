/**************************************************************************
  uci.h - Function prototypes for command line processing.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-1999 Juha Ojanpera.
  *************************************************************************/

#ifndef UCI_H_
#define UCI_H_

/*-- System Headers. --*/
#include <stddef.h>

/*-- Project Headers. --*/
#include "core/defines.h"
#include "core/io/log.h"

/*
   Purpose:     Structure for command line parameters.
   Explanation: - */
typedef struct UCI_Str
{
    int16 args;           /* Number of argument.                        */
    size_t indent_size;   /* Max argument size for verbose logging.     */
    char **argv;          /* Arguments.                                 */
    int16 *argument_used; /* Flag indicating whether argument was used. */
    BOOL show_options;    /* Prints command line options.               */
    LogFile logfile;      /* Receives all messages sent by this module. */

} UCI;

UCI *InitUCI(int argc, char **argv, BOOL show_options, size_t indent_size);

UCI *DeleteUCI(UCI *uci);

BOOL ValidateUCI(UCI *uci);

int16 GetSwitchAddress(UCI *uci, const char *cswitch);

BOOL SwitchEnabled(UCI *uci, const char *cswitch, const char *switch_explanation, BOOL *flag);

BOOL GetSwitchParam(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    int16 *value);

BOOL GetSwitchParam(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    int32 *value);

BOOL GetSwitchString(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    char **text);

BOOL GetSwitchString(
    UCI *uci,
    const char *cswitch,
    const char *string_,
    const char *switch_explanation,
    FLOAT *value);

#endif /* UCI_H_ */
