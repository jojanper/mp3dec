/**************************************************************************
  side.h - Layer III side info interface.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef SIDE_INFO_H_
#define SIDE_INFO_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "mp3def.h"
#include "sfb.h"

/*
   Purpose:     Structure to hold MPEG-2 IS stereo positions.
   Explanation: - */
typedef struct IS_Info_Str
{
    int16 is_len[3];
    int16 nr_sfb[3];

} IS_Info;

/*
   Purpose:     Parent Structure for Layer III Side Information.
   Explanation: - */
typedef struct III_Info_Str
{
    /*-- Side information read from the bitstream. --*/
    BYTE private_bits;
    BYTE scfsi[2][4];
    int main_data_begin;
    III_Channel_Info *ch_info[MAX_CHANNELS];

    /*-- General side information. --*/
    III_SfbData sfbData;         /* Scalefactor band parameters.   */
    StereoMode *s_mode_long;     /* Stereo modes for long blocks.  */
    StereoMode *s_mode_short[3]; /* Stereo modes for short blocks. */
    IS_Info is_info;             /* IS stereo info for MPEG-2.     */

} III_Side_Info;

#endif /* SIDE_INFO_H_ */
