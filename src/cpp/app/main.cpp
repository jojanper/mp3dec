#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/io/console.h"
#include "core/io/iobuf.h"
#include "core/io/uci.h"
#include "core/throw.h"
#include "mcu/consoledecoder.h"

// Parse command line parameters for MPEG audio playback.
static bool
ParseMPCommandLine(
    char **in,
    char **out,
    BOOL *waveOut,
    int argc,
    char **argv,
    draaldecoder::IBaseConsoleDecoder *dec)
{
    /*-- Parse the command line. --*/
    UCI *uci = InitUCI(argc, argv, (argc == 1) ? TRUE : FALSE, 41);
    if (uci == NULL)
        return FALSE;

    BOOL retValue = !uci->show_options;

    // General command line options
    GetSwitchString(uci, "-stream", "<audio-stream>", "Bitstream to be decoded", in);
    SwitchEnabled(uci, "-wave-out", "Write the output to a wave file (default: PCM)", waveOut);
    GetSwitchString(uci, "-out", "<output-file>", "Name of decoded output file", out);

    // Decoder specific command line options and related parsing
    dec->parseCommandLine(uci);

    /*-- End of command line parsing. --*/
    ValidateUCI(uci);
    uci = DeleteUCI(uci);

    return retValue;
}

int
main(int argc, char **argv)
{
    FileBuf fp;
    Console *console = new Console();
    auto dec = draaldecoder::ConsoleDecoder::create(draaldecoder::MP3DEC);

    BOOL waveOut = FALSE;
    char *inStream, *outStream;
    if (!ParseMPCommandLine(&inStream, &outStream, &waveOut, argc, argv, dec))
        return EXIT_FAILURE;

    // Open the input stream
    if (!fp.open(inStream, kFileReadMode)) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }

    // Initialize decoder; pass input and output stream
    dec->init(&fp, console, nullptr);

    // Open the output stream
    auto info = dec->getTrackInfo();
    if (!console->open(outStream, info->sampleRate, info->outChannels, waveOut))
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

    dec->destroy();
    delete console;

    return EXIT_SUCCESS;
}
