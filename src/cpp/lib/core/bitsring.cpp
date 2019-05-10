#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/bitsring.h"


BitBuffer::BitBuffer() :
    BitStreamBuffer(),
    m_mask(0),
    m_wbuf_buf_idx(0),
    m_wbuf_bit_idx(0),
    m_rbuf_buf_idx(0),
    m_rbuf_bit_idx(0)
{}

bool
BitBuffer::open(int size)
{
    auto ret = BitStreamBuffer::open(size);
    if (ret) {
        m_mask = size - 1;
        m_wbuf_buf_idx = 0;
        m_wbuf_bit_idx = SLOT_BITS;
        m_rbuf_buf_idx = -1;
        m_rbuf_bit_idx = 0;
    }

    return ret;
}

void
BitBuffer::putbits8(int n, uint32_t word)
{
    if (m_wbuf_bit_idx == 0) {
        m_wbuf_buf_idx++;
        m_wbuf_bit_idx = SLOT_BITS;
        m_buffer[m_wbuf_buf_idx & m_mask] = 0;
    }

    auto idx = m_wbuf_bit_idx - n;
    if (idx < 0) {
        /*-- Store the upper part to the bit stream buffer. --*/
        m_buffer[m_wbuf_buf_idx & m_mask] |= word >> -idx;

        m_wbuf_buf_idx++;
        m_wbuf_bit_idx = SLOT_BITS + idx;

        /* Store the lower part to the bit stream buffer. */
        m_buffer[m_wbuf_buf_idx & m_mask] = (word << m_wbuf_bit_idx);
    }
    else {
        m_wbuf_bit_idx = idx;
        m_buffer[m_wbuf_buf_idx & m_mask] |= ((word & BITMASK[n]) << m_wbuf_bit_idx);
    }
}

uint32_t
BitBuffer::getbits8(int n)
{
    int idx;
    uint32_t tmp;

    if (m_rbuf_bit_idx == 0) {
        m_rbuf_buf_idx++;
        m_rbuf_bit_idx = SLOT_BITS;
    }

    idx = m_rbuf_bit_idx - n;
    if (idx < 0) {
        tmp = m_buffer[m_rbuf_buf_idx & m_mask] & BITMASK[m_rbuf_bit_idx];
        tmp <<= -idx;

        m_rbuf_buf_idx++;
        m_rbuf_bit_idx = SLOT_BITS + idx;

        tmp |= (m_buffer[m_rbuf_buf_idx & m_mask] >> m_rbuf_bit_idx) & BITMASK[-idx];
    }
    else {
        m_rbuf_bit_idx = idx;
        tmp = (m_buffer[m_rbuf_buf_idx & m_mask] >> m_rbuf_bit_idx) & BITMASK[n];
    }

    return tmp;
}

void
BitBuffer::skipbits8(int n)
{
    if (m_rbuf_bit_idx == 0) {
        m_rbuf_buf_idx++;
        m_rbuf_bit_idx = SLOT_BITS;
    }

    auto idx = m_rbuf_bit_idx - n;
    if (idx < 0) {
        m_rbuf_buf_idx++;
        m_rbuf_bit_idx = SLOT_BITS + idx;
    }
    else
        m_rbuf_bit_idx = idx;
}

uint32_t
BitBuffer::lookAhead(int N)
{
    /*-- Store the current state. --*/
    auto rbuf_dword_idx = this->m_rbuf_buf_idx;
    auto rbuf_bit_idx = this->m_rbuf_bit_idx;
    auto total_bits = this->m_totalBits;

    auto dword = this->getBits(N);

    /*-- Restore the original state. --*/
    this->m_rbuf_buf_idx = rbuf_dword_idx;
    this->m_rbuf_bit_idx = rbuf_bit_idx;
    this->m_totalBits = total_bits;

    return dword;
}

void
BitBuffer::reset()
{
    /*
     * This has the effect that the next time we start to read the bit
     * stream, the stream pointer is not updated before reading. This is
     * because if this function is called then it is assumed that the stream
     * pointer is already in its correct place.
     */
    BitStreamBuffer::reset();
    m_wbuf_buf_idx = 0;
    m_wbuf_bit_idx = SLOT_BITS;
    m_rbuf_buf_idx = -1;
    m_rbuf_bit_idx = 0;
    m_totalBits = 0;
}

void
BitBuffer::addBytes(BitStreamBuffer &bs, size_t bytes, bool aligned)
{
    if (!aligned) {
        for (size_t i = 0; i < bytes; i++)
            this->putBits(8, bs.getBits(8));
    }
    else {
        if (m_wbuf_bit_idx == 0)
            m_wbuf_buf_idx++;

        if (bytes < bs.getSlotsLeft()) {
            auto buf = bs.getBuffer();
            for (size_t i = 0; i < bytes; i++) {
                m_buffer[m_wbuf_buf_idx & m_mask] = buf[i];
                m_wbuf_buf_idx++;
            }
            bs.advanceBufferSlot(bytes);
        }
        else {
            for (size_t i = 0; i < bytes; i++) {
                m_buffer[m_wbuf_buf_idx & m_mask] = bs.getBits(8);
                m_wbuf_buf_idx++;
            }
        }

        if (m_wbuf_bit_idx == 0)
            m_wbuf_buf_idx--;
    }

    m_empty = false;
}

void
BitBuffer::rewindNbits(int n)
{
    /*-- Slot offset within the buffer. --*/
    auto new_buf_idx = n >> NATIVE_WORD_SHIFT;

    /*-- Bit offset within the slot. --*/
    auto new_bit_idx = n & 7;

    /*
      Bit offset within a slot extends to the previous slot.

      For example :

      Let's suppose that the current slot index is 25 and the bit offset in that
      slot is 2, i.e., we have already used 6 bits from that slot. Furthermore,
      we want to rewind back 50 bits. First we have to count how many slots
      we need to go back i.e 50 / 8 = 6 slots. Then count 50 % 8 = 2.
      So we need to go back 6 slot2 plus 2 bits.

      rewind slot : 25 -> 19
      Bit offset in slot 19 is still 2 and we need to go back 2 more bits. When
      doing so we are in slot 18 and the bit offset within that slot is 2.

      The following if-sentences implement the above.
    */
    if ((m_rbuf_bit_idx + new_bit_idx) > SLOT_BITS) {
        m_rbuf_buf_idx -= new_buf_idx + 1;
        m_rbuf_bit_idx = new_bit_idx - (SLOT_BITS - m_rbuf_bit_idx);
    }
    else {
        m_rbuf_buf_idx -= new_buf_idx;
        m_rbuf_bit_idx += new_bit_idx;
    }

    /*-- Implements ring buffer. --*/
    if (m_rbuf_buf_idx < -1 || (m_rbuf_buf_idx == -1 && m_rbuf_bit_idx))
        m_rbuf_buf_idx += this->m_bufLen;

    this->m_totalBits -= n;
}
