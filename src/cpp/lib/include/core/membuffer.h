#pragma once

#include <stddef.h>

#include "core/defines.h"
#include "interface/stream.h"


enum
{
    kLinearBuffer = 1,    // New data is appended to buffer
    kOverWriteBuffer = 2, // New data overwrites existing data
};

/**
 * Block buffer interface.
 */
class MemoryBuffer : public IStreamBuffer
{
public:
    MemoryBuffer();
    virtual ~MemoryBuffer();

    /**
     * Initialize buffer.
     *
     * @param size Buffer size in bytes
     * @param mode Buufer mode
     * @param name Name for the buffer data
     * @return true on succes, false otherwise
     */
    bool init(size_t size, int mode, const char *name = nullptr);

    // Copy external buffer
    bool setBuffer(const uint8_t *buffer, size_t size);

    virtual const char *GetStreamName() const override { return m_deviceName; }
    virtual uint32_t GetStreamSize() const override { return m_bufSize; }
    virtual int32_t SeekBuffer(FilePos fpos, int32_t offset) override;
    virtual uint32_t ReadToBuffer(uint8_t *buffer, uint32_t bufLen) override;
    virtual uint32_t WriteFromBuffer(uint8_t *, uint32_t) override { return 0; };
    virtual void SetLookAheadMode(bool) override { return; };
    virtual bool CanWrite() const override { return false; };

private:
    uint8_t *m_buf;          // Memory buffer
    size_t m_bufSize;        // Memory buffer size
    size_t m_readPos;        // Current read position
    size_t m_writePos;       // Current write index
    int m_mode;              // Buffer mode
    char m_deviceName[2048]; // Name of the buffer
};
