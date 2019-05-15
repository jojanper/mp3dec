/**************************************************************************
  eqband.h - Equalizer interface.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

#ifndef EQBAND_H_
#define EQBAND_H_

/*- Project Headers. --*/
#include "core/defines.h"
/*
   Purpose:     Control schemes for the equalizer.
   Explanation: - */
typedef enum SOUND_CONTROL
{
    NO_CONTROL,
    ROCK,
    POP,
    DISCO,
    JAZZ

} SOUND_CONTROL;

/*
   Purpose:     Information about the frequency bands of the current stream.
                This is needed when the user wants to know what is the
                frequency resolution and corresponding frequencies (in Hz)
                of each band.
   Explanation: This structure is used only by the graphical equalizer. */
typedef struct EQParamInfoStr
{
    int16 *sfbWidth;
    int16 numBands;
    float frequency_resolution;

} EQParamInfo;

/*
   Purpose:     Maximum number of equalizer bands.
   Explanation: The graphical equalizer uses only the 1st 20 bands. */
#define MAX_SFB_BANDS (51)

class EQ_Band
{
public:
    EQ_Band();
    ~EQ_Band() {}

    /*-- Public Methods. --*/
    void resetEQBand(void);
    void setEQBand(int16 band, int16 dBScale);
    void setEQLevelAmp(int16 LevelAmp);
    int16 *computeFrequencyBoundariesInHz();
    FLOAT *getdBScale() { return dBConvertedScale; }

    /*-- Public properties. --*/
    SOUND_CONTROL sound_control;
    EQParamInfo eqParamInfo;

private:
    /*-- Private properties. --*/
    FLOAT dBConvertedScale[MAX_SFB_BANDS];
    FLOAT dBLevelScale;
    BOOL EQBandScaleChanged[MAX_SFB_BANDS];
    int16 EqualizerBandPosition[MAX_SFB_BANDS];
    int16 EqualizerLevel;
};

#endif /* EQBAND_H_*/
