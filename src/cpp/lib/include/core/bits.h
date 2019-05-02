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

    /**
     * Open and initialize bitstream.
     *
     * @param ioBuf Input stream handle
     * @param size Bitstream buffer size
     * @return true on success, false otherwise
     */
    bool open(StreamBuffer *ioBuf, int size);

    /**
     * Close bitstream.
     */
    void close();

    /**
     * Return true if end of bitstream found, false otherwise.
     */
    bool endOfStream() const { return m_eobs; }

    /**
     * Return specified number of bits from bitstream.
     */
    uint32_t getBits(int n);
    void putBits(int n, uint32_t word);
    void skipBits(int n);

    int byteAlign();
    uint32_t lookAhead(int N);

    inline const uint8_t *getBuffer() const
    {
        return &m_buffer[m_bufIndex + ((8 - m_bitCounter) >> 3)];
    }

    int getSlotsLeft() const { return (m_bufLen - m_bufIndex); }

    void advanceBufferSlot(int offset) { m_bufIndex += offset; }

    inline size_t getStreamSize() const { return m_streamSize; }

    uint32_t getCurrStreamPos()
    {
        uint32_t pos = this->m_ioBuf->SeekBuffer(CURRENT_POS, 0);
        pos += m_bufIndex + ((8 - m_bitCounter) >> 3);
        return pos;
    }

    inline void releaseBufferLock() { m_eobs = 0; }

    inline bool adjustBufferSize(size_t newSize)
    {
        // m_bufLen2 = buf_len;
        if (newSize < m_bufLenOrig) {
            m_bufLen = newSize;
            return true;
        }

        return false;
    }

    inline void setDefaultBufferSize() { m_bufLen = m_bufLenOrig; }

    /*-- Public methods related to the seeking of the stream. --*/
    void flushStream();
    int32_t seekStream(FilePos filePos, int32_t offset);

private:
    void skipbits8(int n);
    uint32_t getbits8(int n);
    void putbits8(int n, uint32_t word);
    void ff_buffer(int force_write);

    /*-- Private parameters --*/
    StreamBuffer *m_ioBuf; // Name of file to be opened for bit parsing.
    bool m_eobs;           // End of bitstream.
    uint8_t *m_buffer;     // Bitstream buffer.
    size_t m_bufLen;       // Size of the bit buffer.
    int m_bitCounter;      // Bit counter within bit slot.
    size_t m_bufIndex;     // Read/write index within bit buffer.
    bool m_empty;          // Buffer empty ?
    size_t m_streamSize;   // Stream size in bytes.
    size_t m_bufLenOrig;   // Used for buffer reduction.
};
