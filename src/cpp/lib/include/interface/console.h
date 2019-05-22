#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * Interface for output storage.
 */
class IOutputStream
{
public:
    // Close output stream. Return true on success, false otherwise
    virtual bool close() = 0;

    /**
     * Write data to output stream.
     *
     * @param data Output data
     * @param len Size of data
     * @return true on success, false otherwise
     */
    virtual bool writeBuffer(int16_t *data, uint32_t len) = 0;

protected:
    IOutputStream() {}
    virtual ~IOutputStream() {}
};
