#include "rawer.h"

void rawer_print(BruterList *stack)
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

void rawer_add(BruterList *stack)
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

void rawer_sub(BruterList *stack)
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

void rawer_register(BruterList *stack)
{
    BruterList *context = bruter_pop_pointer(stack);
    BruterMetaValue meta = bruter_pop_meta(stack);
    bruter_push_meta(context, meta);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    BruterList *context = bruter_new(10, true, true);
    bruter_push_pointer(context, rawer_print, "print", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_add, "add", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_sub, "sub", BR_TYPE_FUNCTION);
    bruter_push_pointer(context, rawer_register, "register", BR_TYPE_FUNCTION);
    BruterList *result = parse(context, argv[1]);
    return EXIT_SUCCESS;
}
