#ifndef HUFINFO_H_
#define HUFINFO_H_

#include "core/defines.h"

/*
  Purpose:      Structure defining Huffman codeword values.
  Explanation:  - */
typedef struct Huffman_CodeStr
{
    int16 huf_vector; /* Huffman vector.                 */
    int16 codelen;    /* Length of the Huffman codeword. */
    int32 codeword;   /* Huffman codeword of the vector. */

} Huffman_Code;

/*
  Purpose:      Structure defining Huffman codebook parameters.
  Explanation:  - */
typedef struct Huffman_InfoStr
{
    int huffman_tbl_nr; /* Huffman table number (AAC).               */
    int scale[4];       /* Scalefactors for Huffman vector unpacking.*/
    int escape_cb;      /* Escape codebook flag.                     */
    int sign;           /* Signed(1) / unsigned(0) codebook.         */
    int dim;            /* Dimension of the Huffman vector.          */
    int max_coef;       /* Maximum value of the vector elements.     */
    int max_elements;   /* Number of different vector elements.      */
    int num_of_vectors; /* Size of the codebook.                     */
    int offset;         /* Offset to produce biased output.          */
    Huffman_Code *huf;  /* Codeword parameters for this codebook.    */
    int *unPackIdx;     /* Unpacked Huffman index values.            */

} Huffman_Info;

/*
  Purpose:      Structure defining Huffman coding parameters.
  Explanation:  - */
typedef struct HuffmanStr
{
    int num_of_cb;      /* Number of Huffman codebooks. */
    Huffman_Info **huf; /* Huffman codebook parameters. */

} AAC_Huffman;

/*
   Purpose:     Parent Structure for Huffman decoding.
   Explanation: This structure can be used if dynamic memory allocation
                is not available or if memory consumption is important.
                At worst we have to process the number of codewords equal to
                the size of the codebook. */
typedef struct Huffman_Str
{
    int tree_len;          /* Size of the Huffman tree.                      */
    int linbits;           /* Number of extra bits.                          */
    int16 *codeword;       /* Huffman codewords.                             */
    int16 *packed_symbols; /* x, y and length of the corresponding codeword. */

} MP3_Huffman;

#endif /* HUFINFO_H_ */
