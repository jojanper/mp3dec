/**************************************************************************
  iobuf.h - File I/O class declaration.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifndef IOBUF_H_
#define IOBUF_H_

/*-- System Headers. --*/
#include <stdio.h>

/*-- Project Headers. --*/
#include "core/defines.h"
#include "core/throw.h"

/*
   Purpose:     Indicates how the device should be opened.
   Explanation: - */
#define READ_MODE     (1)
#define WRITE_MODE    (2)
#define APPEND_MODE   (3)

/*
   Purpose:     Size of internal buffer for multimedia file I/O routines.
   Explanation: This is used only when the reading the opened stream. */
#define IO_BUFFER_SIZE (256000) // 256 kB

/*
   Purpose:     Definition for the file handle.
   Explanation: - */
typedef FILE * FILE_HANDLE;
#define INVALID_FILE_HANDLE NULL

/*
   Purpose:     Indicates how the stream should be opened.
   Explanation: - */
typedef enum IO_TYPE
{
  PORTABLE

} IO_TYPE;

/*
   Purpose:     Stream seeking constants.
   Explanation: - */
typedef enum File_Pos
{
  CURRENT_POS,
  START_POS,
  END_POS

} File_Pos;


class IOBuf
{
public:
    IOBuf(void);
    ~IOBuf(void);

    /*-- Class public methods. --*/
    BOOL OpenBuffer(const char *Name, int Mode, IO_TYPE ioType) /* throw (AdvanceExcpt *) */;
    void CloseBuffer(void);

    inline char *GetIOBufName(void) { return m_deviceName; }
    BOOL IsReadOnly(void);

    /*-- Access methods of the stream. --*/
    uint32 GetStreamSize(void);
    int32 SeekBuffer(File_Pos fpos, int32 Offset);
    uint32 ReadToBuffer(BYTE *Buffer, uint32 bufLen);
    uint32 WriteFromBuffer(BYTE *Buffer, uint32 bufLen);

  private :
    FILE_HANDLE m_hFile;     // File handle.
    IO_TYPE m_ioType;        // Indicates how the bits are read.
    char m_deviceName[2048]; // Name of the opened stream.
};

#endif /* IOBUF_H_ */
