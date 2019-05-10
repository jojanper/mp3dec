#pragma once

#include <stdint.h>

#include "core/bitsbuffer.h"
#include "interface/stream.h"

/**
 * Bit interface for reading and writing bits from stream.
 */
class Bit_Stream : public BitStreamBuffer
{
public:
    Bit_Stream();
    virtual ~Bit_Stream();

    virtual void close();

    virtual uint32_t lookAhead(int N);


    /**
     * Open and initialize bitstream.
     *
     * @param ioBuf Input stream handle
     * @param size Bitstream buffer size
     * @return true on success, false otherwise
     */
    bool open(StreamBuffer *ioBuf, int size);

    /**
     * Return true if end of bitstream found, false otherwise.
     */
    bool endOfStream() const { return m_eobs; }

    /**
     * Return the size of the underlying stream.
     */
    inline size_t getStreamSize() const { return m_streamSize; }

    /**
     * Return the current stream position. Position is byte-aligned so any bits read
     * from current slot are not excluded from positioning value.
     */
    uint32_t getCurrStreamPos() const
    {
        uint32_t pos = this->m_ioBuf->SeekBuffer(CURRENT_POS, 0);
        return pos + m_bufIndex + this->residualSlot();
    }

    // Explicit end of stream signaling
    inline void setEndOfStream() { m_eobs = 0; }

    // Set new buffer size.
    inline bool adjustBufferSize(size_t newSize)
    {
        if (newSize < m_bufLenOrig) {
            m_bufLen = newSize;
            return true;
        }

        return false;
    }

    // Restore original size of the bitstream buffer
    inline void setDefaultBufferSize() { m_bufLen = m_bufLenOrig; }

    /**
     * Flush the bitstream. When getBits is called next time, the reading starts
     * from the current stream position.
     */
    void flushStream();

    /**
     * Seek bitstream.
     *
     * @param filePos File reference position
     * @param offset Byte offset with respect to reference position
     * @return New file position
     */
    int32_t seekStream(FilePos filePos, int32_t offset);

protected:
    // Skip at maximum 8 bits
    virtual void skipbits8(int n);

    // Read at maximum 8 bits from the bitstream
    virtual uint32_t getbits8(int n);

    // Write at maximum 8 bits to the bitstream
    virtual void putbits8(int n, uint32_t word);

    /**
     * Refills the bitstream buffer (in read mode) or writes the bitstream
     * buffer into a device (in write mode).
     *
     * Updates the read/write index of the bitstream buffer.
     *  - In read-mode the buffer is refilled, if it is empty.
     *  - In write-mode the buffer, if full, is stored into the stream.
     *
     * @param force_write If set, writes the bitstream buffer into the output
     *                    stream (applicable only in write mode)
     */
    void ff_buffer(int force_write);

private:
    StreamBuffer *m_ioBuf; // Name of file to be opened for bit parsing.
    bool m_eobs;           // End of bitstream.
    size_t m_streamSize;   // Stream size in bytes.
    size_t m_bufLenOrig;   // Used for buffer reduction.
};
