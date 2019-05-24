#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec/mp3/mp3def.h"
#include "codec/mp3/mp3info.h"
#include "codec/mp3/param.h"
#include "core/io/uci.h"
#include "mp3decconsole.h"

namespace draaldecoder {

MP3ConsoleDecoder::MP3ConsoleDecoder() : MP3Decoder(), m_initParam(new CodecInitParam()) {}

MP3ConsoleDecoder::~MP3ConsoleDecoder()
{
    if (m_initParam)
        delete m_initParam;
    m_initParam = nullptr;
}

bool
MP3ConsoleDecoder::init(IStreamBuffer *input, IOutputStream *output)
{
    return MP3Decoder::init(input, m_initParam, output);
}

bool
MP3ConsoleDecoder::parseCommandLine(UCI *uci)
{
    m_initParam->fix_window = FALSE;

    m_initParam->channels = MAX_CHANNELS;
    GetSwitchParam(
        uci,
        "-out-channels",
        "<num-of-channels>",
        "Number of output channels (1 or 2) (default: 2)",
        &m_initParam->channels);

    m_initParam->decim_factor = 1;
    GetSwitchParam(
        uci,
        "-decim-factor",
        "<decimation-factor>",
        "Decimation factor for the synthesis filterbank (1, 2, or 4)"
        "(default: 1)",
        &m_initParam->decim_factor);

    m_initParam->window_pruning = WINDOW_PRUNING_START_IDX;
    GetSwitchParam(
        uci,
        "-window-pruning",
        "<subband-index>",
        "Number of subwindows (0...15) discarded at the windowing stage (default : 2)",
        &m_initParam->window_pruning);

    m_initParam->alias_bands = SBLIMIT - 1;
    GetSwitchParam(
        uci,
        "-alias-subbands",
        "<subband-pairs>",
        "Number of subband pairs (1..31) for alias-reduction "
        "(default: 31 [all pairs])",
        &m_initParam->alias_bands);

    m_initParam->imdct_sbs = SBLIMIT;
    GetSwitchParam(
        uci,
        "-imdct-subbands",
        "<num-subbands>",
        "Number of "
        "subbands pairs (1..32) using IMDCT (default: all subbands)",
        &m_initParam->imdct_sbs);

    m_initParam->bandLimit = MAX_MONO_SAMPLES;
    GetSwitchParam(
        uci,
        "-band_limit",
        "<sfb_bin>",
        "max # of bins to be decoded "
        "(default : all bins)",
        &m_initParam->bandLimit);

    return this->initEQBandFromCommandLine(uci);
}

bool
MP3ConsoleDecoder::initEQBandFromCommandLine(UCI *uci)
{
    int16_t equalizerLevel = 0;

    if (GetSwitchParam(
            uci,
            "-eq-level",
            "<level-amp>",
            "Equalizer level amplification (default: 0 dB)",
            &equalizerLevel)) {
        if (equalizerLevel < -20 || equalizerLevel > 20) {
            fprintf(
                stderr, "%s\n", "The valid range for the level amplification is -20...20 dB");
            return false;
        }
        else
            m_eq->setEQLevelAmp(equalizerLevel);
    }

    int16_t num_bands = 0;

    if (GetSwitchParam(
            uci,
            "-eq",
            "<num-bands> band-0 band-1 band-2 ... ",
            "Number of bands and corresponding settings for the equalizer (default: 0 dB on "
            "all bands)",
            &num_bands)) {
        auto addr = GetSwitchAddress(uci, "-eq");

        if (addr == -1)
            return false;

        addr++;

        if (num_bands > MAX_SFB_BANDS) {
            fprintf(stderr, "The maximum # of bands for the equalizer is %i", MAX_SFB_BANDS);
            num_bands = MAX_SFB_BANDS;
        }

        for (int j = 0, i = addr + 1; i < num_bands + addr + 1; i++, j++) {
            if (uci->argv[i] == NULL)
                return false;

            auto dbScale = atoi(uci->argv[i]);
            if (dbScale < -20 || dbScale > 20) {
                fprintf(
                    stderr,
                    "Band %i: valid range for the level amplification is -20...20 dB\n",
                    j);
                return false;
            }

            m_eq->setEQBand(j, dbScale);
            uci->argument_used[i] = 1;
        }
    }

    return true;
}

} // namespace draaldecoder
