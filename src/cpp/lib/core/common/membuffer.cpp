#include <stdlib.h>
#include <string.h>

#include "core/membuffer.h"


MemoryBuffer::MemoryBuffer() :
    m_buf(nullptr),
    m_bufSize(0),
    m_readPos(0),
    m_mode(kLinearBuffer)
{
    m_deviceName[0] = '\0';
}

MemoryBuffer::~MemoryBuffer()
{
    if (this->m_buf)
        free(this->m_buf);
    this->m_buf = nullptr;
}

bool
MemoryBuffer::init(size_t size, int mode, const char *name)
{
    this->m_mode = mode;

    if (name)
        strcpy(this->m_deviceName, name);

    if (size) {
        this->m_buf = (uint8_t *) calloc(size, sizeof(uint8_t));
        this->m_bufSize = size;
    }

    return this->m_bufSize ? true : false;
}

int32_t
MemoryBuffer::SeekBuffer(FilePos fpos, int32_t offset)
{
    auto nowPos = this->m_readPos;

    if (!this->m_buf || !this->m_bufSize)
        return nowPos;

    switch (fpos) {
        case CURRENT_POS:
            this->m_readPos += offset;
            break;

        case START_POS:
            this->m_readPos = offset;
            break;

        case END_POS: {
            this->m_readPos = this->m_bufSize - offset;
            break;
        }
    }

    if (this->m_readPos > this->m_bufSize)
        this->m_readPos = nowPos;

    return this->m_readPos;
}

uint32_t
MemoryBuffer::ReadToBuffer(uint8_t *buffer, uint32_t bufLen)
{
    if (!this->m_buf || !this->m_bufSize || !bufLen)
        return 0;

    auto len = bufLen;
    if (this->m_readPos + bufLen > this->m_bufSize)
        len = this->m_bufSize - this->m_readPos;

    memcpy(buffer, this->m_buf + this->m_readPos, len);
    if (len < bufLen)
        memset(buffer + len, 0, bufLen - len);

    return len;
}

bool
MemoryBuffer::setBuffer(uint8_t *buffer, size_t size)
{
    if (this->m_mode == kOverWriteBuffer)
        memcpy(this->m_buf, buffer, size);
    else if (this->m_mode == kLinearBuffer) {
        // Make sure out of bounds does not occur
        if (this->m_readPos + size > this->m_bufSize)
            return false;

        memcpy(this->m_buf + this->m_readPos, buffer, size);
    }

    return true;
}
