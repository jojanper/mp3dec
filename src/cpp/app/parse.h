#pragma once

#include "core/defines.h"
#include "mcu/consoledecoder.h"

bool ParseMPCommandLine(
    char **in,
    char **out,
    BOOL *waveOut,
    int argc,
    char **argv,
    size_t indent_size,
    draaldecoder::IBaseConsoleDecoder *dec);
