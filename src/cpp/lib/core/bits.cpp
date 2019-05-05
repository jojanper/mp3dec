#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/bits.h"

/*
  Purpose:     Number of shifts needed to turn multiplication and/or
               division into shifting operation.
  Explanation: - */
#define NATIVE_WORD_SHIFT 3

/*
  Purpose:     Number of bits in one buffer item element.
  Explanation: - */
#define SLOT_BITS 8

/*
   Purpose:     Global buffer for masking purposes.
   Explanation: - */
static const uint32_t mask[] = {
    0x0,        0x1,         0x3,         0x7,         0xF,        0x1F,       0x3F,
    0x7F,       0xFF,        0x1FF,       0x3FF,       0x7FF,      0xFFF,      0x1FFFL,
    0x3FFFL,    0x7FFFL,     0xFFFFL,     0x1FFFFL,    0x3FFFFL,   0x7FFFFL,   0xFFFFFL,
    0x1FFFFFL,  0x3FFFFFL,   0x7FFFFFL,   0xFFFFFFL,   0x1FFFFFFL, 0x3FFFFFFL, 0x7FFFFFFL,
    0xFFFFFFFL, 0x1FFFFFFFL, 0x3FFFFFFFL, 0x7FFFFFFFL, 0xFFFFFFFFL
};


Bit_Stream::Bit_Stream() :
    m_ioBuf(NULL),
    m_eobs(true),
    m_buffer(NULL),
    m_bufLen(0),
    m_bitCounter(0),
    m_bufIndex(0),
    m_empty(false),
    m_streamSize(0)
{
    // bits_written = 0;
    /*
    m_buffer = NULL;
    m_bufLen = 0;
    m_bitCounter = 0;
    m_bufIndex = 0;
    m_eobs = 1;
    mEmpty = false;
    m_streamSize = 0;
    */
}

Bit_Stream::~Bit_Stream(void)
{
    this->close();
    /*
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
    */
}

bool
Bit_Stream::open(StreamBuffer *ioBuf, int size)
{
    m_buffer = NULL;
    this->m_ioBuf = ioBuf;

    m_bufIndex = 0;
    if (!this->m_ioBuf->CanWrite()) {
        m_bitCounter = 0;
        // m_bufIndex = -1;
    }
    else {
        // buf_index = 0;
        m_bitCounter = SLOT_BITS;
    }

    /*-- Allocate the bit buffer. --*/
    m_bufLenOrig = m_bufLen = size;
    m_buffer = (uint8_t *) calloc(1, size);
    // new uint8_t[buf_len];

    // bits_written = 0;
    m_empty = true;
    m_eobs = false;

    /*-- The size is needed when calculating the total length of the file. --*/
    m_streamSize = this->m_ioBuf->GetStreamSize();

    return true;
}

/**************************************************************************
  Title       : Close

  Purpose     : Closes the opened bitstream.

  Usage       : Close()

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::close(void)
{
    if (m_buffer) {
        // printf("Close\n");
        if (this->m_ioBuf->CanWrite() && ((m_bufIndex | m_bitCounter) != 0))
            ff_buffer(1);
        // printf("close done %p\n", m_buffer);

        if (m_buffer)
            free(m_buffer); // delete [] m_buffer;
        m_buffer = NULL;
        // printf("close done done\n");
    }
}


/**************************************************************************
  Title       : ff_buffer

  Purpose     : Refills the bitstream buffer (in READ_MODE) or writes the
                bitstream buffer into a device (in WRITE_MODE).

  Usage       : ff_buffer(write_force)

  Input       : force_write - if set, writes the bitstream buffer into the
                              output device (used only in WRITE_MODE)

  Explanation : This function updates the read/write index of the bitstream
                buffer. In read-mode the buffer is refilled, if it is empty.
                In write-mode the buffer, if full, is stored into the device.

  Author(s)   : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::ff_buffer(int force_write)
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
            // printf("WRITE: %i %i\n", buf_index, buf_len);
            this->m_ioBuf->WriteFromBuffer(m_buffer, m_bufIndex);
            // printf("WRITE DONE\n");

            // if(nitems != buf_index)
            //  ; // Add error handling here if needed.

            m_bufIndex = 0;
        }
    }
}

void
Bit_Stream::putBits(int n, uint32_t word)
{
    /*-- Mask the unwanted bits to zero, just for safety. --*/
    word &= mask[n];

    while (n) {
        auto rbits = (n > SLOT_BITS) ? SLOT_BITS : n;
        n -= rbits;
        // printf("n = %i %i %i %i\n", rbits, n, m_bitCounter, m_bufIndex);
        this->putbits8(rbits, ((word >> n) & mask[rbits]));
        // printf("putbits8 done %i\n", buf_index);
    }
}

/**************************************************************************
  Title        : putbits

  Purpose      : Writes bits to the bit stream.

  Usage        : putbits(n, word);

  Input:       : n    - how many bits are appended to the bit stream
                 word - bits to write

  Explanation  : Note that the maximum number of bits that can be appended
                 is 'uint32_BIT'.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::putbits8(int n, uint32_t word)
{
    // bits_written += n;

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

        // printf("HIP\n");

        /*-- Number of bits needed to store the lower part. --*/
        end = n - m_bitCounter;

        /*-- Store the upper part to the bitstream buffer. --*/
        m_buffer[m_bufIndex] |= ((uint32_t) word >> end);

        /*-- Mask the upper part from 'word' to zero. --*/
        next = (uint32_t) word & mask[end];

        /*-- Update the bitstream buffer index. --*/
        ff_buffer(0);
        m_bitCounter = SLOT_BITS;
        m_buffer[m_bufIndex] = 0;

        m_bitCounter -= end;

        /*-- Store the lower part to the bitstream buffer. --*/
        m_buffer[m_bufIndex] |= ((uint32_t) next << m_bitCounter);
    }
    else {
        // printf("HOP %i\n", buf_index);

        m_bitCounter -= n;

        /*-- For safety, mask the unwanted bits to zero. --*/
        m_buffer[m_bufIndex] |= ((word & mask[n]) << m_bitCounter);
    }
}

uint32_t
Bit_Stream::getBits(int n)
{
    uint32_t value = 0;

    while (n) {
        auto rbits = (n > SLOT_BITS) ? SLOT_BITS : n;
        value <<= rbits;
        value |= this->getbits8(rbits);
        n -= rbits;
    }

    return value;
}

/**************************************************************************
  Title        : getbits

  Purpose      : Reads bits from the bitstream.

  Usage        : y = getbits(n);

  Input        : n - number of bits to be read

  Output       : y - bits read

  Explanation  : Note that the maximum number of bits that can be read
                 from the bitstream buffer is 'uint32_BIT' bits.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

uint32_t
Bit_Stream::getbits8(int n)
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
        /*-- Mask the unwanted bits to zero. --*/
        tmp = (m_buffer[m_bufIndex] & mask[m_bitCounter]) << -idx;

        /*-- Update the bit stream buffer. --*/
        ff_buffer(0);
        m_bitCounter = SLOT_BITS + idx;
        tmp |= (m_buffer[m_bufIndex] >> m_bitCounter) & mask[-idx];
    }
    else {
        m_bitCounter = idx; // m_bitCounter -= n;
        tmp = (m_buffer[m_bufIndex] >> m_bitCounter) & mask[n];
    }

    return tmp;
}


/**************************************************************************
  Title        : skipbits

  Purpose      : Flushes bits from the bitstream.

  Usage        : y = skipbits(n);

  Input        : n - number of bits to be flushed

  Output       : y - flushed read

  Explanation  : The upper limit for the number of bits to be flushed is
                 'uint32_BIT'.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::skipbits8(int n)
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


/**************************************************************************
  Title        : skip_Nbits

  Purpose      : Discard bits from the bitstream.

  Usage        : skipN_bits(n);

  Input        : n - number of bits to be discarded

  Explanation  : No upper limit for the number of bits to be discarded exists.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::skipBits(int n)
{
    int i, bytes, bits_left;

    /*-- Shifting is more efficient than division. --*/
    bytes = (n >> NATIVE_WORD_SHIFT);
    bits_left = n - (bytes << NATIVE_WORD_SHIFT);

    for (i = 0; i < bytes; i++)
        skipbits8(SLOT_BITS);

    if (bits_left)
        skipbits8(bits_left);
}


/**************************************************************************
  Title        : byte_align

  Purpose      : Byte aligns the bitstream.

  Usage        : byte_align()

  Explanation  : Use this function only when you are reading the bitstream.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
Bit_Stream::byteAlign(void)
{
    int bits_to_byte_align;

    bits_to_byte_align = m_bitCounter & 7; // % 8;
    if (bits_to_byte_align)
        skipbits8(bits_to_byte_align);

    return bits_to_byte_align;
}


/**************************************************************************
  Title        : look_ahead

  Purpose      : Looks ahead for the next 'N' bits from the bitstream and
                 returns the read 'N' bits.

  Usage        : y = look_ahead(N);

  Input        : N - number of bits to be read from the bitstream

  Output       : y - bits read

  Author(s)    : Juha Ojanpera
  *************************************************************************/

uint32_t
Bit_Stream::lookAhead(int N)
{
    Bit_Stream bs_tmp;
    uint32_t dword;

    /*-- Store only the most important variables. --*/
    bs_tmp.m_eobs = m_eobs;
    bs_tmp.m_bufLen = m_bufLen;
    bs_tmp.m_bitCounter = m_bitCounter;
    bs_tmp.m_bufIndex = m_bufIndex;
    bs_tmp.m_empty = m_empty;

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

    return dword;
}


/**************************************************************************
  Title        : FlushStream

  Purpose      : Flushes the bitstream. This has the advantage that when
                 the getbits function is next time called, the reading starts
                 from the current stream position.

  Usage        : FlushStream()

  Author(s)    : Juha Ojanpera
  *************************************************************************/

void
Bit_Stream::flushStream(void)
{
    byteAlign();

    /* Calculate how much the stream pointer needs to be updated. */
    auto byte_offset = m_bufIndex;
    byte_offset += (SLOT_BITS - m_bitCounter) >> 3;
    if (byte_offset)
        this->m_ioBuf->SeekBuffer(CURRENT_POS, byte_offset);

    /*
     * This has the effect that the next time we start to read the bit
     * stream, the stream pointer is not updated before reading. This is
     * because if this function is called then it is assumed that the stream
     * pointer is already in its correct place.
     */
    m_empty = true;
    m_bufIndex = 0;
    m_bitCounter = 0;
}

int32_t
Bit_Stream::seekStream(FilePos filePos, int32_t offset)
{
    return this->m_ioBuf->SeekBuffer(filePos, offset);
}
