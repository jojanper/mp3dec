#pragma once

#include "interface/cdecoder.h"

namespace draaldecoder {

static const char *const MP3DEC = "mp3decoder";

class ConsoleDecoder : public virtual IBaseConsoleDecoder
{
public:
    /**
     * Factory interface to create console decoder.
     *
     * @param name Decoder name
     * @return Console decoder handle.
     */
    static IBaseConsoleDecoder *create(const char *name);

protected:
    ConsoleDecoder() {}
    virtual ~ConsoleDecoder() {}
};

} // namespace draaldecoder
