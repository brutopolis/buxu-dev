#include "rawer.h"

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

function(rawer_discard)
{
    clear_context(stack);
}

function(rawer_pop)
{
    BruterInt amount = bruter_pop_int(stack);
    BruterList* list_value = bruter_pop_pointer(stack);
    BruterList *list = (BruterList*)list_value;
    if (amount < 1)
    {
        fprintf(stderr, "ERROR: Cannot pop less than 1 item from the list\n");
    }
    
    if (amount > list->size)
    {
        fprintf(stderr, "ERROR: Cannot pop more items than the list contains\n");
        exit(EXIT_FAILURE);
    }
    
    if (list->size == 0)
    {
        fprintf(stderr, "ERROR: Cannot pop from an empty list\n");
        exit(EXIT_FAILURE);
    }

    for (BruterInt i = 0; i < amount; i++)
    {
        bruter_push_meta(stack, bruter_pop_meta(list));
    }
}

init(std)
{
    bruter_push_pointer(context, rawer_print, "print", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_add, "add", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_sub, "sub", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_rename, "rename", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_register, "register", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_discard, "discard", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_pop, "pop", BR_TYPE_FUNCTION);
}