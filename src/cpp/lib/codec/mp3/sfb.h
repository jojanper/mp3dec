/**************************************************************************
  sfb.h - Scalefactor band parameters for layer III.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef SFB_DATA_H_
#define SFB_DATA_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "mp3def.h"
#include "mpheader.h"

/*
   Purpose:     Structure to hold scalefactor band parameters.
   Explanation: - */
typedef struct SFBAND_DATAStr
{
    int16 l[23]; /* long block.  */
    int16 s[14]; /* short block. */

} SFBAND_DATA;

/*
   Purpose:     Structure to hold scalefactor band tables
                and related parameters.
   Explanation: - */
typedef struct III_SfbData_Str
{
    /* Scalefactor band boundaries for long and short block. */
    int16 sfbOffsetLong[MAX_LONG_SFB_BANDS + 1];
    int16 sfbOffsetShort[MAX_SHORT_SFB_BANDS + 1];

    /* Scalefactor band widths for long and short block. */
    int16 sfbWidthLong[MAX_LONG_SFB_BANDS + 1];
    int16 sfbWidthShort[MAX_SHORT_SFB_BANDS + 1];
    int16 sfbWidthCumShort[MAX_SHORT_SFB_BANDS * 3 + 1];

    int16 sfbLongSfbIdx[MAX_MONO_SAMPLES + 1];

    BOOL ms_stereo;          // MS (Mid/Side) stereo used.
    BOOL is_stereo;          // Intensity stereo used.
    BOOL lsf;                // MPEG-2 LSF bit stream.
    BOOL mpeg25;             // MPEG-2.5 bit stream (non-ISO extension by FhG).
    int16 max_gr;            // Number of granules within each stream frame.
    int16 *sfbLong;          // Pointer to long sfb offset table.
    int16 *sfbShort;         // Pointer to short sfb offset table.
    int16 *sfbWidth;         // Pointer to short sfb width table.
    int16 max_sfb_long;      // Number of sfb bands for long block.
    int16 max_sfb_short;     // Number of sfb bands for short block.
    int16 ms_bands_short[3]; // Last band using MS stereo (long blocks).
    int16 ms_bands_long;     // Last band using MS stereo (short blocks).
    int16 nsfb_mixed;        // Number of (long block) bands for mixed block type.
    int16 sbHybrid;          // Number of IMDCT subbands to inverse transform.
    int16 bandLimit;         // Max. number of bins that need to be decoded.

} III_SfbData;

/*-- Exported methods. --*/
void III_SfbDataInit(III_SfbData *sfbData, MP_Header *header);
void III_BandLimit(III_SfbData *sfbData, int decim_factor);

#endif /* SFB_DATA_H_ */
