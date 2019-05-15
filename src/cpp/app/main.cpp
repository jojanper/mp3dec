#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/io/iobuf.h"
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

int
main(int /*argc*/, char **argv)
{
    FileBuf fp;
    // EQ_Band *eq_band;
    CodecInitParam initParam;
    BitStream *bs = new BitStream();
    MP_Stream *stream = new MP_Stream();

    auto out_param = new Out_Param();
    auto out_complex = new Out_Complexity();
    memset(out_param, 0, sizeof(Out_Param));
    memset(out_complex, 0, sizeof(Out_Complexity));

    /*-- Open the file. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    fp.open(argv[1], kFileWriteMode);
    bs->open(&fp, MAX_SLOTS);

    stream->InitDecoder(bs, out_param, out_complex);

    /*-- This will determine the output quality. --*/
    SetAnyQualityParam(stream, &initParam);
    ReInitEngine(stream);

    /*-- Next, according to the output quality, modify the sfb tables. --*/
    III_BandLimit(&stream->side_info->sfbData, out_param->decim_factor);

    /*-- Store the equalizer settings into the dequantizer module. --*/
    // stream->dbScale = eq_band->GetdBScale();

    SEEK_STATUS sync = SYNC_FOUND;

    do {
        /*-- Get the output samples. --*/
        if (!DecodeFrame(stream, stream->buffer->pcm_sample))
            goto exit;

        /*-- Write to file. --*/
        // console->WriteBuffer(mp->buffer->pcm_sample, mp->out_param->num_out_samples);

        /*-- Find the start of the next frame. --*/
        sync = SeekSync(stream);

    } while (sync == SYNC_FOUND);

exit:
    delete stream;
    delete bs;

    return EXIT_SUCCESS;
}
