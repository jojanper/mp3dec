/**************************************************************************
  iobuf.cpp - File I/O class implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "iobuf.h"

/**************************************************************************
  Title        : IOBuf

  Purpose      : Class constructor.

  Usage        : IOBuf()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

IOBuf::IOBuf(void)
{
  hFile = INVALID_FILE_HANDLE;
  strcpy(DeviceName, "");
}


/**************************************************************************
  Title        : ~IOBuf

  Purpose      : Class destructor.

  Usage        : ~IOBuf()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

IOBuf::~IOBuf(void)
{
  CloseBuffer();
}


/**************************************************************************
  Title        : OpenBuffer

  Purpose      : Opens the stream for reading and/or writing purposes.

  Usage        : y = OpenBuffer(hInst, Name, Mode, ioType)

  Input        : hInst  - current instance handle
                 Name   - name of strema to be opened
                 Mode   - how the stream should be opened
                 ioType - what is the underlying implementation scheme

  Output       : y - TRUE on success, FALSE otherwise;
                     throws AdvanceExcpt * on error

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL __stdcall
IOBuf::OpenBuffer(HINSTANCE hInst, const char *Name, int Mode, IO_TYPE ioType)
throw (AdvanceExcpt *)
{
  MMRESULT mmresult = 0;
  BOOL result = TRUE;
  DWORD flags;
  char buf[128];

  if(!IsReadOnly())
    wsprintf(buf, "Unable to open file %s.", Name);
  else
    wsprintf(buf, "Unable to open file %s. Read-only ?", Name);

  this->ioType = ioType;
  lstrcpy(DeviceName, Name);

  switch(ioType)
  {
#ifdef PORTABLE_IO
    case PORTABLE:
      switch(Mode)
      {
        case READ_MODE:
          hFile = fopen(Name, "rb");
          break;

        case WRITE_MODE:
          hFile = fopen(Name, "wb");
          break;

        case APPEND_MODE:
          hFile = fopen(Name, "ab");
          break;

        default:
          hFile = INVALID_FILE_HANDLE;
          break;
      }
      break;
#else
    case WINDOWS_FILE:
      switch(Mode)
      {
        case READ_MODE:
          flags = FILE_SHARE_READ;
          if(!IsReadOnly())
            flags |= FILE_SHARE_WRITE;

          hFile = CreateFile(Name,                         // open file
                             GENERIC_READ,                 // open for reading
                             flags,                        // share for reading
                             (LPSECURITY_ATTRIBUTES) NULL, // no security
                             OPEN_EXISTING,                // existing file only
                             FILE_ATTRIBUTE_NORMAL,        // normal file
                             (HANDLE) NULL);               // no attr. template
          break;

        case WRITE_MODE:
          hFile =
            CreateFile(Name,                         // open file
                       GENERIC_WRITE,                // open for writing
                       FILE_SHARE_WRITE,             // share for writing
                       (LPSECURITY_ATTRIBUTES) NULL, // no security
                       OPEN_ALWAYS,                  // open or create
                       FILE_ATTRIBUTE_NORMAL,        // normal file
                       (HANDLE) NULL);               // no attr. template
          break;

        case APPEND_MODE:
          hFile =
          CreateFile(Name,                               // open file
                     GENERIC_WRITE,                      // open for writing
                     FILE_SHARE_WRITE | FILE_SHARE_READ, // share for reading
                                                         // and writing
                     (LPSECURITY_ATTRIBUTES) NULL,       // no security
                     OPEN_EXISTING,                      // open or create
                     FILE_ATTRIBUTE_NORMAL,              // normal file
                     (HANDLE) NULL);                     // no attr. template
          break;

        default:
          hFile = INVALID_FILE_HANDLE;
          break;
      }
      break;

    case WINDOWS_IO:
      switch(Mode)
      {
        case READ_MODE:
          if(!IsReadOnly())
            flags = MMIO_READWRITE;
          else
            flags = MMIO_READ;

          hmmio = mmioOpen((char *)Name, NULL, flags | MMIO_ALLOCBUF);
          if(hmmio)
            mmresult = mmioSetBuffer(hmmio, NULL, IO_BUFFER_SIZE, 0);
          break;

        case WRITE_MODE:
          hmmio = mmioOpen((char *)Name, NULL, MMIO_READWRITE | MMIO_ALLOCBUF);
          break;

        case APPEND_MODE:
          hmmio = mmioOpen((char *)Name, NULL, MMIO_READWRITE | MMIO_ALLOCBUF);
          break;

        default:
          hmmio = NULL;
          break;
      }
      break;
#endif
  }

  char title[64];
  wsprintf(title, "%s - File I/O Error", _DLL_UTILBUILD_);

  /* Check if errors occured. */
  if(ioType != WINDOWS_IO)
  {
    if(hFile == INVALID_FILE_HANDLE)
    {
      result = FALSE;
      ASSERT2(hFile == INVALID_FILE_HANDLE, buf, title, NULL);
    }
  }
#ifndef PORTABLE_IO
  else if(hmmio == NULL)
  {
    result = FALSE;
    ASSERT1(hmmio == NULL, hInst, MMIO_ERROR, (DWORD)MMIOERR_INVALIDFILE, title, buf);
  }
  else if(mmresult != 0)
  {
    result = FALSE;
    ASSERT0(mmresult != 0, hInst, mmresult, title, buf);
  }
#endif /* not PORTABLE_IO */

  return (result);
}


/**************************************************************************
  Title        : CloseBuffer

  Purpose      : Closes the opened stream.

  Usage        : CloseBuffer()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void __stdcall
IOBuf::CloseBuffer(void)
{
  switch(ioType)
  {
    case PORTABLE:
      if(hFile)
        fclose(hFile);
      break;
  }

  if(hFile)
  {
    strcpy(DeviceName, "");
    hFile = INVALID_FILE_HANDLE;
  }
}


/**************************************************************************
  Title        : IsReadOnly

  Purpose      : Checks whether the stream is of type read-only.

  Usage        : y = IsReadOnly()

  Output       : y - TRUE is read-only, FALSE otherwise

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
IOBuf::IsReadOnly(void)
{
  return (false);
}


/**************************************************************************
  Title        : GetStreamSize

  Purpose      : Returns size of the stream in bytes.

  Usage        : y = GetStreamSize()

  Output       : y - size in bytes

  Author(s)    : Juha Ojanpera
  *************************************************************************/

uint32
IOBuf::GetStreamSize(void)
{
  uint32 curPos = 0;
  uint32 TotalSize = 0;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  switch(ioType)
  {
    case PORTABLE:
      curPos = ftell(this->m_hFile);
      fseek(this->m_hFile, 0, SEEK_END);
      TotalSize = ftell(this->m_hFile);
      fseek(this->m_hFile, curPos, SEEK_SET);
      break;
  }

  return (TotalSize);
}


/**************************************************************************
  Title       : SeekBuffer

  Purpose     : Changes the file pointer of the opened stream.

  Usage       : y = SeekBuffer(fpos, Offset)

  Input       : fpos   - specifies how the offset should be interpreted
                Offset - Offset to change the file position

  Output      : y - new file position

  Author(s)   : Juha Ojanpera
  *************************************************************************/

int32
IOBuf::SeekBuffer(File_Pos fpos, int32 Offset)
{
  int32 CurrentPosition = -1;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  switch(this->m_ioType)
  {
    case PORTABLE:
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
  }

  return (CurrentPosition);
}


/**************************************************************************
  Title       : ReadToBuffer

  Purpose     : Reads data from stream.

  Usage       : y = ReadToBuffer(Buffer, bufLen)

  Input       : bufLen - number of bytes to read

  Output      : y      - number of bytes actually read
                Buffer - read bytes

  Explanation : Note that this function does not update the stream pointer.
                The user has to explicitly take care of that.

  Author(s)   : Juha Ojanpera
  *************************************************************************/

uint32
IOBuf::ReadToBuffer(BYTE *Buffer, uint32 bufLen)
{
  uint32 BytesRead = 0;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  switch(this->m_ioType)
  {
    case PORTABLE:
      BytesRead = fread(Buffer, sizeof(char), bufLen, this->m_hFile);
      fseek(hFile, -(long)BytesRead, SEEK_CUR);
      break;
  }

  return (BytesRead);
}


/**************************************************************************
  Title       : WriteFromBuffer

  Purpose     : Write data into stream.

  Usage       : y = WriteFromBuffer(buffer, bytes)

  Input       : Buffer - bytes to write
                bufLen - number of bytes to write

  Output      : y - number of bytes actually written

  Author(s)   : Juha Ojanpera
  *************************************************************************/

uint32
IOBuf::WriteFromBuffer(BYTE *Buffer, uint32 bufLen)
{
  uint32 BytesWritten = 0;

  if(this->m_hFile == INVALID_FILE_HANDLE)
    return (0L);

  switch(this->m_ioType)
  {
    case PORTABLE:
      BytesWritten = fwrite(Buffer, sizeof(char), bufLen, this->m_hFile);
      break;
  }

  return (BytesWritten);
}
