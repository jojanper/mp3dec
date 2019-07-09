#pragma once

#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif // __EMSCRIPTEN__

typedef void *DecoderHandle;

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE
void closeDecoder(DecoderHandle handle);

EMSCRIPTEN_KEEPALIVE
DecoderHandle openDecoder();

EMSCRIPTEN_KEEPALIVE
int initDecoder(uint8_t *buffer, int len);

EMSCRIPTEN_KEEPALIVE
int decode();

EMSCRIPTEN_KEEPALIVE
int addInput(uint8_t *buffer, int len);

EMSCRIPTEN_KEEPALIVE
int16_t *getAudio();

EMSCRIPTEN_KEEPALIVE
int getAudioSize();

EMSCRIPTEN_KEEPALIVE
uint8_t *create_buffer(int len);

EMSCRIPTEN_KEEPALIVE
void destroy_buffer(void *data);

#ifdef __cplusplus
}
#endif
