#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./parse.h"
#include "core/io/uci.h"

// Parse command line parameters for MPEG audio playback.
bool
ParseMPCommandLine(
    char **in,
    char **out,
    BOOL *waveOut,
    int argc,
    char **argv,
    size_t indent_size,
    draaldecoder::IBaseConsoleDecoder *dec)
{
    /*-- Parse the command line. --*/
    UCI *uci = InitUCI(argc, argv, (argc == 1) ? TRUE : FALSE, indent_size);
    if (uci == NULL)
        return FALSE;

    BOOL retValue = !uci->show_options;

    // General command line options
    GetSwitchString(uci, "-stream", "<audio-stream>", "Bitstream to be decoded", in);
    SwitchEnabled(uci, "-wave-out", "Write the output to a wave file (default: PCM)", waveOut);
    GetSwitchString(uci, "-out", "<output-file>", "Name of decoded output file", out);

    // Decoder specific command line options and related parsing
    if (dec)
        dec->parseCommandLine(uci);

    /*-- End of command line parsing. --*/
    ValidateUCI(uci);
    uci = DeleteUCI(uci);

    return retValue;
}
