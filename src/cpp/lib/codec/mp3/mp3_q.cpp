/**************************************************************************
  mp3_q.cpp - MPEG-1, MPEG-2 LSF and MPEG-2.5 layer III dequantization subroutines.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <math.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/defines.h"
#include "mp3_q.h"
#include "mstream.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/*
   Purpose:     Enable equalizer.
   Explanation: - */
#define EQUALIZER

/*
   Purpose:     Pre-emphasis tables.
   Explanation: The first table is used when 'pre_flag' is TRUE, otherwise
                the second table is selected. */
static int16 pretab[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0 };
static int16 pretab0[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*
   Purpose:     Scaling table to obtain values for dequantization of
                layer III spectral components.
   Explanation: powTbl[i] = i^(4/3), i = 0,..., MAX_QUANT */
static FLOAT powTbl[MAX_QUANT + 1];

/*
   Purpose:     Gain table for mono and stereo streams.
   Explanation: -*/
static FLOAT _global_gain[MAX_GAIN];
static FLOAT *global_gain = _global_gain + 256;

/**************************************************************************
  Title        : InitMP3DequantModule

  Purpose      : Initializes scaling tables of the module.

  Usage        : InitMP3DequantModule()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
InitMP3DequantModule(void)
{
    for (int i = -256; i < 118 + 4; ++i)
        _global_gain[i + 256] = pow((double) 2.0, -0.25 * (double) (i + 210.0));

    for (int i = 0; i <= MAX_QUANT; ++i)
        powTbl[i] = pow((double) i, ((double) 4.0 / 3.0));
}

/**************************************************************************
  Title        : scaled_sample

  Purpose      : Reconstructs the quantized spectral value.

  Usage        : y = scaled_sample(gain, quant)

  Input        : gain  - scaling factor
                 quant - quantized value

  Output       : y - dequantized value

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static inline FLOAT
scaled_sample(FLOAT gain, int16 quant)
{
    if (quant == -1)
        gain = -gain;
    else if (quant == 0)
        gain = 0.0f;
    else if (quant != 1) {
        if (quant < 0)
            gain *= -powTbl[-quant];
        else
            gain *= powTbl[quant];
    }

    return (gain);
}

/**************************************************************************
  Title        : do_long_quant

  Purpose      : Dequantizes long block.

  Usage        : do_long_quant(ggain, quant, dequant, sfac, sfbs, sfb_offset,
                               gain_shift, pretabTbl, dbS)

  Input        : ggain      - global gain values (table)
                 quant      - quantized values
                 sfac       - scalefactors for the scalefactor bands.
                 sfbs       - number of scalefactor bands to be processed
                 sfb_offset - scalefactor band boundaries
                 gain_shift - helper variable for addressing the gain table
                 pretabTbl  - pre-emphasis table
                 dbS        - equalizer gain for each sfb

  Output       : dequant    - reconstructed samples

  Explanation  : Do not call this function if IS stereo is enabled in the
                 current frame.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

inline void
do_long_quant(FLOAT *ggain,
              int16 *quant,
              FLOAT *dequant,
              BYTE *sfac,
              int sfbs,
              int16 *sfb_offset,
              int gain_shift,
              int16 *pretabTbl,
              FLOAT *dbS)
{
    for (int i = 0, j = sfb_offset[0]; i < sfbs; i++) {
#ifdef EQUALIZER
        FLOAT gain0 = ggain[(*sfac++ + *pretabTbl++) << gain_shift] * *dbS++;
#else
        FLOAT gain0 = ggain[(*sfac++ + *pretabTbl++) << gain_shift];
#endif

        while (j < sfb_offset[i + 1]) {
            *dequant++ = scaled_sample(gain0, *quant++);
            j++;
        }
    }
}

/**************************************************************************
  Title        : do_short_quant

  Purpose      : Dequantizes short block.

  Usage        : do_short_quant(ggain, quant, dequant, sfac, subblock_gain,
                                sfb_start, sfb_width, max_bins, gain_shift, dbS)

  Input        : ggain         - global gain values (table)
                 quant         - quantized values
                 sfac          - scalefactors for the scalefactor bands.
                 subblock_gain - subblock gains for each short window
                 sfb_start     - starting scalefactor band
                 sfb_width     - scalefactor band width
                 max_bins      - max number of spectal bins to be dequantized
                 gain_shift    - helper variable for addressing the gain table
                 dbS           - equalizer gain for each sfb

  Output       : dequant       - reconstructed samples

  Explanation  : Do not call this function if IS stereo is enabled in the
                 current frame.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

inline void
do_short_quant(FLOAT *ggain,
               int16 *quant,
               FLOAT *dequant,
               BYTE *sfac[3],
               BYTE subblock_gain[3],
               int16 sfb_start,
               int16 sfb_end,
               int16 *sfb_width,
               int max_bins,
               int gain_shift,
               FLOAT *dbS)
{
    int cum_sfb_bin = 0;
    int16 *sfb_tbl = sfb_width;
    BYTE *sf[3];
    FLOAT gain1, gain2, gain3;

    sf[0] = sfac[0];
    sf[1] = sfac[1];
    sf[2] = sfac[2];
    for (int i = sfb_start; i < sfb_end; ++i) {
#ifdef EQUALIZER
        gain1 = ggain[subblock_gain[0] + ((*sf[0]++) << gain_shift)] * *dbS;
        gain2 = ggain[subblock_gain[1] + ((*sf[1]++) << gain_shift)] * *dbS;
        gain3 = ggain[subblock_gain[2] + ((*sf[2]++) << gain_shift)] * *dbS++;
#else
        gain1 = ggain[subblock_gain[0] + ((*sf[0]++) << gain_shift)];
        gain2 = ggain[subblock_gain[1] + ((*sf[1]++) << gain_shift)];
        gain3 = ggain[subblock_gain[2] + ((*sf[2]++) << gain_shift)];
#endif

        for (int j = 0; j < *sfb_tbl; ++j)
            *dequant++ = scaled_sample(gain1, *quant++);
        cum_sfb_bin += *sfb_tbl++;
        if (cum_sfb_bin > max_bins)
            return;

        for (int j = 0; j < *sfb_tbl; ++j)
            *dequant++ = scaled_sample(gain2, *quant++);
        cum_sfb_bin += *sfb_tbl++;
        if (cum_sfb_bin > max_bins)
            return;

        for (int j = 0; j < *sfb_tbl; ++j)
            *dequant++ = scaled_sample(gain3, *quant++);
        cum_sfb_bin += *sfb_tbl++;
        if (cum_sfb_bin > max_bins)
            return;
    }
}

/**************************************************************************
  Title        : do_ms_matrix

  Purpose      : Performs inverse MS matrixing.

  Usage        : do_ms_matrix(left_ch, right_ch, sfb_start, sfb_end)

  Input        : left_ch   - Mid values
                 right_ch  - Side values
                 sfb_start - starting scalefactor band
                 sfb_stop  - ending scalefactor band

  Output       : left_ch   - reconstructed samples for left channel
                 right_ch  - reconstructed samples for right channel

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
do_ms_matrix(FLOAT *left_ch, FLOAT *right_ch, int16 sfb_start, int16 sfb_end)
{
    int16 i;
    register FLOAT *left, *right;
    register FLOAT tmp;

    left = left_ch;
    right = right_ch;
    for (i = sfb_start; i < sfb_end; ++i) {
        tmp = *left - *right;
        *left = *left + *right;
        *right++ = tmp;
        left++;
    }
}

/*
 * IS stereo decoding aubroutines.
 */
#include "mp3is_q.cpp"

/**************************************************************************
  Title        : dequantize_cpe

  Purpose      : Dequantizes channel elements where IS is enabled.

  Usage        : dequantize_cpe(mp, gr)

  Input        : mp - MP3 stream parameters
                 gr - granule number (1 or 2)

  Explanation  : This function dequantizes layer 3 frames that are using
                 intensity stereo on certain scalefactor bands.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
dequantize_cpe(MP_Stream *mp, int gr)
{
    BOOL io;
    int gain_shift;
    III_SfbData *sfbData;
    int16 *width, *pretabTbl;
    int16 shift[MAX_CHANNELS];
    Granule_Info *gr_info[MAX_CHANNELS];
    III_Scale_Factors *scale_fac[MAX_CHANNELS];
    FLOAT *g_gain[MAX_CHANNELS << 1] = { 0 }, *glob_gain, *dbS;

    dbS = mp->dbScale;

    // Scalefactor parameters.
    sfbData = &mp->side_info->sfbData;

    // Granule infos for this call.
    gr_info[0] = mp->side_info->ch_info[LEFT_CHANNEL]->gr_info[gr];
    gr_info[1] = mp->side_info->ch_info[RIGHT_CHANNEL]->gr_info[gr];

    // Init the gain pointers.
    shift[0] = 1 + mp->scalefac_scale(gr_info[0]);
    shift[1] = 1 + mp->scalefac_scale(gr_info[1]);
    g_gain[0] = global_gain - gr_info[0]->global_gain;
    g_gain[2] = global_gain - gr_info[1]->global_gain;
    if (sfbData->ms_stereo) {
        g_gain[1] = g_gain[0] + 2;
        g_gain[3] = g_gain[2] + 2;
    }

    io = ((gr_info[0]->scalefac_compress & 1) == 1) ? 0 : 1;

    // Samples to be dequantized.
    qs[0] = mp->frame->ch_quant[LEFT_CHANNEL];
    qs[1] = mp->frame->ch_quant[RIGHT_CHANNEL];

    // Dequantized samples.
    dqs[0] = mp->buffer->ch_reconstructed[LEFT_CHANNEL];
    dqs[1] = mp->buffer->ch_reconstructed[RIGHT_CHANNEL];

    // Scalefactors for left and right channel.
    scale_fac[0] = mp->side_info->ch_info[LEFT_CHANNEL]->scale_fac;
    scale_fac[1] = mp->side_info->ch_info[RIGHT_CHANNEL]->scale_fac;

    switch (gr_info[0]->block_mode) {
        case SHORT_BLOCK_MODE:
            width = sfbData->sfbWidthCumShort;
            for (int i = 0, cum_sfb = 0; i < sfbData->max_sfb_short; i++) {
                for (int j = 0; j < 3; j++, width++) {
                    cum_sfb += *width;

                    switch (mp->side_info->s_mode_short[j][i]) {
                        case ONLY_STEREO:
                            SHORT_QUANT_IS(g_gain[0], g_gain[2], 1.0f, 1.0f,
                                           scale_fac[0]->scalefac_short[j][i],
                                           scale_fac[1]->scalefac_short[j][i],
                                           gr_info[0]->subblock_gain[j],
                                           gr_info[1]->subblock_gain[j], shift[0],
                                           shift[1], *width, dbS);
                            break;

                        case MS_STEREO: {
                            FLOAT *tmp[2] = { dqs[0], dqs[1] };
                            SHORT_QUANT_IS(g_gain[1], g_gain[3], 1.0f, 1.0f,
                                           scale_fac[0]->scalefac_short[j][i],
                                           scale_fac[1]->scalefac_short[j][i],
                                           gr_info[0]->subblock_gain[j],
                                           gr_info[1]->subblock_gain[j], shift[0],
                                           shift[1], *width, dbS);
                            do_ms_matrix(tmp[0], tmp[1], 0, *width);
                        } break;

                        case IS_STEREO: {
                            FLOAT gainl = is_ratio[scale_fac[1]->scalefac_short[j][i]][0];
                            FLOAT gainr = is_ratio[scale_fac[1]->scalefac_short[j][i]][1];
                            FLOAT *qll = dqs[0];
                            FLOAT *qlr = dqs[1];

                            SHORT_QUANT_IS_LEFT1(
                                g_gain[0], gainl, scale_fac[0]->scalefac_short[j][i],
                                gr_info[0]->subblock_gain[j], shift[0], *width, dbS);

                            /*-- Dequantize right channel. --*/
                            for (int k = 0; k < *width; ++k)
                                *qlr++ = *qll++ * gainr;
                        } break;

                        case LSF_IS_STEREO:
                            if (scale_fac[1]->scalefac_short[j][i] != 0) {
                                FLOAT gain;
                                if (scale_fac[1]->scalefac_short[j][i] % 2 == 1) {
                                    int16 *q = qs[1];
                                    qs[1] = qs[0];
                                    gain =
                                        is_ratio_lsf[scale_fac[1]->scalefac_short[j][i]]
                                                    [io];
                                    SHORT_QUANT_IS_LEFT(
                                        g_gain[0], g_gain[2], gain, 1.0f,
                                        scale_fac[0]->scalefac_short[j][i],
                                        scale_fac[0]->scalefac_short[j][i],
                                        gr_info[0]->subblock_gain[j],
                                        gr_info[0]->subblock_gain[j], shift[0], shift[0],
                                        *width, dbS);
                                    qs[1] = q + *width;
                                }
                                else {
                                    int16 *q = qs[1];
                                    qs[1] = qs[0];
                                    gain =
                                        is_ratio_lsf[scale_fac[1]->scalefac_short[j][i]]
                                                    [io];
                                    SHORT_QUANT_IS_RIGHT(
                                        g_gain[0], g_gain[2], 1.0f, gain,
                                        scale_fac[0]->scalefac_short[j][i],
                                        scale_fac[0]->scalefac_short[j][i],
                                        gr_info[0]->subblock_gain[j],
                                        gr_info[0]->subblock_gain[j], shift[0], shift[0],
                                        *width, dbS);
                                    qs[1] = q + *width;
                                }
                            }
                            else {
                                FLOAT *q[2] = { dqs[0], dqs[1] };
                                SHORT_QUANT_IS_LEFT0(
                                    g_gain[0], scale_fac[0]->scalefac_short[j][i],
                                    gr_info[0]->subblock_gain[j], shift[0], *width, dbS);
                                memcpy(q[1], q[0], *width << 2);
                            }
                            break;

                        default:
                            break;
                    }
                    if (cum_sfb > sfbData->sbHybrid)
                        goto exit;
                }
                dbS++;
            }
            break;

        case LONG_BLOCK_MODE: {
            dbS = mp->dbScale;

            int ms_bands = sfbData->ms_bands_long;
            if (ms_bands) {
                // left
                glob_gain = (sfbData->ms_stereo) ? g_gain[1] : g_gain[0];
                gain_shift = shift[0];
                pretabTbl = (mp->pre_flag(gr_info[0])) ? pretab : pretab0;
                do_long_quant(glob_gain, qs[0], dqs[0], scale_fac[0]->scalefac_long,
                              ms_bands, sfbData->sfbLong, gain_shift, pretabTbl, dbS);

                dbS = mp->dbScale;

                // right
                glob_gain = (sfbData->ms_stereo) ? g_gain[3] : g_gain[2];
                gain_shift = shift[1];
                pretabTbl = (mp->pre_flag(gr_info[1])) ? pretab : pretab0;
                do_long_quant(glob_gain, qs[1], dqs[1], scale_fac[1]->scalefac_long,
                              ms_bands, sfbData->sfbLong, gain_shift, pretabTbl, dbS);

                if (sfbData->ms_stereo)
                    do_ms_matrix(mp->buffer->ch_reconstructed[LEFT_CHANNEL],
                                 mp->buffer->ch_reconstructed[RIGHT_CHANNEL], 0,
                                 sfbData->sfbLong[ms_bands]);

                qs[0] += sfbData->sfbLong[ms_bands];
                qs[1] += sfbData->sfbLong[ms_bands];
                dqs[0] += sfbData->sfbLong[ms_bands];
                dqs[1] += sfbData->sfbLong[ms_bands];
            }

            BYTE *sf[2] = { scale_fac[0]->scalefac_long + ms_bands,
                            scale_fac[1]->scalefac_long + ms_bands };

            StereoMode *st = mp->side_info->s_mode_long + ms_bands;

            width = sfbData->sfbWidthLong + ms_bands;

            int16 *pre_tbl[2];
            pre_tbl[0] = (mp->pre_flag(gr_info[0])) ? pretab : pretab0;
            pre_tbl[1] = (mp->pre_flag(gr_info[1])) ? pretab : pretab0;
            pre_tbl[0] += ms_bands;
            pre_tbl[1] += ms_bands;

            int16 sfbIdxSave = 0, sfbBandSave;
            int16 *sfb_ = NULL;
            if (sfbData->sbHybrid < sfbData->bandLimit) {
                sfbBandSave = sfbData->sfbLongSfbIdx[sfbData->sbHybrid];
                sfb_ = &sfbData->sfbLong[sfbBandSave];
                sfbIdxSave = *sfb_;
                *sfb_ = sfbData->sbHybrid;
            }
            else
                sfbBandSave = sfbData->max_sfb_long;

            for (int i = ms_bands; i < sfbBandSave;
                 i++, st++, width++, sf[0]++, sf[1]++, pre_tbl[0]++, pre_tbl[1]++) {
                switch (*st) {
                    case ONLY_STEREO:
                        LONG_QUANT_IS(g_gain[0], g_gain[2], 1.0f, 1.0f, *sf[0], *sf[1],
                                      *pre_tbl[0], *pre_tbl[1], shift[0], shift[1],
                                      *width, dbS);
                        break;

                    case MS_STEREO: {
                        FLOAT *tmp[2] = { dqs[0], dqs[1] };
                        LONG_QUANT_IS(g_gain[1], g_gain[3], 1.0f, 1.0f, *sf[0], *sf[1],
                                      *pre_tbl[0], *pre_tbl[1], shift[0], shift[1],
                                      *width, dbS);
                        do_ms_matrix(tmp[0], tmp[1], 0, *width);
                    } break;

                    case IS_STEREO: {
                        FLOAT gainl = is_ratio[*sf[1]][0];
                        FLOAT gainr = is_ratio[*sf[1]][1];
                        FLOAT *dql = dqs[0];
                        FLOAT *dqr = dqs[1];
                        LONG_QUANT_IS_LEFT1(g_gain[0], gainl, *sf[0], *pre_tbl[0],
                                            shift[0], *width, dbS);
                        /*-- Dequantize right channel. --*/
                        for (int k = 0; k < *width; ++k)
                            *dqr++ = *dql++ * gainr;
                    } break;

                    case LSF_IS_STEREO:
                        if (*sf[1] != 0) {
                            if (*sf[1] & 1) {
                                int16 *q = qs[1];
                                qs[1] = qs[0];
                                LONG_QUANT_IS_LEFT(g_gain[0], g_gain[2],
                                                   is_ratio_lsf[*sf[1]][io], 1.0f, *sf[0],
                                                   *sf[0], *pre_tbl[0], *pre_tbl[0],
                                                   shift[0], shift[0], *width, dbS);
                                qs[1] = q + *width;
                            }
                            else {
                                int16 *q = qs[1];
                                qs[1] = qs[0];
                                LONG_QUANT_IS_RIGHT(g_gain[0], g_gain[2], 1.0f,
                                                    is_ratio_lsf[*sf[1]][io], *sf[0],
                                                    *sf[0], *pre_tbl[0], *pre_tbl[0],
                                                    shift[0], shift[0], *width, dbS);
                                qs[1] = q + *width;
                            }
                        }
                        else {
                            FLOAT *q[2] = { dqs[0], dqs[1] };
                            LONG_QUANT_IS_LEFT0(g_gain[0], *sf[0], *pre_tbl[0], shift[0],
                                                *width, dbS);
                            memcpy(q[1], q[0], *width << 2);
                        }
                        break;

                    default:
                        break;
                }
            }
            if (sfbData->sbHybrid < sfbData->bandLimit)
                *sfb_ = sfbIdxSave;
            break;
        }

        default:
            break;
    }

exit:
    if (sfbData->sbHybrid < sfbData->bandLimit) {
        int diff = sfbData->bandLimit - sfbData->sbHybrid;
        memset(&mp->buffer->ch_reconstructed[0][sfbData->sbHybrid], 0, diff << 2);
        memset(&mp->buffer->ch_reconstructed[1][sfbData->sbHybrid], 0, diff << 2);
    }
}

/**************************************************************************
  Title        : dequantize_sce

  Purpose      : Dequantizes single channel element.

  Usage        : dequantize_sce(mp, sfbData, gr, ch)

  Input        : mp      - MP3 stream parameters
                 sfbData - layer 3 sfb parameters
                 gr      - granule number (1 or 2)
                 ch      - channel number (left or right)

  Explanation  : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
dequantize_sce(MP_Stream *mp, III_SfbData *sfbData, int gr, int ch)
{
    int gain_shift;
    Granule_Info *gr_info;
    III_Scale_Factors *scale_fac;
    FLOAT *dequant, *glob_gain, *dbS;
    int16 *quant, *pretabTbl;

    dbS = mp->dbScale;

    // Granule info for this call.
    gr_info = mp->side_info->ch_info[ch]->gr_info[gr];

    glob_gain = global_gain - gr_info->global_gain;
    if (sfbData->ms_stereo)
        glob_gain += 2;
    gain_shift = 1 + mp->scalefac_scale(gr_info);
    pretabTbl = (mp->pre_flag(gr_info)) ? pretab : pretab0;

    quant = mp->frame->ch_quant[ch];
    dequant = mp->buffer->ch_reconstructed[ch];

    // Scalefactors for long and short blocks.
    scale_fac = mp->side_info->ch_info[ch]->scale_fac;

    switch (gr_info->block_mode) {
        case SHORT_BLOCK_MODE:
            do_short_quant(glob_gain, quant, dequant, scale_fac->scalefac_short,
                           gr_info->subblock_gain, 0, sfbData->max_sfb_short,
                           sfbData->sfbWidthCumShort, gr_info->zero_part_start,
                           gain_shift, dbS);
            break;

        case LONG_BLOCK_MODE:
            if (gr_info->zero_part_start < sfbData->bandLimit) {
                int16 *sfb;
                int16 sfbIdxSave, sfbBandSave;

                sfbBandSave = sfbData->sfbLongSfbIdx[gr_info->zero_part_start];
                sfb = &sfbData->sfbLong[sfbBandSave];
                sfbIdxSave = *sfb;
                *sfb = gr_info->zero_part_start;
                do_long_quant(glob_gain, quant, dequant, scale_fac->scalefac_long,
                              sfbBandSave, sfbData->sfbLong, gain_shift, pretabTbl, dbS);
                *sfb = sfbIdxSave;
            }
            else
                do_long_quant(glob_gain, quant, dequant, scale_fac->scalefac_long,
                              sfbData->max_sfb_long, sfbData->sfbLong, gain_shift,
                              pretabTbl, dbS);
            break;

        default:
            break;
    }

    if (gr_info->zero_part_start < sfbData->bandLimit) {
        int diff = sfbData->bandLimit - gr_info->zero_part_start;
        memset(&mp->buffer->ch_reconstructed[ch][gr_info->zero_part_start], 0, diff << 2);
    }
}

/**************************************************************************
  Title        : StereoMono

  Purpose      : Downmixes stereo samples to mono samples.

  Usage        : StereoMono(left, right, len)

  Input        : left  - left channel samples
                 right - right channel samples
                 len   - # of samples to be downmixed

  Output       : left  - mono samples

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
StereoMono(FLOAT *left, FLOAT *right, int len)
{
    register FLOAT *l = left;
    register FLOAT *r = right;

    for (int i = 0; i < len; ++i, l++)
        *l = (*l + *r++) * 0.5f;
}

/**************************************************************************
  Title        : III_dequantize

  Purpose      : Dequantizes layer III frame.

  Usage        : III_dequantize(mp, gr)

  Input        : mp - MP3 stream parameters
                 gr - granule number (1 or 2)

  Explanation  : If two channels present, dequantizes both channels.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_dequantize(MP_Stream *mp, int gr)
{
    int min_spec;
    III_SfbData *sfbData;
    Granule_Info *gr_info;

    // Scalefactor parameters.
    sfbData = &mp->side_info->sfbData;

    if (!sfbData->is_stereo) {
        /*
         * Following channel configurations are handled here :
         *
         * - only mono
         * - only stereo
         * - only MS stereo
         */

        /*
         * Dequantize left channel.
         */
        gr_info = mp->side_info->ch_info[LEFT_CHANNEL]->gr_info[gr];
        sfbData->sbHybrid = gr_info->zero_part_start;
        dequantize_sce(mp, sfbData, gr, LEFT_CHANNEL);

        /*
         * Dequantize right channel.
         */
        BOOL downMix = (mp->header->channels() == 2 &&
                        mp->out_param->num_out_channels == 1 && !sfbData->ms_stereo);
        if (downMix || mp->out_param->num_out_channels == 2) {
            gr_info = mp->side_info->ch_info[RIGHT_CHANNEL]->gr_info[gr];
            sfbData->sbHybrid = MAX(sfbData->sbHybrid, gr_info->zero_part_start);
            dequantize_sce(mp, sfbData, gr, RIGHT_CHANNEL);
            min_spec = MIN(sfbData->sbHybrid, sfbData->bandLimit);
            if (sfbData->ms_stereo) {
                FLOAT *left = mp->buffer->ch_reconstructed[LEFT_CHANNEL];
                FLOAT *right = mp->buffer->ch_reconstructed[RIGHT_CHANNEL];
                do_ms_matrix(left, right, 0, min_spec);
            }
        }

        if (downMix)
            StereoMono(mp->buffer->ch_reconstructed[LEFT_CHANNEL],
                       mp->buffer->ch_reconstructed[RIGHT_CHANNEL], min_spec);
    }
    else {
        /*
         * Following channel configurations are handled here :
         *
         * - only IS stereo
         * - mixture of only stereo, MS stereo and IS stereo
         */
        register Granule_Info *gr_info_r;
        gr_info = mp->side_info->ch_info[LEFT_CHANNEL]->gr_info[gr];
        gr_info_r = mp->side_info->ch_info[RIGHT_CHANNEL]->gr_info[gr];
        sfbData->sbHybrid = MAX(gr_info->zero_part_start, gr_info_r->zero_part_start);

        dequantize_cpe(mp, gr);

        if (mp->out_param->num_out_channels == 1)
            StereoMono(mp->buffer->ch_reconstructed[LEFT_CHANNEL],
                       mp->buffer->ch_reconstructed[RIGHT_CHANNEL],
                       MIN(sfbData->sbHybrid, sfbData->bandLimit));
    }
}
