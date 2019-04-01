/**************************************************************************
  iobuf.cpp - File I/O class implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "core/iobuf.h"


#define INVALID_FILE_HANDLE NULL


FileBuf::FileBuf():
StreamBuffer()
{
  this->m_hFile = INVALID_FILE_HANDLE;
  strcpy(this->m_deviceName, "");
}


FileBuf::~FileBuf()
{
  this->CloseBuffer();
}


BOOL
FileBuf::OpenBuffer(const char *Name, int Mode)
{
  this->m_mode = Mode;
  strcpy(this->m_deviceName, Name);

  switch(Mode)
  {
  case kFileReadMode:
      this->m_hFile = fopen(Name, "rb");
      break;

  case kFileWriteMode:
      this->m_hFile = fopen(Name, "wb");
      break;

  case kFileAppendMode:
      this->m_hFile = fopen(Name, "ab");
      break;

  default:
      this->m_hFile = INVALID_FILE_HANDLE;
      break;
  }

  return (this->m_hFile == INVALID_FILE_HANDLE) ? false : true;
}


void
FileBuf::CloseBuffer(void)
{
  if(this->m_hFile)
    fclose(this->m_hFile);

  strcpy(this->m_deviceName, "");
  this->m_hFile = INVALID_FILE_HANDLE;
}


uint32_t
FileBuf::GetStreamSize() const
{
  uint32_t curPos = 0;
  uint32_t TotalSize = 0;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  curPos = ftell(this->m_hFile);
  fseek(this->m_hFile, 0, SEEK_END);
  TotalSize = ftell(this->m_hFile);
  fseek(this->m_hFile, curPos, SEEK_SET);

  return (TotalSize);
}


int32_t
FileBuf::SeekBuffer(FilePos fpos, int32_t Offset)
{
  int32_t CurrentPosition = -1;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  switch(fpos)
  {
    case CURRENT_POS:
      fseek(this->m_hFile, Offset, SEEK_CUR);
      CurrentPosition = ftell(this->m_hFile);
      break;

    case START_POS:
      fseek(this->m_hFile, Offset, SEEK_SET);
      CurrentPosition = ftell(this->m_hFile);
      break;

    case END_POS: {
      if(Offset < 0)
        Offset = -Offset;
      fseek(this->m_hFile, Offset, SEEK_END);
      CurrentPosition = ftell(this->m_hFile);
      break;
    }
  }

  return (CurrentPosition);
}


uint32_t
FileBuf::ReadToBuffer(uint8_t *Buffer, uint32_t bufLen)
{
  if(this->m_hFile == INVALID_FILE_HANDLE || this->m_mode == kFileWriteMode)
    return (0L);

  auto BytesRead = fread(Buffer, sizeof(char), bufLen, this->m_hFile);
  fseek(this->m_hFile, -(long) BytesRead, SEEK_CUR);

  return (BytesRead);
}


uint32_t
FileBuf::WriteFromBuffer(uint8_t *Buffer, uint32_t bufLen)
{
  if(this->m_hFile == INVALID_FILE_HANDLE || this->m_mode == kFileReadMode)
    return (0L);

  return fwrite(Buffer, sizeof(uint8_t), bufLen, this->m_hFile);
}
