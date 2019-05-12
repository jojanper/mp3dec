/**************************************************************************
  mp3info.h - Structure definitions for MPEG audio player interface.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef MP3_INFO_H_
#define MP3_INFO_H_

/*-- Project Headers. --*/
#include "core/defines.h"

/*
   Purpose:     Structure interface defining attributes of MPEG audio stream.
   Explanation: - */
typedef struct TrackInfoStr
{
    // Tag *tag;
    int Channels;
    uint32 Frequency;
    char Version[16];
    char Layer[5];
    char Mode[16];
    char Private_bit[8];
    char De_emphasis[5];
    char Copyright[5];
    char Stereo_mode[5];
    char Error_protection[5];
    char Original[5];
    DWORD Length;
    uint16 bitRate;
    uint32 SizeInBytes;
    uint32 TotalFrames;

} TrackInfo;

/*
   Purpose:     Structure interface defining the decoding parameters of the
                MPEG audio decoder.
   Explanation: This structure will be passed to the init routine and the
                decoder will determine based on given values of this structure
                how the input stream should be decoded. */
typedef struct CodecInitParamStr
{
    int16 bandLimit;
    int16 channels;
    int16 decim_factor;
    int16 window_pruning;
    int16 alias_bands;
    int16 imdct_sbs;
    BOOL fix_window;

} CodecInitParam;

#endif /* MP3INFO_H_ */
