/**************************************************************************
  layer3.cpp  -  MPEG-1, MPEG-2 LSF and MPEG-2.5 layer III bitstream parsing
                 subroutines + some computationally light (no further
                 optimization needed ?) subroutines (like re-ordering and
                 anti-alias processing).

  Author(s): Juha Ojanpera
  Copyright (c) 1998-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "layer.h"
#include "mstream.h"

/**************************************************************************
  Title        : III_get_side_info

  Purpose      : Reads Layer III side information from the bitstream.

  Usage        : III_get_side_info(mp)

  Input        : mp - mp3 stream parameters

  Explanation  : This function supports both MPEG-1 and MPEG-2 LSF bitstreams.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_get_side_info(MP_Stream *mp)
{
    int i, j, flag;

    if (mp->header->version() == MPEG_AUDIO_ID) // MPEG-1
    {
        mp->side_info->main_data_begin = mp->bs->getBits(9);

        if (mp->header->channels() == 1) {
            mp->side_info->private_bits = mp->bs->getBits(5);
            mp->side_info->scfsi[0][0] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][1] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][2] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][3] = mp->bs->getBits(1);
        }
        else {
            mp->side_info->private_bits = mp->bs->getBits(3);
            mp->side_info->scfsi[0][0] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][1] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][2] = mp->bs->getBits(1);
            mp->side_info->scfsi[0][3] = mp->bs->getBits(1);
            mp->side_info->scfsi[1][0] = mp->bs->getBits(1);
            mp->side_info->scfsi[1][1] = mp->bs->getBits(1);
            mp->side_info->scfsi[1][2] = mp->bs->getBits(1);
            mp->side_info->scfsi[1][3] = mp->bs->getBits(1);
        }

        for (i = 0; i < 2; i++) {
            for (j = 0; j < mp->header->channels(); j++) {
                register Granule_Info *gr_info = mp->side_info->ch_info[j]->gr_info[i];

                gr_info->part2_3_length = mp->bs->getBits(12);
                gr_info->big_values = mp->bs->getBits(9) << 1;
                gr_info->global_gain = mp->bs->getBits(8);
                gr_info->scalefac_compress = mp->bs->getBits(4);

                gr_info->flags = 0;
                gr_info->flags |= (mp->bs->getBits(1)) ? WINDOW_SWITCHING_FLAG : 0;

                if (mp->win_switch(gr_info)) {
                    // block_type
                    gr_info->flags |= mp->bs->getBits(2);

                    // mixed_block_flag
                    flag = mp->bs->getBits(1);
                    gr_info->flags |= (flag) ? MIXED_BLOCK_FLAG : 0;

                    gr_info->table_select[0] = mp->bs->getBits(5);
                    gr_info->table_select[1] = mp->bs->getBits(5);

                    gr_info->subblock_gain[0] = mp->bs->getBits(3) << 3;
                    gr_info->subblock_gain[1] = mp->bs->getBits(3) << 3;
                    gr_info->subblock_gain[2] = mp->bs->getBits(3) << 3;

                    // Set region_count parameters since they are implicit in this case.
                    if (mp->short_block(gr_info) && !mp->mixed_block(gr_info))
                        gr_info->block_mode = SHORT_BLOCK_MODE;
                    else if (mp->short_block(gr_info) && mp->mixed_block(gr_info))
                        gr_info->block_mode = SHORT_BLOCK_MODE; // MIXED_BLOCK_MODE;
                    else
                        gr_info->block_mode = LONG_BLOCK_MODE;

                    gr_info->region0_count = 7;
                    gr_info->region1_count = 20 - gr_info->region0_count;
                }
                else {
                    gr_info->block_mode = LONG_BLOCK_MODE;
                    gr_info->table_select[0] = mp->bs->getBits(5);
                    gr_info->table_select[1] = mp->bs->getBits(5);
                    gr_info->table_select[2] = mp->bs->getBits(5);
                    gr_info->region0_count = mp->bs->getBits(4);
                    gr_info->region1_count = mp->bs->getBits(3);

                    gr_info->flags &= ~(uint32) 3; // block_type == 0 (LONG)
                }

                flag = mp->bs->getBits(3);
                gr_info->flags |= (flag & 4) ? PRE_FLAG : 0;
                gr_info->flags |= (flag & 2) ? SCALEFAC_SCALE : 0;
                gr_info->flags |= (flag & 1) ? COUNT_1_TABLE_SELECT : 0;
            }
        }
    }
    else // MPEG-2 LSF
    {
        mp->side_info->main_data_begin = mp->bs->getBits(8);

        if (mp->header->channels() == 1)
            mp->side_info->private_bits = mp->bs->getBits(1);
        else
            mp->side_info->private_bits = mp->bs->getBits(2);

        for (i = 0; i < mp->header->channels(); i++) {
            register Granule_Info *gr_info = mp->side_info->ch_info[i]->gr_info[0];

            gr_info->part2_3_length = mp->bs->getBits(12);
            gr_info->big_values = mp->bs->getBits(9) << 1;
            gr_info->global_gain = mp->bs->getBits(8);
            gr_info->scalefac_compress = mp->bs->getBits(9);
            gr_info->flags = 0;
            gr_info->flags |= (mp->bs->getBits(1)) ? WINDOW_SWITCHING_FLAG : 0;

            if (mp->win_switch(gr_info)) {
                // block_type
                gr_info->flags |= mp->bs->getBits(2);

                // mixed_block_flag
                flag = mp->bs->getBits(1);
                gr_info->flags |= (flag) ? MIXED_BLOCK_FLAG : 0;

                gr_info->table_select[0] = mp->bs->getBits(5);
                gr_info->table_select[1] = mp->bs->getBits(5);

                gr_info->subblock_gain[0] = mp->bs->getBits(3) << 3;
                gr_info->subblock_gain[1] = mp->bs->getBits(3) << 3;
                gr_info->subblock_gain[2] = mp->bs->getBits(3) << 3;

                // Set region_count parameters since they are implicit in this case.
                if (mp->short_block(gr_info) && !mp->mixed_block(gr_info)) {
                    gr_info->block_mode = SHORT_BLOCK_MODE;
                    gr_info->region0_count = 5;
                }
                else {
                    if (mp->short_block(gr_info) && mp->mixed_block(gr_info))
                        gr_info->block_mode = SHORT_BLOCK_MODE; // MIXED_BLOCK_MODE;
                    else
                        gr_info->block_mode = LONG_BLOCK_MODE;

                    gr_info->region0_count = 7;
                }

                gr_info->region1_count = 20 - gr_info->region0_count;
            }
            else {
                gr_info->block_mode = LONG_BLOCK_MODE;
                gr_info->table_select[0] = mp->bs->getBits(5);
                gr_info->table_select[1] = mp->bs->getBits(5);
                gr_info->table_select[2] = mp->bs->getBits(5);
                gr_info->region0_count = mp->bs->getBits(4);
                gr_info->region1_count = mp->bs->getBits(3);

                gr_info->flags &= ~(uint32) 3; // block_type == 0 (LONG)
            }

            flag = mp->bs->getBits(2);
            gr_info->flags |= (flag & 2) ? SCALEFAC_SCALE : 0;
            gr_info->flags |= (flag & 1) ? COUNT_1_TABLE_SELECT : 0;
        }
    }

    /* MS stereo. */
    mp->side_info->sfbData.ms_stereo = ((mp->header->mode() == MPG_MD_JOINT_STEREO) &&
                                        (mp->header->mode_extension() & 0x2));

    /* IS stereo. */
    mp->side_info->sfbData.is_stereo = ((mp->header->mode() == MPG_MD_JOINT_STEREO) &&
                                        (mp->header->mode_extension() & 0x1));
}

/*
   Purpose:     Buffer to hold the scalefactors.
   Explanation: This is only used for MPEG-2 LSF bit streams. */
static BYTE scalefac_buffer[54];

/*
   Purpose:     Some of the LSF scalefactors are obtained from table.
   Explanation: Modulo and division operations can be avoided when enabled. */
#define USE_LSF_TBL

#ifdef USE_LSF_TBL
static int slen1[25][4];
static int slen2[180][3];

static void
InitLSFScaleTable(void)
{
    for (int i = 0; i < 25; i++) {
        slen1[i][0] = i / 5;
        slen1[i][1] = i % 5;
        slen1[i][2] = i / 3;
        slen1[i][3] = i % 3;
    }

    for (int i = 0; i < 180; i++) {
        slen2[i][0] = i / 36;
        slen2[i][1] = (i % 36) / 6;
        slen2[i][2] = (i % 36) % 6;
    }
}
#endif /* USE_LSF_TBL */

/*
   Purpose:     Number of scalefactor bands for MPEG-2 LSF bit streams.
   Explanation: - */
static BYTE nr_of_sfb_block[6][3][4] = {
    { { 6, 5, 5, 5 }, { 9, 9, 9, 9 }, { 6, 9, 9, 9 } },
    { { 6, 5, 7, 3 }, { 9, 9, 12, 6 }, { 6, 9, 12, 6 } },
    { { 11, 10, 0, 0 }, { 18, 18, 0, 0 }, { 15, 18, 0, 0 } },
    { { 7, 7, 7, 0 }, { 12, 12, 12, 0 }, { 6, 15, 12, 0 } },
    { { 6, 6, 6, 3 }, { 12, 9, 9, 6 }, { 6, 12, 9, 6 } },
    { { 8, 8, 5, 0 }, { 15, 12, 9, 0 }, { 6, 18, 9, 0 } }
};

/**************************************************************************
  Title        : III_get_LSF_scale_data

  Purpose      : Decodes scalafactors of MPEG-2 LSF bitstreams.

  Usage        : III_get_LSF_scale_data(mp, gr, ch)

  Input        : mp - mp3 stream parameters
                 gr - granule number
                 ch - channel number (left or right)

  Explanation  : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
III_get_LSF_scale_data(MP_Stream *mp, int gr, int ch)
{
    int i, j, k, m;
    int blocktypenumber, blocknumber;
    int scalefac_comp, int_scalefac_comp, new_slen[4];
    Granule_Info *gr_info;

    // Granule info for this call.
    gr_info = mp->side_info->ch_info[ch]->gr_info[gr];

    scalefac_comp = gr_info->scalefac_compress;

    switch (gr_info->block_mode) {
        case MIXED_BLOCK_MODE:
            blocktypenumber = 2;
            break;

        case SHORT_BLOCK_MODE:
            blocktypenumber = 1;
            break;

        case LONG_BLOCK_MODE:
            blocktypenumber = 0;
            break;

        default:
            blocktypenumber = 0;
            break;
    }

    if (!((mp->header->mode_extension() == 1 || mp->header->mode_extension() == 3) &&
          ch == 1)) {
        if (scalefac_comp < 400) {
            int tmp = scalefac_comp >> 4;
#ifdef USE_LSF_TBL
            new_slen[0] = slen1[tmp][0];
            new_slen[1] = slen1[tmp][1];
#else
            new_slen[0] = tmp / 5;
            new_slen[1] = tmp % 5;
#endif
            new_slen[2] = (scalefac_comp & 15) >> 2;
            new_slen[3] = (scalefac_comp & 3);

            blocknumber = 0;
            m = 4;
        }
        else if (scalefac_comp < 500) {
            scalefac_comp -= 400;

            int tmp = scalefac_comp >> 2;
#ifdef USE_LSF_TBL
            new_slen[0] = slen1[tmp][0];
            new_slen[1] = slen1[tmp][1];
#else
            new_slen[0] = tmp / 5;
            new_slen[1] = tmp % 5;
#endif
            new_slen[2] = scalefac_comp & 3;

            blocknumber = 1;
            m = 3;
        }
        else // if(scalefac_comp < 512)
        {
            scalefac_comp -= 500;
#ifdef USE_LSF_TBL
            new_slen[0] = slen1[scalefac_comp][2];
            new_slen[1] = slen1[scalefac_comp][3];
#else
            new_slen[0] = scalefac_comp / 3;
            new_slen[1] = scalefac_comp % 3;
#endif
            gr_info->flags |= (uint32) PRE_FLAG; // pre_flag = 1

            blocknumber = 2;
            m = 2;
        }
    }

    if (((mp->header->mode_extension() == 1 || mp->header->mode_extension() == 3) &&
         ch == 1)) {
        int_scalefac_comp = scalefac_comp >> 1;

        if (int_scalefac_comp < 180) {
#ifdef USE_LSF_TBL
            new_slen[0] = slen2[int_scalefac_comp][0];
            new_slen[1] = slen2[int_scalefac_comp][1];
            new_slen[2] = slen2[int_scalefac_comp][2];
#else
            new_slen[0] = int_scalefac_comp / 36;
            new_slen[1] = (int_scalefac_comp % 36) / 6;
            new_slen[2] = (int_scalefac_comp % 36) % 6;
#endif
            blocknumber = 3;
            m = 3;
        }
        else if (int_scalefac_comp < 244) {
            int_scalefac_comp -= 180;

            new_slen[0] = (int_scalefac_comp & 63) >> 4;
            new_slen[1] = (int_scalefac_comp & 15) >> 2;
            new_slen[2] = int_scalefac_comp & 3;

            blocknumber = 4;
            m = 3;
        }
        else // if(int_scalefac_comp < 255)
        {
            int_scalefac_comp -= 244;
#ifdef USE_LSF_TBL
            new_slen[0] = slen1[int_scalefac_comp][2];
            new_slen[1] = slen1[int_scalefac_comp][3];
#else
            new_slen[0] = int_scalefac_comp / 3;
            new_slen[1] = int_scalefac_comp % 3;
#endif
            new_slen[2] = 0;
            blocknumber = 5;
            m = 2;
        }

        IS_Info *is_info = &mp->side_info->is_info;
        is_info->is_len[0] = (1 << new_slen[0]) - 1;
        is_info->is_len[1] = (1 << new_slen[1]) - 1;
        is_info->is_len[2] = (1 << new_slen[2]) - 1;
        is_info->nr_sfb[0] = nr_of_sfb_block[blocknumber][blocktypenumber][0];
        is_info->nr_sfb[1] = nr_of_sfb_block[blocknumber][blocktypenumber][1];
        is_info->nr_sfb[2] = nr_of_sfb_block[blocknumber][blocktypenumber][2];
    }

    memset(scalefac_buffer, 0, 54);

    for (i = k = 0; i < m; i++) {
        if (new_slen[i] != 0) {
            for (j = 0; j < nr_of_sfb_block[blocknumber][blocktypenumber][i]; j++, k++)
                scalefac_buffer[k] = mp->br->getBits(new_slen[i]);
        }
        else
            k += nr_of_sfb_block[blocknumber][blocktypenumber][i];
    }
}

/*
   Purpose:     Grouping of the scalefactors of long and short blocks.
   Explanation: - */
struct
{
    int l[5];
    int s[3];

} sfbtable = { { 6, 5, 5, 5, 0 }, { 6, 6, 0 } };
// sfbtable = {{0, 6, 11, 16, 21}, {0, 6, 12} };

/*
   Purpose:     Number of bits used for the scalefactors.
   Explanation: - */
static int slen[2][16] = { { 0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 },
                           { 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3 } };

/**************************************************************************
  Title        : III_get_scale_factors

  Purpose      : Reads the scale factors of layer III.

  Usage        : III_get_scale_factors(mp, gr, ch)

  Input        : mp - mp3 stream parameters
                 gr - granule number
                 ch - channel number (left or right)

  Explanation  : This function supports both MPEG-1 and MPEG-2 LSF bit
                 streams.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_get_scale_factors(MP_Stream *mp, int gr, int ch)
{
    BYTE *sf[3];
    int i, sfb, bits, idx;
    Granule_Info *gr_info;
    III_Scale_Factors *scale_fac;

    gr_info = mp->side_info->ch_info[ch]->gr_info[gr];

    scale_fac = mp->side_info->ch_info[ch]->scale_fac;

    idx = 0;

    switch (gr_info->block_mode) {
        case MIXED_BLOCK_MODE:
            scale_fac = mp->side_info->ch_info[ch]->scale_fac;

            /*-- The first two subbands use long blocks. --*/
            idx = mp->side_info->sfbData.nsfb_mixed;

            bits = slen[0][gr_info->scalefac_compress];
            if (bits) {
                sf[0] = scale_fac->scalefac_short[0];
                for (sfb = 0; sfb < idx; sfb++)
                    *sf[0]++ = mp->br->getBits(bits);
            }
            else
                memset(scale_fac->scalefac_long, 0, idx);

            /*-- The rest of the subbands use short blocks. --*/
            sf[0] = scale_fac->scalefac_short[0];
            sf[1] = scale_fac->scalefac_short[1];
            sf[2] = scale_fac->scalefac_short[2];

            memset(sf[0], 0, 12);
            memset(sf[1], 0, 12);
            memset(sf[2], 0, 12);

            if (bits)
                for (sfb = 3; sfb < 6; sfb++) {
                    *sf[0]++ = mp->br->getBits(bits);
                    *sf[1]++ = mp->br->getBits(bits);
                    *sf[2]++ = mp->br->getBits(bits);
                }
            else {
                sf[0] += 3;
                sf[1] += 3;
                sf[2] += 3;
            }

            bits = slen[1][gr_info->scalefac_compress];
            if (bits)
                for (; sfb < 12; sfb++) {
                    *sf[0]++ = mp->br->getBits(bits);
                    *sf[1]++ = mp->br->getBits(bits);
                    *sf[2]++ = mp->br->getBits(bits);
                }
            break;

        case SHORT_BLOCK_MODE:
            sf[0] = scale_fac->scalefac_short[0];
            sf[1] = scale_fac->scalefac_short[1];
            sf[2] = scale_fac->scalefac_short[2];
            memset(sf[0], 0, 12);
            memset(sf[1], 0, 12);
            memset(sf[2], 0, 12);

            for (i = 0; i < 2; ++i) {
                bits = slen[i][gr_info->scalefac_compress];
                if (bits)
                    for (sfb = 0; sfb < sfbtable.s[i]; ++sfb) {
                        *sf[0]++ = mp->br->getBits(bits);
                        *sf[1]++ = mp->br->getBits(bits);
                        *sf[2]++ = mp->br->getBits(bits);
                    }
                else {
                    sf[0] += sfbtable.s[i];
                    sf[1] += sfbtable.s[i];
                    sf[2] += sfbtable.s[i];
                }
            }
            break;

        case LONG_BLOCK_MODE:
            sf[0] = scale_fac->scalefac_long;
            for (i = 0; i < 4; ++i) {
                if (mp->side_info->scfsi[ch][i] == 0 || gr == 0) {
                    bits = slen[(uint32) i >> 1][gr_info->scalefac_compress];
                    if (bits)
                        for (sfb = 0; sfb < sfbtable.l[i]; ++sfb)
                            *sf[0]++ = mp->br->getBits(bits);
                    else {
                        memset(sf[0], 0, sfbtable.l[i]);
                        sf[0] += sfbtable.l[i];
                    }
                }
                else
                    sf[0] += sfbtable.l[i];
            }
            break;

        default:
            break;
    }
}


static void
III_get_scale_factorsLSF(MP_Stream *mp, int gr, int ch)
{
    BYTE *sf[3];
    int sfb, idx;
    Granule_Info *gr_info;
    III_Scale_Factors *scale_fac;

    idx = 0;
    gr_info = mp->side_info->ch_info[ch]->gr_info[gr];
    scale_fac = mp->side_info->ch_info[ch]->scale_fac;

    III_get_LSF_scale_data(mp, gr, ch);

    switch (gr_info->block_mode) {
        case MIXED_BLOCK_MODE:
            scale_fac = mp->side_info->ch_info[ch]->scale_fac;

            /*-- The first two subbands use long blocks. --*/
            idx = mp->side_info->sfbData.nsfb_mixed;
            memcpy(scale_fac->scalefac_long, scalefac_buffer, idx);

            /*-- The rest of the subbands use short blocks. --*/
            sf[0] = scale_fac->scalefac_short[0];
            sf[1] = scale_fac->scalefac_short[1];
            sf[2] = scale_fac->scalefac_short[2];

            for (sfb = 3; sfb < 6; sfb++) {
                *sf[0]++ = scalefac_buffer[idx++];
                *sf[1]++ = scalefac_buffer[idx++];
                *sf[2]++ = scalefac_buffer[idx++];
            }
            for (; sfb < 12; sfb++) {
                *sf[0]++ = scalefac_buffer[idx++];
                *sf[1]++ = scalefac_buffer[idx++];
                *sf[2]++ = scalefac_buffer[idx++];
            }
            break;

        case SHORT_BLOCK_MODE:
            sf[0] = scale_fac->scalefac_short[0];
            sf[1] = scale_fac->scalefac_short[1];
            sf[2] = scale_fac->scalefac_short[2];
            for (sfb = 0; sfb < 12; sfb++) {
                *sf[0]++ = scalefac_buffer[idx++];
                *sf[1]++ = scalefac_buffer[idx++];
                *sf[2]++ = scalefac_buffer[idx++];
            }
            break;

        case LONG_BLOCK_MODE:
            memcpy((void *) scale_fac->scalefac_long, (void *) scalefac_buffer, 21);
            break;

        default:
            break;
    }
}

/*
 * Export the scalefactor parsing routines.
 */
III_SCALEFAC ScaleFunc[2] = { III_get_scale_factors, III_get_scale_factorsLSF };

/*
   Purpose:     Indices for reordering the short blocks.
   Explanation: First row is the destination and second row is the source
                index. */
static int16 reorder_idx[2][MAX_MONO_SAMPLES];

/**************************************************************************
  Title        : init_III_reorder

  Purpose      : Initializes 'reorder_idx' table.

  Usage        : init_III_reorder(sfb_table, sfb_width_table)

  Input        : sfb_table       - scalefactor band boundaries for short blocks
                 sfb_width_table - # of samples within each sfb

  Explanation  : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
init_III_reorder(int16 *sfb_table, int16 *sfb_width_table)
{
    for (int i = 0, sfb = 0; sfb < MAX_SHORT_SFB_BANDS; sfb++) {
        int sfb_start = sfb_table[sfb];

        for (int window = 0; window < 3; window++)
            for (int freq = 0; freq < sfb_width_table[sfb]; freq++) {
                int src_line = sfb_start * 3 + window * sfb_width_table[sfb] + freq;
                int des_line = (sfb_start * 3) + window + (freq * 3);
                reorder_idx[0][i] =
                    ((des_line / SSLIMIT) * SSLIMIT) + (des_line % SSLIMIT);
                reorder_idx[1][i++] =
                    ((src_line / SSLIMIT) * SSLIMIT) + (src_line % SSLIMIT);
            }
    }

#ifdef USE_LSF_TBL
    InitLSFScaleTable();
#endif
}


/**************************************************************************
  Title        : III_reorder

  Purpose      : Re-orders the input frame if short blocks are present.

  Usage        : III_reorder(mp, gr, ch)

  Input        : mp - mp3 stream parameters
                 ch - channel number (left or right)
                 gr - granule number

  Explanation  : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_reorder(MP_Stream *mp, int ch, int gr)
{
    int16 i, sb_start;

    if (mp->side_info->ch_info[ch]->gr_info[gr]->block_mode == LONG_BLOCK_MODE)
        return;

    sb_start = 0;
    if (mp->side_info->ch_info[ch]->gr_info[gr]->block_mode == MIXED_BLOCK_MODE)
        sb_start = 36; // no re-order for the 1st two subbands

    register int16 *id1, *id2;
    FLOAT xr[MAX_MONO_SAMPLES];
    register FLOAT *src, *dst;

    id1 = &reorder_idx[0][sb_start];
    id2 = &reorder_idx[1][sb_start];

    /*
     * First re-order the short block into a temporary buffer
     * and then copy back to the input buffer. Not fully optimal,
     * a better way would be perhaps to do the re-ordering during the
     * dequantization but to my opinion that would complicate the code
     * too much. We also have to remember that short blocks do not occur
     * very frequently, so the penalty of having a separate re-ordering
     * routine is not so time consuming from the overall decoder complexity
     * point of view.
     */
    src = mp->buffer->ch_reconstructed[ch];
    dst = xr;
    for (i = sb_start; i < MAX_MONO_SAMPLES; i++)
        dst[*id1++] = src[*id2++];

        /*-- Copy back. --*/
#if 0
  for(i = sb_start; i < MAX_MONO_SAMPLES; i++)
    src[i] = dst[i];
#else
    memmove(src + sb_start, dst + sb_start, (MAX_MONO_SAMPLES - sb_start) << 2);
    // memmove(src + sb_start, dst + sb_start, sizeof(FLOAT) * (MAX_MONO_SAMPLES -
    // sb_start));
#endif
}
