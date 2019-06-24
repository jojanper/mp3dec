/**************************************************************************
  mp3def.h - Constants and general declarations for MPEG audio decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef MP3_DEF_H_
#define MP3_DEF_H_

/*- Project Headers. --*/
#include "core/defines.h"

/**************************************************************************
  External Objects Provided
  *************************************************************************/

/*-- Some Useful Macros. --*/
#define HIGHWORD(x) ((uint32) x >> 24)
#define LOWWORD(x) ((uint32) x & 0x0000FFFF)
#define HIGHBYTE(x) ((uint32) x >> 8)
#define LOWBYTE(x) ((uint32) x & 0x000000FF)

/*-- Channel definitions. --*/
#define MONO_CHAN (0)
#define MAX_CHANNELS (2)
#define LEFT_CHANNEL (MONO_CHAN)
#define RIGHT_CHANNEL (MONO_CHAN + 1)

/*-- General MPx Definitions. --*/
#define PI (3.14159265358979)
#define MPEG_AUDIO_ID (1)
#define MPEG_PHASE2_LSF (0)
#define SBLIMIT (32)
#define SSLIMIT (18)
#define MAX_MONO_SAMPLES (SBLIMIT * SSLIMIT)
#define MAX_STEREO_SAMPLES (MAX_MONO_SAMPLES * 2)
#define HAN_SIZE (512)
#define SCALE_BLOCK (12)
#define SCALE_RANGE (64)
#define NUM_SUBWIN (16)
#define SCALE (32768L)
#define CRC16_POLYNOMIAL (0x8005)
#define SYNC_WORD ((long) 0x7ff)
#define SYNC_WORD_LENGTH (11)
#define HEADER_BITS (20)
#define MAX_LONG_SFB_BANDS (22)
#define MAX_SHORT_SFB_BANDS (13)
#define MAX_FRAME_SLOTS (1427)

/*
   Purpose:     Layer III flags.
   Explanation: - */
typedef enum LayerIIIFlags
{
    WINDOW_SWITCHING_FLAG = 4,
    MIXED_BLOCK_FLAG = 8,
    PRE_FLAG = 16,
    SCALEFAC_SCALE = 32,
    COUNT_1_TABLE_SELECT = 64

} Layer_III_Flags;

/*
   Purpose:     Stereo modes for layer III.
   Explanation: - */
typedef enum StereoMode
{
    ONLY_MONO,
    ONLY_STEREO,
    MS_STEREO,
    IS_STEREO,
    LSF_IS_STEREO

} StereoMode;

/*
   Purpose:     Block types for layer III.
   Explanation: The first four describe the actual block type for each subband,
                the rest of the declarations describe the block type for the
                whole frame. */
typedef enum WINDOW_TYPE
{
    ONLY_LONG_WINDOW,
    LONG_SHORT_WINDOW,
    ONLY_SHORT_WINDOW,
    SHORT_LONG_WINDOW,

    MIXED_BLOCK_MODE,
    SHORT_BLOCK_MODE,
    LONG_BLOCK_MODE

} WINDOW_TYPE;

/**************************************************************************
                Common structure definitions for all Layers
  *************************************************************************/

/*
   Purpose:     Parent Structure for MPx Frames.
   Explanation: - */
typedef struct MP_Frame
{
    BYTE *bit_alloc;               /* Bit allocation of each subband.         */
    BYTE *scale_factors;           /* Scale factor of each subband and group. */
    int16 *quant;                  /* Quantized spectral data for this frame. */
    int16 *ch_quant[MAX_CHANNELS]; /* Channel pointers.                       */

} MP_Frame;

/*
  Purpose:     Parent Structure for MPx Buffer.
  Explanation: - */
typedef struct MP_Buffer
{
    FLOAT *synthesis_buffer[MAX_CHANNELS];    /* Samples for windowing.        */
    int16 *Fixsynthesis_buffer[MAX_CHANNELS]; /* Samples for fixed point wind. */
    int buf_idx[MAX_CHANNELS];                /* Start index for windowing.    */
    FLOAT *reconstructed;                     /* Dequantized samples.          */
    FLOAT *ch_reconstructed[MAX_CHANNELS];    /* Channel pointers.             */
    int16 *pcm_sample;                        /* Reconstructed output samples. */

} MP_Buffer;

/**************************************************************************
               Structure definitions applicable only to Layer III
  *************************************************************************/
/*
   Purpose:     Parent Structure for Layer III Granule Info.
   Explanation: - */
typedef struct Granule_Info_Str
{
    BYTE global_gain;
    BYTE flags;
    BYTE table_select[3];
    BYTE subblock_gain[3];
    BYTE region0_count;
    BYTE region1_count;
    int part2_3_length;
    int big_values;
    int scalefac_compress;
    WINDOW_TYPE block_mode;
    int zero_part_start;

} Granule_Info;

/*
   Purpose:     Parent Structure for Layer III Scale Factors.
   Explanation: - */
typedef struct III_Scale_Factors_Str
{
    BYTE *scalefac_long;     /* Scalefactors for long blocks.  */
    BYTE *scalefac_short[3]; /* Scalefactors for short blocks. */

} III_Scale_Factors;

/*
   Purpose:     Parent Structure for Layer III Channel Info.
   Explanation: - */
typedef struct III_Channel_Info_Str
{
    Granule_Info *gr_info[2];     /* Granule info for this channel. */
    III_Scale_Factors *scale_fac; /* Scalefactors for this channel. */

} III_Channel_Info;

#endif /* MP3_DEF_H_ */
