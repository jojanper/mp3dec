#include <string.h>

#include "./console/mp3decconsole.h"
#include "consoledecoder.h"
#include "interface/stream.h"

namespace draaldecoder {

IBaseConsoleDecoder *
ConsoleDecoder::create(const char *name)
{
    if (!strcmp(name, MP3DEC))
        return new MP3ConsoleDecoder();

    return nullptr;
}

} // namespace draaldecoder
