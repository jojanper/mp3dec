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

/**************************************************************************
  Title       : Console

  Purpose     : Class constructor.

  Usage       : Console()

  Author(s)   : Juha Ojanpera
  *************************************************************************/

Console::Console(void)
{
    hFile = NULL;
    wave_format = FALSE;
    header_written = FALSE;
    strcpy(filename, "");
}


/**************************************************************************
  Title       : ~Console

  Purpose     : Class destructor.

  Usage       : ~Console()

  Author(s)   : Juha Ojanpera
  *************************************************************************/

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

BOOL
Console::open(const char *stream, int sample_rate, int channels, BOOL use_wave)
{
    char buf[128];
    BOOL result = TRUE;

    wave_format = use_wave;
    header_written = FALSE;
    strcpy(filename, stream);
    sprintf(buf, "Unable to open output file %s.", stream);
    hFile = fopen(filename, "wb");

    if (hFile == NULL) {
        fclose(hFile);

        hFile = NULL;
        result = FALSE;
    }
    else if (wave_format)
        if (!Write_WAVE_Header(sample_rate, channels, hFile))
            result = FALSE;

    return (result);
}


/**************************************************************************
  Title       : CloseConsole

  Purpose     : Closes output stream.

  Usage       : y = CloseConsole()

  Output      : y - TRUE on success, FALSE otherwise

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
Console::close()
{
    BOOL result = FALSE;

    if (hFile != NULL) {
        if (!header_written) {
            // Update the wave header.
            if (wave_format)
                result = Write_WAVE_Flush(hFile);
            else
                result = TRUE;
        }
        header_written = TRUE;

        fclose(hFile);
    }
    hFile = NULL;

    return (result);
}


/**************************************************************************
  Title       : WriteBuffer

  Purpose     : Writes data to output stream.

  Usage       : y = WriteBuffer(data, len)

  Input       : data - data (16-bit) for output
                len  - length of data to be written

  Output      : y - TRUE on success, FALSE otherwise;
                    throws AdvanceExcpt * on error

  Author(s)   : Juha Ojanpera
  *************************************************************************/

BOOL
Console::writeBuffer(int16 *data, uint32 len)
{
    BOOL result = TRUE;

    if (wave_format)
        result = Write_WAVE_Data(data, len, hFile);
    else {
        auto ItemsWritten = fwrite(data, sizeof(int16), len, hFile);

        if (ItemsWritten != len)
            result = FALSE;
    }

    return (result);
}
