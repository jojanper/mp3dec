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


class StreamableDecoderImpl : public StreamableDecoder, public IOutputStream
{
public:
    StreamableDecoderImpl(int mime) :
        m_initialized(false),
        m_dec(nullptr),
        m_decData(nullptr),
        m_decSize(0)
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

    virtual bool init(IAttributes &attrs) override
    {
        int32_t size, mode;

        // 32kB buffer size if not specified via API
        if (!attrs.getInt32Data(kBufferSize, size))
            size = 32768;

        // Buffer mode if not specified via API
        if (!attrs.getInt32Data(kBufferMode, mode))
            mode = kModuloBuffer;

        return this->m_buffer.init(size, mode);
    }

    virtual bool addInput(const uint8_t *buffer, size_t size) override
    {
        return this->m_buffer.setBuffer(buffer, size);
    }

    virtual bool close() override { return true; }

    virtual int16_t *getDecodedAudio(size_t &size)
    {
        size = m_decSize;
        auto ptr = m_decData;

        this->resetReceivedAudio();

        return ptr;
    }

    // Decoder provides decoded samples via this interface
    virtual bool writeBuffer(int16_t *data, uint32_t len) override
    {
        // Just save the pointer for later use
        m_decData = data;
        m_decSize = len;
        return true;
    }

    virtual bool decode() override
    {
        bool result = false;

        this->resetReceivedAudio();

        // If enough input data, make sure decoder is initialized
        if (!this->m_initialized && this->m_buffer.dataSize()) {
            // Pass this class as receiver for the decoded samples
            result = this->m_dec->init(&this->m_buffer, this, nullptr);
            if (result)
                this->m_initialized = true;
        }

        // Decode frame
        if (this->m_initialized) {
            result = this->m_dec->decode();
        }

        return result;
    }

protected:
    void resetReceivedAudio()
    {
        this->m_decData = nullptr;
        this->m_decSize = 0;
    }

    bool m_initialized;
    BaseDecoder *m_dec;
    MemoryBuffer m_buffer;

    int16_t *m_decData;
    uint32_t m_decSize;
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
