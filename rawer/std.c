#include "rawer.h"

// parsers
parser_step(number_parser)
{
    char* token = (char*)splited->data[word_index].p;

    if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) // number
    {
        if (token[0] == '0' && token[1] == 'x') // hex
        {
            bruter_push_int(stack, strtol(token+2, NULL, 16), NULL, BR_TYPE_ANY);
        }
        else if (token[0] == '0' && token[1] == 'b') // bin
        {
            bruter_push_int(stack, strtol(token+2, NULL, 2), NULL, BR_TYPE_ANY);
        }
        else if (token[0] == '0' && token[1] == 'o') // oct
        {
            bruter_push_int(stack, strtol(token+2, NULL, 8), NULL, BR_TYPE_ANY);
        }
        else if (strchr(token, '.')) // float
        {
            bruter_push_float(stack, strtof(token, NULL), NULL, BR_TYPE_FLOAT);
        }
        else // int
        {
            bruter_push_int(stack, strtol(token, NULL, 10), NULL, BR_TYPE_ANY);
        }
        return true;
    }
    return false; // Not a number lets check other parsers
}

parser_step(list_parser)
{
    char *token = (char*)splited->data[word_index].p;

    if (token[0] == '(') // list
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
            bruter_push_pointer(stack, parsed, NULL, BR_TYPE_LIST);
        }
        else if (parsed->size == 1)
        {
            // If the list has only one item, push it directly
            bruter_push_meta(stack, bruter_pop_meta(parsed));
            bruter_free(parsed); // Free the parsed list after use
        }
        else
        {
            bruter_push_pointer(stack, bruter_new(0, false, false), NULL, BR_TYPE_LIST);
        }
        return true;
    }
    return false; // Not a list lets check other parsers
}

parser_step(string_parser)
{
    char *token = (char*)splited->data[word_index].p;

    if (token[0] == '#') // string 
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
            bruter_push_pointer(stack, strdup(str_value), NULL, BR_TYPE_BUFFER);
        }
        else 
        {
            char *str_value = token + 1; // Skip the ':'
            bruter_push_pointer(stack, strdup(str_value), NULL, BR_TYPE_BUFFER);
        }
        return true;
    }
    return false; // Not a string lets check other parsers
}

parser_step(function_run_parser)
{
    char *token = (char*)splited->data[word_index].p;

    if (token[0] == '!') // run
    {
        void (*func)(BruterList *stack) = bruter_pop_pointer(stack);
        if (token[1] == '!')
        {
            // we need to insert the context into the stack too
            // we assume the user will pop the context
            bruter_push_pointer(stack, context, NULL, BR_TYPE_NULL);
        }
        func(stack);
        return true;
    }
    return false; // Not a run lets check other parsers
}

parser_step(comment_parser)
{
    char *token = (char*)splited->data[word_index].p;

    if (token[0] == 'c' && token[1] == 'o' && token[2] == 'm' && token[3] == 'm' && token[4] == 'e' && token[5] == 'n' && token[6] == 't' && token[7] == ':')
    {
        // Ignore comments
        char* next_token = bruter_remove_pointer(splited, word_index + 1);
        // we cannoot free next_token because it is a delocated pointer
        return true; // Indicate that this is a comment and should be ignored
    }
    return false; // Not a comment lets check other parsers
}

parser_step(variable_parser)
{
    char *token = (char*)splited->data[word_index].p;

    // ok its a variable but we dont know yet how the user wants to use it
    // lets get the len of the token as it is expected not to be so long
    size_t len = strlen(token);

    if (len > 0 && token[len - 1] == '!')
    {
        // ok its definitively a stack call
        // lets find it first

        bool need_context = false;

        if (token[len - 2] == '!')
        {
            need_context = true; // we need to insert the context into the stack
            token[len - 2] = '\0'; // Remove the '!!' from the
        }
        else
        {
            token[len - 1] = '\0'; // Remove the '!' from the end
        }

        BruterInt found = bruter_find_key(context, token);
        if (found == -1)
        {
            printf("WARNING: Variable '%s' not found in context\n", token);
            return true; // Not found, we cannot push it
        }

        // we found it, lets push it
        BruterMetaValue meta = bruter_get_meta(context, found);
        meta.key = NULL; // we don't need the key here
        bruter_push_meta(stack, meta);

        // lets turn the token into '!' or '!!'
        if (need_context)
        {
            token[0] = '!';
            token[1] = '!';
            token[2] = '\0'; // Make it '!!'
        }
        else
        {
            token[0] = '!';
            token[1] = '\0'; // Make it '!'
        }

        // now we can just pass everthing to the function step
        function_run_parser(context, stack, splited, word_index);
        return true;
    }

    BruterInt found = bruter_find_key(context, token);
    if (found != -1)
    {
        BruterMetaValue meta = bruter_get_meta(context, found);
        meta.key = NULL; // we don't need the key here
        bruter_push_meta(stack, meta);
    }
    else
    {
        // If not found
        printf("WARNING: Variable '%s' not found in context\n", token);
    }
    return true;
}

// functions
// functions
// functions
// functions

function(rawer_print)
{
    BruterMetaValue value = bruter_pop_meta(stack);
    switch (value.type)
    {
        case BR_TYPE_FLOAT:
            printf("%f\n", value.value.f);
            break;
        case BR_TYPE_BUFFER:
            printf("%s\n", (char*)value.value.p);
            break;
        case BR_TYPE_LIST:
            printf("%p\n", value.value.p);
            break;
        default:
            printf("%d\n", value.value.i);
            break;
    }
}

function(rawer_add)
{
    BruterMetaValue a = bruter_pop_meta(stack);
    BruterMetaValue b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BR_TYPE_FLOAT:
            switch (b.type)
            {
                case BR_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.f + b.value.f, NULL, BR_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_float(stack, a.value.f + b.value.i, NULL, BR_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BR_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.i + b.value.f, NULL, BR_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_int(stack, a.value.i + b.value.i, NULL, 0);
                    break;
            }
            break;
    }
}

function(rawer_sub)
{
    BruterMetaValue a = bruter_pop_meta(stack);
    BruterMetaValue b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BR_TYPE_FLOAT:
            switch (b.type)
            {
                case BR_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.f - b.value.f, NULL, BR_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_float(stack, a.value.f - b.value.i, NULL, BR_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BR_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.i - b.value.f, NULL, BR_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_int(stack, a.value.i - b.value.i, NULL, 0);
                    break;
            }
            break;
    }
}

function(rawer_register)
{
    BruterList *context = bruter_pop_pointer(stack);
    BruterMetaValue meta = bruter_pop_meta(stack);
    bruter_push_meta(context, meta);
}

function(rawer_rename)
{
    char* new_key = bruter_pop_pointer(stack);
    BruterMetaValue value = bruter_pop_meta(stack);

    if (value.key != NULL)
    {
        free(value.key); // Free the old key if it was allocated
    }
    value.key = new_key;
    bruter_push_meta(stack, value);
}

function(rawer_retype)
{
    BruterInt new_type = bruter_pop_int(stack);
    BruterMetaValue value = bruter_pop_meta(stack);
    value.type = new_type; // Update the type of the value
    bruter_push_meta(stack, value);
}

function(rawer_clear)
{
    clear_context(stack);
}

function(rawer_list_pop)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt amount = bruter_pop_int(stack);
    for (BruterInt i = 0; i < amount && list->size > 0; i++)
    {
        bruter_push_meta(stack, bruter_pop_meta(list));
    }
}

function(rawer_list_push)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMetaValue value = bruter_pop_meta(stack);
    bruter_push_meta(list, value);
}

function(rawer_list_shift)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt amount = bruter_pop_int(stack);
    for (BruterInt i = 0; i < amount && list->size > 0; i++)
    {
        bruter_push_meta(stack, bruter_shift_meta(list));
    }
}

function(rawer_list_unshift)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMetaValue value = bruter_pop_meta(stack);
    bruter_unshift_meta(list, value);
}

function(rawer_list_insert)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);
    BruterMetaValue value = bruter_pop_meta(stack);

    if (index < 0 || index > list->size)
    {
        fprintf(stderr, "ERROR: Index out of bounds for list insertion\n");
        exit(EXIT_FAILURE);
    }

    bruter_insert_meta(list, index, value);
}

function(rawer_list_remove)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);

    if (index < 0 || index >= list->size)
    {
        fprintf(stderr, "ERROR: Index out of bounds for list removal\n");
        exit(EXIT_FAILURE);
    }

    bruter_remove_meta(list, index);
}

function(rawer_list_get)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);

    if (index < 0 || index >= list->size)
    {
        fprintf(stderr, "ERROR: Index out of bounds for list access\n");
        exit(EXIT_FAILURE);
    }

    bruter_push_meta(stack, bruter_get_meta(list, index));
}

function(rawer_list_set)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);
    BruterMetaValue value = bruter_pop_meta(stack);

    if (index < 0 || index >= list->size)
    {
        fprintf(stderr, "ERROR: Index out of bounds for list set\n");
        exit(EXIT_FAILURE);
    }

    bruter_set_meta(list, index, value);
}

init(std)
{
    BruterInt found = bruter_find_key(context, "context");
    if (found == -1)
    {
        bruter_push_pointer(context, context, "context", BR_TYPE_ANY);
    }
    
    found = bruter_find_key(context, "parsers");
    if (found == -1)
    {
        bruter_push_pointer(context, bruter_new(8, true, false), "parsers", BR_TYPE_LIST);
        found = bruter_find_key(context, "parsers");
    }

    BruterList *parsers = bruter_get_pointer(context, found);
    bruter_push_pointer(parsers, number_parser, "number", BR_TYPE_FUNCTION);
    bruter_push_pointer(parsers, list_parser, "list", BR_TYPE_FUNCTION);
    bruter_push_pointer(parsers, string_parser, "string", BR_TYPE_FUNCTION);
    bruter_push_pointer(parsers, function_run_parser, "run", BR_TYPE_FUNCTION);
    bruter_push_pointer(parsers, comment_parser, "comment", BR_TYPE_FUNCTION);
    bruter_push_pointer(parsers, variable_parser, "variable", BR_TYPE_FUNCTION);

    bruter_push_pointer(context, rawer_print, "print", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_add, "add", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_sub, "sub", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_rename, "rename", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_retype, "retype", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_register, "register", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_clear, "clear", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_pop, "pop", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_push, "push", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_shift, "shift", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_unshift, "unshift", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_insert, "insert", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_remove, "remove", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_get, "get", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_list_set, "set", BR_TYPE_FUNCTION);

    bruter_push_int(context, BR_TYPE_NULL, "Null", BR_TYPE_ANY);
    bruter_push_int(context, BR_TYPE_ANY, "Any", BR_TYPE_ANY);
    bruter_push_int(context, BR_TYPE_FLOAT, "Float", BR_TYPE_ANY);
    bruter_push_int(context, BR_TYPE_BUFFER, "Buffer", BR_TYPE_ANY);
    bruter_push_int(context, BR_TYPE_LIST, "List", BR_TYPE_ANY);
    bruter_push_int(context, BR_TYPE_FUNCTION, "Function", BR_TYPE_ANY);
}