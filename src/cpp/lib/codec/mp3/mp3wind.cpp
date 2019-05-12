/**************************************************************************
  mp3wind.cpp - Seeking subroutines for MPEG audio player.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/**************************************************************************
  External Objects Needed
  *************************************************************************/

/*-- Project Headers. --*/
#include "brate.h"
#include "layer.h"
#include "mp3.h"
#include "mstream.h"

/**************************************************************************
  Internal Objects
  *************************************************************************/

/**************************************************************************
  Title        : GetSideInfoSlots

  Purpose      : Retrieves the amount of side info for layer III stream.

  Usage        : y = GetSideInfoSlots(header, layerNum)

  Input        : header   - header bits
                 layerNum - layer number

  Output       : y - # of side info in bytes

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
GetSideInfoSlots(MP_Header *header, int layerNum)
{
    int nSlots = 0;

    if (layerNum == 3) {
        if (header->version() != MPEG_PHASE2_LSF)
            nSlots = (header->channels() == 1) ? 17 : 32;
        else
            nSlots = (header->channels() == 1) ? 9 : 17;
    }

    return (nSlots);
}


/**************************************************************************
  Title        : InitAfterSeek

  Purpose      : Refills the bit reservoir buffer before the decoding can
                 continue.

  Usage        : y = InitAfterSeek(mp, main_data)

  Input        : mp        - mp3 stream parameters
                 main_data - max. size of "main data"

  Output       : y - TRUE on success, FALSE otherwise

  Explanation  : Remember that each frame has a pointer that can refer to
                 previous frames. So in order to quarantee that decoding
                 operates correctly after seeking, the content of the bit
                 reservoir buffer must contain valid samples.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static BOOL
InitAfterSeek(MP_Stream *mp, int main_data)
{
    if (mp->header->layer_number() == 3) {
        int readSlots = 0;

        mp->WasSeeking = TRUE;

        /*-- Let's start from the begin within the buffer. --*/
        mp->br->reset();

        /*
         * Refill the bit reservoir buffer so that
         * 'main_data_begin' always points to a valid location.
         */
        for (; readSlots < main_data;) {
            int main_slots;

            /*-- Get the header and (optional) CRC codeword. --*/
            decode_header(mp);

            /*-- Skip the side info. --*/
            mp->bs->skipBits(GetSideInfoSlots(mp->header, 3) << 3);

            readSlots += (main_slots = mp->main_data_slots());

            /*-- Put the payload to the bit reservoir buffer. --*/
            mp->br->addBytes(*mp->bs, main_slots);

            /*-- Skip the bytes what we just read in. --*/
            mp->br->skipBits(main_slots << 3);

            /*-- Find the start of the next frame. --*/
            SEEK_STATUS seekStatus = SeekSync(mp);
            if (seekStatus == SYNC_LOST || seekStatus == EOF_STREAM)
                return (FALSE);
        }
    }

    return (TRUE);
}


/**************************************************************************
  Title        : ExtraDataSlots

  Purpose      : Counts how many extra bytes each free format stream needs in
                 addition to the predetermined payload.

  Usage        : y = ExtraDataSlots(header, lNum)

  Input        : header - hader bits
                 lNum   - layer number

  Output       : y - # of extra bytes

  Explanation  : Use this function only with free format mp3 streams.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

static inline int
ExtraDataSlots(MP_Header *header, int lNum)
{
    int nSlots = 0;

    if (lNum == 3)
        nSlots = GetSideInfoSlots(header, lNum);

    if (header->error_protection())
        nSlots += 2;

    nSlots += 4;

    return (nSlots);
}


/**************************************************************************
  Title        : GetFrameSlots

  Purpose      : Returns the number of bytes reserved for each frame.

  Usage        : y = GetFrameSlots(mp, br)

  Input        : mp - MPx parameters
                 br - bitrate of the stream. For VBR streams, this value
                      represents the average bitrate. Used only in layer 3

  Output       : y - (average) # of bytes for each frame

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
GetFrameSlots(MP_Stream *mp, int16 br)
{
    int nSlots, layer;

    layer = mp->header->layer_number();

    /*-- Count how many bytes each frame need. --*/
    if (mp->header->bit_rate()) {
        FLOAT reciprocal = 1000. / mp->header->frequency();

        if (layer == 1)
            nSlots = (int) (12 * mp->header->bit_rate() * reciprocal);
        else if (layer == 2)
            nSlots = (int) (144 * mp->header->bit_rate() * reciprocal);
        else {
            nSlots = (int) (144 * br * reciprocal);

            if (mp->header->version() == MPEG_PHASE2_LSF)
                nSlots >>= 1;
        }
    }
    else
        nSlots = mp->FreeFormatSlots + ExtraDataSlots(mp->header, layer);

    return (nSlots);
}


/**************************************************************************
  Title        : ReSync

  Purpose      : Finds the next header that contains valid information.

  Usage        : y = ReSync(mp)

  Input        : mp - MPEG audio stream parameters

  Output       : y - TRUE on success, FALSE otherwise (= stream is undecodable)

  Explanation  : When the decoder will detect invalid bitstream syntax, the stream
                 is assumed to be corrupted and that the sync has been
                 (at least temporarily) lost. The purpose of this function is
                 therefore to recover the decoder from the error so that decoding
                 can continue.

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
ReSync(MP_Stream *mp)
{
    if (SeekSync(mp) != SYNC_FOUND)
        return (FALSE);

    int main_data = (mp->header->version() == MPEG_PHASE2_LSF) ? 256 : 512;
    if (InitAfterSeek(mp, main_data) == FALSE)
        return (FALSE);

    return (TRUE);
}


/**************************************************************************
  Title        : WindFrame

  Purpose      : Seeks forward or backward the specified MPEG audio stream.

  Usage        : y = WindFrame(mp, numFrames, direction)

  Input        : mp        - MPEG audio stream parameters
                 numFrames - # of frames to jump
                 direction - if TRUE jump forward, otherwise jump backward

  Output       : y - TRUE on success, FALSE otherwise (= stream is undecodable)

  Author(s)    : Juha Ojanpera
  *************************************************************************/

BOOL
WindFrame(MP_Stream *mp, BrInfo *brInfo, int numFrames, BOOL direction)
{
    int32 byte_offset;

    /*-- Advance the stream pointer to current position. --*/
    mp->bs->flushStream();

    byte_offset = GetFrameSlots(mp, brInfo->GetBitRate());

    int main_data = (mp->header->version() == MPEG_PHASE2_LSF) ? 256 : 512;
    int frameOffset = main_data / byte_offset;
    if (main_data % byte_offset)
        frameOffset++;

    /*-- Total offset. --*/
    if (direction == TRUE)
        byte_offset *= (numFrames - frameOffset);
    else
        byte_offset *= (numFrames + frameOffset);

    /*-- Check that we are jumping to a valid location. --*/
    int32 currentPos = mp->bs->seekStream(CURRENT_POS, 0);

    /*-- Seek forward. --*/
    if (direction == TRUE) {
        uint32 stream_size = mp->bs->getStreamSize();

        /*-- Jump by offset. --*/
        if ((uint32)(currentPos + byte_offset) < stream_size)
            mp->bs->seekStream(CURRENT_POS, byte_offset);
        else
            mp->bs->seekStream(CURRENT_POS, stream_size - 1000);

        /*-- Find the start of the next frame. --*/
        SEEK_STATUS seekStatus = SeekSync(mp);
        if (seekStatus == SYNC_LOST || seekStatus == EOF_STREAM)
            return (FALSE);

        if (InitAfterSeek(mp, main_data) == FALSE)
            return (FALSE);
    }

    /*-- Seek backward. --*/
    else {
        if ((currentPos - byte_offset) > 0) {
            /*-- Jump by offset. --*/
            mp->bs->seekStream(CURRENT_POS, -byte_offset);

            /*-- Find the start of the next frame. --*/
            SEEK_STATUS seekStatus = SeekSync(mp);
            if (seekStatus == SYNC_LOST || seekStatus == EOF_STREAM)
                return (FALSE);

            if (InitAfterSeek(mp, main_data) == FALSE)
                return (FALSE);
        }
        else {
            /*-- Jump to begin. --*/
            mp->bs->seekStream(START_POS, 0);

            /*-- Find the start of the stream. --*/
            SEEK_STATUS seekStatus = SeekSync(mp);
            if (seekStatus == SYNC_LOST || seekStatus == EOF_STREAM)
                return (FALSE);

            /*-- Reset some variables. --*/
            mp->br->reset();
            mp->SkipBr = FALSE;
            mp->PrevSlots = 0;
            mp->WasSeeking = FALSE;
        }
    }

    return (TRUE);
}


/**************************************************************************
  Title        : GetTotalSlots

  Purpose      : Determines the frame size for the specified MPEG audio stream.

  Usage        : y = GetTotalSlots(mp, first_call)

  Input        : mp         - MPEG audio stream paramters
                 first_call - (see the code below for detailed info)

  Output       : y - frame size in bytes

  Author(s)    : Juha Ojanpera
  *************************************************************************/

int
GetTotalSlots(MP_Stream *mp, int first_call)
{
    int nSlots = 4;
    static FLOAT reciprocal = 1.0f;

    if (first_call == 0)
        reciprocal = 1000. / mp->header->frequency();

    switch (mp->header->layer_number()) {
        case 1:
            nSlots = (int) (12 * mp->header->bit_rate() * reciprocal);
            break;

        case 2:
            nSlots = (int) (144 * mp->header->bit_rate() * reciprocal);
            break;

        case 3:
            if (mp->header->bit_rate()) {
                nSlots = (int) (144 * mp->header->bit_rate() * reciprocal);
                if (mp->header->version() == MPEG_PHASE2_LSF)
                    nSlots >>= 1;
            }
            else
                nSlots = mp->FreeFormatSlots;
            break;
    }

    nSlots -= 4;

    return (nSlots);
}
