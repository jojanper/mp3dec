#include "mp3dec.h"
#include "codec/mp3/mstream.h"

namespace draaldecoder {

MP3Decoder::MP3Decoder() : BaseDecoder(), m_dec(nullptr) {}

MP3Decoder::~MP3Decoder()
{
    if (this->m_dec)
        delete this->m_dec;
    this->m_dec = nullptr;
}

bool
MP3Decoder::decode()
{
    return true;
}

bool
MP3Decoder::close()
{
    return true;
}

} // namespace draaldecoder
