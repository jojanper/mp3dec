#pragma once

#include <stdint.h>


/**
 * Stream seeking constants.
 */
typedef enum FilePos
{
    CURRENT_POS,
    START_POS,
    END_POS

} FilePos;


/**
 * Interface for storing and accessing stream related data.
 */
class StreamBuffer
{
public:
    // Get size of stream in bytes
    virtual uint32_t GetStreamSize() const = 0;

    // Seek to specified position within stream, return current position
    virtual int32_t SeekBuffer(FilePos fpos, int32_t Offset) = 0;

    // Read from stream, return number of bytes read
    virtual uint32_t ReadToBuffer(uint8_t *buffer, uint32_t bufLen) = 0;

    // Write to stream, return number of bytes written
    virtual uint32_t WriteFromBuffer(uint8_t *buffer, uint32_t bufLen) = 0;

    // Set stream to lookahead mode
    virtual void SetLookAheadMode(bool enable) = 0;

    // Is stream writable?
    virtual bool CanWrite() const = 0;

protected:
    StreamBuffer() {}
    virtual ~StreamBuffer() {}
};
