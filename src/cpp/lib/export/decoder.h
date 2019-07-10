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

// Open decoder, return handle
EMSCRIPTEN_KEEPALIVE
DecoderHandle openDecoder();

// Close decoder handle
EMSCRIPTEN_KEEPALIVE
DecoderHandle closeDecoder(DecoderHandle handle);

// Initialize decoder
EMSCRIPTEN_KEEPALIVE
int initDecoder(DecoderHandle handle, uint8_t *buffer, int len);

// Decode next audio buffer
EMSCRIPTEN_KEEPALIVE
int decode(DecoderHandle handle);

// Add new input buffer for decoding
EMSCRIPTEN_KEEPALIVE
int addInput(DecoderHandle handle, uint8_t *buffer, int len);

// Return decoded audio buffer
EMSCRIPTEN_KEEPALIVE
int16_t *getAudio(DecoderHandle handle);

// Return size of decoded audio
EMSCRIPTEN_KEEPALIVE
int getAudioSize(DecoderHandle handle);


// Create memory buffer
EMSCRIPTEN_KEEPALIVE
uint8_t *createBuffer(int len);

// Destroy memory buffer
EMSCRIPTEN_KEEPALIVE
void destroyBuffer(void *data);

#ifdef __cplusplus
}
#endif
