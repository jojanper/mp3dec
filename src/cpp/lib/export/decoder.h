#pragma once

#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif // __EMSCRIPTEN__

#ifdef __cplusplus
extern "C" {
#endif

EMSCRIPTEN_KEEPALIVE
int doubler(int x);

EMSCRIPTEN_KEEPALIVE
void inc_array(uint8_t *arr, int len);

EMSCRIPTEN_KEEPALIVE
uint8_t *get_data();

EMSCRIPTEN_KEEPALIVE
uint8_t *create_buffer(int len);

EMSCRIPTEN_KEEPALIVE
void destroy_buffer(void *data);

#ifdef __cplusplus
}
#endif
