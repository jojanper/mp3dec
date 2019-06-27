#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/bits.h"


BitStream::BitStream() : BitStreamBuffer(), m_ioBuf(NULL), m_eobs(true), m_streamSize(0) {}

BitStream::~BitStream()
{
    this->close();
}

bool
BitStream::open(IStreamBuffer *ioBuf, int size)
{
    auto ret = BitStreamBuffer::open(size);
    if (ret) {
        this->m_ioBuf = ioBuf;

        m_bitCounter = !this->m_ioBuf->CanWrite() ? 0 : SLOT_BITS;

        m_eobs = false;
        m_bufLenOrig = m_bufLen;
        m_streamSize = this->m_ioBuf->GetStreamSize();
    }

    return ret;
}

void
BitStream::close(void)
{
    if (m_buffer) {
        if (this->m_ioBuf->CanWrite() && ((m_bufIndex | m_bitCounter) != 0))
            ff_buffer(1);

        BitStreamBuffer::close();
    }
}

void
BitStream::ff_buffer(int force_write)
{
    if (!this->m_ioBuf->CanWrite()) {
        if (!m_eobs) {
            m_bufIndex++;
            if (m_bufIndex == m_bufLen || m_empty) {
                /*
                 * Update the file pointer before reading new bits to the bit
                 * buffer. If 'm_bufIndex' == 0 we are reading for the first
                 * time so no need to update the pointer.
                 */
                if (!m_empty && m_bufIndex != 0)
                    this->m_ioBuf->SeekBuffer(CURRENT_POS, m_bufIndex);
                auto nitems = this->m_ioBuf->ReadToBuffer(m_buffer, m_bufLen);

                if (nitems == 0) {
                    /*
                     * Fill the first bit buffer index with zero if end of
                     * the bitstream has been reached. This ensures that
                     * there should be no sudden changes at the output.
                     */
                    m_eobs = true;
                    memset(this->m_buffer, 0, m_bufLen);
                    return;
                }

                m_bufIndex = 0;
                m_empty = false;
                m_bufLen = nitems;
            }
        }
    }
    else {
        m_bufIndex++;

        /* Check whether the buffer is full. */
        if (m_bufIndex == m_bufLen || force_write) {
            /*
             * Check how much we actually need to save. If 'force_write'
             * is equal to one we might actually need to save only some
             * part of the buffer.
             */
            m_bufIndex = (m_bufIndex == m_bufLen) ? m_bufLen : m_bufIndex;

            /* Write the buffer to output stream. */
            this->m_ioBuf->WriteFromBuffer(m_buffer, m_bufIndex);

            m_bufIndex = 0;
        }
    }
}

void
BitStream::putbits8(int n, uint32_t word)
{
    /*-- Update the bitstream buffer index. --*/
    if (m_bitCounter == 0) {
        ff_buffer(0);
        m_bitCounter = SLOT_BITS;
        m_buffer[m_bufIndex] = 0;
    }

    /*
     *  If 'n' is bigger than 'm_bitCounter', 'word' must be splitted
     *  into two parts.
     *
     *  e.g. 'word' = 00000011111101010101010111111100 (= 16642095)
     *       'n' = 26;
     *       'm_bitCounter' = 20
     *
     *        000000  11111101010101010111        111100
     *                |<---upper part--->|  |<--lower part-->|
     */
    if ((m_bitCounter - n) < 0) {
        int end;
        uint32_t next;

        /*-- Number of bits needed to store the lower part. --*/
        end = n - m_bitCounter;

        /*-- Store the upper part to the bitstream buffer. --*/
        m_buffer[m_bufIndex] |= ((uint32_t) word >> end);

        /*-- BITMASK the upper part from 'word' to zero. --*/
        next = (uint32_t) word & BITMASK[end];

        /*-- Update the bitstream buffer index. --*/
        ff_buffer(0);
        m_bitCounter = SLOT_BITS;
        m_buffer[m_bufIndex] = 0;

        m_bitCounter -= end;

        /*-- Store the lower part to the bitstream buffer. --*/
        m_buffer[m_bufIndex] |= ((uint32_t) next << m_bitCounter);
    }
    else {
        m_bitCounter -= n;

        /*-- For safety, BITMASK the unwanted bits to zero. --*/
        m_buffer[m_bufIndex] |= ((word & BITMASK[n]) << m_bitCounter);
    }
}

uint32_t
BitStream::getbits8(int n)
{
    int idx;
    uint32_t tmp;

    /*-- Update the bitstream buffer index. --*/
    if (m_bitCounter == 0) {
        ff_buffer(0);
        m_bitCounter = SLOT_BITS;
    }

    idx = m_bitCounter - n;
    if (idx < 0) {
        /*-- BITMASK the unwanted bits to zero. --*/
        tmp = (m_buffer[m_bufIndex] & BITMASK[m_bitCounter]) << -idx;

        /*-- Update the bit stream buffer. --*/
        ff_buffer(0);
        m_bitCounter = SLOT_BITS + idx;
        tmp |= (m_buffer[m_bufIndex] >> m_bitCounter) & BITMASK[-idx];
    }
    else {
        m_bitCounter = idx; // m_bitCounter -= n;
        tmp = (m_buffer[m_bufIndex] >> m_bitCounter) & BITMASK[n];
    }

    return tmp;
}

void
BitStream::skipbits8(int n)
{
    int idx;

    /*-- Update the bitstream buffer index. --*/
    if (m_bitCounter == 0) {
        ff_buffer(0);
        m_bitCounter = SLOT_BITS;
    }

    idx = m_bitCounter - n;
    if (idx < 0) {
        /*-- Update the bit stream buffer. --*/
        ff_buffer(0);
        m_bitCounter = SLOT_BITS + idx;
    }
    else
        m_bitCounter = idx; // m_bitCounter -= n;
}

uint32_t
BitStream::lookAhead(int N)
{
    BitStream bs_tmp;
    uint32_t dword;

    /*-- Store only the most important variables. --*/
    bs_tmp.m_eobs = m_eobs;
    bs_tmp.m_bufLen = m_bufLen;
    bs_tmp.m_bitCounter = m_bitCounter;
    bs_tmp.m_bufIndex = m_bufIndex;
    bs_tmp.m_empty = m_empty;
    bs_tmp.m_totalBits = m_totalBits;

    /*
     * Due to the implementation mode of bitstream parsing, there must be
     * a way to "cancel" the bit buffer update. When streaming, the previous
     * buffer must not be released to receive new data from the stream.
     * Following method provides us this feature.
     */
    this->m_ioBuf->SetLookAheadMode(true);

    dword = getBits(N);

    /*
     * Restore previous state of the bitstream buffer.
     * First check if bitstream buffer has been updated.
     */
    if (m_bufIndex < bs_tmp.m_bufIndex) {
        /*
         * Remember that the stream pointer has to be explicitly
         * updated. Due to this, the file pointer has been advanced
         * only by one buffer size. To restore the previous state,
         * we only have to "cancel" the file pointer update to get
         * the previous buffer state.
         *
         * Get the previous streaming buffer. It can be easily obtained
         * since we are in lookahead mode from the StreamBuffer point of view,
         * meaning that the previous buffer has not been released for other use.
         */
        this->m_ioBuf->SeekBuffer(CURRENT_POS, -bs_tmp.m_bufLen);
        m_bufLen = this->m_ioBuf->ReadToBuffer(m_buffer, bs_tmp.m_bufLen);
    }

    /*-- Back to "normal" mode of operation. --*/
    this->m_ioBuf->SetLookAheadMode(false);

    m_eobs = bs_tmp.m_eobs;
    m_bufLen = bs_tmp.m_bufLen;
    m_bitCounter = bs_tmp.m_bitCounter;
    m_bufIndex = bs_tmp.m_bufIndex;
    m_empty = bs_tmp.m_empty;
    m_totalBits = bs_tmp.m_totalBits;

    return dword;
}

void
BitStream::flushStream()
{
    byteAlign();

    /* Calculate how much the stream pointer needs to be updated. */
    auto byte_offset = m_bufIndex;
    byte_offset += (SLOT_BITS - m_bitCounter) >> 3;
    if (byte_offset)
        this->m_ioBuf->SeekBuffer(CURRENT_POS, byte_offset);

    this->reset();
}

int32_t
BitStream::seekStream(FilePos filePos, int32_t offset)
{
    return this->m_ioBuf->SeekBuffer(filePos, offset);
}
