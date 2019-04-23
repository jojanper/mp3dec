#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/iobuf.h"


TEST(TestSuite, Init)
{
    //FileBuf *fp = nullptr;
    Bit_Stream bs;
    //Bit_Stream *bs = nullptr;
    //uint8_t *buf = new uint8_t[10];
    //(void) buf;
    auto fp = new FileBuf();


    fp->OpenBuffer("foo.bs", kFileWriteMode);

    bs.open(fp, 32768);
    bs.putBits(2, 0);
    bs.putBits(2, 3);
    bs.putBits(4, 0);
    bs.putBits(2, 3);
    bs.putBits(6, 0);
    bs.putBits(8, 255);
    printf("DONE\n");
    bs.close();
    printf("DONE CLOSE\n");

    fp->CloseBuffer();

    fp->OpenBuffer("foo.bs", kFileReadMode);

    bs.open(fp, 32768);
    printf("%i\n", bs.getBits(2));
    printf("%i\n", bs.getBits(2));
    printf("%i\n", bs.getBits(4));
    printf("%i\n", bs.getBits(2));
    printf("%i\n", bs.getBits(6));
    printf("%i\n", bs.getBits(8));
    bs.close();

    fp->CloseBuffer();

    bool result = true;

    EXPECT_TRUE(result);

    //delete bs;
    delete fp;
}
