/**************************************************************************
  synfilt2.cpp - Full rate implementations of the synthesis filterbank.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

static FLOAT tmp[SBLIMIT], rec_samples[SBLIMIT];

/**************************************************************************
  Title        : DctChen_32

  Purpose      : Computes 32-point fast DCT transform.

  Usage        : DctChen_32(x, y)

  Input        : x - input samples

  Output       : y - output samples

  Explanation  : Two fast versions are available : scaled DCT and basic DCT.
                 The scaled DCT is uses fewer number of multiplications since
                 some of the scaling factors (basis functions) can be embedded
                 into the window coefficients.

                 The scaled DCT requires 194 additions and 84 multiplications
                 whereas the basic DCT requires also 194 additions and 115
                 multiplications.

  References   : Basic Chen DCT :
                 W.H. Chen, C.H. Smith, S.C. Fralick, "A fast computa-
                 tional algorithm for the discrete cosine transform",
                 IEEE Trans. on Communications, vol COM-25, no. 9, Sep. 1977.

                 Scaled Chen DCT :
                 M.C. Hans and V. Bhaskaran, " A fast integer-based, CPU
                 scalable, MPEG-1 layer-II audio coder", Audio. Eng. Soc.
                 Preprint 4359, Los Angeles, CA, Nov. 1996.

                 M.C. Hans and V. Bhaskaran, " A compliant MPEG-1 layer-II
                 audio decoder with 16-bit arithmetic operations",
                 IEEE Sig., Proc., Letters, vol 4, no. 5, May 1997.

  Author(s)    : Juha Ojanpera
  *************************************************************************/
#if 0
static void inline
DctChen_32(FLOAT *x, FLOAT *y)
{
  FLOAT x0, x1, x2, x3, x4, x5, x6, x7;
  FLOAT tmp[SBLIMIT], rec_samples[SBLIMIT];
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
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++; *a0++ = *a2-- - *a1++;
  *a0++ = *a2-- - *a1++;

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

#ifndef SCALED_DCT
  *y = x0 + x1;
  *(y + 16) = (x0 - x1) * cos1_4;
  *(y + 8) = (tan1_8 * x2 + x3) * cos1_8;
  *(y + 24) = (cot3_8 * x3 - x2) * sin3_8;
#else
  *y = x0 + x1;
  *(y + 16) = x0 - x1;
  *(y + 8) = tan1_8 * x2 + x3;
  *(y + 24) = cot3_8 * x3 - x2;
#endif /* not SCALED_DCT */

  // Stage 4, upper, upper part.
  a1 = tmp + 7;
  x0 = *a0++; x3 = *a1--; x1 = -*a0++ + *a1--; x2 = *a0++ + *a1--;
  x1 *= cos1_4;
  x2 *= cos1_4;
  x4 = x0 + x1; x5 = x0 - x1; x6 = -x2 + x3; x7 = x2 + x3;

#ifndef SCALED_DCT
  *(y + 4) = (tan1_16 * x4 + x7) * cos1_16;
  *(y + 20) = (x5 + (x6 * cot5_16)) * sin5_16;
  *(y + 12) = ((-x5 * tan3_16) + x6) * cos3_16;
  *(y + 28) = (cot7_16 * x7 - x4) * sin7_16;
#else
  *(y + 4) = tan1_16 * x4 + x7;
  *(y + 20) = x5 + x6 * cot5_16;
  *(y + 12) = -x5 * tan3_16 + x6;
  *(y + 28) = cot7_16 * x7 - x4;
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
  *(y + 2) = (*a0 * tan1_32 + *a1) * cos1_32;
  *(y + 30) = (-*a0++ + *a1-- * cot15_32) * sin15_32;
  *(y + 18) = (*a0 + *a1 * cot9_32) * sin9_32;
  *(y + 14) = (-*a0++ * tan7_32 + *a1--) * cos7_32;
  *(y + 10) = (*a0 * tan5_32 + *a1) * cos5_32;
  *(y + 22) = (-*a0++ + *a1-- * cot11_32) * sin11_32;
  *(y + 26) = (*a0 + *a1 * cot13_32) * sin13_32;
  *(y + 6) = (-*a0++ * tan3_32 + *a1--) * cos3_32;
#else
  *(y + 2) = *a0 * tan1_32 + *a1;
  *(y + 30) = -*a0++ + *a1-- * cot15_32;
  *(y + 18) = *a0 + *a1 * cot9_32;
  *(y + 14) = -*a0++ * tan7_32 + *a1--;
  *(y + 10) = *a0 * tan5_32 + *a1;
  *(y + 22) = -*a0++ + *a1-- * cot11_32;
  *(y + 26) = *a0 + *a1 * cot13_32;
  *(y + 6) = -*a0++ * tan3_32 + *a1--;
#endif /* not SCALED_DCT */

  // Stage 2, lower part.
  a0 = rec_samples;
  a1 = tmp + 16;
  a2 = tmp + 27;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = (-*a1++ + *a2--) * cos1_4; *a0++ = (-*a1++ + *a2--) * cos1_4;
  *a0++ = (-*a1++ + *a2--) * cos1_4; *a0++ = (-*a1++ + *a2--) * cos1_4;
  *a0++ = (*a1++ + *a2--) * cos1_4; *a0++ = (*a1++ + *a2--) * cos1_4;
  *a0++ = (*a1++ + *a2--) * cos1_4; *a0++ = (*a1++ + *a2--) * cos1_4;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;

  // Stage 3, lower part.
  a0 = tmp;
  a1 = rec_samples;
  a2 = rec_samples + 7;
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--; x0 = (*a0++ = *a1++ + *a2--);
  x1 = (*a0++ = *a1++ + *a2--);
  x2 = (*a0++ = -*a1++ + *a2--); x3 = (*a0++ = -*a1++ + *a2--);
  *a0++ = -*a1++ + *a2--; *a0++ = -*a1++ + *a2--;

  a2 = rec_samples + 15;
  *a0++ = -*a1++ + *a2--; *a0++ = -*a1++ + *a2--; x4 = (*a0++ = -*a1++ + *a2--);
  x5 = (*a0++ = -*a1++ + *a2--);
  x6 = (*a0++ = *a1++ + *a2--); x7 = (*a0++ = *a1++ + *a2--);
  *a0++ = *a1++ + *a2--; *a0++ = *a1++ + *a2--;

  // Stage 4, lower part.
  a0 = tmp + 2;
  a1 = tmp + 13;
  *a0++ = (-x0 + x7 * tan1_8) * cos1_8; *a0++ = (-x1 + x6 * tan1_8) * cos1_8;
  *a0++ = (-x2 * tan1_8 - x5) * cos1_8; *a0++ = (-x3 * tan1_8 - x4) * cos1_8;
  *a1-- = (x0 * cot3_8 + x7) * sin3_8; *a1-- = (x1 * cot3_8 + x6) * sin3_8;
  *a1-- = (-x2 + x5 * cot3_8) * sin3_8; *a1-- = (-x3 + x4 * cot3_8) * sin3_8;

  // Stage 5, lower part.
  a0 = rec_samples;
  a1 = tmp;
  a2 = tmp + 3;
  *a0++ = *a1++ + *a2--;  x0 = (*a0++ = *a1++ + *a2--);
  x1 = (*a0++ = -*a1++ + *a2--); *a0++ = -*a1++ + *a2--;
  a2 = a1 + 3;
  *a0++ = -*a1++ + *a2--; x2 = (*a0++ = -*a1++ + *a2--);
  x3 = (*a0++ = *a1++ + *a2--);  *a0++ = *a1++ + *a2--;
  a2 = a1 + 3;
  *a0++ = *a1++ + *a2--;  x4 = (*a0++ = *a1++ + *a2--);
  x5 = (*a0++ = -*a1++ + *a2--); *a0++ = -*a1++ + *a2--;
  a2 = a1 + 3;
  *a0++ = -*a1++ + *a2--; x6 = (*a0++ = -*a1++ + *a2--);
  x7 = (*a0++ = *a1++ + *a2--);  *a0++ = *a1++ + *a2--;

  // Stage 6, lower part.
  a0 = rec_samples + 1;
  *a0++ = (-x0 + x7 * tan1_16) * cos1_16;
  *a0++ = (-x1 * tan1_16 - x6) * cos1_16;
  a0 += 2;
  *a0++ = (-x2 * cot5_16 + x5) * sin5_16;
  *a0++ = (-x3 - x4 * cot5_16) * sin5_16;
  a0 += 2;
  *a0++ = (x4 - x3 * tan3_16) * cos3_16;
  *a0++ = (x5 * tan3_16 + x2) * cos3_16;
  a0 += 2;
  *a0++ = (x6 * cot7_16 - x1) * sin7_16;
  *a0++ = (x7 + x0 * cot7_16) * sin7_16;

  // Stage 7, lower part.
  a0 = tmp;
  a1 = rec_samples;
  a2 = rec_samples + 1;
  *a0++ = *a1++ + *a2--; *a0++ = -*a1++ + *a2++;
  a1++; a2++;
  *a0++ = -*a2++ + *a1--; *a0++ = *a2++ + *a1++;
  a1++; a2++;
  *a0++ = *a1++ + *a2--; *a0++ = -*a1++ + *a2++;
  a1++; a2++;
  *a0++ = -*a2++ + *a1--; *a0++ = *a2++ + *a1++;
  a1++; a2++;
  *a0++ = *a1++ + *a2--; *a0++ = -*a1++ + *a2++;
  a1++; a2++;
  *a0++ = -*a2++ + *a1--; *a0++ = *a2++ + *a1++;
  a1++; a2++;
  *a0++ = *a1++ + *a2--; *a0++ = -*a1++ + *a2++;
  a1++; a2++;
  *a0++ = -*a2++ + *a1--; *a0++ = *a2++ + *a1++;

  // Stage 8, output.
  a2 = tmp;
  a1 = tmp + 15;
#ifndef SCALED_DCT
  *(y + 1) = (*a2++ * tan1_64 + *a1--) * cos1_64;
  *(y + 17) = (*a2++ + *a1-- * cot17_64) * sin17_64;
  *(y + 9) = (*a2++ * tan9_64 + *a1--) * cos9_64;
  *(y + 25) = (*a2++ + *a1-- * cot25_64) * sin25_64;
  *(y + 5) = (*a2++ * tan5_64 + *a1--) * cos5_64;
  *(y + 21) = (*a2++ + *a1-- * cot21_64) * sin21_64;
  *(y + 13) = (*a2++ * tan13_64 + *a1--) * cos13_64;
  *(y + 29) = (*a2++ + *a1-- * cot29_64) * sin29_64;
  *(y + 3) = (-*a1-- * tan3_64 + *a2++) * cos3_64;
  *(y + 19) = (-*a1-- + *a2++ * cot19_64) * sin19_64;
  *(y + 11) = (-*a1-- * tan11_64 + *a2++) * cos11_64;
  *(y + 27) = (-*a1-- + *a2++ * cot27_64) * sin27_64;
  *(y + 7) = (-*a1-- * tan7_64 + *a2++) * cos7_64;
  *(y + 23) = (-*a1-- + *a2++ * cot23_64) * sin23_64;
  *(y + 15) = (-*a1-- * tan15_64 + *a2++) * cos15_64;
  *(y + 31) = (-*a1-- + *a2++ * cot31_64) * sin31_64;
#else
  *(y + 1) = *a2++ * tan1_64 + *a1--;
  *(y + 17) = *a2++ + *a1-- * cot17_64;
  *(y + 9) = *a2++ * tan9_64 + *a1--;
  *(y + 25) = *a2++ + *a1-- * cot25_64;
  *(y + 5) = *a2++ * tan5_64 + *a1--;
  *(y + 21) = *a2++ + *a1-- * cot21_64;
  *(y + 13) = *a2++ * tan13_64 + *a1--;
  *(y + 29) = *a2++ + *a1-- * cot29_64;
  *(y + 3) = -*a1-- * tan3_64 + *a2++;
  *(y + 19) = -*a1-- + *a2++ * cot19_64;
  *(y + 11) = -*a1-- * tan11_64 + *a2++;
  *(y + 27) = -*a1-- + *a2++ * cot27_64;
  *(y + 7) = -*a1-- * tan7_64 + *a2++;
  *(y + 23) = -*a1-- + *a2++ * cot23_64;
  *(y + 15) = -*a1-- * tan15_64 + *a2++;
  *(y + 31) = -*a1-- + *a2++ * cot31_64;
#endif /* not SCALED_DCT */

  // V[33-48] = -V''[15-0]
  a0 = y + 33;
  a1 = y + 15;
#ifndef SCALED_DCT
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1++;

  // V[49-63] = -V''[1-15]
  *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++;
  *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++;
  *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++;
  *a0++ = -*a1++; *a0++ = -*a1++; *a0++ = -*a1++;
#else
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1++;

  // V[49-63] = -V''[1-15]
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
#endif /* not SCALED_DCT */

  // V[0-15] = V''[16-31]
  a0 = y;
  a1 = y + 16;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;
  *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++; *a0++ = *a1++;

  // V[16] = 0.0
  *a0++ = 0.0;

  // V[17-32] = -V''[31-16]
  a1 = y + 15;
#ifndef SCALED_DCT
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
  *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--; *a0++ = -*a1--;
#else
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
  *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--; *a0++ = *a1--;
#endif /* not SCALED_DCT */
}
#endif


#ifdef DCT_CHEN
static void inline DctChen_32(FLOAT *x, FLOAT *y)
{
    // Stage 1.
    {
        FLOAT *a0, *a1, *a2;

        a0 = tmp;
        a1 = x;
        a2 = x + 31;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
    }

    // Stage 2, upper part.
    {
        FLOAT *a0, *a1, *a2;

        a0 = rec_samples;
        a1 = tmp;
        a2 = tmp + 15;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
    }

    // Stage 3, upper, upper part.
    {
        FLOAT x0, x1, x2, x3, x4, x5, x6, x7;
        FLOAT *a0, *a1, *a2;

        a0 = tmp;
        a1 = rec_samples;
        a2 = rec_samples + 7;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;
        *a0++ = *a2-- - *a1++;

        // Stage 4, upper, upper, upper part.
        a0 = tmp;
        a1 = tmp + 3;
        x0 = *a0++ + *a1--;
        x1 = *a0++ + *a1--;
        x2 = *a1-- - *a0++;
        x3 = *a1-- - *a0++;

        y[16] = 0; // always so small that can be set to zero

#ifndef SCALED_DCT
        y[48] = -(x0 + x1);                            // 0
        y[32] = -(y[0] = (x0 - x1) * cos1_4);          // 16
        y[40] = y[56] = -(tan1_8 * x2 + x3) * cos1_8;  // 8
        y[24] = -(y[8] = (cot3_8 * x3 - x2) * sin3_8); // 24
#else
        y[48] = x0 + x1;                           // 0
        y[0] = y[32] = x0 - x1;                    // 16
        y[40] = y[56] = tan1_8 * x2 + x3;          // 8
        y[8] = y[24] = cot3_8 * x3 - x2;           // 24
#endif /* not SCALED_DCT */

        // Stage 4, upper, upper part.
        a1 = tmp + 7;
        x0 = *a0++;
        x3 = *a1--;
        x1 = -*a0++ + *a1--;
        x2 = *a0++ + *a1--;
        x1 *= cos1_4;
        x2 *= cos1_4;
        x4 = x0 + x1;
        x5 = x0 - x1;
        x6 = -x2 + x3;
        x7 = x2 + x3;

#ifndef SCALED_DCT
        y[44] = y[52] = -(tan1_16 * x4 + x7) * cos1_16;    // 4
        y[28] = -(y[4] = (x5 + (x6 * cot5_16)) * sin5_16); // 20
        y[36] = y[60] = -((-x5 * tan3_16) + x6) * cos3_16; // 12
        y[20] = -(y[12] = (cot7_16 * x7 - x4) * sin7_16);  // 28
#else
        y[44] = y[52] = tan1_16 * x4 + x7;         // 4
        y[4] = y[28] = x5 + x6 * cot5_16;          // 20
        y[36] = y[60] = -x5 * tan3_16 + x6;        // 12
        y[20] = y[12] = cot7_16 * x7 - x4;         // 28
#endif /* not SCALED_DCT */

        // Stage 3, upper, lower part.

        a0 = rec_samples + 8;
        a1 = rec_samples + 15;
        x0 = *a0++;
        x1 = *a0++;
        x7 = *a1--;
        x6 = *a1--;
        x2 = (-*a0++ + *a1--) * cos1_4;
        x3 = (-*a0++ + *a1--) * cos1_4;
        x4 = (*a0++ + *a1--) * cos1_4;
        x5 = (*a0++ + *a1--) * cos1_4;

        // Stage 4, upper, lower part.
        a0 = tmp;
        *a0++ = x0 + x3;
        *a0++ = x1 + x2;
        *a0++ = x1 - x2;
        x3 = (*a0++ = x0 - x3);
        *a0++ = -x4 + x7;
        *a0++ = -x5 + x6;
        *a0++ = x5 + x6;
        x7 = (*a0-- = x4 + x7);

        // Stage 5, upper, lower part.
        a2 = tmp + 1;
        x1 = (-*a2 + *a0 * tan1_8) * cos1_8;
        x6 = (*a2++ * cot3_8 + *a0--) * sin3_8;
        x2 = (-*a2 * tan1_8 - *a0) * cos1_8;
        x5 = (-*a2 + cot3_8 * *a0) * sin3_8;

        // Stage 6, upper, lower part.
        a0 = rec_samples;
        *a0++ = *tmp + x1;
        *a0++ = *tmp - x1;
        *a0++ = -x2 + x3;
        *a0++ = x2 + x3;
        *a0++ = tmp[4] + x5;
        *a0++ = tmp[4] - x5;
        *a0++ = -x6 + x7;
        *a0++ = x6 + x7;

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
        y[22] = -(y[10] = (*a0 + *a1 * cot13_32) * sin13_32);      // 26
        y[42] = y[54] = -(-*a0++ * tan3_32 + *a1--) * cos3_32;     // 6
#else
        y[46] = y[50] = *a0 * tan1_32 + *a1;       // 2
        y[18] = y[14] = -*a0++ + *a1-- * cot15_32; // 30
        y[30] = y[2] = *a0 + *a1 * cot9_32;        // 18
        y[34] = y[62] = -*a0++ * tan7_32 + *a1--;  // 14
        y[38] = y[58] = *a0 * tan5_32 + *a1;       // 10
        y[26] = y[6] = -*a0++ + *a1-- * cot11_32;  // 22
        y[22] = y[10] = *a0 + *a1 * cot13_32;      // 26
        y[42] = y[54] = -*a0++ * tan3_32 + *a1--;  // 6
#endif /* not SCALED_DCT */

        // Stage 2, lower part.
        a0 = rec_samples;
        a1 = tmp + 16;
        a2 = tmp + 27;
        *a0++ = *a1++;
        *a0++ = *a1++;
        *a0++ = *a1++;
        *a0++ = *a1++;
        *a0++ = (-*a1++ + *a2--) * cos1_4;
        *a0++ = (-*a1++ + *a2--) * cos1_4;
        *a0++ = (-*a1++ + *a2--) * cos1_4;
        *a0++ = (-*a1++ + *a2--) * cos1_4;
        *a0++ = (*a1++ + *a2--) * cos1_4;
        *a0++ = (*a1++ + *a2--) * cos1_4;
        *a0++ = (*a1++ + *a2--) * cos1_4;
        *a0++ = (*a1++ + *a2--) * cos1_4;
        *a0++ = *a1++;
        *a0++ = *a1++;
        *a0++ = *a1++;
        *a0++ = *a1++;

        // Stage 3, lower part.
        a0 = tmp;
        a1 = rec_samples;
        a2 = rec_samples + 7;
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;
        x0 = (*a0++ = *a1++ + *a2--);
        x1 = (*a0++ = *a1++ + *a2--);
        x2 = (*a0++ = -*a1++ + *a2--);
        x3 = (*a0++ = -*a1++ + *a2--);
        *a0++ = -*a1++ + *a2--;
        *a0++ = -*a1++ + *a2--;

        a2 = rec_samples + 15;
        *a0++ = -*a1++ + *a2--;
        *a0++ = -*a1++ + *a2--;
        x4 = (*a0++ = -*a1++ + *a2--);
        x5 = (*a0++ = -*a1++ + *a2--);
        x6 = (*a0++ = *a1++ + *a2--);
        x7 = (*a0++ = *a1++ + *a2--);
        *a0++ = *a1++ + *a2--;
        *a0++ = *a1++ + *a2--;

        // Stage 4, lower part.
        a0 = tmp + 2;
        a1 = tmp + 13;
        *a0++ = (-x0 + x7 * tan1_8) * cos1_8;
        *a0++ = (-x1 + x6 * tan1_8) * cos1_8;
        *a0++ = (-x2 * tan1_8 - x5) * cos1_8;
        *a0++ = (-x3 * tan1_8 - x4) * cos1_8;
        *a1-- = (x0 * cot3_8 + x7) * sin3_8;
        *a1-- = (x1 * cot3_8 + x6) * sin3_8;
        *a1-- = (-x2 + x5 * cot3_8) * sin3_8;
        *a1-- = (-x3 + x4 * cot3_8) * sin3_8;

        // Stage 5, lower part.
        a0 = rec_samples;
        a1 = tmp;
        a2 = tmp + 3;
        *a0++ = *a1++ + *a2--;
        x0 = (*a0++ = *a1++ + *a2--);
        x1 = (*a0++ = -*a1++ + *a2--);
        *a0++ = -*a1++ + *a2--;
        a2 = a1 + 3;
        *a0++ = -*a1++ + *a2--;
        x2 = (*a0++ = -*a1++ + *a2--);
        x3 = (*a0++ = *a1++ + *a2--);
        *a0++ = *a1++ + *a2--;
        a2 = a1 + 3;
        *a0++ = *a1++ + *a2--;
        x4 = (*a0++ = *a1++ + *a2--);
        x5 = (*a0++ = -*a1++ + *a2--);
        *a0++ = -*a1++ + *a2--;
        a2 = a1 + 3;
        *a0++ = -*a1++ + *a2--;
        x6 = (*a0++ = -*a1++ + *a2--);
        x7 = (*a0++ = *a1++ + *a2--);
        *a0++ = *a1++ + *a2--;

        // Stage 6, lower part.
        a0 = rec_samples + 1;
        *a0++ = (-x0 + x7 * tan1_16) * cos1_16;
        *a0++ = (-x1 * tan1_16 - x6) * cos1_16;
        a0 += 2;
        *a0++ = (-x2 * cot5_16 + x5) * sin5_16;
        *a0++ = (-x3 - x4 * cot5_16) * sin5_16;
        a0 += 2;
        *a0++ = (x4 - x3 * tan3_16) * cos3_16;
        *a0++ = (x5 * tan3_16 + x2) * cos3_16;
        a0 += 2;
        *a0++ = (x6 * cot7_16 - x1) * sin7_16;
        *a0++ = (x7 + x0 * cot7_16) * sin7_16;

        // Stage 7, lower part.

        a0 = tmp;
        a1 = rec_samples;
        a2 = rec_samples + 1;
        *a0++ = *a1++ + *a2--;
        *a0++ = -*a1++ + *a2++;
        a1++;
        a2++;
        *a0++ = -*a2++ + *a1--;
        *a0++ = *a2++ + *a1++;
        a1++;
        a2++;
        *a0++ = *a1++ + *a2--;
        *a0++ = -*a1++ + *a2++;
        a1++;
        a2++;
        *a0++ = -*a2++ + *a1--;
        *a0++ = *a2++ + *a1++;
        a1++;
        a2++;
        *a0++ = *a1++ + *a2--;
        *a0++ = -*a1++ + *a2++;
        a1++;
        a2++;
        *a0++ = -*a2++ + *a1--;
        *a0++ = *a2++ + *a1++;
        a1++;
        a2++;
        *a0++ = *a1++ + *a2--;
        *a0++ = -*a1++ + *a2++;
        a1++;
        a2++;
        *a0++ = -*a2++ + *a1--;
        *a0++ = *a2++ + *a1++;

        // Stage 8, output.

        a2 = tmp;
        a1 = tmp + 15;
#ifndef SCALED_DCT
        y[47] = y[49] = -(*a2++ * tan1_64 + *a1--) * cos1_64;      // 1
        y[31] = -(y[1] = (*a2++ + *a1-- * cot17_64) * sin17_64);   // 17
        y[39] = y[57] = -(*a2++ * tan9_64 + *a1--) * cos9_64;      // 9
        y[23] = -(y[9] = (*a2++ + *a1-- * cot25_64) * sin25_64);   // 25
        y[43] = y[53] = -(*a2++ * tan5_64 + *a1--) * cos5_64;      // 5
        y[27] = -(y[5] = (*a2++ + *a1-- * cot21_64) * sin21_64);   // 21
        y[35] = y[61] = -(*a2++ * tan13_64 + *a1--) * cos13_64;    // 13
        y[19] = -(y[13] = (*a2++ + *a1-- * cot29_64) * sin29_64);  // 29
        y[45] = y[51] = -(-*a1-- * tan3_64 + *a2++) * cos3_64;     // 3
        y[29] = -(y[3] = (-*a1-- + *a2++ * cot19_64) * sin19_64);  // 19
        y[37] = y[59] = -(-*a1-- * tan11_64 + *a2++) * cos11_64;   // 11
        y[21] = -(y[11] = (-*a1-- + *a2++ * cot27_64) * sin27_64); // 27
        y[41] = y[55] = -(-*a1-- * tan7_64 + *a2++) * cos7_64;     // 7
        y[25] = -(y[7] = (-*a1-- + *a2++ * cot23_64) * sin23_64);  // 23
        y[33] = y[63] = -(-*a1-- * tan15_64 + *a2++) * cos15_64;   // 15
        y[17] = -(y[15] = (-*a1-- + *a2++ * cot31_64) * sin31_64); // 31
#else
        y[47] = y[49] = *a2++ * tan1_64 + *a1--;   // 1
        y[31] = y[1] = *a2++ + *a1-- * cot17_64;   // 17
        y[39] = y[57] = *a2++ * tan9_64 + *a1--;   // 9
        y[23] = y[9] = *a2++ + *a1-- * cot25_64;   // 25
        y[43] = y[53] = *a2++ * tan5_64 + *a1--;   // 5
        y[27] = y[5] = *a2++ + *a1-- * cot21_64;   // 21
        y[35] = y[61] = *a2++ * tan13_64 + *a1--;  // 13
        y[19] = y[13] = *a2++ + *a1-- * cot29_64;  // 29
        y[45] = y[51] = -*a1-- * tan3_64 + *a2++;  // 3
        y[29] = y[3] = -*a1-- + *a2++ * cot19_64;  // 19
        y[37] = y[59] = -*a1-- * tan11_64 + *a2++; // 11
        y[21] = y[11] = -*a1-- + *a2++ * cot27_64; // 27
        y[41] = y[55] = -*a1-- * tan7_64 + *a2++;  // 7
        y[25] = y[7] = -*a1-- + *a2++ * cot23_64;  // 23
        y[33] = y[63] = -*a1-- * tan15_64 + *a2++; // 15
        y[17] = y[15] = -*a1-- + *a2++ * cot31_64; // 31
#endif /* not SCALED_DCT */
    }
}
#endif /* CHEN_DCT */


static void
Window_Full(FLOAT *synthesis_buffer, int16 *out_samples, int16 *buf_idx, Out_Param *out_param)
{
    static FLOAT r[SBLIMIT];
    int16 *buf_idx0;

    /*
     * Perform windowing. Truncate the synthesis window length.
     * Keep only the windows coefficients W[64],...,W[447] (default).
     */
    buf_idx0 = &buf_idx_offset[*buf_idx][out_param->window_pruning_idx];
    FLOAT *w = dewindow + out_param->window_offset;
    FLOAT *u = r;

    memset(r, 0, SBLIMIT * sizeof(FLOAT));

    for (int i = 0; i < out_param->num_subwindows; i++, w += 32) {
        FLOAT *v;

        v = &synthesis_buffer[*buf_idx0++];

        u[0] += v[0] * w[0];
        u[1] += v[1] * w[1];
        u[2] += v[2] * w[2];
        u[3] += v[3] * w[3];
        u[4] += v[4] * w[4];
        u[5] += v[5] * w[5];
        u[6] += v[6] * w[6];
        u[7] += v[7] * w[7];
        u[8] += v[8] * w[8];
        u[9] += v[9] * w[9];
        u[10] += v[10] * w[10];
        u[11] += v[11] * w[11];
        u[12] += v[12] * w[12];
        u[13] += v[13] * w[13];
        u[14] += v[14] * w[14];
        u[15] += v[15] * w[15];
        u[16] += v[16] * w[16];
        u[17] += v[17] * w[17];
        u[18] += v[18] * w[18];
        u[19] += v[19] * w[19];
        u[20] += v[20] * w[20];
        u[21] += v[21] * w[21];
        u[22] += v[22] * w[22];
        u[23] += v[23] * w[23];
        u[24] += v[24] * w[24];
        u[25] += v[25] * w[25];
        u[26] += v[26] * w[26];
        u[27] += v[27] * w[27];
        u[28] += v[28] * w[28];
        u[29] += v[29] * w[29];
        u[30] += v[30] * w[30];
        u[31] += v[31] * w[31];
    }

    (*buf_idx)++;
    *buf_idx &= 15;

    /*-- Convert the decoded samples to 16-bit integer. --*/
    int16 *samples = out_samples;
#ifndef ROUND_OUTPUT
    *samples = (int16) r[0];
    samples += out_param->num_out_channels;
    *samples = (int16) r[1];
    samples += out_param->num_out_channels;
    *samples = (int16) r[2];
    samples += out_param->num_out_channels;
    *samples = (int16) r[3];
    samples += out_param->num_out_channels;
    *samples = (int16) r[4];
    samples += out_param->num_out_channels;
    *samples = (int16) r[5];
    samples += out_param->num_out_channels;
    *samples = (int16) r[6];
    samples += out_param->num_out_channels;
    *samples = (int16) r[7];
    samples += out_param->num_out_channels;
    *samples = (int16) r[8];
    samples += out_param->num_out_channels;
    *samples = (int16) r[9];
    samples += out_param->num_out_channels;
    *samples = (int16) r[10];
    samples += out_param->num_out_channels;
    *samples = (int16) r[11];
    samples += out_param->num_out_channels;
    *samples = (int16) r[12];
    samples += out_param->num_out_channels;
    *samples = (int16) r[13];
    samples += out_param->num_out_channels;
    *samples = (int16) r[14];
    samples += out_param->num_out_channels;
    *samples = (int16) r[15];
    samples += out_param->num_out_channels;
    *samples = (int16) r[16];
    samples += out_param->num_out_channels;
    *samples = (int16) r[17];
    samples += out_param->num_out_channels;
    *samples = (int16) r[18];
    samples += out_param->num_out_channels;
    *samples = (int16) r[19];
    samples += out_param->num_out_channels;
    *samples = (int16) r[20];
    samples += out_param->num_out_channels;
    *samples = (int16) r[21];
    samples += out_param->num_out_channels;
    *samples = (int16) r[22];
    samples += out_param->num_out_channels;
    *samples = (int16) r[23];
    samples += out_param->num_out_channels;
    *samples = (int16) r[24];
    samples += out_param->num_out_channels;
    *samples = (int16) r[25];
    samples += out_param->num_out_channels;
    *samples = (int16) r[26];
    samples += out_param->num_out_channels;
    *samples = (int16) r[27];
    samples += out_param->num_out_channels;
    *samples = (int16) r[28];
    samples += out_param->num_out_channels;
    *samples = (int16) r[29];
    samples += out_param->num_out_channels;
    *samples = (int16) r[30];
    samples += out_param->num_out_channels;
    *samples = (int16) r[31];
    samples += out_param->num_out_channels;
#else
    OUT_SAMPLE(samples, r[0]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[1]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[2]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[3]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[4]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[5]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[6]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[7]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[8]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[9]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[10]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[11]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[12]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[13]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[14]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[15]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[16]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[17]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[18]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[19]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[20]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[21]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[22]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[23]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[24]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[25]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[26]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[27]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[28]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[29]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[30]);
    samples += out_param->num_out_channels;
    OUT_SAMPLE(samples, r[31]);
    samples += out_param->num_out_channels;
#endif /* not ROUND_OUTPUT */
}
