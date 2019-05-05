/**************************************************************************
  iobuf.h - File I/O class declaration.

  Author(s): Juha Ojanpera
  Copyright (c) 1999, 2019 Juha Ojanpera.
  *************************************************************************/

#ifndef IOBUF_H_
#define IOBUF_H_

/*-- System Headers. --*/
#include <stdio.h>

/*-- Project Headers. --*/
#include "core/defines.h"
#include "interface/stream.h"


enum
{
    kFileReadMode = 1,
    kFileWriteMode = 2,
    kFileAppendMode = 3
};

/*
   Purpose:     Definition for the file handle.
   Explanation: - */
typedef FILE *FILE_HANDLE;


/**
 * File interface.
 */
class FileBuf : public StreamBuffer
{
public:
    FileBuf();
    virtual ~FileBuf();

    /**
     * Open file.
     *
     * @param name File name
     * @param mode File mode
     * @return true on succes, false otherwise
     */
    bool open(const char *name, int mode);

    /**
     * Close file.
     */
    void close();

    /**
     * Return file name.
     */
    const char *name() const { return this->m_deviceName; }

    virtual uint32_t GetStreamSize() const;
    virtual int32_t SeekBuffer(FilePos fpos, int32_t offset);
    virtual uint32_t ReadToBuffer(uint8_t *buffer, uint32_t bufLen);
    virtual uint32_t WriteFromBuffer(uint8_t *buffer, uint32_t bufLen);
    virtual void SetLookAheadMode(bool) {} // Not applicable for file stream
    virtual bool CanWrite() const;

private:
    void reset();

    int m_mode;              // File mode
    FILE_HANDLE m_hFile;     // File handle.
    char m_deviceName[2048]; // Name of the file.
};

#endif /* IOBUF_H_ */
