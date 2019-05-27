/**************************************************************************
  mp3.h - High level interface functions for MPEG audio decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef MP3_DEC_H_
#define MP3_DEC_H_

/*-- Project Headers. --*/
#include "brate.h"
#include "core/defines.h"
#include "mp3info.h"

/*-- The following functions are defined in module 'mp3.cpp'. --*/
void InitMP3DecoderData(void);
void ResetEngine(MP_Stream *mp);
int FindFreeFormatSlotCount(MP_Stream *mp);
void ReInitEngine(MP_Stream *mp);
SEEK_STATUS SeekSync(MP_Stream *mp);
void ReComputeEngineParameters(MP_Stream *mp, int decim_factor);
BOOL DecodeFrame(MP_Stream *mp, int16 *pcm_sample);
void InitCodecInitParam(CodecInitParam *param);

/*-- The following functions are defined in module 'mp3wind.cpp'. --*/
BOOL ReSync(MP_Stream *mp);
int GetFrameSlots(MP_Stream *mp, int16 br);
int GetTotalSlots(MP_Stream *mp, int first_call);
int GetSideInfoSlots(MP_Header *header, int layerNum);
BOOL WindFrame(MP_Stream *mp, BrInfo *brInfo, int numFrames, BOOL direction);

#endif /* MP3_DEC_H_ */
