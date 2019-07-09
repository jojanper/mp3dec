#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./decoder.h"
#include "interface/attributes.h"
#include "interface/decoder.h"
#include "interface/defs.h"

void
closeDecoder(DecoderHandle handle)
{
    draaldecoder::StreamableDecoder *dec = (draaldecoder::StreamableDecoder *) handle;

    if (dec)
        dec->destroy();
    dec = nullptr;
}

draaldecoder::StreamableDecoder *dec = nullptr;

DecoderHandle
openDecoder()
{
    auto attrs = draaldecoder::IAttributes::create();
    attrs->setString("mime", draaldecoder::MP3MIME);
    dec = draaldecoder::StreamableDecoder::create(*attrs);
    attrs->destroy();

    return dec;
}

int
initDecoder(uint8_t *buffer, int len)
{
    printf("Len is %i", len);
    auto attrs = draaldecoder::IAttributes::create();
    attrs->setInt32Data(draaldecoder::kBufferSize, len);
    attrs->setInt32Data(draaldecoder::kBufferMode, draaldecoder::kModuloBuffer);
    auto result = dec->init(*attrs, buffer, len);
    attrs->destroy();

    return result;
}

int
decode()
{
    auto result = dec->decode();
    return result;
}

int
addInput(uint8_t *buffer, int len)
{
    return dec->addInput(buffer, len);
}

size_t audioSize;

int16_t *
getAudio()
{
    return dec->getDecodedAudio(audioSize);
}

int
getAudioSize()
{
    return (int) audioSize;
}

uint8_t *
create_buffer(int len)
{
    return (uint8_t *) calloc(len, sizeof(uint8_t));
}

void
destroy_buffer(void *data)
{
    free(data);
}
