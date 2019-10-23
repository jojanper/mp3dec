/**************************************************************************
  huffman.cpp - Interface to layer III Huffman decoding subroutines.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

#include <stdio.h>

/*-- Project Headers. --*/
#include "layer.h"
#include "mstream.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

// Defined in hufdec.cpp.
void init_decode_codeword(void);

/*
 * Include Huffman tables.
 */
#include "huftbl.cpp"

/*
 * Include Huffman decoding functions.
 */
#include "hufdec.cpp"

/*
 * Include layer III decoding subroutines.
 */
#include "hufl3.cpp"

/**************************************************************************
  Title        : III_huffman_decode

  Purpose      : Interface for Layer III Huffman decoding.

  Usage        : III_huffman_decode(mp, gr, ch, part2)

  Input        : mp    - MP3 stream parameters
                 gr    - granule number (1 or 2)
                 ch    - channel number (left or right)
                 part2 - # of bits read from the start of the frame

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
III_huffman_decode(MP_Stream *mp, int gr, int ch, int part2)
{
    int16 *quant;
    Granule_Info *gr_info;
    int i, x, region1Start, region2Start, limit;

    // Quantized spectral samples.
    quant = mp->frame->ch_quant[ch];

    // Granule info for this frame.
    gr_info = mp->side_info->ch_info[ch]->gr_info[gr];

    // Find region boundaries.
    region1Start = mp->side_info->sfbData.sfbLong[gr_info->region0_count + 1];
    region2Start =
        mp->side_info->sfbData.sfbLong[gr_info->region0_count + gr_info->region1_count + 2];

    /*
     * How many samples actually need to be Huffman decoded.
     */
    limit = mp->side_info->sfbData.bandLimit;
    if (gr_info->big_values > limit)
        gr_info->big_values = limit;

    // Read bigvalues area.
    int section1 = MIN(gr_info->big_values, region1Start);
    if (section1 > 0) {
        pairtable(mp, section1, gr_info->table_select[0], quant);
        quant += section1;
    }

    int section2 = MIN(gr_info->big_values, region2Start) - region1Start;
    if (section2 > 0) {
        pairtable(mp, section2, gr_info->table_select[1], quant);
        quant += section2;
    }

    int section3 = gr_info->big_values - region2Start;
    if (section3 > 0) {
        pairtable(mp, section3, gr_info->table_select[2], quant);
        quant += section3;
    }

    i = gr_info->big_values;
    part2 += gr_info->part2_3_length;

    // Check whether the samples between -1,...,1 need to be Huffman decoded.
    if (i < limit) {
        // Read count1 area.
        int table_num = 32 + ((gr_info->flags & COUNT_1_TABLE_SELECT) ? 1 : 0);

        x = quadtable(mp, i, part2, table_num, quant, limit);
        quant += x - i;

        // TODO: This should not be needed for WebAssembly
#ifdef HAVE_WEBASSEMBLY9
        auto len = (x - i) / 2;
        auto remaining = (x - i) - len;
        memset(
            mp->frame->ch_quant[ch] + gr_info->big_values + remaining,
            0,
            (MAX_MONO_SAMPLES - gr_info->big_values - remaining) * sizeof(int16));
#endif

        i = x;
    }

    if (mp->br->bitsRead() > (uint32) part2) {
        quant -= 4;
        i -= 4;
        mp->br->rewindNbits(mp->br->bitsRead() - part2);
    }

    /* Dismiss stuffing Bits */
    if (mp->br->bitsRead() < (uint32) part2)
        mp->br->skipBits(part2 - mp->br->bitsRead());

    gr_info->zero_part_start = (i < limit) ? (i >= 0) ? i : 0 : limit;
}
