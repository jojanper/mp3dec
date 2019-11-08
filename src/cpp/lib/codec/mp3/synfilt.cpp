/**************************************************************************
  synfilt.cpp - Synthesis filterbank implementations for MPEG audio decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <math.h>
#include <string.h>

/*-- Project Headers. --*/
#include "mstream.h"
#include "param.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/*
   Purpose:     PI with high precision.
   Explanation: - */
static const FLOAT J_PI = 3.14159265358979323846;

#ifdef COSINE_SYNTHESIS
/**************************************************************************
  Title        : Cosine_Synthesis

  Purpose      : Performs matrixing operation for the MPEG audio synthesis filterbank.

  Usage        : Cosine_Synthesis(sb_transform, y)

  Input        : sb_transform - subband samples

  Output       : y - output samples

  Explanation  : Direct implementation, execution is therefore very slow.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Cosine_Synthesis(FLOAT *sb_transform, FLOAT *y)
{
    int i, k;
    static int init = 0;
    static FLOAT cos_table[2 * SBLIMIT][2 * SBLIMIT];

    if (init == 0) {
        for (i = 0; i < (SBLIMIT << 1); i++) {
            for (k = 0; k < SBLIMIT; k++)
                cos_table[i][k] = cos((J_PI / (SBLIMIT << 1) * i + J_PI / 4) * (2 * k + 1));
        }
        init++;
    }

    for (i = 0; i < (SBLIMIT << 1); i++) {
        y[i] = 0.0;
        for (k = 0; k < SBLIMIT; k++)
            y[i] += sb_transform[k] * cos_table[i][k];
    }
}
#else
#ifdef DCT_CHEN
static const FLOAT tan1_8 = tan(J_PI / 8.0);
static const FLOAT cot3_8 = 1. / tan(J_PI * 3.0 / 8.0);
static const FLOAT cos1_4 = cos(J_PI / 4.0);
static const FLOAT cos1_8 = cos(J_PI / 8.0);
static const FLOAT sin3_8 = sin(J_PI * 3 / 8.0);
static const FLOAT tan1_16 = tan(J_PI / 16.0);
static const FLOAT cos1_16 = cos(J_PI / 16.0);
static const FLOAT cot5_16 = 1. / tan(J_PI * 5 / 16.0);
static const FLOAT sin5_16 = sin(J_PI * 5 / 16.0);
static const FLOAT tan3_16 = tan(J_PI * 3 / 16.0);
static const FLOAT cos3_16 = cos(J_PI * 3 / 16.0);
static const FLOAT cot7_16 = 1. / tan(J_PI * 7 / 16.0);
static const FLOAT sin7_16 = sin(J_PI * 7 / 16.0);
static const FLOAT tan1_32 = tan(J_PI / 32.0);
static const FLOAT cos1_32 = cos(J_PI / 32.0);
static const FLOAT cot15_32 = 1. / tan(J_PI * 15 / 32.0);
static const FLOAT sin15_32 = sin(J_PI * 15 / 32.0);
static const FLOAT cot9_32 = 1. / tan(J_PI * 9 / 32.0);
static const FLOAT sin9_32 = sin(J_PI * 9 / 32.0);
static const FLOAT tan7_32 = tan(J_PI * 7 / 32.0);
static const FLOAT cos7_32 = cos(J_PI * 7 / 32.0);
static const FLOAT tan5_32 = tan(J_PI * 5 / 32.0);
static const FLOAT cos5_32 = cos(J_PI * 5 / 32.0);
static const FLOAT cot11_32 = 1. / tan(J_PI * 11 / 32.0);
static const FLOAT sin11_32 = sin(J_PI * 11 / 32.0);
static const FLOAT cot13_32 = 1. / tan(J_PI * 13 / 32.0);
static const FLOAT sin13_32 = sin(J_PI * 13 / 32.0);
static const FLOAT tan3_32 = tan(J_PI * 3 / 32.0);
static const FLOAT cos3_32 = cos(J_PI * 3 / 32.0);
static const FLOAT tan1_64 = tan(J_PI / 64.0);
static const FLOAT cos1_64 = cos(J_PI / 64.0);
static const FLOAT cot17_64 = 1. / tan(J_PI * 17 / 64.0);
static const FLOAT sin17_64 = sin(J_PI * 17 / 64.0);
static const FLOAT tan9_64 = tan(J_PI * 9 / 64.0);
static const FLOAT cos9_64 = cos(J_PI * 9 / 64.0);
static const FLOAT cot25_64 = 1. / tan(J_PI * 25 / 64.0);
static const FLOAT sin25_64 = sin(J_PI * 25 / 64.0);
static const FLOAT tan5_64 = tan(J_PI * 5 / 64.0);
static const FLOAT cos5_64 = cos(J_PI * 5 / 64.0);
static const FLOAT cot21_64 = 1. / tan(J_PI * 21 / 64.0);
static const FLOAT sin21_64 = sin(J_PI * 21 / 64.0);
static const FLOAT tan13_64 = tan(J_PI * 13 / 64.0);
static const FLOAT cos13_64 = cos(J_PI * 13 / 64.0);
static const FLOAT cot29_64 = 1. / tan(J_PI * 29 / 64.0);
static const FLOAT sin29_64 = sin(J_PI * 29 / 64.0);
static const FLOAT tan3_64 = tan(J_PI * 3 / 64.0);
static const FLOAT cos3_64 = cos(J_PI * 3 / 64.0);
static const FLOAT cot19_64 = 1. / tan(J_PI * 19 / 64.0);
static const FLOAT sin19_64 = sin(J_PI * 19 / 64.0);
static const FLOAT tan11_64 = tan(J_PI * 11 / 64.0);
static const FLOAT cos11_64 = cos(J_PI * 11 / 64.0);
static const FLOAT cot27_64 = 1. / tan(J_PI * 27 / 64.0);
static const FLOAT sin27_64 = sin(J_PI * 27 / 64.0);
static const FLOAT tan7_64 = tan(J_PI * 7 / 64.0);
static const FLOAT cos7_64 = cos(J_PI * 7 / 64.0);
static const FLOAT cot23_64 = 1. / tan(J_PI * 23 / 64.0);
static const FLOAT sin23_64 = sin(J_PI * 23 / 64.0);
static const FLOAT tan15_64 = tan(J_PI * 15 / 64.0);
static const FLOAT cos15_64 = cos(J_PI * 15 / 64.0);
static const FLOAT cot31_64 = 1. / tan(J_PI * 31 / 64.0);
static const FLOAT sin31_64 = sin(J_PI * 31 / 64.0);

#ifdef GENERATE_SCALED_CHEN_DCT_WINDOW
static const FLOAT lambda[64] = {
    cos1_4,  sin17_64,  sin9_32,   sin19_64,  sin5_16,  sin21_64,  sin11_32,  sin23_64,
    sin3_8,  sin25_64,  sin13_32,  sin27_64,  sin7_16,  sin29_64,  sin15_32,  sin31_64,
    0,       -sin31_64, -sin15_32, -sin29_64, -sin7_16, -sin27_64, -sin13_32, -sin25_64,
    -sin3_8, -sin23_64, -sin11_32, -sin21_64, -sin5_16, -sin19_64, -sin9_32,  -sin17_64,
    -cos1_4, -cos15_64, -cos7_32,  -cos13_64, -cos3_16, -cos11_64, -cos5_32,  -cos9_64,
    -cos1_8, -cos7_64,  -cos3_32,  -cos5_64,  -cos1_16, -cos3_64,  -cos1_32,  -cos1_64,
    -1.0,    -cos1_64,  -cos1_32,  -cos3_64,  -cos1_16, -cos5_64,  -cos3_32,  -cos7_64,
    -cos1_8, -cos9_64,  -cos5_32,  -cos11_64, -cos3_16, -cos13_64, -cos7_32,  -cos15_64
};
#endif /* GENERATE_SCALED_CHEN_DCT_WINDOW */

#if 0
/*
 * Decimated (6) version of 32-point forward DCT.
 */
static void inline
DctChen_32_sub6(FLOAT *x, FLOAT *y)
{
  FLOAT x0, x1, x2, x3, x4, x5, x6, x7;
  FLOAT *a0, *a1, *a2;

  // Stage 1.
  a0 = tmp;
  a1 = x;
  a2 = x + 31;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--;

  // Stage 2, upper part.
  a0 = rec_samples;
  a1 = tmp;
  a2 = tmp + 15;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;

  // Stage 3, upper, upper part.
  a0 = tmp;
  a1 = rec_samples;
  a2 = rec_samples + 7;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;

  // Stage 4, upper, upper, upper part.
  a0 = tmp;
  a1 = tmp + 3;
  x0 = *a0++ + *a1--; x1 = *a0++ + *a1--;
  x2 = *a1-- - *a0++; x3 = *a1-- - *a0++;

  y[16] = 0; // always so small that can be set to zero

#ifndef SCALED_DCT
  y[32] = -(y[0] = (x0 - x1) * cos1_4);          // 16
  y[40] = y[56] = -(tan1_8 * x2 + x3) * cos1_8;  // 8
  y[24] = -(y[8] = (cot3_8 * x3 - x2) * sin3_8); // 24
#else
  y[0] = y[32] = x0 - x1;                        // 16
  y[40] = y[56] = tan1_8 * x2 + x3;              // 8
  y[8] = y[24] = cot3_8 * x3 - x2;               // 24
#endif /* not SCALED_DCT */

  // Stage 4, upper, upper part.
  a1 = tmp + 7;
  x0 = *a0++; x3 = *a1--; x1 = -*a0++ + *a1--; x2 = *a0++ + *a1--;
  x1 *= cos1_4;
  x2 *= cos1_4;
  x4 = x0 + x1; x5 = x0 - x1; x6 = -x2 + x3; x7 = x2 + x3;

#ifndef SCALED_DCT
  y[44] = y[52] = -(tan1_16 * x4 + x7) * cos1_16;     // 4
#else
  y[44] = y[52] = tan1_16 * x4 + x7;                  // 4
#endif /* not SCALED_DCT */

  // Stage 3, upper, lower part.
  a0 = rec_samples + 8;
  a1 = rec_samples + 15;
  x0 = *a0++; x1 = *a0++;
  x7 = *a1--; x6 = *a1--;
  x2 = (-*a0++ + *a1--) * cos1_4;
  x3 = (-*a0++ + *a1--) * cos1_4;
  x4 = (*a0++ + *a1--) * cos1_4;
  x5 = (*a0++ + *a1--) * cos1_4;

  // Stage 4, upper, lower part.
  a0 = tmp;
  *a0++ = x0 + x3; *a0++ = x1 + x2;
  *a0++ = x1 - x2; x3 = (*a0++ = x0 - x3);
  *a0++ = -x4 + x7; *a0++ = -x5 + x6; *a0++ = x5 + x6;
  x7 = (*a0-- = x4 + x7);

  // Stage 5, upper, lower part.
  a2 = tmp + 1;
  x1 = (-*a2 + *a0 * tan1_8) * cos1_8;
  x6 = (*a2++ * cot3_8 + *a0--) * sin3_8;
  x2 = (-*a2 * tan1_8 - *a0) * cos1_8;
  x5 = (-*a2 + cot3_8 * *a0) * sin3_8;

  // Stage 6, upper, lower part.
  a0 = rec_samples;
  *a0++ = *tmp + x1; *a0++ = *tmp - x1; *a0++ = -x2 + x3; *a0++ = x2 + x3;
  *a0++ = tmp[4] + x5; *a0++ = tmp[4] - x5; *a0++ = -x6 + x7; *a0++ = x6 + x7;

  // Stage 7, output
  a0 = rec_samples;
  a1 = rec_samples + 7;
#ifndef SCALED_DCT
  y[46] = y[50] = -(*a0 * tan1_32 + *a1) * cos1_32;          // 2
  y[18] = -(y[14] = (-*a0++ + *a1-- * cot15_32) * sin15_32); // 30
  y[30] = -(y[2] = (*a0 + *a1 * cot9_32) * sin9_32);         // 18
  y[34] = y[62] = -(-*a0++ * tan7_32 + *a1--) * cos7_32;     // 14
  y[38] = y[58] = -(*a0 * tan5_32 + *a1) * cos5_32;          // 10
  y[26] = -(y[6] = (-*a0++ + *a1-- * cot11_32) * sin11_32);  // 22
#else
  y[46] = y[50] = *a0 * tan1_32 + *a1;                       // 2
  y[18] = y[14] = -*a0++ + *a1-- * cot15_32;                 // 30
  y[30] = y[2] = *a0 + *a1 * cot9_32;                        // 18
  y[34] = y[62] = -*a0++ * tan7_32 + *a1--;                  // 14
  y[38] = y[58] = *a0 * tan5_32 + *a1;                       // 10
  y[26] = y[6] = -*a0++ + *a1-- * cot11_32;                  // 22
#endif /* not SCALED_DCT */

}
#endif
#else
static const FLOAT MY_PI = 3.14159265358979323846;
static const FLOAT cos1_64 = 1.0 / (2.0 * cos(MY_PI / 64.0));
static const FLOAT cos3_64 = 1.0 / (2.0 * cos(MY_PI * 3.0 / 64.0));
static const FLOAT cos5_64 = 1.0 / (2.0 * cos(MY_PI * 5.0 / 64.0));
static const FLOAT cos7_64 = 1.0 / (2.0 * cos(MY_PI * 7.0 / 64.0));
static const FLOAT cos9_64 = 1.0 / (2.0 * cos(MY_PI * 9.0 / 64.0));
static const FLOAT cos11_64 = 1.0 / (2.0 * cos(MY_PI * 11.0 / 64.0));
static const FLOAT cos13_64 = 1.0 / (2.0 * cos(MY_PI * 13.0 / 64.0));
static const FLOAT cos15_64 = 1.0 / (2.0 * cos(MY_PI * 15.0 / 64.0));
static const FLOAT cos17_64 = 1.0 / (2.0 * cos(MY_PI * 17.0 / 64.0));
static const FLOAT cos19_64 = 1.0 / (2.0 * cos(MY_PI * 19.0 / 64.0));
static const FLOAT cos21_64 = 1.0 / (2.0 * cos(MY_PI * 21.0 / 64.0));
static const FLOAT cos23_64 = 1.0 / (2.0 * cos(MY_PI * 23.0 / 64.0));
static const FLOAT cos25_64 = 1.0 / (2.0 * cos(MY_PI * 25.0 / 64.0));
static const FLOAT cos27_64 = 1.0 / (2.0 * cos(MY_PI * 27.0 / 64.0));
static const FLOAT cos29_64 = 1.0 / (2.0 * cos(MY_PI * 29.0 / 64.0));
static const FLOAT cos31_64 = 1.0 / (2.0 * cos(MY_PI * 31.0 / 64.0));
static const FLOAT cos1_32 = 1.0 / (2.0 * cos(MY_PI / 32.0));
static const FLOAT cos3_32 = 1.0 / (2.0 * cos(MY_PI * 3.0 / 32.0));
static const FLOAT cos5_32 = 1.0 / (2.0 * cos(MY_PI * 5.0 / 32.0));
static const FLOAT cos7_32 = 1.0 / (2.0 * cos(MY_PI * 7.0 / 32.0));
static const FLOAT cos9_32 = 1.0 / (2.0 * cos(MY_PI * 9.0 / 32.0));
static const FLOAT cos11_32 = 1.0 / (2.0 * cos(MY_PI * 11.0 / 32.0));
static const FLOAT cos13_32 = 1.0 / (2.0 * cos(MY_PI * 13.0 / 32.0));
static const FLOAT cos15_32 = 1.0 / (2.0 * cos(MY_PI * 15.0 / 32.0));
static const FLOAT cos1_16 = 1.0 / (2.0 * cos(MY_PI / 16.0));
static const FLOAT cos3_16 = 1.0 / (2.0 * cos(MY_PI * 3.0 / 16.0));
static const FLOAT cos5_16 = 1.0 / (2.0 * cos(MY_PI * 5.0 / 16.0));
static const FLOAT cos7_16 = 1.0 / (2.0 * cos(MY_PI * 7.0 / 16.0));
static const FLOAT cos1_8 = 1.0 / (2.0 * cos(MY_PI / 8.0));
static const FLOAT cos3_8 = 1.0 / (2.0 * cos(MY_PI * 3.0 / 8.0));
static const FLOAT cos1_4 = 1.0 / (2.0 * cos(MY_PI / 4.0));

/**************************************************************************
  Title        : DctLee_32

  Purpose      : Computes 32-point fast DCT transform.

  Usage        : DctLee_32(sb_transform, y)

  Input        : sb_transform - input samples

  Output       : y - output samples.

  Explanation  : This function implements Lee's fast DCT transfrom.

  References   : B.G. Lee, "A new algorithm to compute discrete cosine
                 transform", IEEE Trans. Acoust., Speech., Signal
                 Processing, vol. ASSP-32, pp. 1243-1245, Dec. 1984

  Author(s)    : Jeff Tsay
  *************************************************************************/

static void inline DctLee_32(FLOAT *sb_transform, FLOAT *y)
{
    FLOAT new_v[32]; // new V[0-15] and V[33-48] of Figure 3-A.2 in ISO DIS 11172-3
    FLOAT *x1, *x2, tmp;
    FLOAT p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
    FLOAT pp0, pp1, pp2, pp3, pp4, pp5, pp6, pp7, pp8, pp9, pp10, pp11, pp12, pp13, pp14, pp15;

    // compute new values via a fast cosine transform:
    x1 = sb_transform;
    x2 = sb_transform + 31;
    p0 = *x1++ + *x2;
    p1 = *x1++ + *--x2;
    p2 = *x1++ + *--x2;
    p3 = *x1++ + *--x2;
    p4 = *x1++ + *--x2;
    p5 = *x1++ + *--x2;
    p6 = *x1++ + *--x2;
    p7 = *x1++ + *--x2;
    p8 = *x1++ + *--x2;
    p9 = *x1++ + *--x2;
    p10 = *x1++ + *--x2;
    p11 = *x1++ + *--x2;
    p12 = *x1++ + *--x2;
    p13 = *x1++ + *--x2;
    p14 = *x1++ + *--x2;
    p15 = *x1 + *--x2;

    pp0 = p0 + p15;
    pp1 = p1 + p14;
    pp2 = p2 + p13;
    pp3 = p3 + p12;
    pp4 = p4 + p11;
    pp5 = p5 + p10;
    pp6 = p6 + p9;
    pp7 = p7 + p8;
    pp8 = cos1_32 * (p0 - p15);
    pp9 = cos3_32 * (p1 - p14);
    pp10 = cos5_32 * (p2 - p13);
    pp11 = cos7_32 * (p3 - p12);
    pp12 = cos9_32 * (p4 - p11);
    pp13 = cos11_32 * (p5 - p10);
    pp14 = cos13_32 * (p6 - p9);
    pp15 = cos15_32 * (p7 - p8);

    p0 = pp0 + pp7;
    p1 = pp1 + pp6;
    p2 = pp2 + pp5;
    p3 = pp3 + pp4;
    p4 = cos1_16 * (pp0 - pp7);
    p5 = cos3_16 * (pp1 - pp6);
    p6 = cos5_16 * (pp2 - pp5);
    p7 = cos7_16 * (pp3 - pp4);
    p8 = pp8 + pp15;
    p9 = pp9 + pp14;
    p10 = pp10 + pp13;
    p11 = pp11 + pp12;
    p12 = cos1_16 * (pp8 - pp15);
    p13 = cos3_16 * (pp9 - pp14);
    p14 = cos5_16 * (pp10 - pp13);
    p15 = cos7_16 * (pp11 - pp12);

    pp0 = p0 + p3;
    pp1 = p1 + p2;
    pp2 = cos1_8 * (p0 - p3);
    pp3 = cos3_8 * (p1 - p2);
    pp4 = p4 + p7;
    pp5 = p5 + p6;
    pp6 = cos1_8 * (p4 - p7);
    pp7 = cos3_8 * (p5 - p6);
    pp8 = p8 + p11;
    pp9 = p9 + p10;
    pp10 = cos1_8 * (p8 - p11);
    pp11 = cos3_8 * (p9 - p10);
    pp12 = p12 + p15;
    pp13 = p13 + p14;
    pp14 = cos1_8 * (p12 - p15);
    pp15 = cos3_8 * (p13 - p14);

    p0 = pp0 + pp1;
    p1 = cos1_4 * (pp0 - pp1);
    p2 = pp2 + pp3;
    p3 = cos1_4 * (pp2 - pp3);
    p4 = pp4 + pp5;
    p5 = cos1_4 * (pp4 - pp5);
    p6 = pp6 + pp7;
    p7 = cos1_4 * (pp6 - pp7);
    p8 = pp8 + pp9;
    p9 = cos1_4 * (pp8 - pp9);
    p10 = pp10 + pp11;
    p11 = cos1_4 * (pp10 - pp11);
    p12 = pp12 + pp13;
    p13 = cos1_4 * (pp12 - pp13);
    p14 = pp14 + pp15;
    p15 = cos1_4 * (pp14 - pp15);

    tmp = p6 + p7;
    new_v[36 - 17] = -(p5 + tmp);
    new_v[44 - 17] = -(p4 + tmp);
    tmp = p11 + p15;
    new_v[10] = tmp;
    new_v[6] = p13 + tmp;
    tmp = p14 + p15;
    new_v[46 - 17] = -(p8 + p12 + tmp);
    new_v[34 - 17] = -(p9 + p13 + tmp);
    tmp += p10 + p11;
    new_v[38 - 17] = -(p13 + tmp);
    new_v[42 - 17] = -(p12 + tmp);
    new_v[2] = p9 + p13 + p15;
    new_v[4] = p5 + p7;
    new_v[48 - 17] = -p0;
    new_v[0] = p1;
    new_v[8] = p3;
    new_v[12] = p7;
    new_v[14] = p15;
    new_v[40 - 17] = -(p2 + p3);

    x1 = sb_transform;
    x2 = sb_transform + 31;
    p0 = cos1_64 * (*x1++ - *x2);
    p1 = cos3_64 * (*x1++ - *--x2);
    p2 = cos5_64 * (*x1++ - *--x2);
    p3 = cos7_64 * (*x1++ - *--x2);
    p4 = cos9_64 * (*x1++ - *--x2);
    p5 = cos11_64 * (*x1++ - *--x2);
    p6 = cos13_64 * (*x1++ - *--x2);
    p7 = cos15_64 * (*x1++ - *--x2);
    p8 = cos17_64 * (*x1++ - *--x2);
    p9 = cos19_64 * (*x1++ - *--x2);
    p10 = cos21_64 * (*x1++ - *--x2);
    p11 = cos23_64 * (*x1++ - *--x2);
    p12 = cos25_64 * (*x1++ - *--x2);
    p13 = cos27_64 * (*x1++ - *--x2);
    p14 = cos29_64 * (*x1++ - *--x2);
    p15 = cos31_64 * (*x1 - *--x2);

    pp0 = p0 + p15;
    pp1 = p1 + p14;
    pp2 = p2 + p13;
    pp3 = p3 + p12;
    pp4 = p4 + p11;
    pp5 = p5 + p10;
    pp6 = p6 + p9;
    pp7 = p7 + p8;
    pp8 = cos1_32 * (p0 - p15);
    pp9 = cos3_32 * (p1 - p14);
    pp10 = cos5_32 * (p2 - p13);
    pp11 = cos7_32 * (p3 - p12);
    pp12 = cos9_32 * (p4 - p11);
    pp13 = cos11_32 * (p5 - p10);
    pp14 = cos13_32 * (p6 - p9);
    pp15 = cos15_32 * (p7 - p8);

    p0 = pp0 + pp7;
    p1 = pp1 + pp6;
    p2 = pp2 + pp5;
    p3 = pp3 + pp4;
    p4 = cos1_16 * (pp0 - pp7);
    p5 = cos3_16 * (pp1 - pp6);
    p6 = cos5_16 * (pp2 - pp5);
    p7 = cos7_16 * (pp3 - pp4);
    p8 = pp8 + pp15;
    p9 = pp9 + pp14;
    p10 = pp10 + pp13;
    p11 = pp11 + pp12;
    p12 = cos1_16 * (pp8 - pp15);
    p13 = cos3_16 * (pp9 - pp14);
    p14 = cos5_16 * (pp10 - pp13);
    p15 = cos7_16 * (pp11 - pp12);

    pp0 = p0 + p3;
    pp1 = p1 + p2;
    pp2 = cos1_8 * (p0 - p3);
    pp3 = cos3_8 * (p1 - p2);
    pp4 = p4 + p7;
    pp5 = p5 + p6;
    pp6 = cos1_8 * (p4 - p7);
    pp7 = cos3_8 * (p5 - p6);
    pp8 = p8 + p11;
    pp9 = p9 + p10;
    pp10 = cos1_8 * (p8 - p11);
    pp11 = cos3_8 * (p9 - p10);
    pp12 = p12 + p15;
    pp13 = p13 + p14;
    pp14 = cos1_8 * (p12 - p15);
    pp15 = cos3_8 * (p13 - p14);

    p0 = pp0 + pp1;
    p1 = cos1_4 * (pp0 - pp1);
    p2 = pp2 + pp3;
    p3 = cos1_4 * (pp2 - pp3);
    p4 = pp4 + pp5;
    p5 = cos1_4 * (pp4 - pp5);
    p6 = pp6 + pp7;
    p7 = cos1_4 * (pp6 - pp7);
    p8 = pp8 + pp9;
    p9 = cos1_4 * (pp8 - pp9);
    p10 = pp10 + pp11;
    p11 = cos1_4 * (pp10 - pp11);
    p12 = pp12 + pp13;
    p13 = cos1_4 * (pp12 - pp13);
    p14 = pp14 + pp15;
    p15 = cos1_4 * (pp14 - pp15);

    tmp = p13 + p15;
    new_v[1] = p1 + p9 + tmp;
    new_v[5] = p5 + p7 + p11 + tmp;
    tmp += p9;
    new_v[33 - 17] = -(p1 + p14 + tmp);
    tmp += p5 + p7;
    new_v[3] = tmp;
    new_v[35 - 17] = -(p6 + p14 + tmp);
    tmp = p10 + p11 + p12 + p13 + p14 + p15;
    new_v[39 - 17] = -(p2 + p3 + tmp - p12);
    new_v[43 - 17] = -(p4 + p6 + p7 + tmp - p13);
    new_v[37 - 17] = -(p5 + p6 + p7 + tmp - p12);
    new_v[41 - 17] = -(p2 + p3 + tmp - p13);
    tmp = p8 + p12 + p14 + p15;
    new_v[47 - 17] = -(p0 + tmp);
    new_v[45 - 17] = -(p4 + p6 + p7 + tmp);
    tmp = p11 + p15;
    new_v[11] = p7 + tmp;
    tmp += p3;
    new_v[9] = tmp;
    new_v[7] = p13 + tmp;
    new_v[13] = p7 + p15;
    new_v[15] = p15;

    // insert V[0-15] (== new_v[0-15]) into actual v:
    x1 = new_v;
    x2 = y;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1;

    // V[16] is always 0.0:
    *x2++ = 0.0;

    // insert V[17-31] (== -new_v[15-1]) into actual v:
    *x2++ = -*x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;
    *x2++ = -*--x1;

    // insert V[32] (== -new_v[0]) into other v:
    *x2++ = -*--x1;

    // insert V[33-48] (== new_v[16-31]) into other v:
    x1 = new_v + 16;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1++;
    *x2++ = *x1;

    // insert V[49-63] (== new_v[30-16]) into other v:
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
    *x2++ = *--x1;
}

#endif /* DCT_CHEN */
#endif /* COSINE_SYNTHESIS */

/*
 * Include tables.
 */
#include "syntbl.cpp"

#define ROUND_OUTPUT

#define OUT_SAMPLE(sample, sum)  \
    if ((sum) > 32767.0) {       \
        *(sample) = 0x7fff;      \
    }                            \
    else if ((sum) < -32768.0) { \
        *(sample) = -0x8000;     \
    }                            \
    else {                       \
        *(samples) = sum;        \
    }

/*
 * Include dct and windowing implementations.
 */
#include "synfilt2.cpp" // full rate
#include "synfilt3.cpp" // half rate
#include "synfilt4.cpp" // quad rate

/*
   Purpose:     Typedef of DCT functions.
   Explanation: - */
typedef void (*DCT_FUNCTION)(FLOAT *x, FLOAT *y);

/*
   Purpose:     Array to hold DCT routines.
   Explanation: - */
static DCT_FUNCTION DCTFunc[5] = { NULL,
                                   (DCT_FUNCTION) DctChen_32,
                                   (DCT_FUNCTION) DctChen_32_sub2,
                                   NULL,
                                   (DCT_FUNCTION) DctChen_32_sub4 };

/*
   Purpose:     Typedef of windowing functions.
   Explanation: - */
typedef void (*WINDOW_FUNCTION)(
    FLOAT *synthesis_buffer,
    int16 *out_samples,
    int *buf_idx,
    Out_Param *out_param);

/*
   Purpose:     Array to hold windowing routines.
   Explanation: - */
static WINDOW_FUNCTION WinFunc[5] = { NULL,
                                      (WINDOW_FUNCTION) Window_Full,
                                      (WINDOW_FUNCTION) Window_Half,
                                      NULL,
                                      (WINDOW_FUNCTION) Window_Quad };

/**************************************************************************
  Title        : Synthesis

  Purpose      : Synthesis filterbank (floating point).

  Usage        : Synthesis(synthesis_buffer, sb_samples, out_samples,
                           buf_idx, out_param)

  Input        : synthesis_buffer - buffer to hold the past DCT samples
                 sb_samples       - reconstructed subband samples
                 buf_idx          - index to 'buf_idx_offset' table which
                                    describes the location of the latest
                                    DCT samples within the 'synthesis_buffer'
                 out_param        - output stream parameters

  Output       : synthesis_buffer - updated with new samples
                 out_samples      - output samples
                 buf_idx          - updated accordingly (follows modulo 16)

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
Synthesis(
    FLOAT *synthesis_buffer,
    FLOAT *sb_samples,
    int16 *out_samples,
    int *buf_idx,
    Out_Param *out_param)
{
    static int16 buf_idx00[] = { 0,   960, 896, 832, 768, 704, 640, 576,
                                 512, 448, 384, 320, 256, 192, 128, 64 };

    /*-- Apply a variant of the IMDCT to the subband samples. --*/
#ifdef COSINE_SYNTHESIS
    Cosine_Synthesis(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#else
#ifdef DCT_CHEN
    DCTFunc[out_param->decim_factor](sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#else
    DctLee_32(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#endif /* DCT_CHEN */
#endif /* COSINE_SYNTHESIS */

    /* Windowing part of the synthesis filter bank. */
    WinFunc[out_param->decim_factor](synthesis_buffer, out_samples, buf_idx, out_param);
}

/**************************************************************************
  Title        : PolyPhaseFIR

  Purpose      : Interface to synthesis filterbank.

  Usage        : PolyPhaseFIR(mp, sb_samples, out_samples, buf_idx, out_param)

  Input        : mp          - MPx parameters
                 sb_samples  - reconstructed subband samples
                 buf_idx     - index to 'buf_idx_offset' table which
                               describes the location of the latest
                               DCT samples within the 'synthesis_buffer'
                 out_param   - output stream parameters
                 ch          - channel number

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
PolyPhaseFIR(
    MP_Stream *mp,
    FLOAT *sb_samples,
    int16 *out_samples,
    int *buf_idx,
    Out_Param *out_param,
    int ch)
{
    /*-- Floating point synthesis filterbank. --*/
    Synthesis(mp->buffer->synthesis_buffer[ch], sb_samples, out_samples, buf_idx, out_param);
}


#if 0
void __fastcall
Synthesis(FLOAT *synthesis_buffer, FLOAT *sb_samples, int16 *out_samples,
           int *buf_idx, Out_Param *out_param)
{
  int i;
  FLOAT r[SBLIMIT];
  int16 *buf_idx0;

  {
    static int16 buf_idx00[] = {0, 960, 896, 832, 768, 704, 640, 576, 512, 448,
                                384, 320, 256, 192, 128, 64};

    /*-- Apply a variant of the IMDCT to the subband samples. --*/
#ifdef COSINE_SYNTHESIS
    Cosine_Synthesis(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#else
#ifdef DCT_CHEN
    if(out_param->decim_factor == 2)
      DctChen_32_sub2(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
    else if(out_param->decim_factor == 4)
      DctChen_32_sub4(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#if 0
    else if(out_param->decim_factor == 6)
      DctChen_32_sub6(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#endif
    else
      DctChen_32(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#else
    DctLee_32(sb_samples, synthesis_buffer + buf_idx00[*buf_idx]);
#endif /* DCT_CHEN */

#endif /* COSINE_SYNTHESIS */
  }

  /*
   * Perform windowing. Truncate the synthesis window length.
   * Keep only the windows coefficients W[64],...,W[447] (default).
   */
  buf_idx0 = &buf_idx_offset[*buf_idx][out_param->window_pruning_idx];
  FLOAT *w = dewindow + out_param->window_offset;

  switch(out_param->decim_factor)
  {
    /*
     * No decimation, windowing performed as specified in the standard.
     */
    case 1:
      memset(r, 0, SBLIMIT * sizeof(FLOAT));

      for(i = 0; i < out_param->num_subwindows; i++, w += 32)
      {
        FLOAT *u, *v;

        u = r;
        v = &synthesis_buffer[*buf_idx0++];
#if 0
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
        *u++ += (*v++) * (*w++); *u++ += (*v++) * (*w++);
#else
        u[0] += v[0] * w[0]; u[1] += v[1] * w[1];
        u[2] += v[2] * w[2]; u[3] += v[3] * w[3];
        u[4] += v[4] * w[4]; u[5] += v[5] * w[5];
        u[6] += v[6] * w[6]; u[7] += v[7] * w[7];
        u[8] += v[8] * w[8]; u[9] += v[9] * w[9];
        u[10] += v[10] * w[10]; u[11] += v[11] * w[11];
        u[12] += v[12] * w[12]; u[13] += v[13] * w[13];
        u[14] += v[14] * w[14]; u[15] += v[15] * w[15];
        u[16] += v[16] * w[16]; u[17] += v[17] * w[17];
        u[18] += v[18] * w[18]; u[19] += v[19] * w[19];
        u[20] += v[20] * w[20]; u[21] += v[21] * w[21];
        u[22] += v[22] * w[22]; u[23] += v[23] * w[23];
        u[24] += v[24] * w[24]; u[25] += v[25] * w[25];
        u[26] += v[26] * w[26]; u[27] += v[27] * w[27];
        u[28] += v[28] * w[28]; u[29] += v[29] * w[29];
        u[30] += v[30] * w[30]; u[31] += v[31] * w[31];
#endif
      }
      break;

    /*
     * Decimate the output by a factor of two. Supported by all sampling
     * frequencies.
     */
    case 2:
      memset(r, 0, (SBLIMIT >> 1) * sizeof(FLOAT));

      for(i = 0; i < out_param->num_subwindows; i++)
      {
        FLOAT *u, *v;

        u = r;
        v = &synthesis_buffer[*buf_idx0++];

        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
        *u++ += (*v++) * (*w++); v++; w++;  *u++ += (*v++) * (*w++); v++; w++;
      }
      break;

    /*
     * Decimate the output by a factor of four. Only sampling frequencies
     * 48, 44.1 and 32 kHz support this mode.
     */
    case 4:
      memset(r, 0, (SBLIMIT >> 2) * sizeof(FLOAT));

      for(i = 0; i < out_param->num_subwindows; i++)
      {
        FLOAT *u, *v;

        u = r;
        v = &synthesis_buffer[*buf_idx0++];

        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  v +=3 ; w += 3;
        *u++ += (*v++) * (*w++);  w += 3;
      }
      break;
  }

  (*buf_idx)++;
  *buf_idx &= 15;

  /*-- Convert the decoded samples to 16-bit integer. --*/
  int16 *samples = out_samples;
  for(i = 0; i < out_param->num_samples; i++)
  {
#if 0
#ifdef ROUNDING
    if(r[i] > 0)
      foo = (int32)(r[i] + 0.5);
    else
      foo = (int32)(r[i] - 0.5);
#else
    foo = (int32)r[i];
#endif /* ROUNDING */

    /*-- Clip the result if needed. --*/
#ifdef CLIPPING
    if(foo >= SCALE)
      *samples = (int16)(SCALE-1);
    else if(foo < (int16)-SCALE)
      *samples = (int16)(-SCALE);
    else
#endif /* CLIPPING */
      *samples = (int16)foo;
#else
      *samples = (int16)r[i];
#endif

    samples += out_param->num_out_channels;
  }
}
#endif

#ifdef GENERATE_SCALED_CHEN_DCT_WINDOW

/**************************************************************************
  Title        : main

  Purpose      : Generates synthesis window coefficients for scaled DCT.

  Usage        : main

  Input        : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
main(void)
{
    int i, j, k;
    FLOAT *u;
    FILE *fp1;

#ifdef SCALED_DCT
    fprintf(stderr, "Undefine SCALED_DCT compile switch.\n");
    exit(EXIT_FAILURE);
#endif /* SCALED_DCT */

    fp1 = fopen("chenwin.txt", "w");
    u = dewindow;

    for (i = 0, k = 0; i < 8; i++) {
        for (j = 0; j < 64; j++, k++) {
            if (k && ((k % 3) == 0))
                fprintf(fp1, "\n");

            fprintf(fp1, "%+5.20f, ", *u++ * lambda[j]);
        }
    }

    fclose(fp1);
}
#endif /* GENERATE_SCALED_DCT_WINDOW */
