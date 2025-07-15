#include "rawer.h"

void rawer_int_print(BruterList *stack)
{
    BruterList *context = (BruterList*)bruter_pop_pointer(stack);
    BruterValue value = bruter_pop(stack);
    printf("%" PRIdPTR "\n", value.i);
}

void rawer_add(BruterList *stack)
{
    BruterList *context = (BruterList*)bruter_pop_pointer(stack);
    BruterValue a = bruter_pop(stack);
    BruterValue b = bruter_pop(stack);
    bruter_push_int(stack, a.i + b.i, NULL, 0);
}

void rawer_sub

/*
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    VirtualMachine *vm = vm_new(8);
    Stack *tokens = string_split(argv[1]);
    
    if (tokens == NULL)
    {
        fprintf(stderr, "No tokens found in input string.\n");
        return EXIT_FAILURE;
    }

    // Process tokens as needed
    for (int i = 0; i < tokens->size; i++)
    {
        printf("Token %d: %s\n", i, (char*)tokens->data[i].p);
    }

    free(tokens->data[0].p);
    stack_free(tokens);
    return EXIT_SUCCESS;
}
*/