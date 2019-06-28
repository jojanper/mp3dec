#pragma once

namespace draaldecoder {

static const char *const MP3MIME = "audio/mpeg";

enum
{
    // Size of input buffer in bytes
    kBufferSize = 20000,

    // Buffer data handling mode (supported values: kLinearBuffer and kOverWriteBuffer)
    kBufferMode,

    // New data is appended to buffer
    kLinearBuffer,

    // New data overwrites existing data
    kOverWriteBuffer,
};

} // namespace draaldecoder
