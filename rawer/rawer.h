#ifndef RAWER_H
#define RAWER_H 1

#include <bruter.h>

#include <ctype.h>

#define RAWER_VERSION "0.0.1"

enum {
    BR_TYPE_NULL = 0,
    BR_TYPE_ANY,
    BR_TYPE_FLOAT,
    BR_TYPE_BUFFER,
    BR_TYPE_LIST,
    BR_TYPE_FUNCTION,
    BR_TYPE_REFERENCE
};

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
            char* sub_str = strndup(token + 1, strlen(token) - 2);
            bruter_push_pointer(result, parse(context, sub_str), NULL, BR_TYPE_LIST);
            free(sub_str); // Free the duplicated string, we don't need it anymore
        }
        else if(token[0] == ':') // str
        {
            char *str_value = strndup(token + 2, strlen(token) - 3);
            if (str_value == NULL)
            {
                fprintf(stderr, "ERROR: Failed to allocate memory for string value\n");
                exit(EXIT_FAILURE);
            }
            bruter_push_pointer(result, str_value, NULL, BR_TYPE_BUFFER);
        }
        else if (token[0] == '@') // run
        {
            void (*func)(BruterList *result) = bruter_pop_pointer(result);
            if (token[1] == '@')
            {
                // we need to insert the context into the stack too
                // we assume the user will pop the context
                bruter_push_pointer(result, context, NULL, BR_TYPE_NULL);
                i++; // lets go to the next token as we inserted a new one in the current position
            }
            func(result);
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
                // If not found, we ignore it
                printf("BR_WARN: variable '%s' not found in dictionary\n", token);
            }
        }
    }
    free(original_str); // free the original string
    bruter_free(splited); // free the splited list
    return result;
}

#endif // RAWER_H macro