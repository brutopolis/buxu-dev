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
    BruterList *splited = string_split(input_str);
    BruterList *result = bruter_new(8, true, true);

    if (splited->size == 0)
    {
        return NULL;
    }
    char* original_str = (char*)splited->data[0].p; // Keep the original string for cleanup

    for (int i = 0; i < splited->size; i++)
    {
        char *token = (char*)splited->data[i].p;

        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) // number
        {
            if (token[0] == '0' && token[1] == 'x') // hex
            {
                bruter_push_int(result, strtol(token+2, NULL, 16), NULL, BR_TYPE_ANY);
            }
            else if (token[0] == '0' && token[1] == 'b') // bin
            {
                bruter_push_int(result, strtol(token+2, NULL, 2), NULL, BR_TYPE_ANY);
            }
            else if (token[0] == '0' && token[1] == 'o') // oct
            {
                bruter_push_int(result, strtol(token+2, NULL, 8), NULL, BR_TYPE_ANY);
            }
            else if (strchr(token, '.')) // float
            {
                bruter_push_float(result, strtof(token, NULL), NULL, BR_TYPE_FLOAT);
            }
            else // int
            {
                bruter_push_int(result, strtol(token, NULL, 10), NULL, BR_TYPE_ANY);
            }
        }
        else if (token[0] == '(') // list
        {
            char *sub_str = token + 1; // Skip the '('
            char *end_ptr = strchr(sub_str, ')');
            if (end_ptr == NULL)
            {
                fprintf(stderr, "ERROR: Unmatched parentheses in list\n");
                exit(EXIT_FAILURE);
            }
            *end_ptr = '\0'; // Null-terminate the list string
            BruterList *parsed = parse(context, sub_str);
            if (parsed->size > 1)
            {
                bruter_push_pointer(result, parsed, NULL, BR_TYPE_LIST);
            }
            else if (parsed->size == 1)
            {
                // If the list has only one item, push it directly
                bruter_push_meta(result, bruter_pop_meta(parsed));
                bruter_free(parsed); // Free the parsed list after use
            }
            else
            {
                bruter_push_pointer(result, bruter_new(0, false, false), NULL, BR_TYPE_LIST);
            }
        }
        else if (token[0] == ':') //string 
        {
            if (token[1] == '(') 
            {
                char *str_value = token + 2; // Skip the ':('
                char *end_ptr = strchr(str_value, ')');
                if (end_ptr == NULL)
                {
                    fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
                    exit(EXIT_FAILURE);
                }
                *end_ptr = '\0'; // Null-terminate the string
                bruter_push_pointer(result, strdup(str_value), NULL, BR_TYPE_BUFFER);
            }
            else 
            {
                char *str_value = token + 1; // Skip the ':'
                bruter_push_pointer(result, strdup(str_value), NULL, BR_TYPE_BUFFER);
            }
            
        }
        else if (token[0] == '@') // run
        {
            void (*func)(BruterList *result) = bruter_pop_pointer(result);
            if (token[1] == '@')
            {
                // we need to insert the context into the stack too
                // we assume the user will pop the context
                bruter_push_pointer(result, context, NULL, BR_TYPE_NULL);
            }
            func(result);
        }
        else if (token[0] == '#') // comment
        {
            // Ignore comments
            i++; // Skip the next token
            continue;
        }
        else
        {
            BruterInt found = bruter_find_key(context, token);
            if (found != -1)
            {
                BruterMetaValue meta = bruter_get_meta(context, found);
                meta.key = NULL; // we don't need the key here
                bruter_push_meta(result, meta);
            }
            else
            {
                // If not found
                printf("WARNING: Variable '%s' not found in context\n", token);
            }
        }
    }
    free(original_str); // free the original string
    bruter_free(splited); // free the splited list
    return result;
}

#endif // RAWER_H macro