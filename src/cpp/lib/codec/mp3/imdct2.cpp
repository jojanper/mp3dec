/**************************************************************************
  imdct2.cpp - Inverse MDCT routines for layer III.

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
#include "mstream.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/*
   Purpose:     Initial twiddle factors for alias-reduction.
   Explanation: - */
static FLOAT Ci[8] = { -0.6, -0.535, -0.33, -0.185, -0.095, -0.041, -0.0142, -0.0037 };

/*
   Purpose:     Twiddle factors for alias-reduction.
   Explanation: - */
static FLOAT ca[8], cs[8];

/*
   Purpose:     Static tables for fast IMDCT computation.
   Explanation: - */
static FLOAT win[4][36];
static FLOAT win1[4][36];
static FLOAT COS9[9];
static FLOAT COS6_1, COS6_2;
static FLOAT tfcos36[9];
static FLOAT tfcos12[3];

/*
 * Initializes twiddle factors for alias reduction computation.
 */
static void
init_III_antialias(void)
{
    for (int i = 0; i < 8; i++) {
        FLOAT sq = sqrt(1.0 + Ci[i] * Ci[i]);
        cs[i] = 1.0 / sq;
        ca[i] = Ci[i] / sq;
    }
}

/**************************************************************************
  Title        : InitIMDCTData

  Purpose      : Initializes tables for this module.

  Usage        : InitIMDCTData()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
InitIMDCTData(void)
{
    int i, j;

    for (i = 0; i < 18; i++) {
        win[0][i] = win[1][i] = 0.5 * sin(M_PI / 72.0 * (double) (2 * (i + 0) + 1)) /
                                cos(M_PI * (double) (2 * (i + 0) + 19) / 72.0);

        win[0][i + 18] = win[3][i + 18] = 0.5 *
                                          sin(M_PI / 72.0 * (double) (2 * (i + 18) + 1)) /
                                          cos(M_PI * (double) (2 * (i + 18) + 19) / 72.0);
    }

    for (i = 0; i < 6; i++) {
        win[1][i + 18] = 0.5 / cos(M_PI * (double) (2 * (i + 18) + 19) / 72.0);
        win[3][i + 12] = 0.5 / cos(M_PI * (double) (2 * (i + 12) + 19) / 72.0);

        win[1][i + 24] = 0.5 * sin(M_PI / 24.0 * (double) (2 * i + 13)) /
                         cos(M_PI * (double) (2 * (i + 24) + 19) / 72.0);

        win[1][i + 30] = win[3][i] = 0.0;

        win[3][i + 6] = 0.5 * sin(M_PI / 24.0 * (double) (2 * i + 1)) /
                        cos(M_PI * (double) (2 * (i + 6) + 19) / 72.0);
    }

    for (i = 0; i < 9; i++) {
        COS9[i] = cos(M_PI / 18.0 * (double) i);
        tfcos36[i] = 0.5 / cos(M_PI * (double) (i * 2 + 1) / 36.0);
    }

    for (i = 0; i < 3; i++)
        tfcos12[i] = 0.5 / cos(M_PI * (double) (i * 2 + 1) / 12.0);

    COS6_1 = cos(M_PI / 6.0 * (double) 1);
    COS6_2 = cos(M_PI / 6.0 * (double) 2);

    for (i = 0; i < 12; i++)
        win[2][i] = 0.5 * sin(M_PI / 24.0 * (double) (2 * i + 1)) /
                    cos(M_PI * (double) (2 * i + 7) / 24.0);

    for (j = 0; j < 4; j++) {
        static int len[4] = { 36, 36, 12, 36 };

        for (i = 0; i < len[j]; i += 2)
            win1[j][i] = +win[j][i];

        for (i = 1; i < len[j]; i += 2)
            win1[j][i] = -win[j][i];
    }

    init_III_antialias();
}

/**************************************************************************
  Title        : III_antialias

  Purpose      : Alias reduction of long blocks.

  Usage        : III_antialias(mp, ch, gr)

  Input        : mp - mp3 stream parameters
                 ch - channel number (left or right)
                 gr - granule number

  Explanation  : If full quality is not needed this function can be omitted
                 from the decoding chain.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_antialias(MP_Stream *mp, int ch, int gr)
{
    int sblim;

    /*-- Get the number of subband pairs for alias-reduction. --*/
    switch (mp->side_info->ch_info[ch]->gr_info[gr]->block_mode) {
        case LONG_BLOCK_MODE:
            sblim = MIN(mp->side_info->sfbData.sbHybrid >> 4, mp->complex->subband_pairs);
            break;

        default:
            return;
    }

    /*
     * 'sblim' alias-reduction operations between each pair of subbands
     * with 8 butterflies between each pair.
     */
    register FLOAT **spectrum = mp->spectrum[ch];
    for (int sb = 0; sb < sblim; sb++) {
        register FLOAT *ca_p, *cs_p;
        register FLOAT bu, bd;

        ca_p = ca;
        cs_p = cs;
#if 0
    for(int ss = 0; ss < 8; ss++)
    {
      bu = spectrum[sb][17 - ss];
      bd = spectrum[sb + 1][ss];
      spectrum[sb][17 - ss] = (bu * *cs_p) - (bd * *ca_p);
      spectrum[sb + 1][ss] = (bd * *cs_p++) + (bu * *ca_p++);
    }
#else
        bu = spectrum[sb][17];
        bd = spectrum[sb + 1][0];
        spectrum[sb][17] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][0] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 1];
        bd = spectrum[sb + 1][1];
        spectrum[sb][17 - 1] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][1] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 2];
        bd = spectrum[sb + 1][2];
        spectrum[sb][17 - 2] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][2] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 3];
        bd = spectrum[sb + 1][3];
        spectrum[sb][17 - 3] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][3] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 4];
        bd = spectrum[sb + 1][4];
        spectrum[sb][17 - 4] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][4] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 5];
        bd = spectrum[sb + 1][5];
        spectrum[sb][17 - 5] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][5] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 6];
        bd = spectrum[sb + 1][6];
        spectrum[sb][17 - 6] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][6] = (bd * *cs_p++) + (bu * *ca_p++);

        bu = spectrum[sb][17 - 7];
        bd = spectrum[sb + 1][7];
        spectrum[sb][17 - 7] = (bu * *cs_p) - (bd * *ca_p);
        spectrum[sb + 1][7] = (bd * *cs_p++) + (bu * *ca_p++);
#endif
    }
}

/**************************************************************************
  Title        : dct36, dct12

  Purpose      : Fast IMDCT for layer III.

  Explanation  : This uses Byeong Gi Lee's Fast Cosine Transform algorithm,
                 but the 9 point IDCT needs to be reduced further.
                 Unfortunately, I don't know how to do that, because 9 is not
                 an even number. - Jeff.

                 9 Point Inverse Discrete Cosine Transform

                 This piece of code is Copyright 1997 Mikko Tommila and is
                 freely usable by anybody. The algorithm itself is of course
                 in the public domain. Again derived heuristically from the
                 9-point WFTA.

                 The algorithm is optimized (?) for speed, not for small
                 rounding errors or good readability.

                 36 additions, 11 multiplications

                 Again this is very likely sub-optimal. The code is optimized
                 to use a minimum number of temporary variables, so it should
                 compile quite well even on 8-register Intel x86 processors.
                 This makes the code quite obfuscated and very difficult to
                 understand.

                 References:
                 [1] S. Winograd: "On Computing the Discrete Fourier Transform",
                 Mathematics of Computation, Volume 32, Number 141, January 1978,
                 Pages 175-199

  Author(s)    : (see above)
  *************************************************************************/

static inline void
dct36(FLOAT *inbuf, FLOAT *o1, FLOAT *o2, FLOAT *wintab, FLOAT *tsbuf)
{
    register FLOAT *in = inbuf;

    in[17] += in[16];
    in[16] += in[15];
    in[15] += in[14];
    in[14] += in[13];
    in[13] += in[12];
    in[12] += in[11];
    in[11] += in[10];
    in[10] += in[9];
    in[9] += in[8];
    in[8] += in[7];
    in[7] += in[6];
    in[6] += in[5];
    in[5] += in[4];
    in[4] += in[3];
    in[3] += in[2];
    in[2] += in[1];
    in[1] += in[0];
    in[17] += in[15];
    in[15] += in[13];
    in[13] += in[11];
    in[11] += in[9];
    in[9] += in[7];
    in[7] += in[5];
    in[5] += in[3];
    in[3] += in[1];

    {
#define MACRO0(v)                                                \
    {                                                            \
        FLOAT tmp;                                               \
        out2[9 + (v)] = (tmp = sum0 + sum1) * w[27 + (v)];       \
        out2[8 - (v)] = tmp * w[26 - (v)];                       \
    }                                                            \
    sum0 -= sum1;                                                \
    ts[SBLIMIT * (8 - (v))] = out1[8 - (v)] + sum0 * w[8 - (v)]; \
    ts[SBLIMIT * (9 + (v))] = out1[9 + (v)] + sum0 * w[9 + (v)];

#define MACRO1(v)                              \
    {                                          \
        FLOAT sum0, sum1;                      \
        sum0 = tmp1a + tmp2a;                  \
        sum1 = (tmp1b + tmp2b) * tfcos36[(v)]; \
        MACRO0(v);                             \
    }

#define MACRO2(v)                              \
    {                                          \
        FLOAT sum0, sum1;                      \
        sum0 = tmp2a - tmp1a;                  \
        sum1 = (tmp2b - tmp1b) * tfcos36[(v)]; \
        MACRO0(v);                             \
    }

        register const FLOAT *c = COS9;
        register FLOAT *out2 = o2;
        register FLOAT *w = wintab;
        register FLOAT *out1 = o1;
        register FLOAT *ts = tsbuf;

        FLOAT ta33, ta66, tb33, tb66;

        ta33 = in[6] * c[3];
        ta66 = in[12] * c[6];
        tb33 = in[7] * c[3];
        tb66 = in[13] * c[6];

        {
            FLOAT tmp1a, tmp2a, tmp1b, tmp2b;

            tmp1a = in[2] * c[1] + ta33 + in[10] * c[5] + in[14] * c[7];
            tmp1b = in[3] * c[1] + tb33 + in[11] * c[5] + in[15] * c[7];
            tmp2a = in[0] + in[4] * c[2] + in[8] * c[4] + ta66 + in[16] * c[8];
            tmp2b = in[1] + in[5] * c[2] + in[9] * c[4] + tb66 + in[17] * c[8];

            MACRO1(0);
            MACRO2(8);
        }

        {
            FLOAT tmp1a, tmp2a, tmp1b, tmp2b;

            tmp1a = (in[2] - in[10] - in[14]) * c[3];
            tmp1b = (in[3] - in[11] - in[15]) * c[3];
            tmp2a = (in[4] - in[8] - in[16]) * c[6] - in[12] + in[0];
            tmp2b = (in[5] - in[9] - in[17]) * c[6] - in[13] + in[1];

            MACRO1(1);
            MACRO2(7);
        }

        {
            FLOAT tmp1a, tmp2a, tmp1b, tmp2b;

            tmp1a = in[2] * c[5] - ta33 - in[10] * c[7] + in[14] * c[1];
            tmp1b = in[3] * c[5] - tb33 - in[11] * c[7] + in[15] * c[1];
            tmp2a = in[0] - in[4] * c[8] - in[8] * c[2] + ta66 + in[16] * c[4];
            tmp2b = in[1] - in[5] * c[8] - in[9] * c[2] + tb66 + in[17] * c[4];

            MACRO1(2);
            MACRO2(6);
        }

        {
            FLOAT tmp1a, tmp2a, tmp1b, tmp2b;

            tmp1a = in[2] * c[7] - ta33 + in[10] * c[1] - in[14] * c[5];
            tmp1b = in[3] * c[7] - tb33 + in[11] * c[1] - in[15] * c[5];
            tmp2a = in[0] - in[4] * c[4] + in[8] * c[8] + ta66 - in[16] * c[2];
            tmp2b = in[1] - in[5] * c[4] + in[9] * c[8] + tb66 - in[17] * c[2];

            MACRO1(3);
            MACRO2(5);
        }

        {
            FLOAT sum0, sum1;

            sum0 = in[0] - in[4] + in[8] - in[12] + in[16];
            sum1 = (in[1] - in[5] + in[9] - in[13] + in[17]) * tfcos36[4];

            MACRO0(4);
        }
    }
}

static inline void
dct12(FLOAT *in, FLOAT *rawout1, FLOAT *rawout2, register FLOAT *wi, register FLOAT *ts)
{
#define DCT12_PART1           \
    in5 = in[5 * 3];          \
    in5 += (in4 = in[4 * 3]); \
    in4 += (in3 = in[3 * 3]); \
    in3 += (in2 = in[2 * 3]); \
    in2 += (in1 = in[1 * 3]); \
    in1 += (in0 = in[0 * 3]); \
                              \
    in5 += in3;               \
    in3 += in1;               \
                              \
    in2 *= COS6_1;            \
    in3 *= COS6_1;

#define DCT12_PART2                 \
    in0 += in4 * COS6_2;            \
                                    \
    in4 = in0 + in2;                \
    in0 -= in2;                     \
                                    \
    in1 += in5 * COS6_2;            \
                                    \
    in5 = (in1 + in3) * tfcos12[0]; \
    in1 = (in1 - in3) * tfcos12[2]; \
                                    \
    in3 = in4 + in5;                \
    in4 -= in5;                     \
                                    \
    in2 = in0 + in1;                \
    in0 -= in1;


    {
        FLOAT in0, in1, in2, in3, in4, in5;
        register FLOAT *out1 = rawout1;

        ts[SBLIMIT * 0] = out1[0];
        ts[SBLIMIT * 1] = out1[1];
        ts[SBLIMIT * 2] = out1[2];
        ts[SBLIMIT * 3] = out1[3];
        ts[SBLIMIT * 4] = out1[4];
        ts[SBLIMIT * 5] = out1[5];

        DCT12_PART1

        {
            FLOAT tmp0, tmp1 = (in0 - in4);
            {
                FLOAT tmp2 = (in1 - in5) * tfcos12[1];
                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            ts[16 * SBLIMIT] = out1[16] + tmp0 * wi[10];
            ts[13 * SBLIMIT] = out1[13] + tmp0 * wi[7];
            ts[7 * SBLIMIT] = out1[7] + tmp1 * wi[1];
            ts[10 * SBLIMIT] = out1[10] + tmp1 * wi[4];
        }

        DCT12_PART2

        ts[17 * SBLIMIT] = out1[17] + in2 * wi[11];
        ts[12 * SBLIMIT] = out1[12] + in2 * wi[6];
        ts[14 * SBLIMIT] = out1[14] + in3 * wi[8];
        ts[15 * SBLIMIT] = out1[15] + in3 * wi[9];

        ts[6 * SBLIMIT] = out1[6] + in0 * wi[0];
        ts[11 * SBLIMIT] = out1[11] + in0 * wi[5];
        ts[8 * SBLIMIT] = out1[8] + in4 * wi[2];
        ts[9 * SBLIMIT] = out1[9] + in4 * wi[3];
    }

    in++;

    {
        FLOAT in0, in1, in2, in3, in4, in5;
        register FLOAT *out2 = rawout2;

        DCT12_PART1

        {
            FLOAT tmp0, tmp1 = (in0 - in4);
            {
                FLOAT tmp2 = (in1 - in5) * tfcos12[1];
                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            out2[4] = tmp0 * wi[10];
            out2[1] = tmp0 * wi[7];
            ts[13 * SBLIMIT] += tmp1 * wi[1];
            ts[16 * SBLIMIT] += tmp1 * wi[4];
        }

        DCT12_PART2

        out2[5] = in2 * wi[11];
        out2[0] = in2 * wi[6];
        out2[2] = in3 * wi[8];
        out2[3] = in3 * wi[9];

        ts[12 * SBLIMIT] += in0 * wi[0];
        ts[17 * SBLIMIT] += in0 * wi[5];
        ts[14 * SBLIMIT] += in4 * wi[2];
        ts[15 * SBLIMIT] += in4 * wi[3];
    }

    in++;

    {
        FLOAT in0, in1, in2, in3, in4, in5;
        register FLOAT *out2 = rawout2;
        out2[12] = out2[13] = out2[14] = out2[15] = out2[16] = out2[17] = 0.0;

        DCT12_PART1

        {
            FLOAT tmp0, tmp1 = (in0 - in4);
            {
                FLOAT tmp2 = (in1 - in5) * tfcos12[1];
                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            out2[10] = tmp0 * wi[10];
            out2[7] = tmp0 * wi[7];
            out2[1] += tmp1 * wi[1];
            out2[4] += tmp1 * wi[4];
        }

        DCT12_PART2

        out2[11] = in2 * wi[11];
        out2[6] = in2 * wi[6];
        out2[8] = in3 * wi[8];
        out2[9] = in3 * wi[9];

        out2[0] += in0 * wi[0];
        out2[5] += in0 * wi[5];
        out2[2] += in4 * wi[2];
        out2[3] += in4 * wi[3];
    }
}

/**************************************************************************
  Title        : III_hybrid2

  Purpose      : Interface for layer III IMDCT computations.

  Usage        : III_hybrid2(mp, tsOut, ch, gr)

  Input        : mp    - mp3 stream parameters
                 gr    - granule number
                 ch    - channel number

  Output       : tsOut - output frame

  Explanation  : The # of subbands, where IMDCT should be applied, varies as a
                 function of frame. This value is determined during the Huffman
                 decoding, see 'gr_info->zero_part_start'.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_hybrid2(MP_Stream *mp, FLOAT tsOut[SSLIMIT][SBLIMIT], int ch, int gr)
{
    int bt, b, sb, subband_limit, widx;
    FLOAT *rawout1, *rawout2, *winTbl[2];
    FLOAT **fsIn, *tspnt = (FLOAT *) tsOut;

    // Number of subbands where IMDCT is applied.
    subband_limit = MIN(mp->side_info->sfbData.sbHybrid, mp->complex->imdct_subbands);

    sb = widx = 0;
    fsIn = mp->spectrum[ch];

    b = mp->blc[ch];
    rawout1 = mp->OverlapBlock[b][ch];
    b = -b + 1;
    rawout2 = mp->OverlapBlock[b][ch];
    mp->blc[ch] = b;

    // Block type for this frame.
    bt = (mp->side_info->ch_info[ch]->gr_info[gr]->flags & 3);

    // Now apply IMDCT to the subbands.
    if (bt == 2) {
        winTbl[0] = &win[2][0];
        winTbl[1] = &win1[2][0];
        for (; sb < subband_limit;
             sb += SSLIMIT, tspnt++, rawout1 += SSLIMIT, rawout2 += SSLIMIT, widx++)
            dct12(fsIn[widx], rawout1, rawout2, winTbl[widx & 0x1], tspnt);
    }
    else {
        winTbl[0] = &win[bt][0];
        winTbl[1] = &win1[bt][0];
        for (; sb < subband_limit;
             sb += SSLIMIT, tspnt++, rawout1 += SSLIMIT, rawout2 += SSLIMIT, widx++)
            dct36(fsIn[widx], rawout1, rawout2, winTbl[widx & 0x1], tspnt);
    }

    // Zero out those subbands where IMDCT was not applied.
    for (; sb < mp->complex->imdct_subbands; sb += SSLIMIT, tspnt++, rawout2 += SSLIMIT) {
        memset(rawout2, 0, 72 /* SSLIMIT * sizeof(FLOAT) */);
        for (int i = 0; i < SSLIMIT; i++)
            tspnt[i << 5] = 0.0f;
    }
}

/*
 * Mixes the two given subband spectras by adding 'srcSpec' to the
 * 'dstSpec'. The number of subband bins to be mixed is specified by the
 * 'mixBins' parameter.
 */
void
III_hybridmix(FLOAT *dstSpec, FLOAT *srcSpec, int mixBins)
{
    int subbands = (int) ((mixBins * 0.05555555555f) + 0.5);

    for (int i = 0; i < subbands; i++, dstSpec++, srcSpec++) {
        register FLOAT *a0, *b0;
        a0 = dstSpec;
        b0 = srcSpec;
        for (int j = 0; j < SSLIMIT; j++, a0 += SBLIMIT, b0 += SBLIMIT)
            *a0 = (*a0 + *b0) * 0.5f;
    }
}
