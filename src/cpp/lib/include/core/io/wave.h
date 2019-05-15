/**************************************************************************
  wave.h - Function prototypes for portable WAVE conversion.

  Author(s): Juha Ojanpera
  Copyright (c) 1998-1999 Juha Ojanpera.
  *************************************************************************/

#ifndef WAVE_FORMAT_H_
#define WAVE_FORMAT_H_

/*-- Project Headers. --*/
#include "core/defines.h"
#include "core/io/iobuf.h"

BOOL Write_WAVE_Header(int32 sample_rate, int channels, FILE_HANDLE hFile);

BOOL Write_WAVE_Data(int16 *data, int32 data_len, FILE_HANDLE hFile);

BOOL Write_WAVE_Flush(FILE_HANDLE hFile);

#endif /* WAVE_FORMAT_H_ */
