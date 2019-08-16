#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./decoder.h"
#include "interface/attributes.h"
#include "interface/decoder.h"
#include "interface/defs.h"

typedef struct DecoderHandleWrapperStr
{
    draaldecoder::StreamableDecoder *dec;
    size_t audioSize;

} DecoderHandleWrapper;


DecoderHandle
closeDecoder(DecoderHandle handle)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;

    if (decoder) {
        if (decoder->dec)
            decoder->dec->destroy();
        decoder->dec = nullptr;

        free(decoder);
    }

    return NULL;
}

DecoderHandle
openDecoder()
{
    DecoderHandleWrapper *decoder =
        (DecoderHandleWrapper *) calloc(1, sizeof(DecoderHandleWrapper));

    if (decoder) {
        auto attrs = draaldecoder::IAttributes::create();
        attrs->setString("mime", draaldecoder::MP3MIME);
        decoder->dec = draaldecoder::StreamableDecoder::create(*attrs);
        attrs->destroy();

        if (!decoder->dec)
            return closeDecoder(decoder);
    }

    return decoder;
}

int
initDecoder(DecoderHandle handle, uint8_t *buffer, int len)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;

    auto attrs = draaldecoder::IAttributes::create();
    attrs->setInt32Data(draaldecoder::kBufferSize, len);
    attrs->setInt32Data(draaldecoder::kBufferMode, draaldecoder::kModuloBuffer);
    auto result = decoder->dec->init(*attrs, buffer, len);
    attrs->destroy();

    return result;
}

int
decode(DecoderHandle handle)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;
    return decoder->dec->decode();
}

int
addInput(DecoderHandle handle, uint8_t *buffer, int len)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;
    return decoder->dec->addInput(buffer, len);
}

int16_t *
getAudio(DecoderHandle handle)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;
    return decoder->dec->getDecodedAudio(decoder->audioSize);
}

int
getAudioSize(DecoderHandle handle)
{
    DecoderHandleWrapper *decoder = (DecoderHandleWrapper *) handle;

    // In bytes, currently decoded audio is 16-bit per sample
    return (int) decoder->audioSize * 2;
}

uint8_t *
createBuffer(int len)
{
    return (uint8_t *) calloc(len, sizeof(uint8_t));
}

void
destroyBuffer(void *data)
{
    free(data);
}
