/**************************************************************************
  synfilt3.cpp - Half rate implementations of the synthesis filterbank.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*
 * Decimated (2) version of 32-point forward DCT.
 */
#ifdef DCT_CHEN
static void inline DctChen_32_sub2(FLOAT *x, FLOAT *y)
{
    FLOAT tmp[SBLIMIT], rec_samples[SBLIMIT];
    FLOAT x0, x1, x2, x3, x4, x5, x6, x7;
    FLOAT *a0, *a1, *a2;

    // Stage 1.
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

    // Stage 2, upper part.
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

    // Stage 3, upper, upper part.
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
}
#endif /* CHEN_DCT */


static void
Window_Half(FLOAT *synthesis_buffer, int16 *out_samples, int *buf_idx, Out_Param *out_param)
{
    FLOAT r[SBLIMIT >> 1];
    const int16 *buf_idx0;

    /*
     * Perform windowing. Truncate the synthesis window length.
     * Keep only the windows coefficients W[64],...,W[447] (default).
     */
    buf_idx0 = &buf_idx_offset[*buf_idx][out_param->window_pruning_idx];
    FLOAT *w = dewindow + out_param->window_offset;

    memset(r, 0, (SBLIMIT >> 1) * sizeof(FLOAT));

    for (int i = 0; i < out_param->num_subwindows; i++) {
        FLOAT *u, *v;

        u = r;
        v = &synthesis_buffer[*buf_idx0++];

#if 0
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
    *u++ += (*v++) * (*w++); v++; w++; *u++ += (*v++) * (*w++); v++; w++;
#else
        *u++ += v[0] * w[0];
        *u++ += v[2] * w[2];
        *u++ += v[4] * w[4];
        *u++ += v[6] * w[6];
        *u++ += v[8] * w[8];
        *u++ += v[10] * w[10];
        *u++ += v[12] * w[12];
        *u++ += v[14] * w[14];
        *u++ += v[16] * w[16];
        *u++ += v[18] * w[18];
        *u++ += v[20] * w[20];
        *u++ += v[22] * w[22];
        *u++ += v[24] * w[24];
        *u++ += v[26] * w[26];
        *u++ += v[28] * w[28];
        *u++ += v[30] * w[30];
        w += SBLIMIT;
#endif
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
#endif /* not ROUND_OUTPUT */
}
