#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-- Project Headers. --*/
#include "core/bitsbuffer.h"

/*
   Purpose:     Global buffer for masking purposes.
   Explanation: - */
const uint32_t BitStreamBuffer::BITMASK[33] = {
    0x0,        0x1,         0x3,         0x7,         0xF,        0x1F,       0x3F,
    0x7F,       0xFF,        0x1FF,       0x3FF,       0x7FF,      0xFFF,      0x1FFFL,
    0x3FFFL,    0x7FFFL,     0xFFFFL,     0x1FFFFL,    0x3FFFFL,   0x7FFFFL,   0xFFFFFL,
    0x1FFFFFL,  0x3FFFFFL,   0x7FFFFFL,   0xFFFFFFL,   0x1FFFFFFL, 0x3FFFFFFL, 0x7FFFFFFL,
    0xFFFFFFFL, 0x1FFFFFFFL, 0x3FFFFFFFL, 0x7FFFFFFFL, 0xFFFFFFFFL
};


BitStreamBuffer::BitStreamBuffer() :
    m_buffer(NULL),
    m_bufLen(0),
    m_bitCounter(0),
    m_bufIndex(0),
    m_empty(false),
    m_totalBits(0)
{}

BitStreamBuffer::~BitStreamBuffer()
{
    this->close();
}

bool
BitStreamBuffer::open(int size)
{
    m_bufLen = size;
    m_empty = true;
    m_buffer = (uint8_t *) calloc(1, size);
    return (m_buffer) ? true : false;
}

void
BitStreamBuffer::close(void)
{
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
}

void
BitStreamBuffer::putBits(int n, uint32_t word)
{
    /*-- BITMASK the unwanted bits to zero, just for safety. --*/
    word &= BITMASK[n];

    while (n) {
        auto rbits = (n > SLOT_BITS) ? SLOT_BITS : n;
        n -= rbits;
        this->putbits8(rbits, ((word >> n) & BITMASK[rbits]));
    }
}

uint32_t
BitStreamBuffer::getBits(int n)
{
    uint32_t value = 0;

    this->m_totalBits += n;

    while (n) {
        auto rbits = (n > SLOT_BITS) ? SLOT_BITS : n;
        value <<= rbits;
        value |= this->getbits8(rbits);
        n -= rbits;
    }

    return value;
}

void
BitStreamBuffer::skipBits(int n)
{
    int i, bytes, bits_left;

    this->m_totalBits += n;

    /*-- Shifting is more efficient than division. --*/
    bytes = (n >> NATIVE_WORD_SHIFT);
    bits_left = n - (bytes << NATIVE_WORD_SHIFT);

    for (i = 0; i < bytes; i++)
        skipbits8(SLOT_BITS);

    if (bits_left)
        skipbits8(bits_left);
}

int
BitStreamBuffer::byteAlign()
{
    auto bits_to_byte_align = m_bitCounter & 7; // % 8;
    if (bits_to_byte_align)
        this->skipBits(bits_to_byte_align);

    return bits_to_byte_align;
}

void
BitStreamBuffer::reset()
{
    /*
     * This has the effect that the next time we start to read the bit
     * stream, the stream pointer is not updated before reading. This is
     * because if this function is called then it is assumed that the stream
     * pointer is already in its correct place.
     */
    m_bufIndex = 0;
    m_bitCounter = 0;
    m_totalBits = 0;
    m_empty = true;
}
