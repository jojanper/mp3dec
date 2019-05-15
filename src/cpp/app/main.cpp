#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/eqband.h"
#include "core/io/console.h"
#include "core/io/iobuf.h"
#include "core/io/uci.h"
#include "core/throw.h"
#include "mp3.h"
#include "mp3info.h"
#include "mstream.h"

static void
SetRestParam(Out_Param *out_param)
{
    out_param->sampling_frequency /= out_param->decim_factor;
    out_param->num_samples = (int16)(SBLIMIT / out_param->decim_factor);
    out_param->num_out_samples = (int16)(SBLIMIT / out_param->decim_factor);
    out_param->window_offset = (int16)(SBLIMIT * out_param->window_pruning_idx);
    out_param->num_subwindows = (int16)(NUM_SUBWIN - 2 * out_param->window_pruning_idx);
}


/**************************************************************************
  Title       : SetAnyQualityParam

  Purpose     : Initializes decoder engine parameters based on the quality
                parameters that are given as an input.

  Usage       : SetAnyQualityParam(initParam)

  Input       : initParam - quality parameters for the mp3 stream

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void
SetAnyQualityParam(MP_Stream *mp, CodecInitParam *initParam)
{
    Out_Param *out_param = mp->out_param;

    mp->side_info->sfbData.bandLimit = MAX_MONO_SAMPLES;
    if (initParam->bandLimit > 0 && initParam->bandLimit <= MAX_MONO_SAMPLES)
        mp->side_info->sfbData.bandLimit = initParam->bandLimit;

    out_param->sampling_frequency = mp->header->frequency();
    out_param->num_out_channels = (int16) mp->header->channels();
    if (initParam->channels > 0 && initParam->channels <= mp->header->channels())
        out_param->num_out_channels = initParam->channels;

    out_param->decim_factor = 1;
    if (initParam->decim_factor == 2 || initParam->decim_factor == 4)
        out_param->decim_factor = initParam->decim_factor;

    out_param->window_pruning_idx = WINDOW_PRUNING_START_IDX;
    if (initParam->window_pruning > 0 && initParam->window_pruning < SBLIMIT + 1)
        out_param->window_pruning_idx = initParam->window_pruning;

    out_param->num_samples = SBLIMIT;

    mp->complex->subband_pairs = 15;
    if (initParam->alias_bands >= 0 && initParam->alias_bands < SBLIMIT - 1)
        mp->complex->subband_pairs = initParam->alias_bands;

    mp->complex->imdct_subbands = MAX_MONO_SAMPLES;
    if (initParam->imdct_sbs > 0 && initParam->imdct_sbs <= SBLIMIT)
        mp->complex->imdct_subbands = initParam->imdct_sbs * SSLIMIT;

    mp->complex->fix_window = initParam->fix_window;

    SetRestParam(out_param);
}

#if 0
bool
InitEQBandFromCommandLine(EQ_Band *eqband, UCI *uci)
{
    int16 num_bands = 0;

    if (GetSwitchParam(uci, "-eq_level", "<level_amp>",
                       "Equalizer level "
                       "amplification (default : 0 dB)",
                       &EqualizerLevel)) {
        if (EqualizerLevel < -20 || EqualizerLevel > 20) {
            char buf[64];

            wsprintf(buf, "%s - Equalizer Settings Error", _DLL_BUILD_);
            MessageBox(GetActiveWindow(),
                       "The Valid Range for the Level "
                       "Amplification Is -20...20 dB.",
                       buf, flags);
        }
        else
            dBLevelScale = ConvertdBScale(EqualizerLevel);
    }

    if (GetSwitchParam(uci, "-eq", "<num_bands> band_0 band_1 band_2 ... ",
                       "Number of bands and corresponding settings for the "
                       "equalizer (default : 0 dB on all bands)",
                       &num_bands)) {
        int16 addr = GetSwitchAddress(uci, "-eq");

        if (addr == -1)
            return (FALSE);

        addr++;

        if (num_bands > MAX_SFB_BANDS) {
            char buf[64], buf2[64];

            wsprintf(buf, "%s - Equalizer Settings Warning", _DLL_BUILD_);
            wsprintf(buf2, "The Maximum # of Bands for the Equalizer is %i.",
                     MAX_SFB_BANDS);
            MessageBox(GetActiveWindow(), buf2, buf, flags);
            num_bands = MAX_SFB_BANDS;
        }

        for (int j = 0, i = addr + 1; i < num_bands + addr + 1; i++, j++) {
            if (uci->argv[i] == NULL)
                return (FALSE);

            EqualizerBandPosition[j] = (int16) atoi(uci->argv[i]);
            if (EqualizerBandPosition[j] < -20 || EqualizerBandPosition[j] > 20) {
                ResetEQBand();

                char buf[64];

                wsprintf(buf, "%s - Equalizer Settings Error", _DLL_BUILD_);
                MessageBox(GetActiveWindow(),
                           "The Valid Range for the Equalizer Bands "
                           "Is -20...20 dB.",
                           buf, flags);

                return (TRUE);
            }

            dBConvertedScale[j] = ConvertdBScale(EqualizerBandPosition[j]) * dBLevelScale;
            uci->argument_used[i] = 1;
        }
    }

    return (TRUE);
}
#endif

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
ParseMPCommandLine(char *InStream,
                   EQ_Band * /*eq_band*/,
                   char *OutFileName,
                   BOOL *waveOut,
                   int argc,
                   char **argv,
                   CodecInitParam *initParam)
{
    UCI *uci;
    BOOL retValue = TRUE;
    // char buf[256], buf0[64];
    char *txt = NULL;

    /*-- Parse the command line. --*/
    uci = InitUCI(argc, argv, (argc == 1) ? TRUE : FALSE);
    if (uci != NULL) {
        retValue = !uci->show_options;

        strcpy(InStream, "");
        if (GetSwitchString(uci, "-stream", "<MPEG_audio_stream>", "Bitstream to be decoded",
                            &txt))
            strcpy(InStream, txt);

        initParam->channels = MAX_CHANNELS;
        GetSwitchParam(uci, "-out_channels", "<num_channels>",
                       "Number of output channels (1 or 2) (default : same as input)",
                       &initParam->channels);

        initParam->decim_factor = 1;
        GetSwitchParam(uci, "-decim_factor", "<decimation_factor>",
                       "Decimation factor for the synthesis filterbank "
                       "(default : 1)",
                       &initParam->decim_factor);

        initParam->window_pruning = WINDOW_PRUNING_START_IDX;
        GetSwitchParam(uci, "-window_pruning", "<subband_index>",
                       "Number of subwindows (0...15) discarded at the windowing "
                       "stage (default : 2)",
                       &initParam->window_pruning);

        initParam->alias_bands = SBLIMIT - 1;
        GetSwitchParam(uci, "-alias_subbands", "<subband_pairs>",
                       "Number of subband pairs (1..31) for alias-reduction "
                       "(default : 31 [all pairs])",
                       &initParam->alias_bands);

        initParam->imdct_sbs = SBLIMIT;
        GetSwitchParam(uci, "-imdct_subbands", "<num_subbands>",
                       "Number of "
                       "subbands pairs (1..32) using IMDCT (default : all "
                       "subbands)",
                       &initParam->imdct_sbs);

        initParam->fix_window = FALSE;
        SwitchEnabled(uci, "-fix_window",
                      "Performs the windowing part using fixed "
                      "point arithmetic",
                      &initParam->fix_window);

        *waveOut = FALSE;
        SwitchEnabled(uci, "-wave_out",
                      "Write the output to a wave file (default "
                      ": pcm/raw file)",
                      waveOut);

        initParam->bandLimit = MAX_MONO_SAMPLES;
        GetSwitchParam(uci, "-band_limit", "<sfb_bin>",
                       "max # of bins to be decoded "
                       "(default : all bins)",
                       &initParam->bandLimit);

#if 0
        /*-- Get the equalizer settings from the command line. --*/
        if (eq_band->InitEQBandFromCommandLine(uci) == FALSE) {
            wsprintf(buf, "%s - Command Line Parsing Error", _DLL_BUILD_);
            MessageBox(GetActiveWindow(),
                       "An Error Occured While Processing "
                       "Equalizer Settings.\n\tDisabling Equalizer.",
                       buf, MB_OK | MB_ICONINFORMATION);
        }
#endif

        strcpy(OutFileName, "");
        if (GetSwitchString(uci, "-out", "<output_file>", "Name of decoded output file", &txt))
            strcpy(OutFileName, txt);

        /*-- End of command line parsing. --*/
        printf("VALIDATE\n");
        ValidateUCI(uci);
        printf("VALIDATE DONE\n");
        DeleteUCI(uci);
        printf("DELETE\n");
        uci = NULL;
    }
#if 0
    else {
        wsprintf(buf, "%s - Command Line Parsing Error", _DLL_BUILD_);
        ASSERT2(uci == NULL, "Unable to start command line processing.", buf, NULL);
    }
#endif

    return (retValue);
}

int
main(int argc, char **argv)
{
    InitMP3DecoderData();

    FileBuf fp;
    EQ_Band *eq_band = new EQ_Band();
    CodecInitParam initParam;
    BitStream *bs = new BitStream();
    MP_Stream *stream = new MP_Stream();

    Console *console = new Console();

    auto out_param = new Out_Param();
    auto out_complex = new Out_Complexity();
    memset(out_param, 0, sizeof(Out_Param));
    memset(out_complex, 0, sizeof(Out_Complexity));

    BOOL waveOut = FALSE;
    char inStream[1024], outStream[1024];

    if (!ParseMPCommandLine(inStream, eq_band, outStream, &waveOut, argc, argv, &initParam))
        return EXIT_FAILURE;

    printf("PARSE DONE\n");
    fflush(stdout);
    // return EXIT_SUCCESS;

    /*-- Open the file. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    if (!fp.open(inStream, kFileReadMode)) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }
    bs->open(&fp, MAX_SLOTS);

    printf("CREATE BITSTREAM\n");
    fflush(stdout);

    stream->InitDecoder(bs, out_param, out_complex);

    printf("DECODER INITIALIZED\n");
    fflush(stdout);

    /*-- This will determine the output quality. --*/
    SetAnyQualityParam(stream, &initParam);
    ReInitEngine(stream);

    /*-- Next, according to the output quality, modify the sfb tables. --*/
    III_BandLimit(&stream->side_info->sfbData, out_param->decim_factor);

    if (!console->open(outStream, 44100 /*out_param->sampling_frequency*/,
                       2 /*out_param->num_out_channels*/, waveOut))
        return EXIT_FAILURE;

    /*-- Store the equalizer settings into the dequantizer module. --*/
    stream->dbScale = eq_band->getdBScale();

    /*
    for (size_t i = 0; i < 51; i++)
        printf("%zu: %f\n", i, stream->dbScale[i]);
    */

    fprintf(stdout, "\nStream parameters for %s :\n", inStream);
    fprintf(stdout, "Version : %s\n",
            (stream->header->version())
                ? "Mpeg-1"
                : ((stream->header->mp25version()) ? "Mpeg-2.5" : "Mpeg-2 LSF"));
    fprintf(stdout, "Layer : %s\n", stream->header->layer_string());
    fprintf(stdout, "Checksums? : %s\n", (stream->header->error_protection() ? "Yes" : "No"));
    fprintf(stdout, "Bitrate: %i kbps\n", stream->header->bit_rate());
    fprintf(stdout, "Sampling Frequency : %i kHz\n", stream->header->frequency());
    fprintf(stdout, "Padding bit ? : %s\n", (stream->header->padding() ? "Yes" : "No"));
    fprintf(stdout, "Private bit ? : %s\n", (stream->header->private_bit() ? "Yes" : "No"));
    fprintf(stdout, "Mode String : %s\n", stream->header->mode_string());
    fprintf(stdout, "Copyright : %s\n", (stream->header->copyright() ? "Yes" : "No"));
    fprintf(stdout, "Original : %s\n", (stream->header->original() ? "Yes" : "No"));
    fprintf(stdout, "De-emphasis : %s\n\n", stream->header->de_emphasis());

    SEEK_STATUS sync = SYNC_FOUND;

    size_t frames = 0;
    do {
        /*-- Get the output samples. --*/
        if (!DecodeFrame(stream, stream->buffer->pcm_sample))
            goto exit;

        /*-- Write to file. --*/
        // printf("SAMPLES TO WRITE: %i\n", stream->out_param->num_out_samples);
        console->writeBuffer(stream->buffer->pcm_sample, stream->out_param->num_out_samples);

        /*-- Find the start of the next frame. --*/
        sync = SeekSync(stream);

        fprintf(stdout, "Frames decoded: %zu\r", frames++);
        fflush(stdout);

    } while (sync == SYNC_FOUND);

    console->close();

exit:
    delete stream;
    delete bs;

    return EXIT_SUCCESS;
}
