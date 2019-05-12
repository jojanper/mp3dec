#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mstream.h"

int
main(int /*argc*/, char ** /*argv*/)
{
    BitStream *bs = new BitStream();
    MP_Stream *stream = new MP_Stream();

#if 0
    /*-- Open the bitstream. --*/
#define MAX_SLOTS ((MAX_FRAME_SLOTS << 1) + 1)
    result = bs->Open(hInst, mp_stream, READ_MODE, MAX_SLOTS, sBuf);
    if (result == FALSE)
        return (result);
#endif

    delete stream;
    delete bs;

    return EXIT_SUCCESS;
}
