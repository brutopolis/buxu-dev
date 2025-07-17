#include "rawer.h"

//<functions>
init(std);



int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    BruterList *context = bruter_new(8, true, true);

    //<functions_init>
    init_std(context); // Initialize the standard functions

    if (strcmp(argv[1], "eval") == 0)
    {
        BruterList *result = parse(context, argv[2]);
    }
    else if (strcmp(argv[1], "run") == 0)
    {
        char* filename = argv[2];
        FILE *file = fopen(filename, "r");
        if (!file)
        {
            fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
            return EXIT_FAILURE;
        }
        // read the entire file into a string
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *input_str = malloc(length + 1);
        if (!input_str)
        {
            fprintf(stderr, "ERROR: Could not allocate memory for input string\n");
            fclose(file);
            return EXIT_FAILURE;
        }
        fread(input_str, 1, length, file);
        input_str[length] = '\0'; // null-terminate the string
        fclose(file);
        BruterList *result = parse(context, input_str);
        bruter_free(result); // Free the result list after use
        free(input_str); // Free the input string after use
    }
    else
    {
        fprintf(stderr, "ERROR: Unknown command '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }
    clear_context(context); // Clear the context to free allocated memory
    bruter_free(context); // Free the context after use
    return EXIT_SUCCESS;
}
