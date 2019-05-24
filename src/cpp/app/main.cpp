#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "core/eqband.h"
#include "core/io/console.h"
#include "core/io/iobuf.h"
#include "core/io/uci.h"
#include "core/throw.h"
#include "mp3def.h"
#include "mp3info.h"
#include "param.h"

#include "mcu/mp3dec.h"

bool
InitEQBandFromCommandLine(EQ_Band *eqband, UCI *uci)
{
    int16 equalizerLevel = 0;
    if (GetSwitchParam(
            uci,
            "-eq-level",
            "<level-amp>",
            "Equalizer level amplification (default: 0 dB)",
            &equalizerLevel)) {
        if (equalizerLevel < -20 || equalizerLevel > 20) {
            fprintf(
                stderr, "%s\n", "The valid range for the level amplification is -20...20 dB");
            return FALSE;
        }
        else
            eqband->setEQLevelAmp(equalizerLevel);
    }

    int16 num_bands = 0;
    if (GetSwitchParam(
            uci,
            "-eq",
            "<num-bands> band-0 band-1 band-2 ... ",
            "Number of bands and corresponding settings for the equalizer (default: 0 dB on "
            "all bands)",
            &num_bands)) {
        auto addr = GetSwitchAddress(uci, "-eq");

        if (addr == -1)
            return FALSE;

        addr++;

        if (num_bands > MAX_SFB_BANDS) {
            fprintf(stderr, "The maximum # of bands for the equalizer is %i", MAX_SFB_BANDS);
            num_bands = MAX_SFB_BANDS;
        }

        for (int j = 0, i = addr + 1; i < num_bands + addr + 1; i++, j++) {
            if (uci->argv[i] == NULL)
                return (FALSE);

            auto dbScale = atoi(uci->argv[i]);
            if (dbScale < -20 || dbScale > 20) {
                fprintf(
                    stderr,
                    "Band %i: valid range for the level amplification is -20...20 dB\n",
                    j);
                return FALSE;
            }

            eqband->setEQBand(j, dbScale);
            uci->argument_used[i] = 1;
        }
    }

    return TRUE;
}

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
    EQ_Band *eq_band,
    char *OutFileName,
    BOOL *waveOut,
    int argc,
    char **argv,
    CodecInitParam *initParam)
{
    UCI *uci;
    BOOL retValue = TRUE;
    char *txt = NULL;

    /*-- Parse the command line. --*/
    uci = InitUCI(argc, argv, (argc == 1) ? TRUE : FALSE);
    if (uci != NULL) {
        retValue = !uci->show_options;

        initParam->fix_window = FALSE;

        strcpy(InStream, "");
        if (GetSwitchString(
                uci, "-stream", "<MPEG-audio-stream>", "Bitstream to be decoded", &txt))
            strcpy(InStream, txt);

        initParam->channels = MAX_CHANNELS;
        GetSwitchParam(
            uci,
            "-out-channels",
            "<num-of-channels>",
            "Number of output channels (1 or 2) (default: 2)",
            &initParam->channels);

        initParam->decim_factor = 1;
        GetSwitchParam(
            uci,
            "-decim-factor",
            "<decimation-factor>",
            "Decimation factor for the synthesis filterbank (1, 2, or 4)"
            "(default: 1)",
            &initParam->decim_factor);

        initParam->window_pruning = WINDOW_PRUNING_START_IDX;
        GetSwitchParam(
            uci,
            "-window-pruning",
            "<subband-index>",
            "Number of subwindows (0...15) discarded at the windowing stage (default : 2)",
            &initParam->window_pruning);

        initParam->alias_bands = SBLIMIT - 1;
        GetSwitchParam(
            uci,
            "-alias-subbands",
            "<subband-pairs>",
            "Number of subband pairs (1..31) for alias-reduction "
            "(default: 31 [all pairs])",
            &initParam->alias_bands);

        initParam->imdct_sbs = SBLIMIT;
        GetSwitchParam(
            uci,
            "-imdct-subbands",
            "<num-subbands>",
            "Number of "
            "subbands pairs (1..32) using IMDCT (default: all subbands)",
            &initParam->imdct_sbs);

        *waveOut = FALSE;
        SwitchEnabled(
            uci,
            "-wave-out",
            "Write the output to a wave file (default: pcm/raw file)",
            waveOut);

        initParam->bandLimit = MAX_MONO_SAMPLES;
        GetSwitchParam(
            uci,
            "-band_limit",
            "<sfb_bin>",
            "max # of bins to be decoded "
            "(default : all bins)",
            &initParam->bandLimit);

        /*-- Get the equalizer settings from the command line. --*/
        if (!InitEQBandFromCommandLine(eq_band, uci))
            return FALSE;

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
    CodecInitParam initParam;
    Console *console = new Console();

    draaldecoder::MP3Decoder *dec = new draaldecoder::MP3Decoder();

    BOOL waveOut = FALSE;
    char inStream[1024], outStream[1024];

    if (!ParseMPCommandLine(
            inStream, dec->getEQ(), outStream, &waveOut, argc, argv, &initParam))
        return EXIT_FAILURE;

        /*-- Open the file. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    if (!fp.open(inStream, kFileReadMode)) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }

    dec->init(&fp, &initParam, console);

    auto out_param = &dec->getInfo()->param;
    if (!console->open(
            outStream, out_param->sampling_frequency, out_param->num_out_channels, waveOut))
        return EXIT_FAILURE;

    char infoBuffer[4096];
    sprintf(infoBuffer, "\nStream parameters for %s :\n", inStream);
    fprintf(stdout, "%s\n", dec->getTrackProperties(infoBuffer));

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

    delete console;
    delete dec;

    return EXIT_SUCCESS;
}
