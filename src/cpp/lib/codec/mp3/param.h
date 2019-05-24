/**************************************************************************
  param.h - Structure interfaces that define complexity parameters of MPEG audio streams.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef DEC_PARAM_H_
#define DEC_PARAM_H_

/*-- Project Headers. --*/
#include "core/defines.h"

/*
   Purpose:     Discards the lowest and highest 'SBLIMIT' samples from
                the windowing operations.
   Explanation: - */
#define WINDOW_PRUNING_START_IDX 2

/*
   Purpose:     Parameters that control the decoding complexity of mp3 decoder.
   Explanation: - */
typedef struct Out_ComplexityStr
{
    int16 subband_pairs;  /* Number of subband pairs where alias-reduction
                             is performed. Varies between 1...'SMLIMIT' -1.   */
    int16 imdct_subbands; /* Number of subbands where IMDC is applied.        */
    BOOL fix_window;      /* Windowing performed using fixed point arithmetic.*/

} Out_Complexity;

/*
   Purpose:     Information about the current MPEG audio stream.
   Explanation: - */
typedef struct Out_ParamStr
{
    int32 sampling_frequency; /* Output sampling frequency.                */
    int16 num_out_channels;   /* Output number of channels.                */
    int16 decim_factor;       /* Decimation factor.                        */
    int16 num_samples;        /* Number of output samples per subband.     */
    int16 num_out_samples;    /* Total number of output samples per frame. */

    /*
     * Specifies how many window coefficients are discarded from the synthesis
     * window. In units of 'SBLIMIT' samples. The start index can vary
     * between 0 and 'NUM_SUBWIN' but if you don't want to reduce the overall
     * audio quality, the default value 2 should be used.
     * (These values are mp3 specific !!).
     */
    int16 window_pruning_idx;
    int16 num_subwindows;
    int16 window_offset;

} Out_Param;

/*
   Purpose:     Let's map the above structures together.
   Explanation: - */
typedef struct Out_InfoStr
{
    Out_Param param;
    Out_Complexity complex;

} Out_Info;

#endif /* DEC_PARAM_H_ */
