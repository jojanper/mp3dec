#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/iobuf.h"

static const char *filename = "foo.bs";

TEST(BitStreamTestSuite, Init)
{
    Bit_Stream bs;
    auto fp = new FileBuf();

    fp->open(filename, kFileWriteMode);

    bs.open(fp, 32768);
    bs.putBits(2, 0);
    bs.putBits(2, 3);
    bs.putBits(4, 0);
    bs.putBits(2, 3);
    bs.putBits(6, 0);
    bs.putBits(8, 255);
    // printf("DONE\n");
    bs.close();
    // printf("DONE CLOSE\n");

    fp->close();

    fp->open(filename, kFileReadMode);

    bs.open(fp, 2);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 0);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 3);
    EXPECT_EQ(bs.getBits(4), (uint32_t) 0);
    EXPECT_EQ(bs.getBits(2), (uint32_t) 3);
    EXPECT_EQ(bs.getBits(6), (uint32_t) 0);
    EXPECT_EQ(bs.getBits(8), (uint32_t) 255);

    EXPECT_EQ(bs.getBits(27), (uint32_t) 0);
    EXPECT_TRUE(bs.endOfStream());

    bs.close();

    fp->close();
    delete fp;
}
