/**************************************************************************
  mpheader.h - MPEG-1/MPEG-2/MPEG-2.5 frame header interface.

  Author(s): Juha Ojanpera
  Copyright (c) 1999 Juha Ojanpera.
  *************************************************************************/

#ifndef MPHEADER_H_
#define MPHEADER_H_

/*-- Project Headers. --*/
#include "core/defines.h"

/*-- MPEG Header Definitions - Mode Values --*/
#define MPG_MD_STEREO (0)
#define MPG_MD_JOINT_STEREO (1)
#define MPG_MD_DUAL_CHANNEL (2)
#define MPG_MD_MONO (3)

/*
   Purpose:     Masks those bit fields from the header to zero that
                do not remain fixed from frame to frame.
   Explanation: Following fields are supposed to be fixed :
                 * 12th sync bit
                 * version
                 * layer description
                 * sampling rate
                 * channel mode (layer 3 only)
                 * copyright bit
                 * original bit
                 * de-emphasis

                Following fields can vary from frame to frame :
                 * protection bit
                 * bit rate
                 * padding bit
                 * private bit
                 * channel mode extension
                */
#define HEADER_MASK2(header) ((uint32) header & 0x001E0C0F)
#define HEADER_MASK3(header) ((uint32) header & 0x001E0CCF)

/*
   Purpose:     Macro to extract layer description.
   Explanation: This is the bit value, use MP_Header::layer_number method
                to interpret this value. */
#define LAYER_MASK(header) (((uint32) header >> 17) & 3)


class MP_Header
{
public:
    MP_Header() : header(0), crc(0) {}
    ~MP_Header() {}

    /*-- Public methods. --*/
    inline int32 SetHeader(int32 header)
    {
        int32 tmp = this->header;
        this->header = header;
        return tmp;
    }

    uint32 GetFrameTime() const;
    FLOAT GetFrameTickCount() const;

    /*-- Methods to parse header bit fields. --*/

    // MPEG-1 (1) or MPEG-2 (0)
    inline int version() const { return ((header >> 19) & 1); }

    // MPEG-2.5
    inline BOOL mp25version() const { return ((header & 0x100000) ? FALSE : TRUE); }

    // Layer number (1, 2 or 3)
    inline int layer_number() const { return (int) ((4 - ((header >> 17) & 3))); }

    // Layer number as a string (I, II or III)
    const char *layer_string() const;

    // Error protection bit.
    inline BOOL error_protection() const { return (!((header >> 16) & 1)); }

    // Bit rate of stream.
    int bit_rate() const;

    // Bit rate index.
    inline int bit_rate_idx() const { return ((header >> 12) & 0xF); }

    // Table holding legal bit rates for this stream.
    const int *GetBitRateTable() const;

    // Sampling frequency.
    int32 frequency() const;

    // Sampling frequency index.
    inline int sfreq() const { return (int) (header >> 10) & 3; }

    // Padding bit.
    inline BOOL padding() const { return (BOOL)((header >> 9) & 1); }

    // Private bit.
    inline BOOL private_bit() const { return (BOOL)((header >> 8) & 1); }

    // Value of mode field.
    inline int mode() const { return (int) ((header >> 6) & 3); }

    // Type of stream (mono, stereo, joint or dual).
    const char *mode_string() const;

    // Value of mode extension.
    inline int mode_extension() const { return (int) ((header >> 4) & 3); }

    // Copyright bit.
    inline BOOL copyright() const { return (BOOL)((header >> 3) & 1); }

    // Original bit.
    inline BOOL original() const { return (BOOL)((header >> 2) & 1); }

    // Name of de-emphasis used.
    const char *de_emphasis() const;

    /*-- Methods to interprete header parameters. --*/

    // Number of channels; 1 for mono, 2 for stereo
    inline int channels() const { return ((mode() == MPG_MD_MONO) ? 1 : 2); }

    // Layer I and II specific methods.
    int jsbound();
    int subbands();

    // Layer II only specific methods.
    inline int GetLayer2TableIdx() const { return Layer_II_alloc_table; }
    inline int GetLayer2Alloc() const { return Layer_II_b_alloc; }

    uint32 header; // MPx header bits.
    int16 crc;     // Optional CRC error-check word.

private:
    int pick_table();

    int Layer_II_alloc_table;
    int Layer_II_b_alloc;
};

#endif /* MPHEADER_H_ */
