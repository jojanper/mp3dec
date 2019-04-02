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
#include "interface/stream.h"


enum {
    kFileReadMode   = 1,
    kFileWriteMode  = 2,
    kFileAppendMode = 3
};

/*
   Purpose:     Definition for the file handle.
   Explanation: - */
typedef FILE * FILE_HANDLE;


class FileBuf: public StreamBuffer
{
public:
    FileBuf();
    virtual ~FileBuf();

    /*-- Class public methods. --*/
    BOOL OpenBuffer(const char *Name, int Mode);
    void CloseBuffer(void);
    const char *GetFileBufName() const { return this->m_deviceName; }

    /*-- Access methods of the stream. --*/
    virtual uint32_t GetStreamSize() const;
    virtual int32_t SeekBuffer(FilePos fpos, int32_t Offset);
    virtual uint32_t ReadToBuffer(uint8_t *Buffer, uint32_t bufLen);
    virtual uint32_t WriteFromBuffer(uint8_t *Buffer, uint32_t bufLen);
    virtual void SetLookAheadMode(bool) {}

private:
    int m_mode;              // File mode
    FILE_HANDLE m_hFile;     // File handle.
    char m_deviceName[2048]; // Name of the file.
};

#endif /* IOBUF_H_ */
