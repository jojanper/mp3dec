#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./decoder.h"
#include "interface/attributes.h"
#include "interface/decoder.h"
#include "interface/defs.h"
#include "mcu/decoders/mp3dec.h"

draaldecoder::StreamableDecoder *dec = nullptr;
// static draaldecoder::MP3Decoder *dec = nullptr;

void
closeDecoder()
{
    if (dec)
        dec->destroy();
    dec = nullptr;
}

void
openDecoder()
{
    auto attrs = draaldecoder::IAttributes::create();
    attrs->setString("mime", draaldecoder::MP3MIME);
    dec = draaldecoder::StreamableDecoder::create(*attrs);
    attrs->destroy();
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


int
doubler(int x)
{
    if (dec == nullptr)
        return 123;

    return 2 * x;
}

void
inc_array(uint8_t *arr, int len)
{
    for (int i = 0; i < len; i++)
        arr[i] = 100;
}

static uint8_t staticData[] = { 3, 2, 1 };

uint8_t *
get_data()
{
    return staticData;
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
