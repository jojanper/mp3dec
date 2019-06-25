#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "./decoder.h"
#include "mcu/decoders/mp3dec.h"

draaldecoder::MP3Decoder *dec = nullptr;

int
doubler(int x)
{
    if (dec == nullptr)
        dec = new draaldecoder::MP3Decoder();

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
    return (uint8_t *) calloc(sizeof(uint8_t), len);
}

void
destroy_buffer(void *data)
{
    delete dec;

    return free(data);
}
