#pragma once

#include <map>
#include <string>

#include "interface/attributes.h"

namespace draaldecoder {

typedef std::map<std::string, std::string> StringData;
typedef std::map<int32_t, int32_t> Int32Data;

class AudioAttributes : public IAttributes
{
public:
    AudioAttributes();
    virtual ~AudioAttributes();

    virtual void destroy() override;

    virtual bool setInt32Data(int32_t key, int32_t value) override;
    virtual bool getInt32Data(int32_t key, int32_t &val) const override;

    virtual bool setString(const char *key, const char *value) override;
    virtual const char *getString(const char *key) const override;

private:
    void clear();

    StringData mStringStorage;
    Int32Data mInt32Storage;
};

} // namespace draaldecoder
