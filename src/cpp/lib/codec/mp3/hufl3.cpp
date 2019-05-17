/**************************************************************************
  hufl3.cpp - Decodes spectral samples for mp3 decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>


/**************************************************************************
  Title        : pairtable

  Purpose      : Decodes samples that are coded using 2-tuple Huffman tables.

  Usage        : pairtable(mp, section_length, table_num, quant)

  Input        : mp             - MP3 stream parameters
                 section_length - # of samples to be decoded
                 table_num      - Huffman table number

  Output       : quant          - quantized spectral components

  Explanation  : -

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
pairtable(MP_Stream *mp, int section_length, int table_num, int16 *quant)
{
    MP3_Huffman *h = &mp->huffman[table_num];

    if (h->tree_len == 0)
        memset(quant, 0, section_length << 1);
    else {
        tbl_idx = -1;
        int16 *q = quant;
        if (h->linbits) {
            for (int i = 0; i < section_length; i += 2, q += 2) {
#ifndef HUFFMAN_TREE_DECODER
                uint32 qSamples = decode_codeword(mp->br, h);
#else
                uint32 qSamples = treebased_codeword0(mp->br, mp->huftree[table_num]);
#endif /* not HUFFMAN_TREE_DECODER */

                // Unpack coefficients.
                *q = qSamples >> 4;
                q[1] = qSamples & 15;

                // Read extra bits (if needed) and sign bits (if needed).
                if (h->linbits) {
                    if (*q == 15) {
                        tbl_idx -= h->linbits;
                        *q += mp->br->getBits(h->linbits);
                    }
                    if (*q) {
                        tbl_idx--;
                        *q = (mp->br->getBits(1)) ? -*q : *q;
                    }

                    if (q[1] == 15) {
                        tbl_idx -= h->linbits;
                        q[1] += mp->br->getBits(h->linbits);
                    }
                    if (q[1]) {
                        tbl_idx--;
                        q[1] = (mp->br->getBits(1)) ? -q[1] : q[1];
                    }
                }
                else {
                    if (*q) {
                        tbl_idx--;
                        *q = (mp->br->getBits(1)) ? -*q : *q;
                    }
                    if (q[1]) {
                        tbl_idx--;
                        q[1] = (mp->br->getBits(1)) ? -q[1] : q[1];
                    }
                }
            }
        }
        else // no linbits
        {
            for (int i = 0; i < section_length; i += 2, q += 2) {
#ifndef HUFFMAN_TREE_DECODER
                uint32 qSamples = decode_codeword(mp->br, h);
#else
                uint32 qSamples = treebased_codeword0(mp->br, mp->huftree[table_num]);
#endif /* not HUFFMAN_TREE_DECODER */

                // Unpack coefficients.
                *q = qSamples >> 4;
                q[1] = qSamples & 15;

                // Read extra bits (not needed) and sign bits (if needed)
                if (*q) {
                    tbl_idx--;
                    *q = (mp->br->getBits(1)) ? -*q : *q;
                }
                if (q[1]) {
                    tbl_idx--;
                    q[1] = (mp->br->getBits(1)) ? -q[1] : q[1];
                }
            }
        }
    }
}


/**************************************************************************
  Title        : quadtable

  Purpose      : Decodes samples that are coded using 4-tuple Huffman tables.

  Usage        : y = quadtable(mp, start, part2, table_num, quant, max_sfb_bins)

  Input        : mp           - MP3 stream parameters
                 start        - offset to start of spectral samples
                 part2        - # of bits read from the start of the frame
                 table_num    - Huffman table number
                 max_sfb_bins - max # of spectral bins to be decoded

  Output       : y            - spectral bin # of last decoded sample
                 quant        - quantized spectral components

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static int
quadtable(MP_Stream *mp, int start, int part2, int table_num, int16 *quant, int max_sfb_bins)
{
    int i;
    int16 *q = quant;
#ifndef HUFFMAN_TREE_DECODER
    MP3_Huffman *h = &mp->huffman[table_num];
#endif

    tbl_idx = -1;
    for (i = start; (mp->br->bitsRead() < (uint32) part2 && i < max_sfb_bins);
         i += 4, q += 4) {
        uint32 qSamples;

        if (table_num == 33) {
            tbl_idx -= 4;
            qSamples = 15 - mp->br->getBits(4);
        }
        else
#ifndef HUFFMAN_TREE_DECODER
            qSamples = decode_codeword(mp->br, h);
#else
            qSamples = treebased_codeword0(mp->br, mp->huftree[table_num]);
#endif /* not HUFFMAN_TREE_DECODER */

        // Unpack coefficients.
        *q = (qSamples >> 3) & 1;
        q[1] = (qSamples >> 2) & 1;
        q[2] = (qSamples >> 1) & 1;
        q[3] = qSamples & 1;

        if (tbl_idx < 3) {
            codeword = mp->br->lookAhead(32);
            tbl_idx = 31;
        }

        // Sign bits.
        int bits_read = 0, sbits = codeword >> (tbl_idx - 3);
        if (*q) {
            bits_read++;
            *q = (sbits & 8) ? -*q : *q;
        }
        if (q[1]) {
            bits_read++;
            q[1] = (sbits & 4) ? -q[1] : q[1];
        }
        if (q[2]) {
            bits_read++;
            q[2] = (sbits & 2) ? -q[2] : q[2];
        }
        if (q[3]) {
            bits_read++;
            q[3] = (sbits & 1) ? -q[3] : q[3];
        }

        if (bits_read) {
            tbl_idx -= bits_read;
            mp->br->skipBits(bits_read);
        }
    }

    return (i);
}
