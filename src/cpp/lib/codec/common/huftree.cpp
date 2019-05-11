/**************************************************************************
  huftree.cpp - Huffman tree implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- System Headers. --*/
#include <stdlib.h>

/*-- Project Headers. --*/
#include "huftree.h"

/*************************************************************************
  Internal Objects
  *************************************************************************/

static HufNode *CreateNode(int16 symbol, int16 depth);

static HufNode *RemoveNode(HufNode *pNode);

static void AddChild(HufNode *pParent, HufNode *pLeft, HufNode *pRight);

static BOOL PutCodeword(HufNode *Root, int32 codeword, int16 symbol, int len);

/*
   Purpose:     Table for detecting whether bit in a certain location
                is one or zero.
   Explanation: - */
static int32 bit_detect[] = { 1,     2,     4,     8,      16,     32,    64,
                              128,   256,   512,   1024,   2048,   4096,  8192,
                              16384, 32768, 65536, 131072, 262144, 524288 };


/**************************************************************************
  Title       : CreateMP3HuffmanTree

  Purpose     : Creates Huffman tree from the mp3 Huffman codebook parameters.

  Usage       : y = CreateMP3HuffmanTree(huf_codes)

  Input       : huf_codes - mp3 Huffman codebook parameters

  Output      : y - Huffman tree for mp3 decoder

  Author(s)   : Juha Ojanpera
  *************************************************************************/

HufNode *
CreateMP3HuffmanTree(MP3_Huffman *huf_codes)
{
    int i, code_len;
    HufNode *pNode = NULL;

    // Parent node.
    pNode = CreateNode(-1, -1);

    for (code_len = i = 0; i < huf_codes->tree_len; i++) {
        // The length of each symbol is stored differentially
        code_len += huf_codes->packed_symbols[i] & 31;

        // Put the codeword into the tree.
        if (PutCodeword(pNode, huf_codes->codeword[i], huf_codes->packed_symbols[i],
                        code_len) == FALSE) {
            DeleteTree(pNode);
            return (NULL);
        }
    }

    return (pNode);
}


/**************************************************************************
  Title       : CreateAACHuffmanTree

  Purpose     : Creates Huffman tree from the AAC Huffman codebook parameters.

  Usage       : y = CreateAACHuffmanTree(huf_info)

  Input       : huf_info - AAC Huffman codebook parameters

  Output      : y - Huffman tree for AAC decoder

  Author(s)   : Juha Ojanpera
  *************************************************************************/

HufNode *
CreateAACHuffmanTree(Huffman_Info *huf_info)
{
    int i, code_len;
    HufNode *pNode = NULL;
    Huffman_Code *huf_code = huf_info->huf;

    // Parent node.
    pNode = CreateNode(-1, -1);

    for (code_len = i = 0; i < huf_info->num_of_vectors; i++) {
        // The length of each symbol is stored differentially
        code_len += huf_code[i].codelen;

        // Put the codeword into the tree.
        if (PutCodeword(pNode, huf_code[i].codeword, huf_code[i].huf_vector, code_len) ==
            FALSE) {
            DeleteTree(pNode);
            return (NULL);
        }
    }

    return (pNode);
}


/**************************************************************************
  Title       : PutCodeword

  Purpose     : Stores one Huffman codeword into the Huffman tree.

  Usage       : y = PutCodeword(memChnk, Root, codeword, symbol, len)

  Input       : memChnk  - memory allocation class
                Root     - root node of Huffman tree
                codeword - Huffman codeword
                symbol   - symbol corresponding the codeword
                len      - length of the codeword

  Output      : y - TRUE on success, FALSE otherwise

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
PutCodeword(HufNode *Root, int32 codeword, int16 symbol, int len)
{
    int i, bit_set;
    HufNode *pNode;

    pNode = Root;

    /*
     * First traverse the tree down and create intermediate nodes if needed.
     */
    for (i = 0; i < len - 1; i++) {
        bit_set = codeword & bit_detect[len - 1 - i];

        /*
         * Zero bit means that we proceed to the left node, otherwise
         * right node is chosen. Could be vice versa as long as this
         * rule is followed during the construction of the Huffman trees.
         */
        if (!bit_set) {
            // Create an intermediate left node.
            if (pNode->pLeft == NULL) {
                pNode->pLeft = CreateNode(-1, -1);
                AddChild(pNode, pNode->pLeft, NULL);
                pNode = pNode->pLeft;
            }
            else
                pNode = pNode->pLeft;
        }
        else {
            // Create an intermediate right node.
            if (pNode->pRight == NULL) {
                pNode->pRight = CreateNode(-1, -1);
                AddChild(pNode, NULL, pNode->pRight);
                pNode = pNode->pRight;
            }
            else
                pNode = pNode->pRight;
        }
    }

    /*
     * Now we can store the Huffman codeword parameters. The codeword is not
     * needed anymore since the codeword is already embedded into the tree.
     */
    bit_set = codeword & bit_detect[0];
    if (!bit_set) {
        pNode->pLeft = CreateNode(symbol, len);
        AddChild(pNode, pNode->pLeft, NULL);
    }
    else {
        pNode->pRight = CreateNode(symbol, len);
        AddChild(pNode, NULL, pNode->pRight);
    }

    return (TRUE);
}


/**************************************************************************
  Title       : CreateNode

  Purpose     : Creates Huffman node.

  Usage       : y = CreateNode(memChnk, symbol, depth)

  Input       : memChnk - memory allocation class
                symbol  - symbol of the codeword
                depth   - height of the node within the tree

  Output      : y - Huffman node

  Author(s)   : Juha Ojanpera
  *************************************************************************/

HufNode *
CreateNode(int16 symbol, int16 depth)
{
    HufNode *pNode = NULL;

    pNode = new HufNode();

    pNode->pLeft = pNode->pRight = pNode->pParent = NULL;

    pNode->symbol = symbol;
    pNode->depth = depth;

    return (pNode);
}


/**************************************************************************
  Title       : RemoveNode

  Purpose     : Deletes Huffman node.

  Usage       : y = RemoveNode(pNode)

  Input       : pNode - Huffman node to be deleted

  Output      : y - NULL

  Author(s)   : Juha Ojanpera
  *************************************************************************/

HufNode *
RemoveNode(HufNode *pNode)
{
    HufNode *pParent;

    if (pNode == NULL)
        return (NULL);

    pParent = pNode->pParent;

    if (pParent != NULL) {
        if (pParent->pLeft == pNode)
            pParent->pLeft = NULL;
        else
            pParent->pRight = NULL;
    }

    pNode->pLeft = pNode->pRight = pNode->pParent = NULL;

    delete pNode;

    return (pParent);
}


/**************************************************************************
  Title       : AddChild

  Purpose     : Adds either left or right node to the Huffman tree.

  Usage       : y = AddChild(pParent, pLeft, pRight)

  Input       : pParent - parent node
                pLeft   - left node of parent
                pRight  - right node of parent node

  Output      : -

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void
AddChild(HufNode *pParent, HufNode *pLeft, HufNode *pRight)
{
    if (pRight == NULL) // Add left node
    {
        pLeft->pParent = pParent;
        pParent->pLeft = pLeft;
    }
    else // Add right node
    {
        pRight->pParent = pParent;
        pParent->pRight = pRight;
    }

    return;
}


/**************************************************************************
  Title       : DeleteTree

  Purpose     : Deletes Huffman tree.

  Usage       : y = DeleteTree(Root)

  Input       : Root - root node of Huffman tree

  Output      : y - NULL

  Author(s)   : Juha Ojanpera
  *************************************************************************/

HufNode *
DeleteTree(HufNode *Root)
{
    HufNode *node, *node_tmp;

    if (Root == NULL)
        return (NULL);

    if (Root->pLeft == NULL && Root->pRight == NULL) {
        Root = RemoveNode(Root);
        return (NULL);
    }

    node = Root->pLeft;

    while (node != NULL) {
        do {
            do {
                node_tmp = node->pLeft;
                if (node_tmp != NULL)
                    node = node->pLeft;

            } while (node_tmp != NULL);

            node = RemoveNode(node);

            if (node->pRight != NULL)
                node = node->pRight;
            else
                break;

        } while (node != NULL && node->pLeft != NULL && node->pRight != NULL);

        node = RemoveNode(node);

        while (1) {
            if (node != NULL && node->pLeft == NULL && node->pRight == NULL)
                node = RemoveNode(node);
            else if (node == NULL)
                break;
            else {
                node = node->pRight;
                break;
            }
        }
    }

    return (NULL);
}
