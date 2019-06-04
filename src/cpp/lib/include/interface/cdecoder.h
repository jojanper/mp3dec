#pragma once

#include "interface/decoder.h"

typedef struct UCI_Str UCI;

namespace draaldecoder {

// Class interface for command line decoder
class IBaseConsoleDecoder : public virtual BaseDecoder
{
public:
    virtual bool parseCommandLine(UCI *uci) = 0;
    virtual void destroy() = 0;

protected:
    IBaseConsoleDecoder() {}
    virtual ~IBaseConsoleDecoder() {}
};

} // namespace draaldecoder
