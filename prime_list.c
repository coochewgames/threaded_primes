#include <stdio.h>
#include <stdlib.h>

#include "prime_list.h"


static PRIME_ENTRY *create_prime_entry(PRIME_ENTRY *previous_entry, unsigned long prime);


PRIME_ENTRY *add_prime_to_list(volatile PRIME_LIST *list, PRIME_ENTRY *previous_entry, unsigned long prime)
{
    PRIME_ENTRY *new = create_prime_entry(previous_entry, prime);

    if (previous_entry == NULL)
    {
        list->first = new;
    }

    if (new->next == NULL)
    {
        list->last = new;
    }

    return new;
}

static PRIME_ENTRY *create_prime_entry(PRIME_ENTRY *previous_entry, unsigned long prime)
{
    PRIME_ENTRY *new = malloc(sizeof(PRIME_ENTRY));

    new->prime = prime;
    new->next = NULL;

    if (previous_entry != NULL)
    {
        if (previous_entry->next != NULL)
        {
            new->next = previous_entry->next;
        }

        previous_entry->next = new;
    }

    return new;
}

PRIME_ENTRY *insert_prime_list(PRIME_ENTRY *start_entry, PRIME_LIST new_list)
{
    if (new_list.first == NULL)
    {
        return NULL;
    }

    // Find the place in the list to insert the range of primes
    PRIME_ENTRY *current_entry = start_entry;

    while (current_entry->next != NULL)
    {
        if (current_entry->next->prime > new_list.first->prime)
        {
            break;
        }

        current_entry = current_entry->next;
    }

    PRIME_ENTRY *previous_next = current_entry->next;

    current_entry->next = new_list.first;
    new_list.last->next = previous_next;

    return new_list.last->next;
}

PRIME_ENTRY *get_next_entry(PRIME_ENTRY *entry)
{
    return entry->next;
}

void output_list(PRIME_LIST list)
{
    PRIME_ENTRY *entry = list.first;
    unsigned long count = 0;

    while(entry != NULL)
    {
        printf("%lu: %lu\n", ++count, entry->prime);
        entry = entry->next;
    }
}

void free_list(PRIME_LIST list)
{
    PRIME_ENTRY *entry = list.first;

    while(entry != NULL)
    {
        PRIME_ENTRY *next = entry->next;
        free(entry);
        
        entry = next;
    }
}
