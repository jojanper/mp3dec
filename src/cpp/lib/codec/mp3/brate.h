/**************************************************************************
  brate.h - Interface for retrieving bitrate and playback position of mp3 stream.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef BITRATE_H_
#define BITRATE_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "mstream.h"

/*
   Purpose:     Size of buffer holding the bitrate value of past frames.
   Explanation: - */
#define MAX_PAST_SIZE 64
#define SIZE_MASK (MAX_PAST_SIZE - 1)

class BrInfo
{
public:
    /*-- Constructor. --*/
    BrInfo(MP_Stream *mp,
           BOOL VBR_TagPresent = FALSE,
           DWORD VBR_NumFrames = 0,
           DWORD VBR_FileSize = 0,
           BOOL IsStreaming = FALSE);

    /*-- Destructor. --*/
    ~BrInfo(void) { ; }

    /*-- Public methods. --*/
    int16 GetBitRate(void);
    DWORD GetCurrTime(void);
    inline DWORD GetTotalTime(void) { return (maxLength); }
    inline uint32 GetTotalFrames(void) { return (totalFrames); }
    inline void SetBitRate(int16 bitrate)
    {
        past_bitrate[curr_idx & SIZE_MASK] = bitrate;
        curr_idx++;
    }

private:
    /*-- Private methods. --*/
    BOOL ComputeStartBitRate(MP_Stream *mp, int lookahead_frames);
    DWORD ComputeTotalTime(MP_Stream *mp);

    /*-- Private properties. --*/
    MP_Stream *mp_internal;
    int16 start_bitrate;
    int16 past_bitrate[MAX_PAST_SIZE];
    int16 curr_idx;
    BOOL vbr;
    BOOL free_br;
    FLOAT vbr_slots;
    DWORD maxLength;
    uint32 totalFrames;
};

#endif /* BITRATE_H_ */
