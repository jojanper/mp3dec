#pragma once

#include "core/eqband.h"
#include "interface/console.h"

namespace draaldecoder {

class BaseDecoder
{
public:
    virtual bool decode() = 0;
    virtual bool close() = 0;

    EQ_Band *getEQ() const { return m_eq; }

protected:
    BaseDecoder() : m_eq(new EQ_Band()), m_output(nullptr) {}
    virtual ~BaseDecoder()
    {
        if (m_eq)
            delete m_eq;
        m_eq = nullptr;
    }

    EQ_Band *m_eq;           // Equalizer settings
    IOutputStream *m_output; // Output stream interface
};

} // namespace draaldecoder
