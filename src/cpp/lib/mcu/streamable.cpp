#include <stddef.h>
#include <string.h>

#include "./decoders/mp3dec.h"
#include "interface/decoder.h"

namespace draaldecoder {

enum
{
    kMimeMP3 = 1,
};

class StreamableDecoderImpl : public StreamableDecoder
{
public:
    StreamableDecoderImpl(size_t mime) : m_dec(nullptr)
    {
        if (mime == kMimeMP3)
            m_dec = new MP3Decoder();
    }

    virtual ~StreamableDecoderImpl()
    {
        if (this->m_dec)
            this->m_dec->destroy();
        this->m_dec = nullptr;
    }

    virtual void addInput(const uint8_t * /*buffer*/, size_t /*size*/) override {}

    // Assign output stream for the decoder
    virtual void setOutput(IOutputStream * /*output*/) override {}

    // Decode frame from input buffer
    virtual bool decode() override { return false; }

protected:
    BaseDecoder *m_dec;
};

StreamableDecoder *
StreamableDecoder::create(const IAttributes &attrs)
{
    size_t mimeId = 0;
    auto mime = attrs.getString("mime");

    if (mime && !strcmp(mime, MP3MIME))
        mimeId = kMimeMP3;

    return (mimeId) ? new StreamableDecoderImpl(mimeId) : nullptr;
}

void
StreamableDecoder::destroy()
{
    delete this;
}

} // namespace draaldecoder
