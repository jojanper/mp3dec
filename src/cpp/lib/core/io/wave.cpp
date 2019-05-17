/**************************************************************************
  wave.cpp - Portable WAVE conversion implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <stddef.h>

/*-- Project Headers. --*/
#include "core/io/wave.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/*
   Purpose:     WAVE header.
   Explanation: - */
typedef struct
{
    unsigned char riff[4];
    unsigned char size[4];
    unsigned char wave[4];
    unsigned char fmt[4];
    unsigned char fmtsize[4];
    unsigned char tag[2];
    unsigned char nChannels[2];
    unsigned char nSamplesPerSec[4];
    unsigned char nAvgBytesPerSec[4];
    unsigned char nBlockAlign[2];
    unsigned char nBitsPerSample[2];
    unsigned char data[4];
    unsigned char pcm_bytes[4];

} BYTE_WAVE;

static BYTE_WAVE wave = { { 82, 73, 70, 70 }, // "RIFF"
                          { sizeof(BYTE_WAVE) - 8, 0, 0, 0 },
                          { 87, 65, 86, 69 },    // "WAVE"
                          { 102, 109, 116, 32 }, // "fmt "
                          { 16, 0, 0, 0 },
                          { 1, 0 },
                          { 1, 0 },
                          { 34, 86, 0, 0 },  // 86 * 256 + 34 = 22050
                          { 172, 68, 0, 0 }, // 172 * 256 + 68 = 44100
                          { 2, 0 },
                          { 16, 0 },
                          { 100, 97, 116, 97 }, // "data"
                          { 0, 0, 0, 0 } };

/*
   Purpose:     Number of bytes written to file.
   Explanation: - */
static unsigned long pcm_bytes = 0;

/*
   Purpose:     Machine byte order flag.
   Explanation: - */
static int cvt_flag;


/**************************************************************************
  Title       : SwapBytes

  Purpose     : Swaps the byte order of 16-bit word.

  Usage       : SwapBytes(data)

  Input       : data - 16-bit word to be swapped

  Output      : data - byte swapped word

  Author(s)   : Juha Ojanpera
  *************************************************************************/

static void inline SwapBytes(int16 *data)
{
    uint16 dat = *data;

    *data = (dat >> 8) & 0xff;
    *data |= (dat << 8);
}


/**************************************************************************
  Title       : SetWAVE

  Purpose     : Writes information into a wave header field.

  Usage       : SetWAVE(w, n, x)

  Input       : w - wave header field
                n - size of wave header field
                x - data for wave header field

  Output      : w - new wave header field

  Author(s)   : Juha Ojanpera
  *************************************************************************/

static void
SetWAVE(unsigned char *w, int n, long x)
{
    int i;

    for (i = 0; i < n; i++) {
        w[i] = (unsigned char) (x & 0xff);
        x >>= 8;
    }
}


/**************************************************************************
  Title       : Write_WAVE_Header

  Purpose     : Writes wave header to a file.

  Usage       : y = Write_WAVE_Header(sample_rate, channels, hFile)

  Input       : sample_rate - sampling rate of the file
                channels    - number of channels present in the file
                hFile       - file handle

  Output      : y - TRUE on success, FALSE otherwise

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
Write_WAVE_Header(int32 sample_rate, int channels, FILE_HANDLE hFile)
{
    int big_ender;
    int bits = 16, type = 0;

    pcm_bytes = 0;

    // Determine byte order.
    big_ender = 1;
    if ((*(unsigned char *) &big_ender) == 1)
        big_ender = 0;

    cvt_flag = 0;
    if (big_ender)
        cvt_flag = 1;
    cvt_flag |= (sizeof(int16) > 2);

    // Init WAVE header
    if (type == 0)
        SetWAVE(wave.tag, sizeof(wave.tag), 1);
    else if (type == 10)
        SetWAVE(wave.tag, sizeof(wave.tag), 7);
    else
        return FALSE;

    SetWAVE(wave.size, sizeof(wave.size), sizeof(wave) - 8);
    SetWAVE(wave.nChannels, sizeof(wave.nChannels), channels);
    SetWAVE(wave.nSamplesPerSec, sizeof(wave.nSamplesPerSec), sample_rate);
    SetWAVE(
        wave.nAvgBytesPerSec, sizeof(wave.nAvgBytesPerSec),
        (channels * sample_rate * bits + 7) / 8);

    SetWAVE(wave.nBlockAlign, sizeof(wave.nBlockAlign), (channels * bits + 7) / 8);
    SetWAVE(wave.nBitsPerSample, sizeof(wave.nBitsPerSample), bits);
    SetWAVE(wave.pcm_bytes, sizeof(wave.pcm_bytes), 0);

    // Write header
    int nwrite = fwrite(&wave, sizeof(wave), 1, hFile);
    if (nwrite != 1)
        return FALSE;

    return TRUE;
}


/**************************************************************************
  Title       : Write_WAVE_Data

  Purpose     : Writes wave data to file.

  Usage       : y = Write_WAVE_Data(data, data_len, hFile)

  Input       : data        - data to be written
                data_length - size of data buffer
                hFile       - file handle

  Output      : y - TRUE on success, FALSE otherwise

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
Write_WAVE_Data(int16 *data, int32 data_len, FILE_HANDLE hFile)
{
    int16 *pcm_samples;
    int32 i, nwrite;

    if (cvt_flag) {
        pcm_samples = data;
        for (i = 0; i < data_len; i++)
            SwapBytes(pcm_samples++);
    }

    nwrite = fwrite(data, sizeof(int16), data_len, hFile);
    pcm_bytes += nwrite << 1;
    if (nwrite != data_len)
        return FALSE;

    return TRUE;
}


/**************************************************************************
  Title       : Write_WAVE_Flush

  Purpose     : Re-corrects the size of the file in the wave header.

  Usage       : y = Write_WAVE_Flush(hFile)

  Input       : hFile - file handle

  Output      : y - TRUE on success, FALSE otherwise

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
Write_WAVE_Flush(FILE_HANDLE hFile)
{
    SetWAVE(wave.size, sizeof(wave.size), sizeof(wave) - 8 + pcm_bytes);
    SetWAVE(wave.pcm_bytes, sizeof(wave.pcm_bytes), pcm_bytes);

    /*-- pos to header --*/
    if (fseek(hFile, 0, SEEK_SET) == -1)
        return FALSE;

    /*-- new wave header --*/
    auto nwrite = fwrite(&wave, sizeof(wave), 1, hFile);

    /*-- restore pos --*/
    if (fseek(hFile, 0, SEEK_END) == -1)
        return FALSE;

    if (nwrite != 1)
        return FALSE;

    return TRUE;
}
