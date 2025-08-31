#include "bruter.h"
#include <stdlib.h>

/* BEGIN cc blocks */
/* start include: ./std/list.c */
#include "bruter.h"
// functions
// functions
// functions
// functions
function(feraw_list)
{
    BruterInt size = bruter_pop_int(stack);
    BruterList *list = bruter_new(BRUTER_DEFAULT_SIZE, true, true);
    for (BruterInt i = 0; i < size; i++)
    {
        BruterMeta value = bruter_pop_meta(stack);
        bruter_push_meta(list, value);
    }
    bruter_push_pointer(stack, list, NULL, BRUTER_TYPE_LIST);
}
function(feraw_pop)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(list);
    free(value.key); // Free the key if it exists
    bruter_push_meta(stack, value); // Push the popped value back to the stack
}
function(feraw_push)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(stack);
    bruter_push_meta(list, value);
}
function(feraw_shift)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_shift_meta(list);
    free(value.key); // Free the key if it exists
    bruter_push_meta(stack, value); // Push the shifted value back to the stack
}
function(feraw_unshift)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(stack);
    bruter_unshift_meta(list, value);
}
function(feraw_insert)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);
    BruterMeta value = bruter_pop_meta(stack);
    if (index < 0) // -1 = the last element and so on
    {
        index += list->size; // Adjust negative index to positive
    }
    bruter_insert_meta(list, index, value);
}
function(feraw_remove)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index = bruter_pop_int(stack);
    if (index < 0) // -1 = the last element and so on
    {
        index += list->size; // Adjust negative index to positive
    }
    BruterMeta removed_value = bruter_remove_meta(list, index);
    free(removed_value.key); // Free the key if it exists
    bruter_push_meta(stack, removed_value); // Push the removed value back to the stack
}
function(feraw_get)
{
    BruterMeta list_meta = bruter_pop_meta(stack);
    BruterMeta index_meta = bruter_pop_meta(stack);
    BruterInt index = index_meta.value.i;
    if (list_meta.type == BRUTER_TYPE_BUFFER)
    {
        char* buffer = (char*)list_meta.value.p;
        if (index_meta.type == BRUTER_TYPE_FLOAT)
        {
            index = (BruterInt)index_meta.value.f; // Convert float to integer if necessary
        }
        if (index < 0) // -1 = the last element and so on
        {
            index += strlen(buffer); // Adjust negative index to positive
        }
        bruter_push_int(stack, buffer[index], NULL, BRUTER_TYPE_ANY);
        return;
    }
    else
    {
        BruterList* list = (BruterList*)list_meta.value.p;
        if (index_meta.type == BRUTER_TYPE_FLOAT)
        {
            index = (BruterInt)index_meta.value.f; // Convert float to integer if necessary
        }
        else if (index_meta.type == BRUTER_TYPE_BUFFER)
        {
            index = bruter_find_key(list_meta.value.p, (char*)index_meta.value.p);
            if (index < 0)
            {
                fprintf(stderr, "ERROR: cant get, key '%s' not found in list\n", (char*)index_meta.value.p);
                exit(EXIT_FAILURE);
            }
        }
        if (index < 0) // -1 = the last element and so on
        {
            index += list->size; // Adjust negative index to positive
        }
        bruter_push_meta(stack, bruter_get_meta(list, index));
    }
}
function(feraw_set)
{
    BruterMeta list_meta = bruter_pop_meta(stack);
    BruterMeta index = bruter_pop_meta(stack);
    BruterMeta value = bruter_pop_meta(stack);
    if (list_meta.type == BRUTER_TYPE_BUFFER)
    {
        unsigned char* buffer = (unsigned char*)list_meta.value.p;
        if (index.type == BRUTER_TYPE_FLOAT)
        {
            index.value.i = (BruterInt)index.value.f; // Convert float to integer if necessary
        }
        if (index.value.i < 0) // -1 = the last element and so on
        {
            index.value.i += strlen((char*)buffer); // Adjust negative index to positive
        }
        buffer[index.value.i] = value.value.i; // Directly set the value in the buffer
    }
    else
    {
        BruterList* list = (BruterList*)list_meta.value.p;
        switch (index.type)
        {
            case BRUTER_TYPE_FLOAT:
                index.value.i = (BruterInt)index.value.f;
            case BRUTER_TYPE_ANY:
                if (index.value.i >= list->size)
                {
                    while (list->size <= index.value.i)
                    {
                        bruter_push_meta(list, (BruterMeta){.value = {.i = 0}, .key = NULL, .type = BRUTER_TYPE_ANY});
                    }
                }
                else if (index.value.i < 0) // -1 = the last element and so on
                {
                    index.value.i += list->size; // Adjust negative index to positive
                }
                list->data[index.value.i] = value.value; // Directly set the value
                break;
            case BRUTER_TYPE_BUFFER:
            {
                BruterInt found_index = bruter_find_key(list, (char*)index.value.p);
                if (found_index < 0)
                {
                    value.key = strdup(index.value.p);
                    bruter_push_meta(list, value); // Push the value to the list
                }
                else 
                {
                    list->data[found_index] = value.value; // Directly set the value
                    if (list->keys != NULL)
                    {
                        if (list->keys[found_index] == NULL)
                        {
                            list->keys[found_index] = index.value.p; // Set the new key
                        }
                        else
                        {
                            free(value.key); // Free the old key if it was allocated
                        }
                    }
                    if (list->types != NULL)
                    {
                        list->types[found_index] = value.type; // Set the type   
                    }
                }
            }
            break;
        }
    }
}
function(feraw_where)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(stack);
    BruterInt found_index = -1;
    for (BruterInt i = 0; i < list->size; i++)
    {
        if (list->types[i] == value.type && list->data[i].i == value.value.i)
        {
            found_index = i;
            break;
        }
    }
    bruter_push_int(stack, found_index, NULL, BRUTER_TYPE_ANY);
}
function(feraw_find)
{
    BruterList* list = bruter_pop_pointer(stack);
    char* key = bruter_pop_pointer(stack);
    BruterInt found_index = -1;
    for (BruterInt i = 0; i < list->size; i++)
    {
        if (list->keys[i] != NULL && strcmp(list->keys[i], key) == 0)
        {
            found_index = i;
            break;
        }
    }
    bruter_push_int(stack, found_index, NULL, BRUTER_TYPE_ANY);
}
function(feraw_length)
{
    BruterList* list = bruter_pop_pointer(stack);
    bruter_push_int(stack, list->size, NULL, BRUTER_TYPE_ANY);
}
function(feraw_copy)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterList* new_list = bruter_copy(list);
}
function(feraw_swap)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterInt index1 = bruter_pop_int(stack);
    BruterInt index2 = bruter_pop_int(stack);
    if (index1 < 0 || index1 >= list->size || index2 < 0 || index2 >= list->size)
    {
        fprintf(stderr, "ERROR: cannot swap, index %" PRIdPTR " or %" PRIdPTR " out of range in list of size %" PRIdPTR "\n", index1, index2, list->size);
        exit(EXIT_FAILURE);
    }
    bruter_swap(list, index1, index2);
}
function(feraw_reverse)
{
    BruterList* list = bruter_pop_pointer(stack);
    bruter_reverse(list);
}
function(feraw_point)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(stack);
    if (value.type == BRUTER_TYPE_BUFFER) // we assume its the key of the element we wanto to point to
    {
        BruterInt index = bruter_find_key(list, (char*)value.value.p);
        if (index < 0)
        {
            fprintf(stderr, "ERROR: cannot point to, key '%s' not found in list\n", (char*)value.value.p);
            exit(EXIT_FAILURE);
        }
        bruter_push_pointer(stack, &list->data[index], NULL, BRUTER_TYPE_BUFFER);
    }
    else // we assume its the index of the element we want to point to
    {
        if (value.value.i < 0) // -1 = the last element and so on
        {
            value.value.i += list->size; // Adjust negative index to positive
        }
        if (value.value.i < 0 || value.value.i >= list->size)
        {
            fprintf(stderr, "ERROR: cannot point to, index %" PRIdPTR " out of range in list of size %" PRIdPTR "\n", value.value.i, list->size);
            exit(EXIT_FAILURE);
        }
        bruter_push_pointer(stack, &list->data[value.value.i], NULL, BRUTER_TYPE_BUFFER);
    }
}
function(feraw_alloc)
{
    BruterList *arena = bruter_pop_pointer(stack);
    BruterInt size = bruter_pop_int(stack);
    void *ptr = bruter_alloc(arena, size);
    bruter_push_pointer(stack, ptr, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_dup)
{
    BruterMeta value = bruter_pop_meta(stack);
    bruter_push_meta(stack, value); // Push the value back to the stack
    bruter_push_meta(stack, value); // Duplicate it
}
function(feraw_free)
{
    BruterMeta value = bruter_pop_meta(stack);
    if (value.type == BRUTER_TYPE_BUFFER)
    {
        free(value.value.p); // Free the buffer if it was allocated
    }
    else
    {
        bruter_free((BruterList*)value.value.p); // Free the list if it was allocated
    }
    // No action needed for other types, as they are not dynamically allocated
}
function(feraw_nameof)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta index_meta = bruter_pop_meta(stack);
    switch (index_meta.type)
    {
        case BRUTER_TYPE_FLOAT:
            index_meta.value.i = (BruterInt)index_meta.value.f; // Convert float to integer if necessary
            break;
        case BRUTER_TYPE_ANY:
            // No conversion needed
            break;
        case BRUTER_TYPE_BUFFER:
            index_meta.value.i = bruter_find_key(list, (char*)index_meta.value.p);
            if (index_meta.value.i < 0)
            {
                fprintf(stderr, "ERROR: key '%s' not found in list\n", (char*)index_meta.value.p);
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "ERROR: key expects a float or any type, got %d\n", index_meta.type);
            exit(EXIT_FAILURE);
    }
    if (index_meta.value.i < 0)
    {
        index_meta.value.i += list->size; // Adjust negative index to positive
    }
    if (index_meta.value.i < 0 || index_meta.value.i >= list->size)
    {
        fprintf(stderr, "ERROR: key index %" PRIdPTR " out of range in list of size %" PRIdPTR "\n", index_meta.value.i, list->size);
        exit(EXIT_FAILURE);
    }
    bruter_push_meta(stack, (BruterMeta){.value = {.p = list->keys[index_meta.value.i]}, .key = NULL, .type = BRUTER_TYPE_BUFFER});
}
function(feraw_typeof)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta index = bruter_pop_meta(stack);
    if (index.type == BRUTER_TYPE_FLOAT)
    {
        index.value.i = (BruterInt)index.value.f; // Convert float to integer if necessary
    }
    else if (index.type == BRUTER_TYPE_BUFFER)
    {
        index.value.i = bruter_find_key(list, (char*)index.value.p);
        if (index.value.i < 0)
        {
            fprintf(stderr, "ERROR: cannot typeof, key '%s' not found in list\n", (char*)index.value.p);
            exit(EXIT_FAILURE);
        }
    }
    if (index.value.i < 0 || index.value.i >= list->size)
    {
        fprintf(stderr, "ERROR: cannot typeof, index %" PRIdPTR " out of range in list of size %" PRIdPTR "\n", index.value.i, list->size);
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, list->types[index.value.i], NULL, BRUTER_TYPE_ANY);
}
function(feraw_rename)
{
    BruterList* list = bruter_pop_pointer(stack);
    char* old_name = bruter_pop_pointer(stack);
    char* new_name = bruter_pop_pointer(stack);
    if (list == NULL || old_name == NULL || new_name == NULL)
    {
        fprintf(stderr, "ERROR: cannot rename, list or names are NULL\n");
        exit(EXIT_FAILURE);
    }
    for (BruterInt i = 0; i < list->size; i++)
    {
        if (list->keys != NULL && list->keys[i] != NULL && strcmp(list->keys[i], old_name) == 0)
        {
            free(list->keys[i]); // Free the old key
            list->keys[i] = strdup(new_name); // Set the new key
            return;
        }
    }
    fprintf(stderr, "ERROR: cannot rename, key '%s' not found in list\n", old_name);
    exit(EXIT_FAILURE);
}
function(feraw_retype)
{
    BruterList* list = bruter_pop_pointer(stack);
    BruterMeta index = bruter_pop_meta(stack);
    BruterInt newType = bruter_pop_int(stack);
    if (index.type == BRUTER_TYPE_FLOAT)
    {
        index.value.i = (BruterInt)index.value.f; // Convert float to integer if necessary
    }
    else if (index.type == BRUTER_TYPE_BUFFER)
    {
        index.value.i = bruter_find_key(list, (char*)index.value.p);
        if (index.value.i < 0)
        {
            fprintf(stderr, "ERROR: cannot retype, key '%s' not found in list\n", (char*)index.value.p);
            exit(EXIT_FAILURE);
        }
    }
    if (index.value.i < 0 || index.value.i >= list->size)
    {
        fprintf(stderr, "ERROR: cannot retype, index %" PRIdPTR " out of range in list of size %" PRIdPTR "\n", index.value.i, list->size);
        exit(EXIT_FAILURE);
    }
    list->types[index.value.i] = newType;
}
/* end include: ./std/list.c */
/* start include: ./std/math.c */
#include <bruter.h>
#include <math.h>
#include <time.h>
function(feraw_add)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BRUTER_TYPE_FLOAT:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.f + b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_float(stack, a.value.f + b.value.i, NULL, BRUTER_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.i + b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_int(stack, a.value.i + b.value.i, NULL, BRUTER_TYPE_ANY);
                    break;
            }
            break;
    }
}
function(feraw_sub)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BRUTER_TYPE_FLOAT:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.f - b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_float(stack, a.value.f - b.value.i, NULL, BRUTER_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.i - b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_int(stack, a.value.i - b.value.i, NULL, BRUTER_TYPE_ANY);
                    break;
            }
            break;
    }
}
function(feraw_mul)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BRUTER_TYPE_FLOAT:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.f * b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_float(stack, a.value.f * b.value.i, NULL, BRUTER_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    bruter_push_float(stack, a.value.i * b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    bruter_push_int(stack, a.value.i * b.value.i, NULL, BRUTER_TYPE_ANY);
                    break;
            }
            break;
    }
}
function(feraw_div)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    switch (a.type)
    {
        case BRUTER_TYPE_FLOAT:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    if (b.value.f == 0.0)
                    {
                        fprintf(stderr, "ERROR: Division by zero\n");
                        exit(EXIT_FAILURE);
                    }
                    bruter_push_float(stack, a.value.f / b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    if (b.value.i == 0)
                    {
                        fprintf(stderr, "ERROR: Division by zero\n");
                        exit(EXIT_FAILURE);
                    }
                    bruter_push_float(stack, a.value.f / b.value.i, NULL, BRUTER_TYPE_FLOAT);
                    break;
            }
            break;
        default:
            switch (b.type)
            {
                case BRUTER_TYPE_FLOAT:
                    if (b.value.f == 0.0)
                    {
                        fprintf(stderr, "ERROR: Division by zero\n");
                        exit(EXIT_FAILURE);
                    }
                    bruter_push_float(stack, a.value.i / b.value.f, NULL, BRUTER_TYPE_FLOAT);
                    break;
                default:
                    if (b.value.i == 0)
                    {
                        fprintf(stderr, "ERROR: Division by zero\n");
                        exit(EXIT_FAILURE);
                    }
                    bruter_push_int(stack, a.value.i / b.value.i, NULL, BRUTER_TYPE_ANY);
                    break;
            }
            break;
    }
}
function(feraw_mod)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Modulus operator not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    if (b.value.i == 0)
    {
        fprintf(stderr, "ERROR: Division by zero in modulus operation\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i % b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_pow)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        bruter_push_float(stack, pow(a.value.f, b.value.f), NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        bruter_push_int(stack, (BruterInt)pow((double)a.value.i, (double)b.value.i), NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_sqrt)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type != BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Square root function requires a float argument\n");
        exit(EXIT_FAILURE);
    }
    if (a.value.f < 0.0)
    {
        fprintf(stderr, "ERROR: Cannot compute square root of negative number\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_float(stack, sqrt(a.value.f), NULL, BRUTER_TYPE_FLOAT);
}
function(feraw_abs)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT)
    {
        bruter_push_float(stack, fabs(a.value.f), NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        bruter_push_int(stack, a.value.i < 0 ? -a.value.i : a.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_min)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        double val_a = (a.type == BRUTER_TYPE_FLOAT) ? a.value.f : (double)a.value.i;
        double val_b = (b.type == BRUTER_TYPE_FLOAT) ? b.value.f : (double)b.value.i;
        bruter_push_float(stack, fmin(val_a, val_b), NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        bruter_push_int(stack, (a.value.i < b.value.i) ? a.value.i : b.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_max)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        double val_a = (a.type == BRUTER_TYPE_FLOAT) ? a.value.f : (double)a.value.i;
        double val_b = (b.type == BRUTER_TYPE_FLOAT) ? b.value.f : (double)b.value.i;
        bruter_push_float(stack, fmax(val_a, val_b), NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        bruter_push_int(stack, (a.value.i > b.value.i) ? a.value.i : b.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_sin)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type != BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Sine function requires a float argument\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_float(stack, sin(a.value.f), NULL, BRUTER_TYPE_FLOAT);
}
function(feraw_cos)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type != BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Cosine function requires a float argument\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_float(stack, cos(a.value.f), NULL, BRUTER_TYPE_FLOAT);
}
function(feraw_tan)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type != BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Tangent function requires a float argument\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_float(stack, tan(a.value.f), NULL, BRUTER_TYPE_FLOAT);
}
function(feraw_inc)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT)
    {
        a.value.f += 1.0;
        bruter_push_float(stack, a.value.f, NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        a.value.i += 1;
        bruter_push_int(stack, a.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_dec)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT)
    {
        a.value.f -= 1.0;
        bruter_push_float(stack, a.value.f, NULL, BRUTER_TYPE_FLOAT);
    }
    else
    {
        a.value.i -= 1;
        bruter_push_int(stack, a.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
// random functions
function(feraw_seed)
{
    BruterMeta seed_meta = bruter_pop_meta(stack);
    unsigned int seed;
    if (seed_meta.type == BRUTER_TYPE_FLOAT)
    {
        seed = (unsigned int)seed_meta.value.f;
    }
    else
    {
        seed = (unsigned int)seed_meta.value.i;
    }
    srand(seed);
}
function(feraw_rand)
{
    BruterFloat result = rand();
    bruter_push_float(stack, result, NULL, BRUTER_TYPE_FLOAT);
}
function(feraw_random)
{
    BruterMeta min_meta = bruter_pop_meta(stack);
    BruterMeta max_meta = bruter_pop_meta(stack);
    if (min_meta.type == BRUTER_TYPE_FLOAT || max_meta.type == BRUTER_TYPE_FLOAT)
    {
        BruterFloat min = 0, max = 0;
        switch (min_meta.type)
        {
            case BRUTER_TYPE_FLOAT:
                min = min_meta.value.f;
                break;
            default:
                min = (BruterFloat)min_meta.value.i;
                break;
        }
        switch (max_meta.type)
        {
            case BRUTER_TYPE_FLOAT:
                max = max_meta.value.f;
                break;
            default:
                max = (BruterFloat)max_meta.value.i;
                break;
        }
        BruterFloat result = fmod(rand(), (max - min + 1) + min);
        bruter_push_float(stack, result, NULL, BRUTER_TYPE_FLOAT);
    }
    else // definively int
    {
        BruterInt min, max;
        min = min_meta.value.i;
        max = min_meta.value.i;
        BruterInt result = rand() % (max - min + 1) + min;
        bruter_push_int(stack, result, NULL, BRUTER_TYPE_ANY);
    }
}
// rouding functions
function(feraw_floor)
{
    BruterFloat value = bruter_pop_float(stack);
    bruter_push_int(stack, (BruterInt)floor(value), NULL, BRUTER_TYPE_ANY);
}
function(feraw_ceil)
{
    BruterFloat value = bruter_pop_float(stack);
    bruter_push_int(stack, (BruterInt)ceil(value), NULL, BRUTER_TYPE_ANY);
}
function(feraw_round)
{
    BruterFloat value = bruter_pop_float(stack);
    bruter_push_int(stack, (BruterInt)round(value), NULL, BRUTER_TYPE_ANY);
}
function(feraw_trunc)
{
    BruterFloat value = bruter_pop_float(stack);
    bruter_push_int(stack, (BruterInt)trunc(value), NULL, BRUTER_TYPE_ANY);
}
// bitwise operations
function(feraw_bit_and)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise AND operator not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i & b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_bit_or)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise OR operator not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i | b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_bit_xor)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise XOR operator not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i ^ b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_bit_not)
{
    BruterMeta a = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise NOT operator not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, ~a.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_lshift)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise shift operators not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i << b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_rshift)
{
    BruterMeta a = bruter_pop_meta(stack);
    BruterMeta b = bruter_pop_meta(stack);
    if (a.type == BRUTER_TYPE_FLOAT || b.type == BRUTER_TYPE_FLOAT)
    {
        fprintf(stderr, "ERROR: Bitwise shift operators not supported for float types\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_int(stack, a.value.i >> b.value.i, NULL, BRUTER_TYPE_ANY);
}
function(feraw_time)
{
    bruter_push_int(stack, (BruterInt)time(NULL), NULL, BRUTER_TYPE_ANY);
}
function(feraw_clock)
{
    bruter_push_int(stack, (BruterInt)clock(), NULL, BRUTER_TYPE_ANY);
}
/* end include: ./std/math.c */
/* start include: ./std/mem.c */
#include "bruter.h"
function(feraw_malloc)
{
    BruterInt size = bruter_pop_int(stack);
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, "ERROR: Memory allocation failed for size %" PRIdPTR "\n", size);
        exit(EXIT_FAILURE);
    }
    bruter_push_pointer(stack, ptr, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_calloc)
{
    BruterInt count = bruter_pop_int(stack);
    BruterInt size = bruter_pop_int(stack);
    void *ptr = calloc(count, size);
    if (ptr == NULL)
    {
        fprintf(stderr, "ERROR: Memory allocation failed for count %" PRIdPTR " and size %" PRIdPTR "\n", count, size);
        exit(EXIT_FAILURE);
    }
    bruter_push_pointer(stack, ptr, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_realloc)
{
    void *ptr = bruter_pop_pointer(stack);
    BruterInt new_size = bruter_pop_int(stack);
    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Memory reallocation failed for size %" PRIdPTR "\n", new_size);
        exit(EXIT_FAILURE);
    }
    bruter_push_pointer(stack, new_ptr, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_memset)
{
    void *ptr = bruter_pop_pointer(stack);
    BruterInt value = bruter_pop_int(stack);
    BruterInt size = bruter_pop_int(stack);
    if (ptr == NULL)
    {
        fprintf(stderr, "ERROR: Cannot set memory, pointer is NULL\n");
        exit(EXIT_FAILURE);
    }
    memset(ptr, value, size);
}
function(feraw_memcpy)
{
    void *dest = bruter_pop_pointer(stack);
    void *src = bruter_pop_pointer(stack);
    BruterInt size = bruter_pop_int(stack);
    if (dest == NULL || src == NULL)
    {
        fprintf(stderr, "ERROR: Cannot copy memory, one of the pointers is NULL\n");
        exit(EXIT_FAILURE);
    }
    memcpy(dest, src, size);
}
function(feraw_memcmp)
{
    void *ptr1 = bruter_pop_pointer(stack);
    void *ptr2 = bruter_pop_pointer(stack);
    BruterInt size = bruter_pop_int(stack);
    if (ptr1 == NULL || ptr2 == NULL)
    {
        fprintf(stderr, "ERROR: Cannot compare memory, one of the pointers is NULL\n");
        exit(EXIT_FAILURE);
    }
    BruterInt result = memcmp(ptr1, ptr2, size);
    bruter_push_int(stack, result, NULL, BRUTER_TYPE_ANY);
}
function(feraw_memmove)
{
    void *dest = bruter_pop_pointer(stack);
    void *src = bruter_pop_pointer(stack);
    BruterInt size = bruter_pop_int(stack);
    if (dest == NULL || src == NULL)
    {
        fprintf(stderr, "ERROR: Cannot move memory, one of the pointers is NULL\n");
        exit(EXIT_FAILURE);
    }
    memmove(dest, src, size);
}
/* end include: ./std/mem.c */
/* start include: ./std/io.c */
#include <bruter.h>
function (feraw_print)
{
    BruterMeta value = bruter_pop_meta(stack);
    switch (value.type)
    {
        case BRUTER_TYPE_FLOAT:
            printf("%f", value.value.f);
            break;
        case BRUTER_TYPE_BUFFER:
            printf("%s", (char*)value.value.p);
            break;
        case BRUTER_TYPE_LIST:
            for (BruterInt i = 0; i < ((BruterList*)value.value.p)->size; i++)
            {
                BruterMeta item = bruter_get_meta((BruterList*)value.value.p, i);
                switch (item.type)
                {
                    case BRUTER_TYPE_FLOAT:
                        printf("%f ", item.value.f);
                        break;
                    case BRUTER_TYPE_BUFFER:
                        printf("%s ", (char*)item.value.p);
                        break;
                    case BRUTER_TYPE_LIST:
                        printf("[List] ");
                        break;
                    default:
                        printf("%ld ", item.value.i);
                        break;
                }
            }
            break;
        default:
            printf("%ld", value.value.i);
            break;
    }
}
function(feraw_print_string)
{
    char *str = bruter_pop_pointer(stack);
    if (str == NULL)
    {
        printf("NULL");
        return;
    }
    printf("%s", str);
}
function(feraw_print_int)
{
    BruterInt value = bruter_pop_int(stack);
    printf("%ld", value);
}
function(feraw_print_float)
{
    BruterFloat value = bruter_pop_float(stack);
    printf("%f", value);
}
function(feraw_print_bool)
{
    BruterInt value = bruter_pop_int(stack);
    printf(value ? "true" : "false");
}
function(feraw_println)
{
    feraw_print(stack);
    printf("\n");
}
function(feraw_ls)
{
    // [index, type, "name"] = value;
    BruterList* list = bruter_pop_pointer(stack);
    for (BruterInt i = 0; i < list->size; i++)
    {
        printf("[%ld", i);
        if (list->types != NULL)
        {
            printf(", %d", list->types[i]);
        }
        if (list->keys != NULL)
        {
            if (list->keys[i] != NULL)
            {
                printf(", '%s'] = ", list->keys[i]);
            }
            else
            {
                printf("] = ");
            }
        }
        else
        {
            printf("] = ");
        }
        if (list->types == NULL)
        {
            printf("%ld\n", list->data[i].i);
            continue;
        }
        else 
        {
            switch (list->types[i])
            {
                case BRUTER_TYPE_FLOAT:
                    printf("%f\n", list->data[i].f);
                    break;
                case BRUTER_TYPE_BUFFER:
                    printf("%s\n", (char*)list->data[i].p);
                    break;
                case BRUTER_TYPE_LIST:
                    printf("[List]\n");
                    break;
                default:
                    printf("%ld\n", list->data[i].i);
                    break;
            }
        }
    }
}
/* end include: ./std/io.c */
/* start include: ./std/string.c */
#include "bruter.h"
function(feraw_strdup)
{
    char* str = bruter_pop_pointer(stack);
    if (str == NULL)
    {
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_ANY);
        return;
    }
    char* new_str = strdup(str);
    if (new_str == NULL)
    {
        printf("BRUTER_ERROR: failed to duplicate string\n");
        exit(EXIT_FAILURE);
    }
    bruter_push_pointer(stack, new_str, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_strcat)
{
    char* str1 = bruter_pop_pointer(stack);
    char* str2 = bruter_pop_pointer(stack);
    if (str1 == NULL || str2 == NULL)
    {
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_ANY);
        return;
    }
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* new_str = (char*)malloc(len1 + len2 + 1);
    if (new_str == NULL)
    {
        printf("BRUTER_ERROR: failed to allocate memory for concatenated string\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_str, str1);
    strcat(new_str, str2);
    bruter_push_pointer(stack, new_str, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_strcpy)
{
    char* dest = bruter_pop_pointer(stack);
    char* src = bruter_pop_pointer(stack);
    if (src == NULL || dest == NULL)
    {
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_ANY);
        return;
    }
    strcpy(dest, src);
    bruter_push_pointer(stack, dest, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_strncpy)
{
    char* dest = bruter_pop_pointer(stack);
    char* src = bruter_pop_pointer(stack);
    BruterInt n = bruter_pop_int(stack);
    if (src == NULL || dest == NULL)
    {
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_ANY);
        return;
    }
    strncpy(dest, src, n);
    dest[n] = '\0'; // Ensure null termination
    bruter_push_pointer(stack, dest, NULL, BRUTER_TYPE_BUFFER);
}
function (feraw_strcmp)
{
    char* str1 = bruter_pop_pointer(stack);
    char* str2 = bruter_pop_pointer(stack);
    if (str1 == NULL || str2 == NULL)
    {
        bruter_push_int(stack, 0, NULL, BRUTER_TYPE_ANY);
        return;
    }
    int result = strcmp(str1, str2);
    bruter_push_int(stack, result, NULL, BRUTER_TYPE_ANY);
}
function(feraw_strlen)
{
    char* str = bruter_pop_pointer(stack);
    if (str == NULL)
    {
        bruter_push_int(stack, 0, NULL, BRUTER_TYPE_ANY);
        return;
    }
    BruterInt length = strlen(str);
    bruter_push_int(stack, length, NULL, BRUTER_TYPE_ANY);
}
function(feraw_strchr)
{
    char* str = bruter_pop_pointer(stack);
    char c = (char)bruter_pop_int(stack);
    char* result = strchr(str, c);
    bruter_push_pointer(stack, result, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_strstr)
{
    char* str1 = bruter_pop_pointer(stack);
    char* str2 = (char*)bruter_pop_int(stack);
    char* result = strstr(str1, str2);
    bruter_push_pointer(stack, result, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_strsplit)
{
    char* original_str = (char*)bruter_pop_pointer(stack);
    char* delimiters = (char*)bruter_pop_pointer(stack);
    BruterList* result = bruter_new(BRUTER_DEFAULT_SIZE, false, true);
    for (char* token = strtok(original_str, delimiters); token != NULL; token = strtok(NULL, delimiters))
    {
        bruter_push_pointer(result, strdup(token), NULL, BRUTER_TYPE_BUFFER);
    }
    bruter_push_pointer(stack, result, NULL, BRUTER_TYPE_LIST);
}
/* end include: ./std/string.c */
/* start include: ./std/condition.c */
#include "bruter.h"
function(feraw_equals)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a == b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_not_equals)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a != b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_greater)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a > b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_greater_equal)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a >= b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_less)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a < b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_less_equal)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a <= b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_and)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a && b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_or)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a || b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
function(feraw_includes)
{
    BruterInt a = bruter_pop_int(stack);
    BruterInt b = bruter_pop_int(stack);
    bruter_push_int(stack, a || b ? 1 : 0, NULL, BRUTER_TYPE_ANY);
}
/* end include: ./std/condition.c */
/* start include: ./std/types.c */
#include "bruter.h"
function(feraw_int)
{
    BruterMeta meta = bruter_pop_meta(stack);
    if (meta.type == BRUTER_TYPE_FLOAT)
    {
        bruter_push_int(stack, (BruterInt)meta.value.f, NULL, BRUTER_TYPE_ANY);
    }
    else if (meta.type == BRUTER_TYPE_BUFFER)
    {
        bruter_push_int(stack, atoi((char*)meta.value.p), NULL, BRUTER_TYPE_ANY);
    }
    else
    {
        bruter_push_int(stack, meta.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_float)
{
    BruterMeta meta = bruter_pop_meta(stack);
    if (meta.type == BRUTER_TYPE_FLOAT)
    {
        bruter_push_float(stack, meta.value.f, NULL, BRUTER_TYPE_ANY);
    }
    else if (meta.type == BRUTER_TYPE_BUFFER)
    {
        bruter_push_float(stack, atof((char*)meta.value.p), NULL, BRUTER_TYPE_ANY);
    }
    else
    {
        bruter_push_float(stack, (float)meta.value.i, NULL, BRUTER_TYPE_ANY);
    }
}
function(feraw_string)
{
    BruterMeta meta = bruter_pop_meta(stack);
    char buffer[32];
    if (meta.type == BRUTER_TYPE_FLOAT)
    {
        snprintf(buffer, sizeof(buffer), "%f", meta.value.f);
        bruter_push_pointer(stack, strdup(buffer), NULL, BRUTER_TYPE_BUFFER);
    }
    else if (meta.type == BRUTER_TYPE_BUFFER)
    {
        bruter_push_pointer(stack, strdup((char*)meta.value.p), NULL, BRUTER_TYPE_BUFFER);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "%ld", meta.value.i);
        bruter_push_pointer(stack, strdup(buffer), NULL, BRUTER_TYPE_BUFFER);
    }
}
/* end include: ./std/types.c */
/* start include: ./std/os.c */
#include <bruter.h>
// file stuff
char* file_read(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return NULL;
    }
    char *code = (char*)malloc(1);
    if (code == NULL)
    {
        printf("ERROR: could not allocate memory for file\n");
        fclose(file);
        return NULL;
    }
    code[0] = '\0';
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1)
    {
        size_t new_size = strlen(code) + strlen(line) + 1;
        char *temp = realloc(code, new_size);
        if (temp == NULL)
        {
            printf("ERROR: could not reallocate memory while reading file\n");
            free(code);
            free(line);
            fclose(file);
            return NULL;
        }
        code = temp;
        strcat(code, line);
    }
    free(line);
    fclose(file);
    return code;
}
void file_write(char *filename, char *code)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        return;
    }
    fprintf(file, "%s", code);
    fclose(file);
}
bool file_exists(char* filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return false;
    }
    fclose(file);
    return true;
}
function(feraw_read_bin)
{
    char *filename = bruter_pop_pointer(stack);
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_BUFFER);
        return;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *buffer = (uint8_t*)malloc(size);
    if (buffer == NULL)
    {
        fclose(file);
        bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_BUFFER);
        return;
    }
    fread(buffer, 1, size, file);
    fclose(file);
    bruter_push_pointer(stack, buffer, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_write_bin)
{
    char *filename = bruter_pop_pointer(stack);
    uint8_t *buffer = bruter_pop_pointer(stack);
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        return;
    }
    // Assuming the first 4 bytes of the buffer contain the size
    uint32_t size = *((uint32_t*)buffer);
    fwrite(buffer + 4, 1, size, file);
    fclose(file);
}
function(feraw_read_file)
{
    char *filename = bruter_pop_pointer(stack);
    char *content = file_read(filename);
    bruter_push_pointer(stack, content, NULL, BRUTER_TYPE_BUFFER);
}
function(feraw_write_file)
{
    char *filename = bruter_pop_pointer(stack);
    char *content = bruter_pop_pointer(stack);
    file_write(filename, content);
}
function(feraw_file_exists)
{
    char *filename = bruter_pop_pointer(stack);
    bool exists = file_exists(filename);
    bruter_push_int(stack, exists ? 1 : 0, NULL, BRUTER_TYPE_ANY); // Push 1 if exists, 0 if not
}
/* end include: ./std/os.c */
/* start include: ./std/time.c */
#include <bruter.h>
#ifdef _WIN32
    #include <windows.h>
    static long long time_ms() {
        static LARGE_INTEGER freq;
        static int initialized = 0;
        if (!initialized) {
            QueryPerformanceFrequency(&freq);
            initialized = 1;
        }
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return (long long)(counter.QuadPart * 1000 / freq.QuadPart);
    }
#else
    #include <time.h>
    static long long time_ms() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
#endif
function(feraw_ms)
{
    bruter_push_int(stack, time_ms(), NULL, BRUTER_TYPE_ANY);
}
/* end include: ./std/time.c */
/* start include: ./std/std.c */
#include "bruter.h"
function(feraw_drop)
{
    bruter_pop_pointer(stack);
}
function(feraw_eval)
{
    BruterList* context = bruter_pop_pointer(stack);
    BruterMeta value = bruter_pop_meta(stack);
    char* code = NULL;
    BruterList* splited = NULL;
    switch (value.type)
    {
        case BRUTER_TYPE_LIST:
            splited = (BruterList*)value.value.p;
            break;
        case BRUTER_TYPE_BUFFER:
            code = (char*)value.value.p;
            break;
        default:
            fprintf(stderr, "ERROR: cannot eval, expected buffer or any type, got %d\n", value.type);
            exit(EXIT_FAILURE);
    }
    bruter_interpret(context, code, splited, stack);
}
/* end include: ./std/std.c */
/* start include: bsr.c */
#define BSR_H 1
#define OLIVEC_IMPLEMENTATION
#include "olive.c"
#include "MiniFB.h"
#include <bruter.h>
// olive.c RGBA to ARGB patch
// i am not sure if this patch does anything at all
// seems right, but i see no difference at all between the image generated using this patch and the original olive.c without this patch
// lets first undefine the olivec original macros
#undef OLIVEC_ALPHA
#undef OLIVEC_RED
#undef OLIVEC_GREEN
#undef OLIVEC_BLUE
#undef OLIVEC_ARGB
#undef OLIVEC_RGBA
// now redefine them to use ARGB instead of RGBA
#define OLIVEC_ALPHA(color)   (((color) & 0xFF000000) >> 24)
#define OLIVEC_RED(color)     (((color) & 0x00FF0000) >> 16)
#define OLIVEC_GREEN(color)   (((color) & 0x0000FF00) >> 8)
#define OLIVEC_BLUE(color)    (((color) & 0x000000FF) >> 0)
#define OLIVEC_ARGB(a,r,g,b)  ( ((a)<<24) | ((r)<<16) | ((g)<<8) | (b) )
#define OLIVEC_RGBA(r,g,b,a)  OLIVEC_ARGB(a, r, g, b)
typedef struct 
{
    struct mfb_window *window; // window pointer
    char *title; // window title
    Olivec_Canvas canvas; // canvas for drawing
    uint8_t keys[350]; // key states
    uint8_t key_mods[6]; // shift, ctrl, alt, super, caps lock, num lock
    float mouse_scroll_delta_x; // mouse scroll x
    float mouse_scroll_delta_y; // mouse scroll y
    char character; // last character input
    uint8_t flags;
} BSRWindow;
BruterList *bsr_windows = NULL;
// callbacks
static void resize(struct mfb_window *window, int width, int height)
{
    uint32_t x = 0;
    uint32_t y = 0;
    BSRWindow *current_window = (BSRWindow *)mfb_get_user_data(window);
    if(width > current_window->canvas.width)
    {
        x = (width - current_window->canvas.width) >> 1;
        width = current_window->canvas.width;
    }
    if(height > current_window->canvas.height)
    {
        y = (height - current_window->canvas.height) >> 1;
        height = current_window->canvas.height;
    }
    current_window->canvas.pixels = realloc(current_window->canvas.pixels, width * height * 4); // reallocate buffer for the new window size
    if (current_window->canvas.pixels == NULL) {
        printf("Failed to reallocate buffer for window\n");
        return;
    }
    current_window->canvas.width = width;
    current_window->canvas.height = height;
    mfb_set_viewport(window, x, y, width, height);
}
static void keyboard(struct mfb_window *window, mfb_key key, mfb_key_mod mod, bool isPressed) 
{
    BSRWindow *current_window = (BSRWindow *)mfb_get_user_data(window);
    current_window->keys[key] = isPressed ? 1 : 0;
    current_window->key_mods[0] = (mod & KB_MOD_SHIFT) ? 1 : 0;
    current_window->key_mods[1] = (mod & KB_MOD_CONTROL) ? 1 : 0;
    current_window->key_mods[2] = (mod & KB_MOD_ALT) ? 1 : 0;
    current_window->key_mods[3] = (mod & KB_MOD_SUPER) ? 1 : 0;
    current_window->key_mods[4] = (mod & KB_MOD_CAPS_LOCK) ? 1 : 0;
    current_window->key_mods[5] = (mod & KB_MOD_NUM_LOCK) ? 1 : 0;
}
static void char_input(struct mfb_window *window, unsigned int charCode) 
{
    BSRWindow *current_window = (BSRWindow *)mfb_get_user_data(window);
    current_window->character = (char)charCode;
}
static void mouse_btn(struct mfb_window *window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) 
{
    BSRWindow *current_window = (BSRWindow *)mfb_get_user_data(window);
    if (button == MOUSE_LEFT) 
    {
        current_window->keys[MOUSE_BTN_1] = isPressed ? 1 : 0;
    } 
    else if (button == MOUSE_RIGHT) 
    {
        current_window->keys[MOUSE_BTN_2] = isPressed ? 1 : 0;
    } 
    else if (button == MOUSE_MIDDLE) 
    {
        current_window->keys[MOUSE_BTN_3] = isPressed ? 1 : 0;
    } 
    else if (button >= MOUSE_BTN_4 && button <= MOUSE_BTN_7) 
    {
        current_window->keys[button] = isPressed ? 1 : 0;
    }
    current_window->key_mods[0] = (mod & KB_MOD_SHIFT) ? 1 : 0;
    current_window->key_mods[1] = (mod & KB_MOD_CONTROL) ? 1 : 0;
    current_window->key_mods[2] = (mod & KB_MOD_ALT) ? 1 : 0;
    current_window->key_mods[3] = (mod & KB_MOD_SUPER) ? 1 : 0;
    current_window->key_mods[4] = (mod & KB_MOD_CAPS_LOCK) ? 1 : 0;
    current_window->key_mods[5] = (mod & KB_MOD_NUM_LOCK) ? 1 : 0;
}
static void mouse_scroll(struct mfb_window *window, mfb_key_mod mod, float deltaX, float deltaY) 
{
    BSRWindow *current_window = (BSRWindow *)mfb_get_user_data(window);
    current_window->mouse_scroll_delta_x += deltaX;
    current_window->mouse_scroll_delta_y += deltaY;
    current_window->key_mods[0] = (mod & KB_MOD_SHIFT) ? 1 : 0;
    current_window->key_mods[1] = (mod & KB_MOD_CONTROL) ? 1 : 0;
    current_window->key_mods[2] = (mod & KB_MOD_ALT) ? 1 : 0;
    current_window->key_mods[3] = (mod & KB_MOD_SUPER) ? 1 : 0;
    current_window->key_mods[4] = (mod & KB_MOD_CAPS_LOCK) ? 1 : 0;
    current_window->key_mods[5] = (mod & KB_MOD_NUM_LOCK) ? 1 : 0;
}
// functions
function(new_window) 
{
    char *title = bruter_pop_pointer(stack);
    int width = bruter_pop_int(stack);
    int height = bruter_pop_int(stack);
    int flags = bruter_pop_int(stack);
    struct mfb_window *window = mfb_open_ex(title, width, height, flags);
    if (window == NULL) 
    {
        printf("Failed to create window\n");
        return;
    }
    BSRWindow *bsr_window = (BSRWindow*)malloc(sizeof(BSRWindow));
    if (bsr_window == NULL) 
    {
        printf("Failed to allocate memory for BSRWindow\n");
        mfb_close(window);
        return;
    }
    bsr_window->window = window;
    bsr_window->title = title;
    bsr_window->canvas = olivec_canvas(
        (uint32_t*)malloc(width * height * sizeof(uint32_t)), // allocate memory for pixels
        width, height, width // stride is the same as width in pixels
    );
    bsr_window->flags = flags;
    memset(bsr_window->keys, 0, sizeof(bsr_window->keys));
    memset(bsr_window->key_mods, 0, sizeof(bsr_window->key_mods));
    bsr_window->mouse_scroll_delta_x = 0.0f;
    bsr_window->mouse_scroll_delta_y = 0.0f;
    bsr_window->character = 0;
    mfb_set_user_data(window, bsr_window);
    mfb_set_resize_callback(window, resize);
    mfb_set_keyboard_callback(window, keyboard);
    mfb_set_char_input_callback(window, char_input);
    mfb_set_mouse_button_callback(window, mouse_btn);
    mfb_set_mouse_scroll_callback(window, mouse_scroll);
    mfb_set_viewport(window, 0, 0, width, height);
    bruter_push_pointer(bsr_windows, bsr_window, NULL, 0);
    bruter_push_pointer(stack, bsr_window, NULL, BRUTER_TYPE_BUFFER);
}
function(close_window)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    for (BruterInt i = 0; i < bsr_windows->size; i++)
    {
        BSRWindow *bsr_window = (BSRWindow*)bsr_windows->data[i].p;
        if (bsr_window && bsr_window->window == window)
        {
            free(bsr_window->canvas.pixels);
            free(bsr_window);
            bsr_windows->data[i].p = NULL; // mark as removed
            bruter_remove(bsr_windows, i);
            break;
        }
    }
    mfb_close(window);
}
function(update_window) 
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    uint8_t *buffer = (uint8_t*)(current_window)->canvas.pixels;
    int width = current_window->canvas.width;
    int height = current_window->canvas.height;
    if (window == NULL || buffer == NULL)
    {
        printf("Window or buffer is NULL\n");
    }
    if (mfb_update_ex(window, buffer, width, height) < 0)
    {
        printf("Failed to update window\n");
    }
    return;
}
function(update_window_events) 
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    mfb_update_events(window);
}
function(is_window_active)
{
    bruter_push_int(stack, mfb_is_window_active((struct mfb_window*)((BSRWindow*)(bruter_pop_pointer(stack)))->window), NULL, BRUTER_TYPE_ANY);
}
function(get_window_width)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_window_width(window), NULL, BRUTER_TYPE_ANY);
}
function(get_window_height)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_window_height(window), NULL, BRUTER_TYPE_ANY);
}
function(get_mouse_x)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_mouse_x(window), NULL, BRUTER_TYPE_ANY);
}
function(get_mouse_y)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_mouse_y(window), NULL, BRUTER_TYPE_ANY);
}
function(get_mouse_scroll_x)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_mouse_scroll_x(window), NULL, BRUTER_TYPE_ANY);
}
function(get_mouse_scroll_y)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_int(stack, mfb_get_mouse_scroll_y(window), NULL, BRUTER_TYPE_ANY);
}
function(wait_sync)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    bruter_push_pointer(stack, current_window, NULL, BRUTER_TYPE_BUFFER);
}
// io functions
function(is_key_pressed)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    mfb_key key = (mfb_key)bruter_pop_int(stack);
    bruter_push_int(stack, current_window->keys[key], NULL, BRUTER_TYPE_ANY);
}
function(get_framebuffer)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    uint8_t *buffer = (uint8_t*)(current_window)->canvas.pixels;
    if (buffer == NULL)
    {
        printf("Buffer is NULL\n");
        return;
    }
    bruter_push_pointer(stack, buffer, NULL, BRUTER_TYPE_BUFFER);
}
// drawing functions
function(fill)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_fill(*canvas, color);    
}
function(draw_rect)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int width = bruter_pop_int(stack);
    int height = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_rect(*canvas, x, y, width, height, color);
}
function(draw_frame)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int width = bruter_pop_int(stack);
    int height = bruter_pop_int(stack);
    int thickness = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_frame(*canvas, x, y, width, height, thickness, color);
}
function(draw_circle)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int radius = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_circle(*canvas, x, y, radius, color);
}
function(draw_ellipse)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int radius_x = bruter_pop_int(stack);
    int radius_y = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_ellipse(*canvas, x, y, radius_x, radius_y, color);
}
function(draw_line)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_line(*canvas, x1, y1, x2, y2, color);
}
function(draw_triangle)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    int x3 = bruter_pop_int(stack);
    int y3 = bruter_pop_int(stack);
    uint32_t color = bruter_pop_int(stack);
    olivec_triangle(*canvas, x1, y1, x2, y2, x3, y3, color);
}
function(draw_triangle3c)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    int x3 = bruter_pop_int(stack);
    int y3 = bruter_pop_int(stack);
    uint32_t c1 = bruter_pop_int(stack);
    uint32_t c2 = bruter_pop_int(stack);
    uint32_t c3 = bruter_pop_int(stack);
    olivec_triangle3c(*canvas, x1, y1, x2, y2, x3, y3, c1, c2, c3);
}
function(draw_triangle3z)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    int x3 = bruter_pop_int(stack);
    int y3 = bruter_pop_int(stack);
    float z1 = bruter_pop_float(stack);
    float z2 = bruter_pop_float(stack);
    float z3 = bruter_pop_float(stack);
    olivec_triangle3z(*canvas, x1, y1, x2, y2, x3, y3, z1, z2, z3);
}
function(draw_triangle3uv)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    int x3 = bruter_pop_int(stack);
    int y3 = bruter_pop_int(stack);
    float tx1 = bruter_pop_float(stack);
    float ty1 = bruter_pop_float(stack);
    float tx2 = bruter_pop_float(stack);
    float ty2 = bruter_pop_float(stack);
    float tx3 = bruter_pop_float(stack);
    float ty3 = bruter_pop_float(stack);
    float z1 = bruter_pop_float(stack);
    float z2 = bruter_pop_float(stack);
    float z3 = bruter_pop_float(stack);
    Olivec_Canvas* texture = (Olivec_Canvas*)bruter_pop_pointer(stack);
    if (texture == NULL) {
        printf("Texture is NULL\n");
        return;
    }
    olivec_triangle3uv(*canvas, x1, y1, x2, y2, x3, y3, tx1, ty1, tx2, ty2, tx3, ty3, z1, z2, z3, *texture);
}
function(draw_triangle3uv_bilinear)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x1 = bruter_pop_int(stack);
    int y1 = bruter_pop_int(stack);
    int x2 = bruter_pop_int(stack);
    int y2 = bruter_pop_int(stack);
    int x3 = bruter_pop_int(stack);
    int y3 = bruter_pop_int(stack);
    float tx1 = bruter_pop_float(stack);
    float ty1 = bruter_pop_float(stack);
    float tx2 = bruter_pop_float(stack);
    float ty2 = bruter_pop_float(stack);
    float tx3 = bruter_pop_float(stack);
    float ty3 = bruter_pop_float(stack);
    float z1 = bruter_pop_float(stack);
    float z2 = bruter_pop_float(stack);
    float z3 = bruter_pop_float(stack);
    Olivec_Canvas* texture = (Olivec_Canvas*)bruter_pop_pointer(stack);
    if (texture == NULL) {
        printf("Texture is NULL\n");
        return;
    }
    olivec_triangle3uv_bilinear(*canvas, x1, y1, x2, y2, x3, y3, tx1, ty1, tx2, ty2, tx3, ty3, z1, z2, z3, *texture);
}
/*
OLIVECDEF void olivec_sprite_blend(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite);
OLIVECDEF void olivec_sprite_copy(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite);
OLIVECDEF void olivec_sprite_copy_bilinear(Olivec_Canvas oc, int x, int y, int w, int h, Olivec_Canvas sprite);
*/
function(draw_sprite_blend)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int w = bruter_pop_int(stack);
    int h = bruter_pop_int(stack);
    Olivec_Canvas* sprite = (Olivec_Canvas*)bruter_pop_pointer(stack);
    olivec_sprite_blend(*canvas, x, y, w, h, *sprite);
}
function(draw_sprite_copy)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int w = bruter_pop_int(stack);
    int h = bruter_pop_int(stack);
    Olivec_Canvas* sprite = (Olivec_Canvas*)bruter_pop_pointer(stack);
    olivec_sprite_copy(*canvas, x, y, w, h, *sprite);
}
function(draw_sprite_copy_bilinear)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    int x = bruter_pop_int(stack);
    int y = bruter_pop_int(stack);
    int w = bruter_pop_int(stack);
    int h = bruter_pop_int(stack);
    Olivec_Canvas* sprite = (Olivec_Canvas*)bruter_pop_pointer(stack);
    olivec_sprite_copy_bilinear(*canvas, x, y, w, h, *sprite);
}
function(argb_to_canvas)
{
    uint32_t *data = (uint32_t*)bruter_pop_pointer(stack);
    uint32_t width = data[0];
    uint32_t height = data[1];
    Olivec_Canvas* canvas = (Olivec_Canvas*)malloc(sizeof(Olivec_Canvas));
    if (canvas == NULL) {
        printf("Failed to allocate memory for Olivec_Canvas\n");
        return;
    }
    canvas->pixels = malloc(width * height * sizeof(uint32_t));
    memcpy(canvas->pixels, data + 2, width * height * sizeof(uint32_t));
    canvas->width = width;
    canvas->height = height;
    canvas->stride = width;
    bruter_push_pointer(stack, canvas, NULL, BRUTER_TYPE_BUFFER);
}
function(new_canvas)
{
    uint32_t width = bruter_pop_int(stack);
    uint32_t height = bruter_pop_int(stack);
    Olivec_Canvas* canvas = (Olivec_Canvas*)malloc(sizeof(Olivec_Canvas));
    if (canvas == NULL) {
        printf("Failed to allocate memory for Olivec_Canvas\n");
        return;
    }
    canvas->pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (canvas->pixels == NULL) {
        printf("Failed to allocate memory for canvas pixels\n");
        free(canvas);
        return;
    }
    canvas->width = width;
    canvas->height = height;
    canvas->stride = width;
    bruter_push_pointer(stack, canvas, NULL, BRUTER_TYPE_BUFFER);
}
function(get_window_canvas)
{
    BSRWindow *current_window = (BSRWindow*)(bruter_pop_pointer(stack));
    struct mfb_window *window = (struct mfb_window*)(current_window)->window;
    if (window == NULL)
    {
        printf("Window is NULL\n");
        return;
    }
    Olivec_Canvas* canvas = &current_window->canvas;
    bruter_push_pointer(stack, canvas, NULL, BRUTER_TYPE_BUFFER);
}
function(canvas_get_width)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    if (canvas == NULL)
    {
        printf("Canvas is NULL\n");
        return;
    }
    bruter_push_int(stack, canvas->width, NULL, BRUTER_TYPE_ANY);
}
function(canvas_get_height)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    if (canvas == NULL)
    {
        printf("Canvas is NULL\n");
        return;
    }
    bruter_push_int(stack, canvas->height, NULL, BRUTER_TYPE_ANY);
}
function(free_canvas)
{
    Olivec_Canvas* canvas = (Olivec_Canvas*)bruter_pop_pointer(stack);
    if (canvas == NULL)
    {
        printf("Canvas is NULL\n");
        return;
    }
    if (canvas->pixels != NULL)
    {
        free(canvas->pixels);
    }
    free(canvas);
}
void __bsr_at_exit(void)
{
    if (bsr_windows != NULL)
    {
        for (BruterInt i = 0; i < bsr_windows->size; i++)
        {
            BSRWindow *bsr_window = (BSRWindow*)bsr_windows->data[i].p;
            if (bsr_window != NULL)
            {
                mfb_close(bsr_window->window);
                free(bsr_window);
            }
        }
        bruter_free(bsr_windows);
    }
}
function(init_bsr)
{
    BruterList *context = bruter_pop_pointer(stack);
    bsr_windows = bruter_new(BRUTER_DEFAULT_SIZE, true, false);
    // add bsr_windows to context
    bruter_push_pointer(context, bsr_windows, "bsr_windows", BRUTER_TYPE_LIST);
    // register at exit function
    atexit(__bsr_at_exit);
}
/* end include: bsr.c */
    /* lê um u32 little-endian do arquivo (independente da endianness da CPU) */
static int read_u32_le(FILE *f, uint32_t *out) {
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4) return 0;
    *out = (uint32_t)b[0] | ((uint32_t)b[1]<<8) | ((uint32_t)b[2]<<16) | ((uint32_t)b[3]<<24);
    return 1;
}
/* Retorna arr no layout: arr[0]=w, arr[1]=h, arr[2..]=pixels em 0xAARRGGBB.
   Retorna NULL em erro. Use free() no retorno. */
    uint32_t *load_argb_as_header_layout(const char *path) {
        FILE *f = fopen(path, "rb");
        if (!f) { perror("fopen"); return NULL; }
        uint32_t w, h;
        if (!read_u32_le(f, &w) || !read_u32_le(f, &h)) {
            fprintf(stderr, "header inválido em %s\n", path);
            fclose(f);
            return NULL;
        }
        /* checa overflow em w*h*4 */
        if (h != 0 && w > SIZE_MAX / 4 / h) {
            fprintf(stderr, "overflow: %ux%u muito grande\n", w, h);
            fclose(f);
            return NULL;
        }
        size_t pixels = (size_t)w * (size_t)h;
        /* valida tamanho do arquivo: 8 + w*h*4 */
        if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); return NULL; }
        long fsz = ftell(f);
        if (fsz < 0) { perror("ftell"); fclose(f); return NULL; }
        long expected = 8 + (long)pixels * 4;
        if (fsz != expected) {
            fprintf(stderr, "tamanho incorreto: arquivo=%ld, esperado=%ld (%ux%u)\n",
                    fsz, expected, w, h);
            fclose(f);
            return NULL;
        }
        if (fseek(f, 8, SEEK_SET) != 0) { perror("fseek"); fclose(f); return NULL; }
        /* lê payload ARGB (bytes A,R,G,B) */
        uint8_t *tmp = (uint8_t *)malloc(pixels * 4);
        if (!tmp) { fprintf(stderr, "malloc tmp falhou\n"); fclose(f); return NULL; }
        if (fread(tmp, 1, pixels * 4, f) != pixels * 4) {
            fprintf(stderr, "erro lendo pixels\n");
            free(tmp);
            fclose(f);
            return NULL;
        }
        fclose(f);
        /* monta array no mesmo layout do .h */
        uint32_t *arr = (uint32_t *)malloc((pixels + 2) * sizeof(uint32_t));
        if (!arr) { fprintf(stderr, "malloc arr falhou\n"); free(tmp); return NULL; }
        arr[0] = w;
        arr[1] = h;
        /* repack: (A,R,G,B bytes) -> 0xAARRGGBB uint32 */
        for (size_t i = 0; i < pixels; i++) {
            uint8_t a = tmp[4*i + 0];
            uint8_t r = tmp[4*i + 1];
            uint8_t g = tmp[4*i + 2];
            uint8_t b = tmp[4*i + 3];
            arr[2 + i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
        free(tmp);
        return arr;
    }
    function(feraw_load_argb)
    {
        const char *filename = bruter_pop_pointer(stack);
        uint32_t *pixels = load_argb_as_header_layout(filename);
        if (pixels) 
        {
            bruter_push_pointer(stack, pixels, NULL, BRUTER_TYPE_BUFFER);
        } 
        else 
        {
            bruter_push_pointer(stack, NULL, NULL, BRUTER_TYPE_NULL);
        }
    }
    function(feraw_get_frame)
    {
        BruterList* player_images = bruter_pop_pointer(stack);
        BruterInt direction = bruter_pop_int(stack);
        BruterInt anim = 0;
        static BruterInt frame = 0;
    }
/* END cc blocks */

int main(int argc, char *argv[])
{
    BruterList *context = bruter_new(BRUTER_DEFAULT_SIZE, true, true);

    /* BEGIN function registrations */
    bruter_push_pointer(context, feraw_list, "list", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_pop, "pop", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_push, "push", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_shift, "shift", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_unshift, "unshift", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_insert, "insert", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_remove, "remove", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_get, "get", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_set, "set", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_where, "where", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_find, "find", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_length, "length", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_copy, "copy", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_swap, "swap", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_reverse, "reverse", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_point, "point", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_alloc, "alloc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_dup, "dup", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_free, "free", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_nameof, "nameof", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_typeof, "typeof", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_rename, "rename", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_retype, "retype", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_add, "add", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_sub, "sub", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_mul, "mul", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_div, "div", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_mod, "mod", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_pow, "pow", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_sqrt, "sqrt", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_abs, "abs", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_min, "min", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_max, "max", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_sin, "sin", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_cos, "cos", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_tan, "tan", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_inc, "inc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_dec, "dec", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_seed, "seed", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_rand, "rand", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_random, "random", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_floor, "floor", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_ceil, "ceil", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_round, "round", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_trunc, "trunc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_bit_and, "bit_and", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_bit_or, "bit_or", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_bit_xor, "bit_xor", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_bit_not, "bit_not", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_lshift, "lshift", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_rshift, "rshift", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_time, "time", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_clock, "clock", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_malloc, "malloc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_calloc, "calloc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_realloc, "realloc", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_memset, "memset", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_memcpy, "memcpy", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_memcmp, "memcmp", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_memmove, "memmove", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_print, "print", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_print_string, "print_string", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_print_int, "print_int", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_print_float, "print_float", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_print_bool, "print_bool", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_println, "println", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_ls, "ls", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strdup, "strdup", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strcat, "strcat", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strcpy, "strcpy", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strncpy, "strncpy", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strcmp, "strcmp", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strlen, "strlen", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strchr, "strchr", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strstr, "strstr", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_strsplit, "strsplit", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_equals, "equals", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_not_equals, "not_equals", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_greater, "greater", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_greater_equal, "greater_equal", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_less, "less", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_less_equal, "less_equal", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_and, "and", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_or, "or", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_includes, "includes", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_int, "int", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_float, "float", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_string, "string", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_read_bin, "read_bin", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_write_bin, "write_bin", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_read_file, "read_file", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_write_file, "write_file", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_file_exists, "file_exists", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_ms, "ms", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_drop, "drop", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_eval, "eval", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, new_window, "new_window", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, close_window, "close_window", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, update_window, "update_window", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, update_window_events, "update_window_events", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, is_window_active, "is_window_active", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_window_width, "get_window_width", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_window_height, "get_window_height", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_mouse_x, "get_mouse_x", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_mouse_y, "get_mouse_y", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_mouse_scroll_x, "get_mouse_scroll_x", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_mouse_scroll_y, "get_mouse_scroll_y", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, wait_sync, "wait_sync", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, is_key_pressed, "is_key_pressed", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_framebuffer, "get_framebuffer", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, fill, "fill", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_rect, "draw_rect", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_frame, "draw_frame", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_circle, "draw_circle", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_ellipse, "draw_ellipse", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_line, "draw_line", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_triangle, "draw_triangle", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_triangle3c, "draw_triangle3c", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_triangle3z, "draw_triangle3z", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_triangle3uv, "draw_triangle3uv", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_triangle3uv_bilinear, "draw_triangle3uv_bilinear", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_sprite_blend, "draw_sprite_blend", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_sprite_copy, "draw_sprite_copy", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, draw_sprite_copy_bilinear, "draw_sprite_copy_bilinear", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, argb_to_canvas, "argb_to_canvas", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, new_canvas, "new_canvas", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, get_window_canvas, "get_window_canvas", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, canvas_get_width, "canvas_get_width", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, canvas_get_height, "canvas_get_height", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, free_canvas, "free_canvas", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, init_bsr, "init_bsr", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_load_argb, "load_argb", BRUTER_TYPE_FUNCTION);
    bruter_push_pointer(context, feraw_get_frame, "get_frame", BRUTER_TYPE_FUNCTION);
    /* END function registrations */

    const char *embedded_code =
    "0	,Null	@	set	!\n"
    "1	,Any	@	set	!\n"
    "2	,Float	@	set	!\n"
    "3	,Buffer	@	set	!\n"
    "4	,List	@	set	!\n"
    "5	,Function	@	set	!\n"
    "0	,null	@	set	!\n"
    "Null	,null	@	retype	!\n"
    ",\\n \\t\\r	,split_tokens	@	set	!\n"
    "1	,bsr_flag_resizable	@	set	!\n"
    "2	,bsr_flag_fullscreen	@	set	!\n"
    "4	,bsr_flag_fullscreen_desktop	@	set	!\n"
    "8	,bsr_flag_borderless	@	set	!\n"
    "16	,bsr_flag_always_on_top	@	set	!\n"
    "@	init_bsr	!\n"
    "0	,anim_idle	@	set	!\n"
    "1	,anim_run	@	set	!\n"
    "2	,anim_use	@	set	!\n"
    "0	480	640	,you will be called 264	new_window	!	,win	@	set	!\n"
    "win	get_window_canvas	!	,canvas	@	set	!\n"
    "0	list	!	,images	@	set	!\n"
    "0	list	!	,player	images	set	!\n"
    "0	list	!	,idle	,player	images	get	!	set	!\n"
    "0	list	!	0	,idle	,player	images	get	!	get	!	set	!\n"
    ",data/player/idle/0/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/idle/0/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	0	,idle	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	,run	,player	images	get	!	set	!\n"
    "0	list	!	0	,run	,player	images	get	!	get	!	set	!\n"
    ",data/player/run/0/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/0/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	0	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	1	,run	,player	images	get	!	get	!	set	!\n"
    ",data/player/run/1/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/1/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	1	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	2	,run	,player	images	get	!	get	!	set	!\n"
    ",data/player/run/2/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/2/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	2	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	3	,run	,player	images	get	!	get	!	set	!\n"
    ",data/player/run/3/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/3/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	3	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	4	,run	,player	images	get	!	get	!	set	!\n"
    ",data/player/run/4/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/run/4/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	4	,run	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	,use	,player	images	get	!	set	!\n"
    "0	list	!	0	,use	,player	images	get	!	get	!	set	!\n"
    ",data/player/use/0/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/0/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	0	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	1	,use	,player	images	get	!	get	!	set	!\n"
    ",data/player/use/1/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/1/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	1	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "0	list	!	2	,use	,player	images	get	!	get	!	set	!\n"
    ",data/player/use/2/360.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	0	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/45.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	1	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/90.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	2	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/135.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	3	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/180.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	4	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/225.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	5	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/270.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	6	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    ",data/player/use/2/315.argb	load_argb	!	,argb_img	@	set	!\n"
    "argb_img	argb_to_canvas	!	7	2	,use	,player	images	get	!	get	!	get	!	set	!\n"
    "argb_img	free	!\n"
    "2	malloc	!	,direction	@	set	!\n"
    "0	0	direction	set	!\n"
    "0	1	direction	set	!\n"
    "0	list	!	,player	@	set	!\n"
    "0	list	!	,position	player	set	!\n"
    "0	,x	,position	player	get	!	set	!\n"
    "0	,y	,position	player	get	!	set	!\n"
    "0	list	!	,direction	player	set	!\n"
    "0	,x	,direction	player	get	!	set	!\n"
    "0	,y	,direction	player	get	!	set	!\n"
    ",idle	,state	player	set	!\n"
    "0	,rotation	player	set	!\n"
    "0	,frame	player	set	!\n"
    "50	,interval	@	set	!\n"
    "150	,frame_interval	@	set	!\n"
    "ms	!	,last_time	@	set	!\n"
    "2247	win	wait_sync	!	?	\n"
    "2911	1	?\n"
    "4278211925	canvas	fill	!	\n"
    "1996553984	50	120	160	canvas	draw_circle	!\n"
    "2274	2	interval	div	!	last_time	ms	!	sub	!	greater	!	?\n"
    "2778	1	?\n"
    "2283	265	win	is_key_pressed	!	?	\n"
    "2313	1	?\n"
    "-1	1	,direction	player	get	!	set	!	\n"
    "2	,y	,position	player	get	!	get	!	sub	!	,y	,position	player	get	!	set	!\n"
    ",run	,state	player	set	!\n"
    "2322	263	win	is_key_pressed	!	?\n"
    "2352	1	?\n"
    "-1	0	,direction	player	get	!	set	!	\n"
    "2	,x	,position	player	get	!	get	!	sub	!	,x	,position	player	get	!	set	!\n"
    ",run	,state	player	set	!\n"
    "2361	262	win	is_key_pressed	!	?\n"
    "2391	1	?\n"
    "1	0	,direction	player	get	!	set	!	\n"
    "2	,x	,position	player	get	!	get	!	add	!	,x	,position	player	get	!	set	!\n"
    ",run	,state	player	set	!\n"
    "2400	264	win	is_key_pressed	!	?\n"
    "2430	1	?\n"
    "1	1	,direction	player	get	!	set	!	\n"
    "2	,y	,position	player	get	!	get	!	add	!	,y	,position	player	get	!	set	!\n"
    ",run	,state	player	set	!\n"
    "2439	81	win	is_key_pressed	!	?\n"
    "2445	1	?\n"
    ",You pressed Q!	println	!	\n"
    "2911	1	?\n"
    "2472	0	1	,direction	player	get	!	get	!	equals	!	0	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2482	1	?\n"
    ",idle	,state	player	set	!	\n"
    "0	,frame	player	set	!\n"
    "2509	0	1	,direction	player	get	!	get	!	equals	!	1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2519	1	?\n"
    ",run	,state	player	set	!	\n"
    "4	,rotation	player	set	!\n"
    "2546	0	1	,direction	player	get	!	get	!	equals	!	-1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2556	1	?\n"
    ",run	,state	player	set	!	\n"
    "0	,rotation	player	set	!\n"
    "2583	1	1	,direction	player	get	!	get	!	equals	!	0	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2593	1	?\n"
    ",run	,state	player	set	!	\n"
    "2	,rotation	player	set	!\n"
    "2620	-1	1	,direction	player	get	!	get	!	equals	!	0	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2630	1	?\n"
    ",run	,state	player	set	!	\n"
    "6	,rotation	player	set	!\n"
    "2657	1	1	,direction	player	get	!	get	!	equals	!	1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2667	1	?\n"
    ",run	,state	player	set	!	\n"
    "3	,rotation	player	set	!\n"
    "2694	-1	1	,direction	player	get	!	get	!	equals	!	1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2704	1	?\n"
    ",run	,state	player	set	!	\n"
    "5	,rotation	player	set	!\n"
    "2731	1	1	,direction	player	get	!	get	!	equals	!	-1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2741	1	?\n"
    ",run	,state	player	set	!	\n"
    "1	,rotation	player	set	!\n"
    "2768	-1	1	,direction	player	get	!	get	!	equals	!	-1	0	,direction	player	get	!	get	!	equals	!	and	!	?\n"
    "2778	1	?\n"
    ",run	,state	player	set	!	\n"
    "7	,rotation	player	set	!\n"
    "2793	0	,run	,state	player	get	!	strcmp	!	equals	!	?\n"
    "2843	1	?\n"
    "2805	4	,frame	player	get	!	equals	!	?	\n"
    "2813	1	?\n"
    "0	,frame	player	set	!	\n"
    "2843	1	?\n"
    "2826	frame_interval	last_time	ms	!	sub	!	greater	!	?\n"
    "2843	1	?\n"
    "ms	!	,last_time	@	set	!	\n"
    "1	,frame	player	get	!	add	!	,frame	player	set	!\n"
    ",rotation	player	get	!	,frame	player	get	!	,state	player	get	!	,player	images	get	!	get	!	get	!	get	!	64	64	,y	,position	player	get	!	get	!	,x	,position	player	get	!	get	!	canvas	draw_sprite_blend	!	\n"
    "win	update_window	!\n"
    ",idle	,state	player	set	!\n"
    "0	0	,direction	player	get	!	set	!\n"
    "0	1	,direction	player	get	!	set	!\n"
    "2239	1	?\n"
    ;

    bruter_interpret(context, embedded_code, NULL, NULL);
    bruter_free(context);
    return EXIT_SUCCESS;
}
