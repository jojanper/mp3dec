#include <stddef.h>
#include <stdio.h>
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
}
// Supported decoder MIME types
static const MIMEMAP[] = { { MP3MIME, kMimeMP3 } };

// Map MIME type into internal decoder ID
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
        m_bufferInitialized(false),
        m_eos(false),
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

    virtual bool init(IAttributes &attrs, const uint8_t *buffer, size_t size) override
    {
        int32_t bufsize, mode;

        if (this->m_initialized)
            return false;

        // 32kB buffer size if not specified via API
        if (!attrs.getInt32Data(kBufferSize, bufsize))
            size = 32768;

        // Buffer mode if not specified via API
        if (!attrs.getInt32Data(kBufferMode, mode))
            mode = kModuloBuffer;

        if (mode == kModuloBuffer)
            // bufsize *= 2;
            bufsize += 2 * (2 * 1427 + 1);

        // Initialize input buffer
        auto result = this->m_buffer.init(bufsize, mode);
        if (result) {
            this->m_bufferInitialized = true;
            if (!this->addInput(buffer, size))
                return false;

            if (!this->m_buffer.dataSize())
                return false;

            // Initialize decoder handle
            // Pass this class as receiver for the decoded samples
            result = this->m_dec->init(&this->m_buffer, this, nullptr);
            if (result)
                this->m_initialized = true;
        }

        return result;
    }

    virtual bool addInput(const uint8_t *buffer, size_t size) override
    {
        if (!this->m_bufferInitialized)
            return false;

        return this->m_buffer.setBuffer(buffer, size);
    }

    virtual void setEndOfStream() override { this->m_eos = true; }

    virtual int16_t *getDecodedAudio(size_t &size) override
    {
        size = m_decSize;
        auto ptr = m_decData;

        this->resetReceivedAudio();

        return ptr;
    }

    // IOutputStream: Close stream
    virtual bool close() override { return true; }

    // IOutputStream: Decoder provides decoded samples via this interface
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

        if (!this->m_initialized)
            return result;

        this->resetReceivedAudio();

        // printf("this->m_buffer.dataLeft() = %zu\n", this->m_buffer.dataLeft());

        if (!this->m_eos && this->m_buffer.dataLeft() < 2 * (2 * 1427 + 1)) {
            printf("\nNOT ENOUGH DATA: %20zu %i\n", this->m_buffer.dataLeft(), this->m_eos);
            return false;
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
    bool m_bufferInitialized;
    bool m_eos;

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
