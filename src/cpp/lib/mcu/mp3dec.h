#pragma once

#include "interface/decoder.h"

class MP_Stream;

namespace draaldecoder {

class MP3Decoder : public BaseDecoder
{
public:
    MP3Decoder();
    virtual ~MP3Decoder();

    virtual bool decode();
    virtual bool close();

protected:
    MP_Stream *m_dec;
};

} // namespace draaldecoder
