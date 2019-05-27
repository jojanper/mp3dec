/**************************************************************************
  eqband.cpp - Equalizer implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000, 2019 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <math.h>
#include <stdlib.h>

/*-- Project Headers. --*/
#include "interface/eqband.h"

namespace draaldecoder {

static float
convertdBScale(int dBScale)
{
    return (float) (pow((double) 10.0f, (double) (dBScale) / 23.0f));
}

// +- 20dB
int EQ_Band::MAX_AMP = 20;

EQ_Band::EQ_Band() : profile(NO_PROFILE)
{
    this->resetEQBand();
}

void
EQ_Band::resetEQBand()
{
    for (int i = 0; i < MAX_EQ_BANDS; i++) {
        dBConvertedScale[i] = 1.0; // 0 dB
        EQBandScaleChanged[i] = false;
        EqualizerBandPosition[i] = 0;
    }

    dBLevelScale = 1.0;
    EqualizerLevel = 0;
}

void
EQ_Band::setEQBand(int16_t band, int16_t dBScale)
{
    EqualizerBandPosition[band] = dBScale;
    dBConvertedScale[band] = convertdBScale(dBScale) * dBLevelScale;
}

void
EQ_Band::setEQLevelAmp(int16_t LevelAmp)
{
    EqualizerLevel = LevelAmp;
    dBLevelScale = convertdBScale(LevelAmp);
    for (int i = 0; i < MAX_EQ_BANDS; i++)
        dBConvertedScale[i] = convertdBScale(EqualizerBandPosition[i]) * dBLevelScale;
}

int16_t *
EQ_Band::computeFrequencyBoundariesInHz()
{
    int16_t i, cumulativeHz = 0;

    for (i = 0; i < eqParamInfo.numBands; i++) {
        int widthHz = eqParamInfo.sfbWidth[i] * eqParamInfo.frequency_resolution;
        cumulativeHz += widthHz;
        hzBoundary[i] = cumulativeHz;
    }

    return hzBoundary;
}

} // namespace draaldecoder
