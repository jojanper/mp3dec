/**************************************************************************
  mstream.h - Low level interface for decoding MPEG audio streams.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef MP_STREAM_H_
#define MP_STREAM_H_

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- Project Headers. --*/
#include "codec/common/huftree.h"
#include "core/bits.h"
#include "core/bitsring.h"
#include "core/defines.h"
#include "mp3def.h"
#include "mpheader.h"
#include "param.h"
#include "sfb.h"
#include "side.h"

/*
   Purpose:     Frame detection status.
   Explanation: - */
typedef enum SEEK_STATUS
{
    SYNC_FOUND,
    SYNC_LOST,
    EOF_STREAM

} SEEK_STATUS;

/*
   Purpose:     Sync detection status.
   Explanation: - */
typedef enum SYNC_STATUS
{
    LAYER1_STREAM,
    LAYER2_STREAM,
    LAYER3_STREAM,
    FIRST_FRAME_WITH_LAYER1,
    FIRST_FRAME_WITH_LAYER2,
    FIRST_FRAME_WITH_LAYER3,
    LAYER_UNDEFINED

} SYNC_STATUS;

/*
   Purpose:     Parameters for synchronization.
   Explanation: - */
typedef struct SyncInfoStr
{
    int16 sync_length;       // Length of sync word.
    int16 sync_word;         // Synchronization word.
    int16 sync_mask;         // Bitmask for searching sync word from the bitstream.
    SYNC_STATUS sync_status; // Which layer we are supposed to be decoding.

} SyncInfo;

/*
   Purpose:     This interface holds the decoded MPEG audio stream parameters.
                The parameters include such as quantized and dequantized spectral
                samples, side information, Huffman tables and trees, etc.
   Explanation: - */
class MP_Stream
{
public:
    /*-- Constructor. --*/
    MP_Stream();
    MP_Stream(BitStream *input);

    /*-- Destructor. --*/
    ~MP_Stream();

#if 0
    /*-- Public methods. --*/
    BOOL InitDecoder(HINSTANCE hInst,
                     HWND hWnd,
                     const char *mp_stream,
                     Out_Param *out_param,
                     Out_Complexity *complex,
                     StreamBuffer *sBuf = NULL) throw(AdvanceExcpt *);

    void __fastcall ReleaseDecoder(void);

    SEEK_STATUS Init_Sync(SYNC_STATUS layer,
                          BOOL use_assert = FALSE) throw(AdvanceExcpt *);

    // Payload size of layer III frame.
    int __fastcall main_data_slots(void);
#endif

    // Checks whether window switching is used in layer III.
    inline BOOL win_switch(Granule_Info *gr_info)
    {
        return (gr_info->flags & WINDOW_SWITCHING_FLAG);
    }

    // Checks whether short blocks are used in layer III.
    inline BOOL short_block(Granule_Info *gr_info) { return ((gr_info->flags & 3) == 2); }

    // Checks whether mixed blocks are present in layer III.
    inline BOOL mixed_block(Granule_Info *gr_info)
    {
        return (gr_info->flags & MIXED_BLOCK_FLAG);
    }

    // Checks whether 'scalefac_scale' bit is set in layer III.
    inline BOOL scalefac_scale(Granule_Info *gr_info)
    {
        return ((gr_info->flags & SCALEFAC_SCALE) ? 1 : 0);
    }

    // Returns the status of 'pre_flag' bit of layer III.
    inline BOOL pre_flag(Granule_Info *gr_info)
    {
        return ((gr_info->flags & PRE_FLAG) ? 1 : 0);
    }

    /*-- Public parameters. --*/

    /*-- Common to all layers. --*/
    BitStream *bs;
    MP_Frame *frame;
    MP_Buffer *buffer;
    MP_Header *header;
    SyncInfo syncInfo;
    int16 idx_increment;
    MP_Header *prev_header;

    /*-- Layer III specific parameters. --*/
    BOOL SkipBr;
    BOOL WasSeeking;
    int16 PrevSlots;
    int32 FrameStart;
    int16 FreeFormatSlots;

    FLOAT *dbScale;

    BitBuffer *br;
    HufNode **huftree;
    MP3_Huffman *huffman;
    III_Side_Info *side_info;

    int blc[2];
    FLOAT tsOut[SSLIMIT][SBLIMIT];
    FLOAT *spectrum[MAX_CHANNELS][SBLIMIT];
    FLOAT OverlapBlock[2][2][MAX_MONO_SAMPLES];

    /*-- Common complexity reduction and output stream parameters. --*/
    Out_Param *out_param;
    Out_Complexity *complex;

private:
    /*-- Private methods. --*/
    void FillDataSlotTable();
    BOOL InitLayerIIICommonObjects();
    SYNC_STATUS GuessLayer(const char *stream);

    /*-- Private properties. --*/
    BOOL initialized;
    BOOL reduced_class;
    int16 SlotTable[15];
    char streamName[512];
    SYNC_STATUS streamSync;
};

#endif /* MP_STREAM_H_ */
