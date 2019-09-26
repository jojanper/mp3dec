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

    auto dec = draaldecoder::StreamableDecoder::create(*attrs);
    if (!dec) {
        fprintf(stderr, "Unable to create decoder instance\n");
        return EXIT_FAILURE;
    }

    // Read entire file into memory
    auto *buffer = new uint8_t[size];
    size = fread(buffer, sizeof(uint8_t), size, fp);

    if (!dec->init(*attrs, buffer, size)) {
        fprintf(stderr, "Unable to initialize decoder for file %s\n", inStream);
        return EXIT_FAILURE;
    }

    // Open output (file)
    if (!console->open(outStream, 44100, 2, waveOut))
        return EXIT_FAILURE;

    size_t frames = 0;
    bool eos = false;

    fprintf(stdout, "\n");
    fflush(stdout);

    // Decode until end of stream found
    do {
        bool result = true;

        // Decode until no more audio available
        do {
            result = dec->decode();
            if (result) {
                size_t audioSize;
                auto data = dec->getDecodedAudio(audioSize);
                if (data)
                    console->writeBuffer(data, audioSize);

                fprintf(stdout, "Frames decoded: %zu\r", frames++);
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
