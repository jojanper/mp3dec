#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./parse.h"
#include "core/defines.h"
#include "core/io/console.h"
#include "interface/attributes.h"
#include "interface/defs.h"

int
main(int argc, char **argv)
{
    FILE *fp;
    Console *console = new Console();

    BOOL waveOut = FALSE;
    char *inStream, *outStream;
    if (!ParseMPCommandLine(&inStream, &outStream, &waveOut, argc, argv, 22, nullptr))
        return EXIT_FAILURE;

    // Open input file
    fp = fopen(inStream, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open file %s\n", inStream);
        return EXIT_FAILURE;
    }

    // Size of input data buffer
    auto size = 16384;

    // Prepare decoder
    auto attrs = draaldecoder::IAttributes::create();
    attrs->setString("mime", draaldecoder::MP3MIME);
    attrs->setInt32Data(draaldecoder::kBufferSize, size);
    attrs->setInt32Data(draaldecoder::kBufferMode, draaldecoder::kModuloBuffer);

    printf("OK -4\n");

    auto dec = draaldecoder::StreamableDecoder::create(*attrs);
    if (!dec) {
        fprintf(stderr, "Unable to create decoder instance\n");
        return EXIT_FAILURE;
    }

    printf("OK -3\n");

    // Read entire file into memory
    auto *buffer = new uint8_t[size];
    size = fread(buffer, sizeof(uint8_t), size, fp);

    printf("OK -2.0\n");

    if (!dec->init(*attrs, buffer, size)) {
        fprintf(stderr, "Unable to initialize decoder for file %s\n", inStream);
        return EXIT_FAILURE;
    }

    printf("OK -2.2\n");

    // Open output (file)
    if (!console->open(outStream, 44100, 2, waveOut))
        return EXIT_FAILURE;

    size_t frames = 0;
    bool eos = false;

    fprintf(stdout, "\n");
    fflush(stdout);

    printf("OK -2\n");

    FILE *fp2 = fopen("../test.raw", "rb");

    int32_t pos = 0;

    // int k = 0;
    // for (int i = -256; i < 118 + 4; ++i, k++)
    //  printf("%20E,\n", pow((double) 2.0, -0.25 * (double) (i + 210.0)));
    // printf("%i %20E %i\n", i, pow((double) 2.0, -0.25 * (double) (i + 210.0)), k);

#if 0
    for (int i = 0; i <= 8207; ++i) {
        if (i && i % 10 == 0)
            printf("\n");
        printf("%20E,", pow((double) i, ((double) 4.0 / 3.0)));
    }

    getchar();
#endif

    printf("OK -1\n");

    // Decode until end of stream found
    do {
        bool result = true;

        // Decode until no more audio available
        do {
            result = dec->decode();
            if (result) {
                size_t audioSize;
                auto data = dec->getDecodedAudio(audioSize);
                if (data) {
#if 1
                    int16_t audio[16384];

                    printf("OK 0\n");

                    /*auto n =*/fread(audio, sizeof(int16_t), audioSize, fp2);

                    /*
                    for (size_t i = 0; i < n; i++)
                        if (audio[i] != 0)
                            printf("DIFF: %zu %i\n", i, audio[i]);
                            */

                    bool diff = false;
                    size_t count = 0;
                    for (size_t i = 0; i < audioSize; i++) {
                        int audioDiff = abs(data[i] - audio[i]);
                        if (audioDiff > 2) {
                            count++;
                            /*
                            printf(
                                "%zu %i %i %zu %i %i\n",
                                i,
                                data[i],
                                audio[i],
                                n,
                                pos,
                                pos / 2304);
                                */
                            // getchar();
                            diff = true;
                        }
                    }

                    if (diff) {
                        printf("%i %i %zu\n", pos, pos / 2304, count);
                        getchar();
                    }
#endif

                    console->writeBuffer(data, audioSize);
                    pos += audioSize;
                }

                fprintf(stdout, "Frames decoded: %zu\n", frames++);
                fflush(stdout);
            }

        } while (result);

        if (!eos) {
            auto fsize = size;

            // Read new audio data
            size = fread(buffer, sizeof(uint8_t), fsize, fp);
            if (size)
                dec->addInput(buffer, size);

            // Unable to read full data buffer -> Enf of file reached
            if (size != fsize) {
                eos = true;
                dec->setEndOfStream();

                // Decode what is available and then exit the loop
                size = 0;
                continue;
            }
        }

        // if (frames > 2500)
        //  break;

    } while (size);

    fprintf(stdout, "\n");
    fflush(stdout);

    delete[] buffer;

    attrs->destroy();
    dec->destroy();

    console->close();
    delete console;

    fclose(fp);

    return EXIT_SUCCESS;
}
