#pragma once

#include <stdint.h>

//#define CONSOLE

/****************************************************************************/
/***** Don't touch the code below (unles you know what you are doing) *******/
/****************************************************************************/

/*--------------- Compile switches for module 'huffman.cpp' ------------------*/

/*
   Purpose:     Decodes Huffman codewords using Huffman tree.
   Explanation: - */
#define HUFFMAN_TREE_DECODER

/*------------- End of compile switches for module 'huffman.cpp' -------------*/


/*--------------- Compile switches for module 'synfilt.cpp' ------------------*/

/*
   Purpose:     Rounds the output samples of the synthesis filterbank.
   Explanation: - */
//#define ROUNDING

/*
   Purpose:     Performs saturation control to the output samples of the
                synthesis filterbank.
   Explanation: - */
//#define CLIPPING

/*
   Purpose:     Performs the matrixing operations of the synthesis
                filterbank using direct implementation.
   Explanation: - */
//#define COSINE_SYNTHESIS

/*
   Purpose:     Performs the matrixing operations of the synthesis
                filterbank using a fast DCT algorithm proposed by Chen.
   Explanation: - */
#define DCT_CHEN

/*
   Purpose:     Performs the matrixing operations of the synthesis
                filterbank using a fast scaled Chen's DCT algorithm.
   Explanation: - */
#ifdef DCT_CHEN
#define SCALED_DCT
#endif /* DCT_CHEN */

/*
   Purpose:     If neither of the above intruduced compile switches are
                defined, Lee's fast DCT algorithm is used to perform the
                matrixing operations.
   Explanation: - */

/*------------ End of compile switches for module 'synfilt.cpp' --------------*/

/*-- Data types. --*/
typedef int16_t int16;
typedef uint16_t uint16;
typedef float FLOAT;
typedef uint32_t uint32;
typedef int32_t int32;
typedef int32_t DWORD;
typedef int BOOL;
typedef uint8_t BYTE;

#define TRUE true
#define FALSE false

/*-- Some Useful Macros. --*/
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
