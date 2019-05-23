#pragma once

#include "interface/console.h"

namespace draaldecoder {

class BaseDecoder
{
public:
    virtual bool decode() = 0;
    virtual bool close() = 0;

protected:
    BaseDecoder() : m_output(nullptr) {}
    virtual ~BaseDecoder() {}

    IOutputStream *m_output; // Output stream interface
};

} // namespace draaldecoder
