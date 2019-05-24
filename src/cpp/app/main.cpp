#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "core/eqband.h"
#include "core/io/console.h"
#include "core/io/iobuf.h"
#include "core/io/uci.h"
#include "core/throw.h"
#include "param.h"

#include "mcu/mp3decconsole.h"

/**************************************************************************
  Title       : ParseMPCommandLine

  Purpose     : Parses command line parameters for MPEG audio playback.

  Usage       : ParseMPCommandLine(hInst, InStream, eq_band, OutFileName,
                                   waveOut, argc, argv, initParam)

  Input       : hInst       - handle of current instance
                argc        - number of command line arguments
                argv        - command line arguments

  Output      : InStream    - name of MPEG audio stream for decoding
                eq_band     - equalizer setting for the mp3 decoder
                OutFileName - name out output file
                initParam   - init parameters for the codec

  Author(s)   : Juha Ojanpera
  *************************************************************************/

static bool
ParseMPCommandLine(
    char *InStream,
    char *OutFileName,
    BOOL *waveOut,
    int argc,
    char **argv,
    draaldecoder::MP3ConsoleDecoder *dec)
{
    UCI *uci;
    BOOL retValue = TRUE;
    char *txt = NULL;

    /*-- Parse the command line. --*/
    uci = InitUCI(argc, argv, (argc == 1) ? TRUE : FALSE);
    if (uci != NULL) {
        retValue = !uci->show_options;

        strcpy(InStream, "");
        if (GetSwitchString(
                uci, "-stream", "<MPEG-audio-stream>", "Bitstream to be decoded", &txt))
            strcpy(InStream, txt);

        dec->parseCommandLine(uci);

        *waveOut = FALSE;
        SwitchEnabled(
            uci,
            "-wave-out",
            "Write the output to a wave file (default: pcm/raw file)",
            waveOut);

        strcpy(OutFileName, "");
        if (GetSwitchString(uci, "-out", "<output-file>", "Name of decoded output file", &txt))
            strcpy(OutFileName, txt);

        /*-- End of command line parsing. --*/
        ValidateUCI(uci);
        DeleteUCI(uci);
        uci = NULL;
    }

    return (retValue);
}

int
main(int argc, char **argv)
{
    FileBuf fp;
    Console *console = new Console();

    draaldecoder::MP3ConsoleDecoder *dec = new draaldecoder::MP3ConsoleDecoder();

    BOOL waveOut = FALSE;
    char inStream[1024], outStream[1024];

    if (!ParseMPCommandLine(inStream, outStream, &waveOut, argc, argv, dec))
        return EXIT_FAILURE;

    // Open the input stream
    if (!fp.open(inStream, kFileReadMode)) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }

    // Initialize decoder; pass input and output stream
    dec->init(&fp, console);

    // Open the output stream
    auto out_param = &dec->getInfo()->param;
    if (!console->open(
            outStream, out_param->sampling_frequency, out_param->num_out_channels, waveOut))
        return EXIT_FAILURE;

    char infoBuffer[4096];
    sprintf(infoBuffer, "\nStream parameters for %s :\n", inStream);
    fprintf(stdout, "%s\n", dec->getTrackProperties(infoBuffer));

    // Decode until end of stream found
    size_t frames = 0;
    bool result = true;

    do {
        result = dec->decode();

        fprintf(stdout, "Frames decoded: %zu\r", frames++);
        fflush(stdout);

        // if (frames > 5)
        //  break;

    } while (result);

    console->close();

    delete dec;
    delete console;

    return EXIT_SUCCESS;
}
