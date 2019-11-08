/**************************************************************************
  sfb.cpp - Scalefactor band (sfb) implementations for layer III.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#include <string.h>

#include "mpheader.h"
#include "sfb.h"

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*
   Purpose:     Width of scalefactor bands of short blocks.
   Explanation: - */
static int16 sfb_width_table_[9][13] = {
    // MPEG-2
    { 4, 4, 4, 6, 6, 8, 10, 14, 18, 26, 32, 42, 18 },
    { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 32, 44, 12 },
    { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 },
    // MPEG-1
    { 4, 4, 4, 4, 6, 8, 10, 12, 14, 18, 22, 30, 56 },
    { 4, 4, 4, 4, 6, 6, 10, 12, 14, 16, 20, 26, 66 },
    { 4, 4, 4, 4, 6, 8, 12, 16, 20, 26, 34, 42, 12 },
    // MPEG-2.5
    { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 },
    { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 },
    { 8, 8, 8, 12, 16, 20, 24, 28, 36, 2, 2, 2, 26 }
};

/*
   Purpose:     Scalefactor band boundaries.
   Explanation: - */
static SFBAND_DATA sfBandIndex[9] = {
    { // MPEG-2
      { 0,   6,   12,  18,  24,  30,  36,  44,  54,  66,  80, 96,
        116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
      { 0, 4, 8, 12, 18, 24, 32, 42, 56, 74, 100, 132, 174, 192 } }, // 22050 Hz
    { { 0,   6,   12,  18,  24,  30,  36,  44,  54,  66,  80, 96,
        114, 136, 162, 194, 232, 278, 330, 394, 464, 540, 576 },
      { 0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 136, 180, 192 } }, // 24000 Hz
    { { 0,   6,   12,  18,  24,  30,  36,  44,  54,  66,  80, 96,
        116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
      { 0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192 } }, // 16000 Hz
    {                                                                // MPEG-1
      { 0,  4,  8,   12,  16,  20,  24,  30,  36,  44,  52, 62,
        74, 90, 110, 134, 162, 196, 238, 288, 342, 418, 576 },
      { 0, 4, 8, 12, 16, 22, 30, 40, 52, 66, 84, 106, 136, 192 } }, // 44100 Hz
    { { 0,  4,  8,   12,  16,  20,  24,  30,  36,  42,  50, 60,
        72, 88, 106, 128, 156, 190, 230, 276, 330, 384, 576 },
      { 0, 4, 8, 12, 16, 22, 28, 38, 50, 64, 80, 100, 126, 192 } }, // 48000 Hz
    { { 0,  4,   8,   12,  16,  20,  24,  30,  36,  44,  54, 66,
        82, 102, 126, 156, 194, 240, 296, 364, 448, 550, 576 },
      { 0, 4, 8, 12, 16, 22, 30, 42, 58, 78, 104, 138, 180, 192 } }, // 32000 Hz
    {                                                                // MPEG-2.5
      { 0,   6,   12,  18,  24,  30,  36,  44,  54,  66,  80, 96,
        116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
      { 0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192 } }, // 11025 Hz
    { { 0,   6,   12,  18,  24,  30,  36,  44,  54,  66,  80, 96,
        116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
      { 0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192 } }, // 12000 Hz
    { { 0,   12,  24,  36,  48,  60,  72,  88,  108, 132, 160, 192,
        232, 280, 336, 400, 476, 566, 568, 570, 572, 574, 576 },
      { 0, 8, 16, 24, 36, 52, 72, 96, 124, 160, 162, 164, 166, 192 } }
}; // 8000 Hz

/*
   Purpose:     Number of scalefactor bands for mixed blocks.
   Explanation: - */
static const int16 nsfb_mixed[9] = { 6, 6, 6, 8, 8, 8, 6, 6, 3 };

/**************************************************************************
  Internal Objects
  *************************************************************************/

static const int16 *III_sfbOffsetLong(MP_Header *header);

static const int16 *III_sfbOffsetLongLimit(int16 *sfb_offset, int16 *max_sfb, int16 sfb_lines);

static const int16 *
III_sfbWidthShortTblLimit(int16 *sfb_width, int16 *max_sfb, int16 sfb_lines);

static const int16 *III_sfbOffsetShort(MP_Header *header);

static const int16 *III_sfbWidthTblShort(MP_Header *header);


/**************************************************************************
  Title        : III_SfbDataInit

  Purpose      : Initializes layer III scalefactor band parameters.

  Usage        : III_SfbDataInit(sfbData, header)

  Input        : header  - frame header

  Output       : sfbData - sfb parameters for layer III

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_SfbDataInit(III_SfbData *sfbData, MP_Header *header)
{
    if (header->version() == MPEG_AUDIO_ID) // MPEG-1
    {
        sfbData->max_gr = 2;
        sfbData->lsf = FALSE;
    }
    else // MPEG-2
    {
        sfbData->max_gr = 1;
        sfbData->lsf = TRUE;
    }

    sfbData->ms_stereo = FALSE;
    sfbData->is_stereo = FALSE;

    memcpy(
        sfbData->sfbOffsetLong,
        III_sfbOffsetLong(header),
        (MAX_LONG_SFB_BANDS + 1) * sizeof(int16));
    sfbData->sfbLong = sfbData->sfbOffsetLong;

    for (int i = 0; i < MAX_LONG_SFB_BANDS; i++) {
        sfbData->sfbWidthLong[i] = sfbData->sfbLong[i + 1] - sfbData->sfbLong[i];
        for (int j = sfbData->sfbLong[i]; j < sfbData->sfbLong[i + 1]; j++)
            sfbData->sfbLongSfbIdx[j] = i + 1;
    }
    sfbData->sfbLongSfbIdx[MAX_MONO_SAMPLES] = MAX_LONG_SFB_BANDS;

    memcpy(
        sfbData->sfbOffsetShort,
        III_sfbOffsetShort(header),
        (MAX_SHORT_SFB_BANDS + 1) * sizeof(int16));
    sfbData->sfbShort = sfbData->sfbOffsetShort;

    memcpy(
        sfbData->sfbWidthShort,
        III_sfbWidthTblShort(header),
        (MAX_SHORT_SFB_BANDS + 1) * sizeof(int16));
    sfbData->sfbWidth = sfbData->sfbWidthShort;

    for (int i = 0, j = 0; i < MAX_SHORT_SFB_BANDS; i++) {
        sfbData->sfbWidthCumShort[j++] = sfbData->sfbWidth[i];
        sfbData->sfbWidthCumShort[j++] = sfbData->sfbWidth[i];
        sfbData->sfbWidthCumShort[j++] = sfbData->sfbWidth[i];
    }

    sfbData->max_sfb_long = MAX_LONG_SFB_BANDS;
    sfbData->max_sfb_short = MAX_SHORT_SFB_BANDS;

    int fidx = 6 * header->mp25version() + header->sfreq() + header->version() * 3;
    sfbData->nsfb_mixed = nsfb_mixed[fidx];

    sfbData->bandLimit = MAX_MONO_SAMPLES;
}


/**************************************************************************
  Title        : III_BandLimit

  Purpose      : Modifies scalefactor band boundaries.

  Usage        : III_BandLimit(sfbData, decim_factor)

  Input        : sfbData      - scalefactor band parameters
                 decim_factor - decimation factor

  Output       : sfbData - modified scalefactor band parameters

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_BandLimit(III_SfbData *sfbData, int decim_factor)
{
    if (decim_factor != 1) {
        int blim = (decim_factor == 4) ? 160 : 310;
        if (blim < sfbData->bandLimit)
            sfbData->bandLimit = blim;

        III_sfbOffsetLongLimit(sfbData->sfbLong, &sfbData->max_sfb_long, blim);

        for (int i = 0; i < sfbData->max_sfb_long; i++) {
            sfbData->sfbWidthLong[i] = sfbData->sfbLong[i + 1] - sfbData->sfbLong[i];
            for (int j = sfbData->sfbLong[i]; j < sfbData->sfbLong[i + 1]; j++)
                sfbData->sfbLongSfbIdx[j] = i + 1;
        }

        /*
         * Due to grouping of short blocks, modifying the scalefactor boundaries
         * will result in an annoying quality artifact. This is because higher
         * bins are zero and after reordering they appear at a lower frequency.
         * That's why the maximum bin that need to be decoded is a little bit
         * higher than what is actually needed. Therefore, after reordering,
         * all the bins up to the limit are correctly reconstructed and no (?)
         * artifacts appear in the output signal.
         */
        int limit = (decim_factor == 2) ? 310 : 160;
        III_sfbWidthShortTblLimit(sfbData->sfbWidthCumShort, &sfbData->max_sfb_short, limit);
    }
}


/**************************************************************************
  Title        : III_sfbOffsetLong

  Purpose      : Retrieves the scalefactor band table for long blocks.

  Usage        : y = III_sfbOffsetLong(header)

  Input        : header - frame header

  Output       : y - scalefactor band boundaries for long blocks

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int16 *
III_sfbOffsetLong(MP_Header *header)
{
    int mp25idx = 6 * header->mp25version();

    return (&sfBandIndex[mp25idx + header->sfreq() + header->version() * 3].l[0]);
}


/**************************************************************************
  Title        : III_sfbOffsetLongLimit

  Purpose      : Modifies the scalefactor band boundraries of long blocks
                 according to the information given in the parameter list.

  Usage        : y = III_sfbOffsetLongLimit(sfb_offset, max_sfb, sfb_lines)

  Input        : sfb_offset - scalefactor band offsets
                 max_sfb    - maximum # of sfbs
                 sfb_lines  - maximum # of spectral bins that need to be decoded

  Output       : y       - modified scalefactor band offsets
                 max_sfb - maximum # of sfbs after modifications

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int16 *
III_sfbOffsetLongLimit(int16 *sfb_offset, int16 *max_sfb, int16 sfb_lines)
{
    for (int i = 0; i < *max_sfb; i++)
        for (int j = sfb_offset[i]; j < sfb_offset[i + 1]; j++)
            if (j >= sfb_lines) {
                *max_sfb = (sfb_lines == sfb_offset[i]) ? i : i + 1;
                sfb_offset[*max_sfb] = sfb_lines;
                goto exit;
            }

exit:

    return (sfb_offset);
}


/**************************************************************************
  Title        : III_sfbWidthShortTblLimit

  Purpose      : Modifies the scalefactor band boundraries of short blocks
                 according to the information given in the parameter list.

  Usage        : y = III_sfbWidthShortTblLimit(sfb_width, max_sfb, sfb_lines)

  Input        : sfb_width - scalefactor band widths
                 max_sfb   - maximum # of sfbs
                 sfb_lines - maximum # of spectral bins that need to be decoded

  Output       : y       - modified scalefactor band widths
                 max_sfb - maximum # of sfbs after modifications

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int16 *
III_sfbWidthShortTblLimit(int16 *sfb_width, int16 *max_sfb, int16 sfb_lines)
{
    for (int c = 0, i = 0, sf = 0; i < *max_sfb; i++) {
        for (int k = 0; k < 3; k++, sf++) {
            for (int j = 0; j < sfb_width[sf]; j++) {
                c++;
                if (c >= sfb_lines) {
                    sfb_width[sf] = (c == sfb_lines) ? j + 1 : j;
                    *max_sfb = (c == sfb_lines) ? i + 1 : i;
                    goto exit;
                }
            }
        }
    }

exit:

    return (sfb_width);
}


/**************************************************************************
  Title        : III_sfbOffsetShort

  Purpose      : Retrieves the scalefactor band table for short blocks.

  Usage        : y = III_sfbOffsetShort(header)

  Input        : header - frame header

  Output       : y - scalefactor band boundaries for short block

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int16 *
III_sfbOffsetShort(MP_Header *header)
{
    int mp25idx = 6 * header->mp25version();

    return (&sfBandIndex[mp25idx + header->sfreq() + header->version() * 3].s[0]);
}


/**************************************************************************
  Title        : III_sfbWidthTblShort

  Purpose      : Retrieves the scalefactor band width table for short blocks.

  Usage        : y = III_sfbWidthTblShort(header)

  Input        : header - frame header

  Output       : y - scalefactor band widths for short blocks

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int16 *
III_sfbWidthTblShort(MP_Header *header)
{
    int mp25idx = 6 * header->mp25version();

    return (&sfb_width_table_[mp25idx + header->sfreq() + header->version() * 3][0]);
}
