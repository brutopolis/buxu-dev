#ifndef RAWER_H
#define RAWER_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <ctype.h>

#define STATIC_INLINE  

typedef intptr_t Int;
typedef uintptr_t UInt;

#if INTPTR_MAX == INT64_MAX
    typedef double Float;
#else
    typedef float Float;
#endif

typedef struct 
{
    int capacity;
    int size;
    Int data[];
} Stack;

typedef struct 
{
    Stack *values;

    Stack *ref_names;
    Stack *ref_indexes;
} VirtualMachine;

typedef void (*Function)(Stack *stack);

static inline Stack* stack_new(int size)
{
    Stack *stack = (Stack*)malloc(sizeof(Stack) + size * sizeof(Int));
    if (stack == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for Stack\n");
        exit(EXIT_FAILURE);
    }
    stack->capacity = size;
    stack->size = 0;
    return stack;
}

static inline void stack_free(Stack *stack)
{
    free(stack);
}

static inline void stack_push(Stack *stack, Int value)
{
    if (stack->size >= stack->capacity)
    {
        fprintf(stderr, "ERROR: Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    stack->data[stack->size++] = value;
}

static inline Int stack_pop(Stack *stack)
{
    if (stack->size <= 0)
    {
        fprintf(stderr, "ERROR: Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack->data[--stack->size];
}

static inline VirtualMachine* vm_new(int size)
{
    VirtualMachine *vm = (VirtualMachine*)malloc(sizeof(VirtualMachine));
    if (vm == NULL)
    {
        fprintf(stderr, "ERROR: Failed to allocate memory for VirtualMachine\n");
        exit(EXIT_FAILURE);
    }
    vm->values = stack_new(size);
    vm->ref_names = stack_new(size);
    vm->ref_indexes = stack_new(size);
    return vm;
}

static inline void vm_free(VirtualMachine *vm)
{
    stack_free(vm->values);
    stack_free(vm->ref_names);
    stack_free(vm->ref_indexes);
    free(vm);
}

static inline Stack* string_split(char *input_str)
{
    char* str = strdup(input_str);
    if (str == NULL || *str == '\0')
    {
        return stack_new(0);
    }

    Stack *stack = stack_new(2);
    stack_push(stack, (intptr_t)str);
    int recursion = 0;
    while (*str != '\0')
    {
        if (isspace((unsigned char)*str) && recursion == 0)
        {
            *str = '\0';
            str++;
            while (*str && isspace((unsigned char)*str)) str++;
            if (*str != '\0')
            {
                stack_push(stack, (intptr_t)str);
            }
        }
        else if (*str == '(')
        {
            recursion++;
        }
        else if (*str == ')')
        {
            recursion--;
            if (recursion < 0)
            {
                fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
                stack_free(stack);
                exit(EXIT_FAILURE);
            }
        }
        
        str++;
    }

    if (recursion != 0)
    {
        fprintf(stderr, "ERROR: Unmatched parentheses in string\n");
        stack_free(stack);
        exit(EXIT_FAILURE);
    }

    return stack;
}

static inline Stack* parse(VirtualMachine *context, char* input_str)
{
    Stack *tokens = string_split(input_str);
    if (tokens->size == 0)
    {
        stack_free(tokens);
        return NULL;
    }

    Stack *result = stack_new(tokens->size);
    for (int i = 0; i < tokens->size; i++)
    {
        char *token = (char*)tokens->data[i];
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        {
            Int value = strtol(token, NULL, 10);
            stack_push(result, value);
        }
        else if(token[0] == ':') // str
        {
            char *str_value = strndup(token + 2, strlen(token) - 2);
            if (str_value == NULL)
            {
                fprintf(stderr, "ERROR: Failed to allocate memory for string value\n");
                stack_free(tokens);
                stack_free(result);
                exit(EXIT_FAILURE);
            }
            stack_push(result, (intptr_t)str_value);
        }
        else if (token[0] == '@') // run
        {
            Int func_index = stack_pop(result);
            Function func = (Function)context->values->data[func_index];
            stack_push(result, (intptr_t)context);
            func(result);
        }
        else
        {
            Int found = -1;
            for (Int j = 0; j < context->values->size; j++)
            {
                if (strcmp((char*)context->ref_names->data[j], token) == 0)
                {
                    found = context->ref_indexes->data[j];
                    break;
                }
            }

            if (found == -1)
            {
                printf("ERROR: Undefined reference '%s'\n", token);
            }
            
            stack_push(result, found);
        }
    }

    free((void*)(intptr_t)tokens->data[0]); // free the original string
    stack_free(tokens);
    return result;
}

static inline void rawer_int_print(Stack *stack)
{
    VirtualMachine *context = (VirtualMachine*)stack_pop(stack);
    Int value = stack_pop(context->values);
    printf("%" PRIdPTR "\n", value);
}

#endif // RAWER_H macro