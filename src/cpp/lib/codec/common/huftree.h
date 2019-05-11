/**************************************************************************
  huftree.h - Huffman tree declarations.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-1999 Juha Ojanpera.
  *************************************************************************/

#ifndef HUFTREE_H_
#define HUFTREE_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "hufinfo.h"

/*
   Purpose:     Parent Structure for Huffman decoding which utilizes the
                properties of Huffman tree.
   Explanation: The fastest known method of Huffman decoding is to construct
                the Huffman tree and to utilize that for codeword decoding.
                At worst we have to process the number of codewords equal to
                the height of the tree. */
typedef struct HufNodeStr
{
    int32 symbol; // symbol for this node, -1 if not present
    int16 depth;  // length of the codeword, -1 otherwise (intermediate node)

    struct HufNodeStr *pLeft;
    struct HufNodeStr *pRight;
    struct HufNodeStr *pParent;

} HufNode;

HufNode *CreateMP3HuffmanTree(MP3_Huffman *huf_codes);
HufNode *CreateAACHuffmanTree(Huffman_Info *huf_info);
HufNode *DeleteTree(HufNode *Root);

#endif /* HUFTREE_H_ */
