#include "bruter.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    BruterList *context = bruter_new(8, true, true);

    const char *embedded_code =
    ;

    BruterList *result = bruter_parse(context, embedded_code, NULL);
    bruter_free(result);
    bruter_free(context);
    return EXIT_SUCCESS;
}
