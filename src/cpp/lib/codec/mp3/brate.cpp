/**************************************************************************
  brate.cpp - Bitrate and playback position implementations for mp3 decoder.

  Author(s): Juha Ojanpera
  Copyright (c) 1999-2000 Juha Ojanpera.
  *************************************************************************/

/*-- System Headers. --*/
#include <string.h>

/*-- Project Headers. --*/
#include "brate.h"
#include "layer.h"
#include "mp3.h"

/*
   Purpose:     If the bitrate remains the same after this value, the mp3
                stream is assumed to be using constant bitrate.
   Explanation: - */
#define VBR_THRESHOLD (150)

BrInfo::BrInfo(
    MP_Stream *mp,
    BOOL VBR_TagPresent,
    DWORD VBR_NumFrames,
    DWORD VBR_FileSize,
    BOOL IsStreaming)
{
    mp_internal = mp;
    start_bitrate = 0;
    memset(past_bitrate, 0, MAX_PAST_SIZE * sizeof(int16));
    curr_idx = 0;
    vbr = FALSE;
    free_br = FALSE;
    vbr_slots = 0.0;
    maxLength = 0;
    totalFrames = 0;

    /*
     * If we are streaming, seeking features are disabled.
     * Also the total playback time is unknown.
     */
    if (IsStreaming)
        return;

    vbr = VBR_TagPresent;
    vbr_slots = 1;
    free_br = FALSE;

    /*-- Check whether everything is still ok from the VBR stream point of view. --*/
    if (VBR_NumFrames == 0 || VBR_FileSize == 0)
        vbr = FALSE;

    /*
     * Check whether average bitrate needs to be computed or can we
     * rely on the VBR tag.
     */
    if (!vbr) {
        /*
         * No, this mp3 stream did not include VBR tag. So we have to get the
         * average bitrate by simply calculating it directly from the mp3 frames.
         */
        ComputeStartBitRate(mp, -1);
        ComputeTotalTime(mp);
    }
    else {
        /*-- Make sure that the calculations make some sense. --*/
        if (VBR_FileSize == 0)
            VBR_FileSize = 1;
        if (VBR_NumFrames == 0)
            VBR_NumFrames = 1;

        totalFrames = VBR_NumFrames;

        /*-- Count the size of mp3 payload. --*/
        vbr_slots = VBR_FileSize / VBR_NumFrames;

        /*-- This is average bitrate of this stream. --*/
        FLOAT div = (mp->header->version() == MPEG_PHASE2_LSF) ? 72. : 144.;
        start_bitrate = ((mp->header->frequency() / 1000.) * vbr_slots) / div;

        /*-- Estimated length. --*/
        maxLength = VBR_NumFrames * mp->header->GetFrameTickCount();
    }
}

/*
 * Returns the bitrate of the specified mp3 stream. The bitrate is either the
 * same as specified by the frame header or averaged on a long-term basis.
 * The average bitrate is used if the mp3 stream uses variable bitrate. The
 * bitrate needs to be very accurate since seeking and playback position
 * routines rely very heavily on this information.
 */
int16
BrInfo::GetBitRate(void)
{
    if (!vbr && !free_br) {
        FLOAT ave_br = 0.0f;

        /*
         * The bitrate is the average of the past 'MAX_PAST_SIZE' frames and
         * the long-term average bitrate. The new average bitrate becomes after
         * this operation the new long-term average bitrate. This is not the true
         * average bitrate but the deviation (from the true value) is not very large.
         */
        for (int i = 0; i < MAX_PAST_SIZE; i++)
            ave_br += past_bitrate[i];
        ave_br /= MAX_PAST_SIZE;

        ave_br = (ave_br + start_bitrate) / 2.0f;

        start_bitrate = (int16)(ave_br + 0.5);
    }

    return (start_bitrate);
}

/*
 * Determines the initial average bitrate of the specified mp3 stream. This can
 * be either the true average bitrate or an estimate of it, depending on the
 * 'lookahead_frames' parameter.
 */
BOOL
BrInfo::ComputeStartBitRate(MP_Stream *mp, int lookahead_frames)
{
    int i, br, obr, false_vbr_hit;
    DWORD cum_br = 0;
    BOOL result = TRUE, vbr_detected = FALSE;
    SEEK_STATUS seekStatus = SYNC_LOST;

    obr = mp->header->bit_rate();

    /*-- Free format, calculate the bitrate from the size of the payload. --*/
    if (obr == 0) {
        FLOAT tmp = mp->header->frequency() / 1000.0f;
        FLOAT div = (mp->header->version() == MPEG_PHASE2_LSF) ? 72. : 144.;
        start_bitrate = (int16)(tmp * GetFrameSlots(mp, GetBitRate()) / div);
        free_br = TRUE;
        goto exit;
    }

    /*
     * Decrease the size of the buffer. This is because we only read the
     * header and everything else is always skipped.
     */
    mp->bs->adjustBufferSize(25);

    false_vbr_hit = 0;
    for (i = 0; (i < lookahead_frames || lookahead_frames == -1) && i < 25000; i++) {
        /*-- Get the header and (optional) CRC codeword. --*/
        decode_header(mp);

        /*-- Skip the payload. --*/
        mp->bs->flushStream();
        mp->bs->seekStream(CURRENT_POS, GetTotalSlots(mp, i));

        /*-- Find the start of next frames. --*/
        seekStatus = SeekSync(mp);
        if (seekStatus != SYNC_FOUND) {
            result = FALSE;
            break;
        }

        /*
         * If the bitrate hasn't changed after 'VBR_DETECTED' frames,
         * we assume that the stream is using only fixed bitrate.
         */
        br = mp->header->bit_rate();
        if (obr != br) {
            /*
             * In case the stream is sligthly corrupted, this will stop
             * the decoding since there's no point of parsing the whole
             * stream even if the bitrate is changing. For truely variable
             * bitstreams, the bitrate will vary quite a lot already during
             * the first few frames.
             */
            false_vbr_hit++;

            vbr_detected = TRUE;
        }
        else if (i >= VBR_THRESHOLD && (vbr_detected == FALSE || false_vbr_hit < 5))
            break;

        obr = br;
        cum_br += br;
    }

    start_bitrate = (int16)(((FLOAT) cum_br / i) + 0.5);

    for (i = 0; i < MAX_PAST_SIZE; i++)
        past_bitrate[i] = start_bitrate;

    /*-- Bitstream buffer will be locked if EOF found, unlock it. --*/
    mp->bs->releaseBufferLock();
    mp->bs->flushStream();

    /*-- Restore the original size. --*/
    mp->bs->setDefaultBufferSize();

    /*-- Rewind the stream back to the beginning. --*/
    mp->bs->seekStream(START_POS, 0);

    /*
     * Find the start of the first frame again. This should not
     * cause any problems.
     */
    mp->syncInfo.sync_status = (SYNC_STATUS)(mp->syncInfo.sync_status + (SYNC_STATUS) 3);
    SeekSync(mp);
    mp->syncInfo.sync_status = (SYNC_STATUS)(mp->syncInfo.sync_status - (SYNC_STATUS) 3);

exit:

    return (result);
}

/*
 * Returns the total playback time.
 */
DWORD
BrInfo::ComputeTotalTime(MP_Stream *mp)
{
    if (!vbr) {
        /*-- Compute the size of the payload. --*/
        FLOAT ave_slots = GetFrameSlots(mp, GetBitRate());

        /*-- Count the number of frames. --*/
        FLOAT frames = (mp->bs->getIOHandle()->GetStreamSize()) / ave_slots;

        totalFrames = (uint32)(frames + 0.5);

        /*-- Estimated length. --*/
        maxLength = (uint32)(frames * mp->header->GetFrameTime());
    }

    return (maxLength);
}

/*
 * Returns the current playback time.
 */
DWORD
BrInfo::GetCurrTime(void)
{
    FLOAT ave_slots;

    /*-- Compute the size of the payload. --*/
    if (!vbr)
        ave_slots = GetFrameSlots(mp_internal, GetBitRate());
    else
        ave_slots = vbr_slots;

    /*-- Count the number of frames. --*/
    FLOAT frames = mp_internal->bs->getCurrStreamPos() / ave_slots;

    /*-- Elapsed time. --*/
    return (DWORD)(frames * mp_internal->header->GetFrameTime());
}
