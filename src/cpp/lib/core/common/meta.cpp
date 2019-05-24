#include "core/meta.h"

namespace draaldecoder {

AudioAttributes::AudioAttributes()
{
    this->clear();
}

AudioAttributes::~AudioAttributes()
{
    this->clear();
}

bool
AudioAttributes::setInt32Data(int32_t key, int32_t value)
{
    this->mInt32Storage[key] = value;
    return true;
}

bool
AudioAttributes::getInt32Data(int32_t key, int32_t &val) const
{
    auto it = this->mInt32Storage.find(key);
    if (it != this->mInt32Storage.end()) {
        val = it->second;
        return true;
    }

    return false;
}

bool
AudioAttributes::setString(const char *key, const char *value)
{
    this->mStringStorage[key] = value;
    return true;
}

const char *
AudioAttributes::getString(const char *key) const
{
    auto it = this->mStringStorage.find(key);
    return (it != this->mStringStorage.end()) ? it->second.c_str() : nullptr;
}

void
AudioAttributes::clear()
{
    this->mStringStorage.clear();
    this->mInt32Storage.clear();
}

void
AudioAttributes::destroy()
{
    delete this;
}

} // namespace draaldecoder
