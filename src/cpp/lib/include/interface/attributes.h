#pragma once

#include <stdint.h>

namespace draaldecoder {

enum
{
    kKeySampleRate = 0,
    kKeyChannels
};

class IAttributes
{
public:
    virtual void destroy() = 0;

    virtual bool setInt32Data(int32_t key, int32_t value) = 0;
    virtual bool getInt32Data(int32_t key, int32_t &val) const = 0;

    virtual bool setString(const char *key, const char *value) = 0;
    virtual const char *getString(const char *key) const = 0;

    virtual bool setDataPtr(int32_t key, const void *ptr) = 0;
    virtual const void *getDataPtr(int32_t key) const = 0;

protected:
    IAttributes() {}
    virtual ~IAttributes() {}
};

} // namespace draaldecoder
