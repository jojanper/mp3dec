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

void
retrieveTrackInfo(TrackInfo *trackInfo, const MP_Header *header)
{
    // trackInfo->tag = tag;

    if (header->version())
        strcpy(trackInfo->Version, "MPEG-1");
    else if (header->mp25version())
        strcpy(trackInfo->Version, "MPEG-2.5");
    else
        strcpy(trackInfo->Version, "MPEG-2 LSF");

    trackInfo->Channels = header->channels();
    trackInfo->Frequency = header->frequency();
    strcpy(trackInfo->Layer, header->layer_string());
    strcpy(trackInfo->Mode, header->mode_string());
    strcpy(trackInfo->Private_bit, header->private_bit() ? "Yes" : "No");
    strcpy(trackInfo->De_emphasis, header->de_emphasis());
    strcpy(trackInfo->Copyright, header->copyright() ? "Yes" : "No");
    strcpy(trackInfo->Stereo_mode, header->mode_string());
    strcpy(trackInfo->Error_protection, header->error_protection() ? "Yes" : "No");
    strcpy(trackInfo->Original, header->original() ? "Yes" : "No");

    /*
      trackInfo->Length = player->brInfo->GetTotalTime();
      trackInfo->bitRate = player->brInfo->GetBitRate();
      trackInfo->SizeInBytes = player->mp->bs->GetStreamSize();
      trackInfo->TotalFrames = player->brInfo->GetTotalFrames();
      */
}

char *
showTrackProperties(char *buf, const TrackInfo *trackInfo)
{
    char tmpTxtBuf[64];

    /*-- Collect the info from the structure to the message buffer. --*/
    strcpy(buf, "");
    sprintf(tmpTxtBuf, "\nVersion : %s", trackInfo->Version);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nLayer : ");
    strcat(buf, trackInfo->Layer);

    strcat(buf, "\nChecksums ? : ");
    strcat(buf, trackInfo->Error_protection);

    strcat(buf, "\nBitrate : ");
    sprintf(tmpTxtBuf, "%i kbps", trackInfo->bitRate);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nSample Rate : ");
    sprintf(tmpTxtBuf, "%i Hz", trackInfo->Frequency);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nFrames : ");
    sprintf(tmpTxtBuf, "%i", trackInfo->TotalFrames);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nLength : ");
    sprintf(tmpTxtBuf, "%i s", trackInfo->Length / 1000);
    strcat(buf, tmpTxtBuf);

    strcat(buf, "\nPrivate bit ? : ");
    strcat(buf, trackInfo->Private_bit);

    strcat(buf, "\nMode String : ");
    strcat(buf, trackInfo->Mode);

    strcat(buf, "\nCopyright ? : ");
    strcat(buf, trackInfo->Copyright);

    strcat(buf, "\nOriginal ? : ");
    strcat(buf, trackInfo->Original);

    strcat(buf, "\nDe-emphasis : ");
    strcat(buf, trackInfo->De_emphasis);

    return buf;
}

int
main(int argc, char **argv)
{
    InitMP3DecoderData();

    FileBuf fp;

    TrackInfo trackInfo;
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

        /*-- Open the file. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    if (!fp.open(inStream, kFileReadMode)) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }
    bs->open(&fp, MAX_SLOTS);

    stream->InitDecoder(bs, out_param, out_complex);

    /*-- This will determine the output quality. --*/
    SetAnyQualityParam(stream, &initParam);
    ReInitEngine(stream);

    if (!console->open(
            outStream, out_param->sampling_frequency, out_param->num_out_channels, waveOut))
        return EXIT_FAILURE;

    /*-- Store the equalizer settings into the dequantizer module. --*/
    stream->dbScale = eq_band->getdBScale();

    char infoBuffer[4096];
    retrieveTrackInfo(&trackInfo, stream->header);
    sprintf(infoBuffer, "\nStream parameters for %s :\n", inStream);
    fprintf(stdout, "%s\n", showTrackProperties(infoBuffer, &trackInfo));
    /*
    fprintf(
        stdout,
        "Version : %s\n",
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
    */

    size_t frames = 0;
    SEEK_STATUS sync = SYNC_FOUND;

    do {
        /*-- Get the output samples. --*/
        if (!DecodeFrame(stream, stream->buffer->pcm_sample))
            goto exit;

        /*-- Write to file. --*/
        console->writeBuffer(stream->buffer->pcm_sample, stream->out_param->num_out_samples);

        /*-- Find the start of the next frame. --*/
        sync = SeekSync(stream);

        fprintf(stdout, "Frames decoded: %zu\r", frames++);
        fflush(stdout);

        // if (frames > 2)
        //  break;
    } while (sync == SYNC_FOUND);

    console->close();

exit:
    delete out_param;
    delete out_complex;
    delete stream;
    delete bs;
    delete eq_band;
    delete console;

    return EXIT_SUCCESS;
}
