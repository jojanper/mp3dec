/**************************************************************************
  console.cpp - Output stream implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <stdio.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/io/console.h"
#include "core/io/wave.h"

/*
   Purpose:     Upper limit for the frames to be decoded.
   Explanation: - */
#ifndef INFINITE
#define INFINITE (-1)
#endif

Console::Console() : hFile(NULL), wave_format(false), header_written(false)
{
    strcpy(filename, "");
}

Console::~Console(void)
{
    this->close();
}

/**************************************************************************
  Title       : OpenConsole

  Purpose     : Opens the output stream.

  Usage       : y = OpenConsole(stream, out_info, use_wave)

  Input       : stream   - name of output stream
                out_info - output stream parameters
                use_wave - if TRUE WAVE file is generated, otherwise PCM is used

  Output      : y - TRUE on success, FALSE otherwise;
                    throws AdvanceExcpt * on error

  Author(s)   : Juha Ojanpera
  *************************************************************************/

bool
Console::open(const char *stream, int sample_rate, int channels, bool use_wave)
{
    bool result = true;

    wave_format = use_wave;
    header_written = false;
    strcpy(filename, stream);
    hFile = fopen(filename, "wb");

    if (hFile == NULL) {
        fclose(hFile);

        hFile = NULL;
        result = false;
    }
    else if (wave_format)
        if (!Write_WAVE_Header(sample_rate, channels, hFile))
            result = false;

    return (result);
}

bool
Console::close()
{
    bool result = false;

    if (hFile != NULL) {
        if (!header_written) {
            // Update the wave header.
            if (wave_format)
                result = Write_WAVE_Flush(hFile);
            else
                result = true;
        }
        header_written = true;

        fclose(hFile);
    }
    hFile = NULL;

    return (result);
}

bool
Console::writeBuffer(int16_t *data, uint32_t len)
{
    bool result = true;

    if (wave_format)
        result = Write_WAVE_Data(data, len, hFile);
    else {
        auto ItemsWritten = fwrite(data, sizeof(int16), len, hFile);

        if (ItemsWritten != len)
            result = false;
    }

    return result;
}
