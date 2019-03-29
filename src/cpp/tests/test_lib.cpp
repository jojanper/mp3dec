#include <gtest/gtest.h>

#include "core/bits.h"


TEST(TestSuite, Init)
{
    Bit_Stream *bs = nullptr;
    //uint8_t *buf = new uint8_t[10];
    //(void) buf;

    bool result = true;

    EXPECT_TRUE(result);

    delete bs;
}
