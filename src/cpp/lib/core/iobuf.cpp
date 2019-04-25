/**************************************************************************
  iobuf.cpp - File I/O class implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999, 2019 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "core/iobuf.h"


#define INVALID_FILE_HANDLE NULL


FileBuf::FileBuf()
: StreamBuffer()
{
    this->m_hFile = INVALID_FILE_HANDLE;
    strcpy(this->m_deviceName, "");
}


FileBuf::~FileBuf()
{
    this->CloseBuffer();
}


bool FileBuf::OpenBuffer(const char *name, int mode)
{
    this->m_mode = mode;
    strcpy(this->m_deviceName, name);

    switch (mode) {
        case kFileReadMode:
            this->m_hFile = fopen(name, "rb");
            break;

        case kFileWriteMode:
            this->m_hFile = fopen(name, "wb");
            break;

        case kFileAppendMode:
            this->m_hFile = fopen(name, "ab");
            break;

        default:
            this->m_hFile = INVALID_FILE_HANDLE;
            break;
    }

    return (this->m_hFile == INVALID_FILE_HANDLE) ? false : true;
}


void FileBuf::CloseBuffer(void)
{
    if (this->m_hFile) fclose(this->m_hFile);

    strcpy(this->m_deviceName, "");
    this->m_hFile = INVALID_FILE_HANDLE;
}


uint32_t FileBuf::GetStreamSize() const
{
    uint32_t curPos = 0;
    uint32_t TotalSize = 0;

    if (this->m_hFile == INVALID_FILE_HANDLE) return (0L);

    curPos = ftell(this->m_hFile);
    fseek(this->m_hFile, 0, SEEK_END);
    TotalSize = ftell(this->m_hFile);
    fseek(this->m_hFile, curPos, SEEK_SET);

    return (TotalSize);
}


int32_t FileBuf::SeekBuffer(FilePos fpos, int32_t offset)
{
    int32_t currentPosition = -1;

    if (this->m_hFile == INVALID_FILE_HANDLE) return (0L);

    switch (fpos) {
        case CURRENT_POS:
            fseek(this->m_hFile, offset, SEEK_CUR);
            currentPosition = ftell(this->m_hFile);
            break;

        case START_POS:
            fseek(this->m_hFile, offset, SEEK_SET);
            currentPosition = ftell(this->m_hFile);
            break;

        case END_POS: {
            // if (offset < 0) offset = -offset;
            fseek(this->m_hFile, -offset, SEEK_END);
            currentPosition = ftell(this->m_hFile);
            break;
        }
    }

    return currentPosition;
}

uint32_t FileBuf::ReadToBuffer(uint8_t *buffer, uint32_t bufLen)
{
    if (this->m_hFile == INVALID_FILE_HANDLE || this->m_mode == kFileWriteMode) return (0L);

    auto bytesRead = fread(buffer, sizeof(char), bufLen, this->m_hFile);
    fseek(this->m_hFile, -(long) bytesRead, SEEK_CUR);

    return bytesRead;
}

uint32_t FileBuf::WriteFromBuffer(uint8_t *buffer, uint32_t bufLen)
{
    if (this->m_hFile == INVALID_FILE_HANDLE || this->m_mode == kFileReadMode) return (0L);

    return fwrite(buffer, sizeof(uint8_t), bufLen, this->m_hFile);
}

bool FileBuf::CanWrite() const
{
    return (this->m_mode == kFileWriteMode || this->m_mode == kFileAppendMode);
}
