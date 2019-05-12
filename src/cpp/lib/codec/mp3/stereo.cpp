/**************************************************************************
  stereo.cpp - Layer III IS stereo decoding subroutines.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "mstream.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/**************************************************************************
  Title        : snzq

  Purpose      : Searches the start of the "zero_part" of the spectrum of the
                 right channel for IS stereo processing.

  Usage        : y = snzq(side, quant, sfb_stop, offset, is_long)

  Input        : side     - layer 3 side info parameters
                 quant    - quantized samples of the right channel
                 sfb_stop - sfb limit for short blocks (0=short, 3=mixed block)
                 offset   - For long blocks :
                            bin number of the last decoded quantized
                            coefficient as determined by function
                            'III_huffman_decode' in module 'huffman.cpp'
                            For short blocks :
                            short block number (0, 1, 2)
                 is_long  - TRUE if long block, FALSE otherwise

  Output       : y - index to IS stereo coded part of the spectrum

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static int16
snzq(III_Side_Info *side, int16 *quant, int sfb_stop, int offset, BOOL is_long)
{
    int16 i, *q;

    /*-- Long blocks. --*/
    if (is_long) {
        if (offset < side->sfbData.bandLimit)
            memset(quant + offset, 0, (side->sfbData.bandLimit - offset) * sizeof(int16));

        offset--;
        q = quant + offset;
        while (offset && *q-- == 0)
            offset--;

        i = 0;
        while (side->sfbData.sfbLong[i] <= offset)
            i++;
    }

    /*-- Short blocks. --*/
    else {
        int16 sfb, lines;

        offset++;
        sfb = 12;
        i = sfb_stop;
        while (sfb >= sfb_stop) {
            lines = side->sfbData.sfbWidth[sfb];
            q = quant + 3 * side->sfbData.sfbShort[sfb] + offset * lines - 1;

            while (lines >= 0) {
                if (*q-- != 0) {
                    lines = -1;
                    i = sfb + 1;
                    sfb = -1;
                    break;
                }

                lines--;
            }

            sfb--;
        }
    }

    return (i);
}

/**************************************************************************
  Title        : do_is

  Purpose      : Processes the given scale factor array and assigns a stereo
                 code for each scalefactor band according to the value of the
                 scale factor in that scalefactor band.

  Usage        : do_is(stereo_mode, scale_fac, sfb_start, max_sfb, ms_stereo)

  Input        : scale_fac   - scale factors for each scalefactor band
                 sfb_start   - first scalefactor band for IS stereo processing
                 max_sfb     - maximum # of scalefactor bands
                 ms_stereo   - TRUE is MS stereo enabled, FALSE otherwise

  Output       : stereo_mode - stereo codes for each scalefactor band

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
do_is(StereoMode *stereo_mode, BYTE *scale_fac, int sfb_start, int max_sfb, int ms_stereo)
{
    BYTE *scale_factors;
    StereoMode s1, *tmp;

    scale_factors = scale_fac;
    tmp = stereo_mode;

    s1 = (ms_stereo) ? MS_STEREO : ONLY_STEREO;

    scale_factors += sfb_start;
    for (int i = 0; i < sfb_start; i++)
        *tmp++ = s1;

    for (int sfb = sfb_start; sfb < max_sfb; sfb++) {
        /*-- Intensity stereo. --*/
        if (*scale_factors++ != 7)
            *tmp++ = IS_STEREO;
        else
            *tmp++ = s1;
    }
}

/**************************************************************************
  Title        : IS_MPEG1

  Purpose      : MPEG-1 IS stereo mapping.

  Usage        : IS_MPEG1(side, quant, gr)

  Input        : side  - layer 3 side info parameters
                 quant - quantized spectral samples of left channel
                 gr    - granule number

  Explanation  : The illegal stereo position is hardcoded (= 7).

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
IS_MPEG1(III_Side_Info *side, int16 *quant, int gr)
{
    int zero_part;
    int16 i, sfb_start;
    III_Scale_Factors *scale_fac;

    scale_fac = side->ch_info[RIGHT_CHANNEL]->scale_fac;
    zero_part = side->ch_info[RIGHT_CHANNEL]->gr_info[gr]->zero_part_start;
    switch (side->ch_info[LEFT_CHANNEL]->gr_info[gr]->block_mode) {
        case SHORT_BLOCK_MODE:
            if (zero_part < MAX_MONO_SAMPLES)
                memset(quant + zero_part, 0,
                       (MAX_MONO_SAMPLES - zero_part) * sizeof(int16));

            /*-- Assign stereo codes for each short block. --*/
            for (i = 0; i < 3; i++) {
                sfb_start = snzq(side, quant, 0, i, FALSE);
                do_is(side->s_mode_short[i], scale_fac->scalefac_short[i], sfb_start,
                      MAX_SHORT_SFB_BANDS, side->sfbData.ms_stereo);
                side->sfbData.ms_bands_short[i] = sfb_start;
            }
            break;

        case LONG_BLOCK_MODE:
            sfb_start = snzq(side, quant, -1, zero_part, TRUE);
            do_is(side->s_mode_long, scale_fac->scalefac_long, sfb_start,
                  MAX_LONG_SFB_BANDS, side->sfbData.ms_stereo);
            side->sfbData.ms_bands_long = sfb_start;
            break;

        default:
            break;
    }
}

/**************************************************************************
  Title        : IS_MPEG2

  Purpose      : MPEG-2 LSF and MPEG-2.5 IS stereo mapping.

  Usage        : IS_MPEG2(side, quant, gr)

  Input        : side  - layer 3 side info parameters
                 quant - quantized spectral samples of left channel
                 gr    - granule number

  Explanation  : The illegal stereo position depends on the maximum value
                 that is allowed for each group of scale factors.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
IS_MPEG2(III_Side_Info *side, int16 *quant, int gr)
{
    int zero_part;
    BYTE *scale_factors;
    III_Scale_Factors *scale_fac;
    int16 sfb_start, il_pos[MAX_SHORT_SFB_BANDS * 3];

    scale_fac = side->ch_info[RIGHT_CHANNEL]->scale_fac;
    StereoMode s1 = (side->sfbData.ms_stereo) ? MS_STEREO : ONLY_STEREO;
    zero_part = side->ch_info[RIGHT_CHANNEL]->gr_info[gr]->zero_part_start;

    for (int w = 0, k = 0; w < 3; w++)
        for (int i = 0; i < side->is_info.nr_sfb[w]; i++)
            il_pos[k++] = side->is_info.is_len[w];

    switch (side->ch_info[LEFT_CHANNEL]->gr_info[gr]->block_mode) {
        case SHORT_BLOCK_MODE:
            if (zero_part < MAX_MONO_SAMPLES)
                memset(quant + zero_part, 0,
                       (MAX_MONO_SAMPLES - zero_part) * sizeof(int16));

            /*-- Assign stereo codes for each short block. --*/
            for (int i = 0; i < 3; i++) {
                sfb_start = snzq(side, quant, 0, i, FALSE);
                side->sfbData.ms_bands_short[i] = sfb_start;
                scale_factors = &scale_fac->scalefac_short[i][sfb_start];

                StereoMode *stmp = side->s_mode_short[i];
                for (int j = 0; j < sfb_start; j++)
                    *stmp++ = s1;

                for (int j = sfb_start, k = sfb_start + i; j < MAX_SHORT_SFB_BANDS;
                     j++, k += 3) {
                    /*-- Intensity stereo. --*/
                    if (*scale_factors++ != il_pos[k] || il_pos[k] == 0)
                        *stmp++ = LSF_IS_STEREO;
                    else
                        *stmp++ = s1;
                }
            }
            break;

        case LONG_BLOCK_MODE: {
            sfb_start = snzq(side, quant, -1, zero_part, TRUE);
            side->sfbData.ms_bands_long = sfb_start;
            scale_factors = scale_fac->scalefac_long + sfb_start;

            StereoMode *stmp = side->s_mode_long;
            for (int i = 0; i < sfb_start; i++)
                *stmp++ = s1;

            for (int i = sfb_start; i < MAX_LONG_SFB_BANDS; i++) {
                /*-- Intensity stereo. --*/
                if (*scale_factors++ != il_pos[i] || il_pos[i] == 0)
                    *stmp++ = LSF_IS_STEREO;
                else
                    *stmp++ = s1;
            }
            break;
        }

        default:
            break;
    }
}

typedef void (*IS_FUNCTION)(III_Side_Info *side, int16 *quant, int gr);

/*
 * IS stereo function pointers for ease of access.
 */
static IS_FUNCTION ISFunc[2] = { IS_MPEG1, IS_MPEG2 };

/**************************************************************************
  Title        : III_stereo_mode

  Purpose      : Interface for layer III IS stereo mapping.

  Usage        : III_stereo_mode(mp, gr)

  Input        : mp - input stream parameters
                 gr - granule number

  Explanation  : This function only assigns a stereo code to each scalefactor
                 band. This information is then utilized during the
                 dequantization to fully minimize the number of multiplications.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_stereo_mode(MP_Stream *mp, int gr)
{
    ISFunc[mp->side_info->sfbData.lsf](mp->side_info, mp->frame->ch_quant[RIGHT_CHANNEL],
                                       gr);
}
