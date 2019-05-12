#pragma once

#include <stdint.h>

#include "core/bitsbuffer.h"
#include "interface/stream.h"

/**
 * Bit interface for reading and writing bits from ring buffer.
 */
class BitBuffer : public BitStreamBuffer
{
public:
    BitBuffer();

    virtual bool open(int size);

    virtual uint32_t lookAhead(int n);

    virtual void reset();


    // Adds bytes to ring buffer
    void addBytes(BitStreamBuffer &bs, size_t bytes, bool aligned = false);

    // Rewinds the bit buffer 'n' bits back.
    void rewindNbits(int n);

    inline void setBitsRead(int bits) { this->m_totalBits = bits; }
    inline void resetCounter() { this->m_totalBits = 0; }

protected:
    // Skip at maximum 8 bits
    virtual void skipbits8(int n);

    // Read at maximum 8 bits from the bitstream
    virtual uint32_t getbits8(int n);

    // Write at maximum 8 bits to the bitstream
    virtual void putbits8(int n, uint32_t word);

private:
    size_t m_mask;      // Buffer index mask
    int m_wbuf_buf_idx; // Buffer write index
    int m_wbuf_bit_idx; // Bit index for writing
    int m_rbuf_buf_idx; // Buffer read index
    int m_rbuf_bit_idx; // Bit index for reading
};
