#ifndef RAWER_H
#define RAWER_H 1

#include <bruter.h>

#include <ctype.h>

#define RAWER_VERSION "0.0.3"

enum {
    BR_TYPE_NULL = 0,
    BR_TYPE_ANY,
    BR_TYPE_FLOAT,
    BR_TYPE_BUFFER,
    BR_TYPE_LIST,
    BR_TYPE_FUNCTION,
};

#define function(name) \
    void name(BruterList *stack)

#define init(name) \
    void init_##name(BruterList *context)

#define parser_step(name) \
    bool name(BruterList *context, BruterList *stack, BruterList *splited, BruterInt word_index)

typedef void (*Function)(BruterList *stack);

typedef bool (*ParserStep)(BruterList *context, BruterList *stack, BruterList *splited, BruterInt word_index);

static inline void clear_context(BruterList *context)
{
    for (BruterInt i = 0; i < context->size; i++)
    {
        switch (context->types[i])
        {
            case BR_TYPE_BUFFER:
                free(context->data[i].p);
                break;
            case BR_TYPE_LIST:
                bruter_free((BruterList*)context->data[i].p);
                break;
            case BR_TYPE_FUNCTION:
                // No need to free function pointers
                break;
            default:
                // For other types, no specific cleanup is needed
                break;
        }

        if (context->keys && context->keys[i])
        {
            free(context->keys[i]); // Free the key if it was allocated
            context->keys[i] = NULL; // Set to NULL to avoid dangling pointers
        }
    }
    context->size = 0; // Reset the size to 0
}

static inline BruterList* string_split(char *input_str)
{
    if (input_str == NULL || *input_str == '\0')
    {
        return bruter_new(0, false, false); // Return an empty list if input is NULL or empty
    }

    char* str = strdup(input_str);
    if (!str) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    BruterList *stack = bruter_new(2, false, false);
    char *ptr = str;
    bruter_push_pointer(stack, str, NULL, 0); // Push the original string for cleanup later

    int recursion = 0;
    while (*ptr != '\0')
    {
        if (isspace((unsigned char)*ptr) && recursion == 0)
        {
            // break word
            *ptr = '\0'; 
            ptr++;

            // skip spaces
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;

            if (*ptr != '\0')
            {
                // Push the current token onto the stack
                bruter_push_pointer(stack, ptr, NULL, 0);
            }
            else
            {
                // If we hit the end of the string, we can break
                break;
            }

            continue;
        }
        else if (*ptr == '(')
        {
            recursion++;
        }
        else if (*ptr == ')')
        {
            recursion--;
            if (recursion < 0)
            {
                fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
                exit(EXIT_FAILURE);
            }
        }

        ptr++;
    }

    if (recursion != 0)
    {
        fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
        exit(EXIT_FAILURE);
    }

    return stack;
}

static inline BruterList* parse(BruterList *context, char* input_str)
{
    BruterInt found = bruter_find_key(context, "parsers");
    if (found == -1)
    {
        fprintf(stderr, "ERROR: No parsers found in context\n");
        return NULL;
    }
    BruterList *parsers = bruter_get_pointer(context, found);

    BruterList *splited = string_split(input_str);
    BruterList *result = bruter_new(8, true, true);

    if (splited->size == 0)
    {
        return NULL;
    }
    char* original_str = (char*)splited->data[0].p; // Keep the original string for cleanup

    for (int i = 0; i < splited->size; i++)
    {
        for (BruterInt j = 0; j < parsers->size; j++)
        {
            ParserStep step = bruter_get_pointer(parsers, j);
            if (step(context, result, splited, i))
            {
                break; // If a parser matched, break to the next token
            }
        }
    }
    free(original_str); // free the original string
    bruter_free(splited); // free the splited list
    return result;
}

#endif // RAWER_H macro