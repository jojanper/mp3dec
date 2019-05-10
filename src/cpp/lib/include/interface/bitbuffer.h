#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Bit buffer interface for reading and writing bits.
 */
class IBitBuffer
{
public:
    /**
     * Open and initialize bit buffer.
     *
     * @param size Bit buffer size
     * @return true on success, false otherwise
     */
    virtual bool open(int size = 4096) = 0;

    /**
     * Close bitstream.
     */
    virtual void close() = 0;

    /**
     * Return specified number of bits from bitstream.
     */
    virtual uint32_t getBits(int n) = 0;

    /**
     * Write specified number of bits to bitstream.
     */
    virtual void putBits(int n, uint32_t word) = 0;

    /**
     * Skip specified number of bits from bitstream.
     */
    virtual void skipBits(int n) = 0;

    /**
     * Byte align the bitstream. Use only when reading the bits.
     */
    virtual int byteAlign() = 0;

    /**
     * Looks ahead for the next 'N' bits from the bitstream.
     */
    virtual uint32_t lookAhead(int N) = 0;

    // Reset bitstream
    virtual void reset() = 0;

protected:
    IBitBuffer() {}
    virtual ~IBitBuffer() {}
};
