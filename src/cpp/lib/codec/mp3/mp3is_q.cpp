/**************************************************************************
  mp3is_q.cpp - MPEG-1, MPEG-2 LSF and MPEG-2.5 IS stereo subroutines
                (include file to 'mp3_q.cpp').

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- Project Headers. --*/
#include "core/defines.h"

inline void
LONG_QUANT_IS(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    int16 pre_tbll,
    int16 pre_tblr,
    int16 shfl,
    int16 shfr,
    int16 width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[(sfl + pre_tbll) << shfl] * *dbS;
    gainr *= globr[(sfr + pre_tblr) << shfr] * *dbS++;
#else
    gainl *= globl[(sfl + pre_tbll) << shfl];
    gainr *= globr[(sfr + pre_tblr) << shfr];
#endif

    int i = 0;
    while (i < width) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
        i++;
    }
}


inline void
LONG_QUANT_IS_LEFT(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    int16 pre_tbll,
    int16 pre_tblr,
    int16 shfl,
    int16 shfr,
    int16 width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[(sfl + pre_tbll) << shfl] * *dbS;
    gainr = globr[(sfr + pre_tblr) << shfr] * *dbS++;
#else
    gainl *= globl[(sfl + pre_tbll) << shfl];
    gainr = globr[(sfr + pre_tblr) << shfr];
#endif

    int i = 0;
    while (i < width) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
        i++;
    }
}

inline void
LONG_QUANT_IS_RIGHT(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    int16 pre_tbll,
    int16 pre_tblr,
    int16 shfl,
    int16 shfr,
    int16 width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl = globl[(sfl + pre_tbll) << shfl] * *dbS;
    gainr *= globr[(sfr + pre_tblr) << shfr] * *dbS++;
#else
    gainl = globl[(sfl + pre_tbll) << shfl];
    gainr *= globr[(sfr + pre_tblr) << shfr];
#endif

    int i = 0;
    while (i < width) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
        i++;
    }
}

inline void
LONG_QUANT_IS_LEFT0(
    const FLOAT *globl,
    int16 sfl,
    int16 pre_tbll,
    int16 shfl,
    int16 width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
    FLOAT gainl;

#ifdef EQUALIZER
    gainl = globl[(sfl + pre_tbll) << shfl] * *dbS++;
#else
    gainl = globl[(sfl + pre_tbll) << shfl];
#endif

    int i = 0;
    while (i < width) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        i++;
    }
    qs[1] += width;
    dqs[1] += width;
}

inline void
LONG_QUANT_IS_LEFT1(
    const FLOAT *globl,
    FLOAT gainl,
    int16 sfl,
    int16 pre_tbll,
    int16 shfl,
    int16 width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[(sfl + pre_tbll) << shfl] * *dbS++;
#else
    gainl *= globl[(sfl + pre_tbll) << shfl];
#endif

    int i = 0;
    while (i < width) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        i++;
    }
    qs[1] += width;
    dqs[1] += width;
}

inline void
SHORT_QUANT_IS(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    BYTE subblock_gainl,
    BYTE subblock_gainr,
    int16 shfl,
    int16 shfr,
    int16 sfb_width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[subblock_gainl + (sfl << shfl)] * *dbS;
    gainr *= globr[subblock_gainr + (sfr << shfr)] * *dbS;
#else
    gainl *= globl[subblock_gainl + (sfl << shfl)];
    gainr *= globr[subblock_gainr + (sfr << shfr)];
#endif

    for (int j = 0; j < sfb_width; ++j) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
    }
}

inline void
SHORT_QUANT_IS_LEFT(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    BYTE subblock_gainl,
    BYTE subblock_gainr,
    int16 shfl,
    int16 shfr,
    int16 sfb_width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[subblock_gainl + (sfl << shfl)] * *dbS;
    gainr = globr[subblock_gainr + (sfr << shfr)] * *dbS;
#else
    gainl *= globl[subblock_gainl + (sfl << shfl)];
    gainr = globr[subblock_gainr + (sfr << shfr)];
#endif

    for (int j = 0; j < sfb_width; ++j) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
    }
}

inline void
SHORT_QUANT_IS_RIGHT(
    const FLOAT *globl,
    const FLOAT *globr,
    FLOAT gainl,
    FLOAT gainr,
    int16 sfl,
    int16 sfr,
    BYTE subblock_gainl,
    BYTE subblock_gainr,
    int16 shfl,
    int16 shfr,
    int16 sfb_width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl = globl[subblock_gainl + (sfl << shfl)] * *dbS;
    gainr *= globr[subblock_gainr + (sfr << shfr)] * *dbS;
#else
    gainl = globl[subblock_gainl + (sfl << shfl)];
    gainr *= globr[subblock_gainr + (sfr << shfr)];
#endif

    for (int j = 0; j < sfb_width; ++j) {
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
        *dqs[1]++ = scaled_sample(gainr, *qs[1]++);
    }
}

inline void
SHORT_QUANT_IS_LEFT0(
    const FLOAT *globl,
    int16 sfl,
    BYTE subblock_gainl,
    int16 shfl,
    int16 sfb_width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
    FLOAT gainl;

#ifdef EQUALIZER
    gainl = globl[subblock_gainl + (sfl << shfl)] * *dbS;
#else
    gainl = globl[subblock_gainl + (sfl << shfl)];
#endif

    for (int j = 0; j < sfb_width; ++j)
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
    qs[1] += sfb_width;
    dqs[1] += sfb_width;
}

inline void
SHORT_QUANT_IS_LEFT1(
    const FLOAT *globl,
    FLOAT gainl,
    int16 sfl,
    BYTE subblock_gainl,
    int16 shfl,
    int16 sfb_width,
    const FLOAT *dbS,
    int16 *qs[2],
    FLOAT *dqs[2])
{
#ifdef EQUALIZER
    gainl *= globl[subblock_gainl + (sfl << shfl)] * *dbS;
#else
    gainl *= globl[subblock_gainl + (sfl << shfl)];
#endif

    for (int j = 0; j < sfb_width; ++j)
        *dqs[0]++ = scaled_sample(gainl, *qs[0]++);
    qs[1] += sfb_width;
    dqs[1] += sfb_width;
}
