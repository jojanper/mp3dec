#include <stddef.h>
#include <string.h>

#include "./decoders/mp3dec.h"
#include "core/membuffer.h"
#include "core/meta.h"
#include "interface/decoder.h"
#include "interface/defs.h"

namespace draaldecoder {

enum
{
    kMimeMP3 = 1,
};

struct mimeMap_s
{
    const char *str;
    int id;
} static const MIMEMAP[] = { { MP3MIME, kMimeMP3 } };

static int
getMimeId(const char *mime)
{
    if (mime) {
        for (size_t i = 0; i < sizeof(MIMEMAP) / sizeof(struct mimeMap_s); i++)
            if (!strcmp(mime, MIMEMAP[i].str))
                return MIMEMAP[i].id;
    }

    return -1;
}

class StreamableDecoderImpl : public StreamableDecoder
{
public:
    StreamableDecoderImpl(int mime) : m_initialized(false), m_dec(nullptr)
    {
        if (mime == kMimeMP3)
            m_dec = new MP3Decoder();

        if (m_dec) {
            // 32kB buffer size if not specified via API
            this->m_attrs.setInt32Data(kBufferSize, 32768);

            // Buffer mode if not specified via API
            this->m_attrs.setInt32Data(kBufferMode, kOverWriteBuffer);
        }
    }

    virtual ~StreamableDecoderImpl()
    {
        if (this->m_dec)
            this->m_dec->destroy();
        this->m_dec = nullptr;
    }

    virtual IAttributes *getAttributes() override { return &m_attrs; }

    virtual bool init() override
    {
        int32_t size, mode;
        this->m_attrs.getInt32Data(kBufferSize, size);
        this->m_attrs.getInt32Data(kBufferMode, mode);
        return this->m_buffer.init(size, mode);
    }

    virtual bool addInput(const uint8_t *buffer, size_t size) override
    {
        return this->m_buffer.setBuffer(buffer, size);
    }

    virtual void setOutput(IOutputStream *output) override { this->m_output = output; }

    virtual bool decode() override
    {
        bool result = true;

        if (!this->m_initialized)
            result = this->m_dec->init(&this->m_buffer, this->m_output, nullptr);

        if (result)
            result = this->m_dec->decode();

        return result;
    }

protected:
    bool m_initialized;
    BaseDecoder *m_dec;
    MemoryBuffer m_buffer;
    AudioAttributes m_attrs;
    IOutputStream *m_output;
};

StreamableDecoder *
StreamableDecoder::create(const IAttributes &attrs)
{
    auto mimeId = getMimeId(attrs.getString("mime"));
    return (mimeId > 0) ? new StreamableDecoderImpl(mimeId) : nullptr;
}

void
StreamableDecoder::destroy()
{
    delete this;
}

} // namespace draaldecoder
