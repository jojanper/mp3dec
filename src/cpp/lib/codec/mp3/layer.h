/**************************************************************************
  layer.h - Low level interface functions for MPEG audio decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef LAYER_ALL_ROUTINES_H_
#define LAYER_ALL_ROUTINES_H_

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- Project Headers. --*/
#include "mstream.h"

/**************************************************************************
  External Objects Provided
  *************************************************************************/

/* The following functions are implemented in module "commmon.cpp" */
void decode_header(MP_Stream *mp);

/* The following functions are implemented in module "layer1.cpp" */
void I_decode_scale(MP_Stream *mp);
void I_buffer_sample(MP_Stream *mp);
void I_decode_bit_alloc(MP_Stream *mp);
void I_dequantize_sample(MP_Stream *mp);

/* The following functions are implemented in module "layer2.cpp" */
void II_decode_scale(MP_Stream *mp);
void II_buffer_sample(MP_Stream *mp);
void II_decode_bit_alloc(MP_Stream *mp);
void II_dequantize_sample(MP_Stream *mp, BYTE *scale_factor);

/* The following functions are implemented in module "layer3.cpp" */
void III_get_side_info(MP_Stream *mp);
void III_reorder(MP_Stream *mp, int ch, int gr);
void init_III_reorder(int16 *sfb_table, int16 *sfb_width_table);

/* The following functions are implemented in module "mp3_q.cpp" */
void InitMP3DequantModule(void);
void III_dequantize(MP_Stream *mp, int gr);
void InitMP3DequantEqualizer(FLOAT *dbScaleIn);

/* The following functions are defined implemented in module 'stereo.cpp' */
void III_stereo_mode(MP_Stream *mp, int gr);

/* The following functions are implemented in module "huffman.cpp" */
BOOL InitMP3Huffman(MP3_Huffman *h);
void III_huffman_decode(MP_Stream *mp, int gr, int ch, int part2);

/* The following functions are implemented in module "synfilt.cpp" */
void PolyPhaseFIR(MP_Stream *mp,
                  FLOAT *sb_samples,
                  int16 *out_samples,
                  int *buf_idx,
                  Out_Param *out_param,
                  int ch);

/* The following functions are implemented in module "imdct2.cpp" */
void InitIMDCTData(void);
void III_antialias(MP_Stream *mp, int ch, int gr);
void III_hybridmix(FLOAT *dstSpec, FLOAT *srcSpec, int mixBins);
void III_hybrid2(MP_Stream *mp, FLOAT tsOut[SSLIMIT][SBLIMIT], int ch, int gr);

/*
   Purpose:     Typedef for scalefactor parsing implementations.
   Explanation: - */
typedef void (*III_SCALEFAC)(MP_Stream *mp, int gr, int ch);

/*-- This is defined in module 'layer3.cpp'. --*/
extern III_SCALEFAC ScaleFunc[2];

#endif /* LAYER_ALL_ROUTINES_H_ */
