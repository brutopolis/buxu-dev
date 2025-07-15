#ifndef RAWER_H
#define RAWER_H 1

#include <bruter.h>

#define RAWER_VERSION "0.0.1"

enum {
    BR_TYPE_NULL = 0,
    BR_TYPE_ANY,
    BR_TYPE_FLOAT,
    BR_TYPE_BUFFER,
    BR_TYPE_LIST,
    BR_TYPE_FUNCTION
};

static inline BruterList* string_split(char *input_str)
{
    if (input_str == NULL || *input_str == '\0')
    {
        return stack_new(0);
    }

    char* str = strdup(input_str);
    if (!str) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    printf("Splitting string: '%s'\n", str);

    BruterList *stack = bruter_new(2, false, false);
    char *ptr = str;
    bruter_push_pointer(stack, str, NULL, 0); // Push the original string for cleanup later

    int recursion = 0;
    while (*ptr != '\0')
    {
        printf("Current char: '%c'\n", *ptr);

        if (isspace((unsigned char)*ptr) && recursion == 0)
        {
            *ptr = '\0';  // Quebra a palavra
            ptr++;

            // Pula espaços consecutivos
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
                free(str);
                stack_free(stack);
                exit(EXIT_FAILURE);
            }
        }

        ptr++;
    }

    if (recursion != 0)
    {
        fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
        free(str);
        stack_free(stack);
        exit(EXIT_FAILURE);
    }

    return stack;
}


static inline BruterList* parse(BruterList *context, char* input_str)
{
    BruterList *result = string_split(input_str);
    result->types = (int8_t*)calloc((size_t)result->capacity, sizeof(int8_t));

    if (result->types == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for tokens types\n");
        stack_free(result);
        exit(EXIT_FAILURE);
    }

    char* original_str = (char*)result->data[0].p; // Keep the original string for cleanup
    if (result->size == 0)
    {
        stack_free(result);
        return NULL;
    }

    BruterList *result = bruter_new(1, false, false);

    for (int i = 0; i < result->size; i++)
    {
        char *token = (char*)result->data[i].p;
        result->types[i] = BR_TYPE_BUFFER; // Default type for strings

        if ((token[0] >= '0' && token[0] <= '9') || (token[0] == '-' && isdigit(token[1]))) // number
        {
            if (token[0] == '0' && token[1] == 'x') // hex
            {
                result->data[i].i = strtol(token+2, NULL, 16);
                result->types[i] = BR_TYPE_ANY;
            }
            else if (token[0] == '0' && token[1] == 'b') // bin
            {
                result->data[i].i = strtol(token+2, NULL, 2);
                result->types[i] = BR_TYPE_ANY;
            }
            else if (token[0] == '0' && token[1] == 'o') // oct
            {
                result->data[i].i = strtol(token+2, NULL, 8);
                result->types[i] = BR_TYPE_ANY;
            }
            else if (strchr(token, '.')) // float
            {
                result->data[i].f = atof(token);
                result->types[i] = BR_TYPE_FLOAT;
            }
            else // int
            {
                result->data[i].i = atol(token);
                result->types[i] = BR_TYPE_ANY;
            }
        }
        else if(token[0] == ':') // str
        {
            char *str_value = strndup(token + 2, strlen(token) - 2);
            if (str_value == NULL)
            {
                fprintf(stderr, "ERROR: Failed to allocate memory for string value\n");
                stack_free(result);
                exit(EXIT_FAILURE);
            }
            result->data[i].p = str_value;
            result->types[i] = BR_TYPE_BUFFER;
        }
        else if (token[0] == '@') // run
        {
            bruter_push_int(result, INTPTR_MIN, NULL, 0); // Placeholder for run 
        }
        else
        {
            BruterInt found = bruter_find_key(context, token);
            if (found != -1)
            {
                result->data[i] = context->data[found];
                result->types[i] = context->types[found];
            }
            else
            {
                // If not found, we can treat it as a string
                result->data[i].p = strdup(token);
                if (result->data[i].p == NULL)
                {
                    fprintf(stderr, "ERROR: Failed to allocate memory for string token\n");
                    stack_free(result);
                    exit(EXIT_FAILURE);
                }
                result->types[i] = BR_TYPE_BUFFER;
            }
        }
    }

    free(original_str); // free the original string
    //stack_free(tokens);
    return result;
}

#endif // RAWER_H macro