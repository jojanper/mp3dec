#pragma once

#include "interface/cdecoder.h"
#include "mcu/consoledecoder.h"
#include "mcu/decoders/mp3dec.h"

typedef struct CodecInitParamStr CodecInitParam;

namespace draaldecoder {

// Console based MP3 decoder
class MP3ConsoleDecoder : public virtual IBaseConsoleDecoder, public MP3Decoder
{
public:
    MP3ConsoleDecoder();

    virtual bool
    init(IStreamBuffer *input, IOutputStream *output, const IAttributes *attrs) override;

    virtual bool parseCommandLine(UCI *uci) override;

    virtual void destroy() override;

protected:
    virtual ~MP3ConsoleDecoder();

    bool initEQBandFromCommandLine(UCI *uci);

    CodecInitParam *m_initParam;
};

} // namespace draaldecoder
