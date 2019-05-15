/**************************************************************************
  eqband.cpp - Equalizer implementations.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <math.h>
#include <stdlib.h>

/*-- Project Headers. --*/
#include "core/eqband.h"

static float
convertdBScale(int dBScale)
{
    return (float) (pow((double) 10.0f, (double) (dBScale) / 23.0f));
}

/**************************************************************************
  External Objects Provided
  *************************************************************************/

EQ_Band::EQ_Band() : sound_control(NO_CONTROL)
{
    this->resetEQBand();
}


void
EQ_Band::resetEQBand()
{
    for (int i = 0; i < MAX_SFB_BANDS; i++) {
        dBConvertedScale[i] = 1.0; // 0 dB
        EQBandScaleChanged[i] = FALSE;
        EqualizerBandPosition[i] = 0;
    }

    dBLevelScale = 1.0;
    EqualizerLevel = 0;
}


void
EQ_Band::setEQBand(int16 band, int16 dBScale)
{
    EqualizerBandPosition[band] = dBScale;
    dBConvertedScale[band] = convertdBScale(dBScale) * dBLevelScale;
}


void
EQ_Band::setEQLevelAmp(int16 LevelAmp)
{
    EqualizerLevel = LevelAmp;
    dBLevelScale = convertdBScale(LevelAmp);
    for (int i = 0; i < MAX_SFB_BANDS; i++)
        dBConvertedScale[i] = convertdBScale(EqualizerBandPosition[i]) * dBLevelScale;
}


int16 *
EQ_Band::computeFrequencyBoundariesInHz()
{
    int16 i, cumulativeHz = 0;
    static int16 HzBoundary[MAX_SFB_BANDS];

    for (i = 0; i < eqParamInfo.numBands; i++) {
        int widthHz = eqParamInfo.sfbWidth[i] * eqParamInfo.frequency_resolution;
        cumulativeHz += widthHz;
        HzBoundary[i] = cumulativeHz;
    }

    return (HzBoundary);
}
