/**************************************************************************
  synfilt4.cpp - Quad rate implementations of the synthesis filterbank.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*
 * Decimated (4) version of 32-point forward DCT.
 */
#ifdef DCT_CHEN
static void inline DctChen_32_sub4(FLOAT *x, FLOAT *y)
{
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
    y[48] = x0 + x1;                    // 0
    y[0] = y[32] = x0 - x1;             // 16
    y[40] = y[56] = tan1_8 * x2 + x3;   // 8
    y[8] = y[24] = cot3_8 * x3 - x2;    // 24
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
    y[44] = y[52] = tan1_16 * x4 + x7;  // 4
    y[4] = y[28] = x5 + x6 * cot5_16;   // 20
    y[36] = y[60] = -x5 * tan3_16 + x6; // 12
    y[20] = y[12] = cot7_16 * x7 - x4;  // 28
#endif /* not SCALED_DCT */
}
#endif /* CHEN_DCT */


static void
Window_Quad(FLOAT *synthesis_buffer, int16 *out_samples, int *buf_idx, Out_Param *out_param)
{
    static FLOAT r[SBLIMIT >> 2];
    int16 *buf_idx0;

    /*
     * Perform windowing. Truncate the synthesis window length.
     * Keep only the windows coefficients W[64],...,W[447] (default).
     */
    buf_idx0 = &buf_idx_offset[*buf_idx][out_param->window_pruning_idx];
    FLOAT *w = dewindow + out_param->window_offset;

    memset(r, 0, (SBLIMIT >> 2) * sizeof(FLOAT));

    for (int i = 0; i < out_param->num_subwindows; i++) {
        FLOAT *u, *v;

        u = r;
        v = &synthesis_buffer[*buf_idx0++];

#if 1
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        v += 3;
        w += 3;
        *u++ += (*v++) * (*w++);
        w += 3;
#else
        *u++ += v[0] * w[0];
        *u++ += v[4] * w[4];
        *u++ += v[8] * w[8];
        *u++ += v[12] * w[12];
        *u++ += v[16] * w[16];
        *u++ += v[20] * w[20];
        *u++ += v[24] * w[24];
        *u++ += v[28] * w[28];
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
#endif /* not ROUND_OUTPUT */
}
