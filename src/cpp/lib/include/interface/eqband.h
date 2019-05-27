#pragma once

#include <stdint.h>

namespace draaldecoder {

static const int MAX_EQ_BANDS = 51;

// Equalizer profiles
typedef enum SOUND_PROFILE
{
    NO_PROFILE,
    ROCK,
    POP,
    DISCO,
    JAZZ

} SOUND_PROFILE;

/**
 * Information about the frequency bands of the current stream.
 * This is needed when the user wants to know what is the frequency resolution
 * and corresponding frequencies (in Hz) of each band.
 */
typedef struct EQParamInfoStr
{
    int16_t *sfbWidth;
    int16_t numBands;
    float frequency_resolution;

} EQParamInfo;

class EQ_Band
{
public:
    EQ_Band();
    virtual ~EQ_Band() {}

    // Maximum level amplification
    static int MAX_AMP;

    virtual void resetEQBand();
    virtual void setEQBand(int16_t band, int16_t dBScale);
    virtual void setEQLevelAmp(int16_t LevelAmp);
    virtual int16_t *computeFrequencyBoundariesInHz();
    virtual float *getdBScale() { return dBConvertedScale; };

    SOUND_PROFILE profile;
    EQParamInfo eqParamInfo;

protected:
    float dBConvertedScale[MAX_EQ_BANDS];
    float dBLevelScale;
    bool EQBandScaleChanged[MAX_EQ_BANDS];
    int16_t EqualizerBandPosition[MAX_EQ_BANDS];
    int16_t EqualizerLevel;
    int16_t hzBoundary[MAX_EQ_BANDS];
};

} // namespace draaldecoder
