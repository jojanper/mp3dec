#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/iobuf.h"


size_t
createTestBitstream(const char *filename)
{
    BitStream bs;
    FileBuf fp;

    fp.open(filename, kFileWriteMode);

    // Write some bits to bitstream
    bs.open(&fp, 32768);
    bs.putBits(2, 1);
    bs.putBits(2, 3);
    bs.putBits(4, 14);
    bs.putBits(2, 3);
    bs.putBits(6, 0);
    bs.putBits(8, 255);
    bs.putBits(12, 123);
    bs.close();
    fp.close();

    return 5; // 5 bytes added to file
}
