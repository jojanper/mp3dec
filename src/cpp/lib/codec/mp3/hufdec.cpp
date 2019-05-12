/**************************************************************************
  hufdec.cpp - Huffman decoding subroutines for mp3 decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifdef HUFFMAN_TREE_DECODER
/*
   Purpose:     Table for detecting whether a bit in a certain location
                is one or zero.
   Explanation: - */
static uint32 bit_detect[32] = {
    1,        2,        4,        8,         16,        32,        64,         128,
    256,      512,      1024,     2048,      4096,      8192,      16384,      32768,
    65536,    131072,   262144,   524288,    1048576,   2097152,   4194304,    8388608,
    16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648
};

/**************************************************************************
  Title        : treebased_codeword

  Purpose      : Decodes one Huffman codeword using the Huffman tree.

  Usage        : y = treebased_codeword(br, hNode)

  Input        : br   - layer III bit reservoir
                 Node - Huffman tree

  Explanation  : y - Huffman symbol

  Author(s)    : Juha Ojanpera
  *************************************************************************/

#if 0
static inline int
treebased_codeword(Bit_Reservoir *br, HufNode *hNode)
{
  int bits_read, tbl_idx = 19;
  uint32 *bit_tbl = &bit_detect[18];
  uint32 codeword;
  HufNode *pNode;

  /*-- Read the maximum codeword. --*/
  codeword = br->look_ahead(19);
  bits_read = 0;
  pNode = hNode;

  while(pNode->depth == -1)
  {
    bits_read++;
    /*
     * Zero bit means that we proceed to the left child node, otherwise we
     * proceed to the right child node.
     */
    pNode = (!(codeword & *bit_tbl--)) ? pNode->pLeft : pNode->pRight;
  }

  /*-- # of bits actually read. --*/
  br->skipbits(bits_read);

  /*-- Decoded Huffman symbol. --*/
  return (pNode->symbol >> 5);
}
#endif

static int tbl_idx = -1;
static uint32 codeword = 0;

static inline int
treebased_codeword0(BitBuffer *br, HufNode *hTree)
{
    int bits_read = 0;
    HufNode *pNode = hTree;

    /*
     * Traverse the Huffman tree until an empty child node has been found.
     */
    while (pNode->depth == -1) {
        if (tbl_idx < 0) {
            if (bits_read > 0)
                br->skipBits(bits_read);
            codeword = br->lookAhead(32);
            bits_read = 0;
            tbl_idx = 31;
        }

        bits_read++;
        /*
         * Zero bit means that we proceed to the left child node,
         * otherwise right child node is selected.
         */
        pNode = (!(codeword & bit_detect[tbl_idx--])) ? pNode->pLeft : pNode->pRight;
    }

    /*-- # of bits actually read. --*/
    br->skipBits(bits_read);

    /*-- Decoded Huffman symbol. --*/
    return (pNode->symbol >> 5);
}
#else
#define HUFFMAN_TBL_OPTIMIZED
#ifdef HUFFAN_TBL_OPTIMIZED
/*
   Purpose:     Matrix to turn substraction into table fetch.
   Explanation: (see below). */
static int16 sub_tbl[20][20];

/**************************************************************************
  Title        : init_decode_codeword

  Purpose      : Initializes 'sub_tbl' matrix.

  Usage        : init_decode_codeword()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static void
init_decode_codeword(void)
{
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 20; j++)
            sub_tbl[i][j] = i - j;
}

#endif /* HUFFMAN_TBL_OPTIMIZED */

/**************************************************************************
  Title        : decode_codeword

  Purpose      : Decodes one Huffman codeword from the bit stream and returns
                 the corresponding symbol.

  Usage        : decode_codeword(br, h)

  Input        : br - layer III bit reservoir
                 h  - Huffman table

  Explanation  : This decoding routine works only if the codewords are
                 sorted according to their length i.e the codeword that has
                 the minimum length appears first in the Huffman table, the
                 length of the second codeword in the table is either greater
                 or equal than the first one and so on.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static int
decode_codeword(BitBuffer *br, MP3_Huffman *h)
{
    int clen;
#ifdef HUFFMAN_TBL_OPTIMIZED
    int shift, N = 19; // N = 19 maximum codeword length
    uint32 cand_codeword;
#endif /* HUFFMAN_TBL_OPTIMIZED */
    uint32 codeword;
    int16 *h_len = h->packed_symbols;
    int16 *h_codeword = h->codeword;

#ifdef HUFFMAN_TBL_OPTIMIZED
    // Read the maximum codeword.
    cand_codeword = br->look_ahead(N);

    /*
     * Codeword length of the codeword that has the
     * highest probability.
     */
#if 0
  clen = *h_len & 31;
#endif
    shift = sub_tbl[N][*h_len & 31]; // N - clen;

    // Candidate codeword for the first codebook entry.
    codeword = cand_codeword >> shift;

    /*
     * Sequentially goes through the whole Huffman tree
     * until a match has been found.
     */
    while (codeword != *h_codeword) {
        h_len++;
        h_codeword++;

        // Increment count of codeword length.
        clen = *h_len & 31;
        if (clen) {
            shift = sub_tbl[shift][clen]; // shift -= clen;

            // New candidate codeword.
            codeword = cand_codeword >> shift;
        }
    }

    // # of bits actually read.
    br->skipBits(sub_tbl[N][shift]); // N - shift

#else
    clen = *h_len & 31;
    codeword = br->getBits(clen);

    while (codeword != *h_codeword) {
        h_len++;
        h_codeword++;

        clen = *h_len & 31;
        if (clen) {
            codeword <<= clen;
            codeword |= br->getBits(clen);
        }
    }
#endif /* HUFFMAN_TBL_OPTIMIZED */

    // Decoded Huffman symbol.
    return (*h_len >> 5);
}

#endif /* HUFFMAN_TREE_DECODER */
