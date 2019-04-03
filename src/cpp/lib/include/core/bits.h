#pragma once

#include <stdint.h>

#include "interface/stream.h"

/**
 * Bit_Stream interface for reading and writing bits from stream.
 */
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

    int byteAlign();
    uint32_t lookAhead(int N);

    const uint8_t *getBuffer() const
    {
      return &bit_buffer[buf_index + ((8 - bit_counter) >> 3)];
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

    uint32_t GetCurrStreamPos()
    {
      uint32_t pos = this->m_ioBuf->SeekBuffer(CURRENT_POS, 0);
      pos += buf_index + ((8 - bit_counter) >> 3);
      return pos;
    }

    void ReleaseBufferLock() { eobs = 0; }

    void SetBufferSize(uint32_t newSize)
    {
      buf_len_old = buf_len;
      buf_len = newSize;
    }

    void SetDefaultBufferSize()
    {
      buf_len = buf_len_old;
    }

    /*-- Public methods related to the seeking of the stream. --*/
    void FlushStream();
    int32_t SeekStream(FilePos filePos, int32_t offset);

private:
    void skipBits(int n);
    uint32_t getbits8(int n);
    void ff_buffer(int force_write);

    /*-- Private parameters --*/
    StreamBuffer *m_ioBuf;      // Name of file to be opened for bit parsing.
    int device_mode;            // File mode (READ, WRITE, APPEND).
    uint32_t bits_written;      // # bits written.
    bool eobs;                  // End of bitstream.
    uint8_t *bit_buffer;        // Bitstream buffer.
    int buf_len;                // Size of the bit buffer.
    int bit_counter;            // Bit counter within bit slot.
    int buf_index;              // Read/write index within bit buffer.
    int buffer_empty;           // Buffer empty ?
    uint32_t streamSize;        // Stream size in bytes.
    uint32_t buf_len_old;       // Used for buffer reduction.
};
