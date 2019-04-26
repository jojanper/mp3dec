#include <gtest/gtest.h>

#include "core/bits.h"
#include "core/iobuf.h"

static const char *filename = "foo.bar";

TEST(FileBufTestSuite, InvalidData)
{
    FileBuf fb;

    EXPECT_FALSE(fb.OpenBuffer(filename, 100));
    EXPECT_FALSE(fb.CanWrite());
    EXPECT_EQ(fb.GetStreamSize(), (uint32_t) 0);
    EXPECT_EQ(fb.SeekBuffer(CURRENT_POS, 10), (int32_t) 0);
    EXPECT_EQ(fb.ReadToBuffer(nullptr, 10), (uint32_t) 0);
    EXPECT_EQ(fb.WriteFromBuffer(nullptr, 10), (uint32_t) 0);
    EXPECT_STREQ(fb.GetFileBufName(), filename);
}

TEST(FileBufTestSuite, ReadWrite)
{
    FileBuf fb;
    uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    EXPECT_TRUE(fb.OpenBuffer(filename, kFileWriteMode));
    EXPECT_EQ(fb.WriteFromBuffer(data, sizeof(data)), sizeof(data));
    fb.CloseBuffer();

    uint8_t data2[10];
    EXPECT_TRUE(fb.OpenBuffer(filename, kFileReadMode));
    EXPECT_EQ(fb.ReadToBuffer(data2, sizeof(data2)), sizeof(data2));

    for (size_t i = 0; i < sizeof(data); i++)
        EXPECT_EQ(data[i], data2[i]) << "Index " << i;

    int32_t offset = 3;
    EXPECT_EQ(fb.SeekBuffer(START_POS, 0), (int32_t) 0);
    EXPECT_EQ(fb.SeekBuffer(CURRENT_POS, offset), offset);
    EXPECT_EQ(fb.ReadToBuffer(data2, sizeof(data2) - offset), sizeof(data2) - offset);

    for (size_t i = offset; i < sizeof(data); i++)
        EXPECT_EQ(data[i], data2[i - offset]) << "Index " << i;

    EXPECT_EQ(fb.GetStreamSize(), (uint32_t) sizeof(data));
    // EXPECT_EQ(fb.SeekBuffer(START_POS, 0), (int32_t) 0);

    offset = 2;
    EXPECT_EQ(fb.SeekBuffer(END_POS, offset), (int32_t)(sizeof(data) - offset));
    EXPECT_EQ(fb.ReadToBuffer(data2, (uint32_t) offset), (uint32_t) offset);
    // printf("DATA: %i %i %i %i\n", data2[0], data2[1], data[8], data[9]);
    for (size_t i = sizeof(data) - offset; i < sizeof(data); i++)
        EXPECT_EQ(data[i], data2[i - (sizeof(data) - offset)])
            << "Index " << i << " " << i - (sizeof(data) - offset);

    fb.CloseBuffer();

    EXPECT_TRUE(fb.OpenBuffer(filename, kFileAppendMode));
    EXPECT_EQ(fb.SeekBuffer(END_POS, 0), (int32_t) sizeof(data));
    EXPECT_EQ(fb.WriteFromBuffer(data, sizeof(data)), sizeof(data));
    EXPECT_EQ(fb.GetStreamSize(), (uint32_t) 2 * sizeof(data));
    fb.CloseBuffer();
}


TEST(TestSuite, Init)
{
    // FileBuf *fp = nullptr;
    Bit_Stream bs;
    // Bit_Stream *bs = nullptr;
    // uint8_t *buf = new uint8_t[10];
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

    // delete bs;
    delete fp;
}
