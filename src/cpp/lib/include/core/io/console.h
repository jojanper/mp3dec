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
#include "interface/console.h"

class Console : public IOutputStream
{
public:
    Console();
    ~Console();

    bool open(const char *stream, int sample_rate, int channels, bool use_wave);
    const char *getName() const { return this->filename; }

    virtual bool close();
    virtual bool writeBuffer(int16_t *data, uint32_t len);

private:
    FILE_HANDLE hFile;   // Handle of output stream.
    bool wave_format;    // Indicates whether WAVE format should be generated.
    bool header_written; // Indicates whether WAVE header is ok.
    char filename[512];  // Name of output stream.
};

#endif /* CONSOLE_H_ */
