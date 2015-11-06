#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

int main(void)
{
    int i;

    vector v;
    vector_init(&v);

    // char temp1[20]

    vector_add(&v, 1);
    vector_add(&v, 2);
    vector_add(&v, 3);
    vector_add(&v, 4);

    for (i = 0; i < vector_total(&v); i++)
        printf("%s ", (char *) vector_get(&v, i));
    printf("\n");

    vector_delete(&v, 3);
    vector_delete(&v, 2);
    vector_delete(&v, 1);

    vector_set(&v, 0, "Hello");
    vector_add(&v, "World");

    for (i = 0; i < vector_total(&v); i++)
        printf("%s ", (char *) vector_get(&v, i));
    printf("\n");

    vector_free(&v);
}