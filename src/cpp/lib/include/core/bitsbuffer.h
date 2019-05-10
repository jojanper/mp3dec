#pragma once

#include <stddef.h>
#include <stdint.h>

#include "interface/bitbuffer.h"

/**
 * Bit buffer base class interface implementation.
 *
 * The class is partly abstract so this will serve as base for
 * final implementations. Currently 2 implementions have been derived
 * from this class.
 *
 * class BitStream:
 *
 *  - This will read bits from stream. The stream may be file, buffer, etc.
 *    The bit buffer is internally updated when new data is required.
 *
 * class BitBuffer:
 *
 * - This will read bits from (ring) buffer. The bit buffer need to be explicitly
 *   updated by the caller.
 */
class BitStreamBuffer : public IBitBuffer
{
public:
    virtual bool open(int size);

    virtual void close();

    virtual uint32_t getBits(int n);

    virtual void putBits(int n, uint32_t word);

    virtual void skipBits(int n);

    virtual int byteAlign();

    virtual void reset();

    size_t totalBits() const { return this->m_totalBits; }

    /**
     * Return bitstream buffer handle. Handle is byte-aligned so any bits read
     * from the current slot does not increment the buffer indexing.
     */
    inline const uint8_t *getBuffer() const
    {
        return (!m_empty) ? m_buffer + m_bufIndex + this->residualSlot() : nullptr;
    }

    /**
     * Return the amount of bytes available for reading the bitstream.
     */
    size_t getSlotsLeft() const
    {
        return (!m_empty) ? m_bufLen - m_bufIndex - this->residualSlot() : 0;
    }

    /**
     * Increment the current read position, increment occurs in byte-aligned manner.
     */
    void advanceBufferSlot(int offset) { m_bufIndex += offset; }

protected:
    BitStreamBuffer();
    virtual ~BitStreamBuffer();

    static const uint32_t BITMASK[33];

    enum
    {
        NATIVE_WORD_SHIFT = 3,
        SLOT_BITS = 8
    };

    // Skip at maximum 8 bits
    virtual void skipbits8(int n) = 0;

    // Read at maximum 8 bits from the bitstream
    virtual uint32_t getbits8(int n) = 0;

    // Write at maximum 8 bits to the bitstream
    virtual void putbits8(int n, uint32_t word) = 0;

    // Return 1 if all bits from current slot have been read, false otherwise
    inline int residualSlot() const { return (8 - m_bitCounter) >> 3; }

    uint8_t *m_buffer;  // Bitstream buffer (buffer slots).
    size_t m_bufLen;    // Size of the bit buffer.
    int m_bitCounter;   // Bit counter within slot.
    size_t m_bufIndex;  // Read/write index within bit buffer.
    bool m_empty;       // Buffer empty ?
    size_t m_totalBits; // Total number of bits read
};
