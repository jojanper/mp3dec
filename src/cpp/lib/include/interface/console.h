#pragma once

#include <stdint.h>


/**
 * Interface for output storage.
 */
class IOutputStream
{
public:
    // Get stream name
    virtual const char *GetStreamName() const = 0;

    // Get size of stream in bytes
    virtual uint32_t GetStreamSize() const = 0;

    // Read from stream, return number of bytes read
    virtual uint32_t ReadToBuffer(uint8_t *buffer, uint32_t bufLen) = 0;

    // Write to stream, return number of bytes written
    virtual uint32_t WriteFromBuffer(uint8_t *buffer, uint32_t bufLen) = 0;

    // Set stream to lookahead mode
    virtual void SetLookAheadMode(bool enable) = 0;

    // Is stream writable?
    virtual bool CanWrite() const = 0;

protected:
    IOutputStream() {}
    virtual ~IOutputStream() {}
};
