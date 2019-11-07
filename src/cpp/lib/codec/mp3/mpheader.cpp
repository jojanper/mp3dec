/**************************************************************************
  mpheader.cpp - Header parsing implementations for MPEG audio.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/*-- Project Headers. --*/
#include "mpheader.h"
#include "mp3def.h"

/*
   Purpose:     Legal bitrates for MPEG-1, MPEG-2 and MPEG-2.5 audio bitstreams.
   Explanation: - */
static const int bitrate[2][3][16] = {
    { // Bit rates for MPEG-2 LSF and MPEG-2.5.
      { 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0 },
      { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
      { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 } },

    // Bit rates for MPEG-1.
    { { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 },
      { 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
      { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 } }
};

static const char *layer_names[3] = { "I", "II", "III" };
static const char *de_emp[4] = { "none", "50/15 microseconds", "", "ccitt j.17" };
static const char *mode_names[4] = { "stereo", "joint-stereo", "dual-channel", "mono" };

/**************************************************************************
  Title        : layer_string

  Purpose      : Returns the layer name of the MPEG audio bitstream.

  Usage        : y = layer_string()

  Output       : y - textual description of the layer

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const char *
MP_Header::layer_string() const
{
    return (layer_names[layer_number() - 1]);
}


/**************************************************************************
  Title        : bit_rate

  Purpose      : Returns the bitrate of the MPEG audio bitstream.

  Usage        : y = bit_rate()

  Output       : y - bitrate in kbps

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
MP_Header::bit_rate() const
{
    return (bitrate[version()][layer_number() - 1][(header >> 12) & 0xF]);
}


/**************************************************************************
  Title        : GetBitRateTable

  Purpose      : Retrieves the address of the bitrate table.

  Usage        : y = GetBitRateTable()

  Output       : y - address of table 'bitrate'

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const int *
MP_Header::GetBitRateTable() const
{
    return (&bitrate[version()][layer_number() - 1][0]);
}


/**************************************************************************
  Title        : frequency

  Purpose      : Returns sampling frequency of the MPEG audio bitstream.

  Usage        : y = frequency()

  Output       : y - sampling frequency in Hz

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int32
MP_Header::frequency() const
{
    int32 s_freq[3][4] = {
        { 22050, 24000, 16000, 0 }, { 44100L, 48000L, 32000, 0 }, { 11025, 12000, 8000, 0 }
    };

    return (s_freq[(header & 0x100000) ? version() : 2][sfreq()]);
}


/**************************************************************************
  Title        : mode_string

  Purpose      : Returns the name of the mode of the MPEG audio bitstream.

  Usage        : y = mode_string()

  Output       : y - textual description of the mode

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const char *
MP_Header::mode_string() const
{
    return (mode_names[mode()]);
}


/**************************************************************************
  Title        : de_emphasis

  Purpose      : Returns the name of the de-emphasis used in the MPEG audio
                 bitstream.

  Usage        : y = de_emphasis()

  Output       : y - textual description of de-emphasis method

  Author(s)    : Juha Ojanpera
  *************************************************************************/

const char *
MP_Header::de_emphasis() const
{
    return (de_emp[header & 2]);
}


/**************************************************************************
  Title        : subbands

  Purpose      : Returns the number of subbands present in layer I and II bitstreams.

  Usage        : y = subbands()

  Output       : y - # of subbands present

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
MP_Header::subbands()
{
    int sblimit;

    if (mode() == MPG_MD_JOINT_STEREO || layer_number() == 2)
        sblimit = pick_table();
    else
        sblimit = SBLIMIT;

    return (sblimit);
}


/**************************************************************************
  Title        : jsbound

  Purpose      : Returns the number of subbands using a separate bit
                 allocation for both left and right channel. The subbands of
                 left and right channels above this value (up-to 'SBLIMIT')
                 share the bit allocation.

  Usage        : y = jsbound()

  Output       : y - # of subbands having individual bit allocation

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
MP_Header::jsbound()
{
    int js_bound;
    int jsb_table[3][4] = {
        { 4, 8, 12, 16 }, { 4, 8, 12, 16 }, { 0, 4, 8, 16 }
    }; /* layer + mode_extension -> jsbound */

    if (mode() == MPG_MD_JOINT_STEREO)
        js_bound = jsb_table[layer_number() - 1][mode_extension()];
    else if (layer_number() == 2)
        js_bound = subbands();
    else
        js_bound = SBLIMIT;

    return (js_bound);
}


/**************************************************************************
  Title        : pick_table()

  Purpose      : Returns the number of subbands present in layer II bitstream.

  Usage        : y = pick_table()

  Output       : y - # of subbands for layer II decoding

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
MP_Header::pick_table()
{
    int16 sfrq, s_freq[2][4] = { { 22, 24, 16, 0 }, { 44, 48, 32, 0 } };
    int br_per_ch, sblim = SBLIMIT;

    br_per_ch = bit_rate() >> (channels() - 1);
    sfrq = s_freq[version()][sfreq()];

    /* Decision rules refer to per-channel bitrates (kbits/sec/chan). */
    if ((sfrq == 48 && br_per_ch >= 56) || (br_per_ch >= 56 && br_per_ch <= 80)) {
        sblim = 27;
        Layer_II_alloc_table = 0;
        Layer_II_b_alloc = 0;
    }
    else if (sfrq != 48 && br_per_ch >= 96) {
        sblim = 30;
        Layer_II_alloc_table = 1;
        Layer_II_b_alloc = 0;
    }
    else if (sfrq != 32 && br_per_ch <= 48) {
        sblim = 8;
        Layer_II_alloc_table = 2;
        Layer_II_b_alloc = 9;
    }
    else {
        sblim = 12;
        Layer_II_alloc_table = 2;
        Layer_II_b_alloc = 9;
    }

    return (sblim);
}


/*
 * Length of MPEG audio frame in milliseconds.
 */
uint32
MP_Header::GetFrameTime() const
{
    // Frame length of each sampling frequency.
    uint32 framelength[3][9] = { { 17, 16, 24, 9, 8, 12, 0, 0, 0 },
                                 { 52, 48, 72, 26, 24, 36, 0, 0, 0 },
                                 { 26, 24, 36, 26, 24, 36, 52, 48, 72 } };

    return (framelength[layer_number() - 1][6 * mp25version() + sfreq() + version() * 3]);
}

/*
 * Length of MPEG audio frame in milliseconds (high precision).
 */
FLOAT
MP_Header::GetFrameTickCount() const
{
    // Frame length of each sampling frequency (high precision).
    FLOAT framelength[3][9] = {
        { 17.414965, 16.0, 24.0, 8.707483, 8.0, 12.0, 0.000000, 0.0, 0.0 },
        { 52.244897, 48.0, 72.0, 26.122448, 24.0, 36.0, 0.000000, 0.0, 0.0 },
        { 26.122448, 24.0, 36.0, 26.122448, 24.0, 36.0, 52.224490, 48.0, 72.0 }
    };

    return (framelength[layer_number() - 1][6 * mp25version() + sfreq() + version() * 3]);
}
