#pragma once
/**************************************************************************
  bits.h - Interface for reading bits from the opened stream.
  *************************************************************************/

#include <stdint.h>

/*
   Purpose:     Enables bitstream writing routines.
   Explanation: This needs more work, not tested !! */
//#define BS_WRITE_ROUTINES

/*
   Purpose:     Bitstream buffer mode.
   Explanation: - */
typedef enum BsBufferMode
{
  UNKNOWN_BUFFER,
  FILE_BUFFER,
  STREAM_BUFFER,
  SINGLE_BUFFER

} BsBufferMode;

/*
   Purpose:     Bit_Stream interface.
   Explanation: - */
class Bit_Stream
{
  public:
    Bit_Stream(void);
    ~Bit_Stream(void);

    /*-- Public methods. --*/
    bool open(const char *stream, int mode, int size);
    void close(void);

    bool endOfStream() const { return eobs; }

    uint32_t getBits(int n);
    void skipBits(int n);

    void byteAlign(void);
    uint32_t lookAhead(int N);

    const uint8_t *getBuffer() const
    {
      return &bit_buffer[(buf_index << 2) + ((8 - bit_counter) >> 3)];
    }

    int getSlotsLeft() const
    {
      return (buf_len - buf_index);
    }

    void AdvanceBufferSlot(int offset)
    {
      buf_index += offset;
    }

    uint32_t GetStreamSize() const
    {
      return streamSize;
    }

    uint32_t GetCurrStreamPos(void) const
    {
      uint32_t pos = 0;
      //if(bsMode == FILE_BUFFER) pos = ioBuf.SeekBuffer(CURRENT_POS, 0);
      pos += (buf_index << 2) + ((8 - bit_counter) >> 3);
      return (pos);
    }

    void ReleaseBufferLock(void) { eobs = 0; }

    void SetBufferSize(uint32_t newSize)
    {
      buf_len_old = buf_len;
      buf_len = newSize;
    }

    void SetDefaultBufferSize(void)
    {
      buf_len = buf_len_old;
    }

    /*-- Public methods related to the seeking of the stream. --*/
    //IOBuf *GetIOBuf(void) { return (&ioBuf); }

    void FlushStream(void);

  /*
    int32_t SeekStream(File_Pos filePos, int32_t offset)
    {
      return ioBuf.SeekBuffer(filePos, offset);
    }
    */

  private:
    void ff_buffer(int force_write);

    /*-- Private parameters --*/
    //IOBuf ioBuf;                // Name of file to be opened for bit parsing.
    int device_mode;            // File mode (READ, WRITE, APPEND).
    uint32_t bits_written;      // # bits written.
    bool eobs;                   // End of bitstream ?
    uint8_t *bit_buffer;        // Bitstream buffer.
    int buf_len;                // Size of the bit buffer.
    int bit_counter;            // Bit counter within bit slot.
    int buf_index;              // Read/write index within bit buffer.
    int buffer_empty;           // Buffer empty ?
    uint32_t streamSize;        // Filesize in bytes.
    uint32_t buf_len_old;       // Used for buffer reduction.
    BsBufferMode bsMode;        // Type of bitstream buffer (file, stream, ...)
    //StreamBuffer *sBuf;         // Interface for streaming.
    //BsByteOrder bOrder;         // Byte order used by this machine.
};
