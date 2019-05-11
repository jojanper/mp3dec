#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mstream.h"

int
main(int /*argc*/, char ** /*argv*/)
{
    MP_Stream *stream = new MP_Stream();

    delete stream;

    return EXIT_SUCCESS;
}
