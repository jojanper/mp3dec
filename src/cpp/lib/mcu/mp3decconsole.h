#pragma once

#include "mcu/mp3dec.h"

typedef struct UCI_Str UCI;
typedef struct CodecInitParamStr CodecInitParam;

namespace draaldecoder {

class MP3ConsoleDecoder : public MP3Decoder
{
public:
    MP3ConsoleDecoder();
    virtual ~MP3ConsoleDecoder();

    bool init(IStreamBuffer *input, IOutputStream *output);
    bool parseCommandLine(UCI *uci);

protected:
    bool initEQBandFromCommandLine(UCI *uci);

    CodecInitParam *m_initParam;
};

} // namespace draaldecoder