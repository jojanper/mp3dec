#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codec/mp3/mp3.h"
#include "codec/mp3/mp3def.h"
#include "codec/mp3/mp3info.h"
#include "codec/mp3/param.h"
#include "core/io/uci.h"
#include "core/meta.h"
#include "mp3decconsole.h"

namespace draaldecoder {

MP3ConsoleDecoder::MP3ConsoleDecoder() :
    IBaseConsoleDecoder(),
    MP3Decoder(),
    m_initParam(new CodecInitParam())
{}

MP3ConsoleDecoder::~MP3ConsoleDecoder()
{
    InitCodecInitParam(m_initParam);

    if (m_initParam)
        delete m_initParam;
    m_initParam = nullptr;
}

void
MP3ConsoleDecoder::destroy()
{
    delete this;
}

bool
MP3ConsoleDecoder::init(
    IStreamBuffer *input,
    IOutputStream *output,
    const IAttributes * /*attrs*/)
{
    // Pass initialization parameters to decoder
    auto attrs = AudioAttributes();
    attrs.setDataPtr(kKeyMP3InitParam, m_initParam);

    return MP3Decoder::init(input, output, &attrs);
}

bool
MP3ConsoleDecoder::parseCommandLine(UCI *uci)
{
    GetSwitchParam(
        uci,
        "-out-channels",
        "<num-of-channels>",
        "Number of output channels (1 or 2) (default: 2)",
        &m_initParam->channels);

    GetSwitchParam(
        uci,
        "-decim-factor",
        "<decimation-factor>",
        "Decimation factor for the synthesis filterbank (1, 2, or 4)"
        "(default: 1)",
        &m_initParam->decim_factor);

    GetSwitchParam(
        uci,
        "-window-pruning",
        "<subband-index>",
        "Number of subwindows (0...15) discarded at the windowing stage (default : 2)",
        &m_initParam->window_pruning);

    GetSwitchParam(
        uci,
        "-alias-subbands",
        "<subband-pairs>",
        "Number of subband pairs (1..31) for alias-reduction "
        "(default: 31 [all pairs])",
        &m_initParam->alias_bands);

    GetSwitchParam(
        uci,
        "-imdct-subbands",
        "<num-subbands>",
        "Number of "
        "subbands pairs (1..32) using IMDCT (default: all subbands)",
        &m_initParam->imdct_sbs);

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
    int MAX_AMP = EQ_Band::MAX_AMP;

    if (GetSwitchParam(
            uci,
            "-eq-level",
            "<level-amp>",
            "Equalizer level amplification (default: 0 dB)",
            &equalizerLevel)) {
        if (equalizerLevel < -MAX_AMP || equalizerLevel > MAX_AMP) {
            fprintf(
                stderr,
                "The valid range for the level amplification is -%i...%i dB\n",
                MAX_AMP,
                MAX_AMP);
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

        if (num_bands > MAX_EQ_BANDS) {
            fprintf(stderr, "The maximum # of bands for the equalizer is %i", MAX_EQ_BANDS);
            num_bands = MAX_EQ_BANDS;
        }

        for (int j = 0, i = addr + 1; i < num_bands + addr + 1; i++, j++) {
            if (uci->argv[i] == NULL)
                return false;

            auto dbScale = atoi(uci->argv[i]);
            if (dbScale < -MAX_AMP || dbScale > MAX_AMP) {
                fprintf(
                    stderr,
                    "Band %i: valid range for the level amplification is -%i...%i dB\n",
                    j,
                    MAX_AMP,
                    MAX_AMP);
                return false;
            }

            m_eq->setEQBand(j, dbScale);
            uci->argument_used[i] = 1;
        }
    }

    return true;
}

} // namespace draaldecoder
