/**************************************************************************
  console.h - Output stream class declaration.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifndef CONSOLE_H_
#define CONSOLE_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "core/io/iobuf.h"

/*
   Purpose:     Upper limit for the frames to be decoded.
   Explanation: - */
#ifndef INFINITE
#define INFINITE (-1)
#endif

class Console
{
public:
    Console();
    ~Console();

    /*-- Public methods. --*/
    BOOL open(const char *stream, int sample_rate, int channels, BOOL use_wave);
    BOOL close();
    BOOL writeBuffer(int16 *data, uint32 len);
    inline BOOL isUsed(void) { return (hFile) ? TRUE : FALSE; };

private:
    FILE_HANDLE hFile;   // Handle of output stream.
    BOOL wave_format;    // Indicates whether WAVE format should be generated.
    BOOL header_written; // Indicates whether WAVE header is ok.
    char filename[512];  // Name of output stream.
};

#endif /* CONSOLE_H_ */
