#ifndef PRIME_LIST_H
#define PRIME_LIST_H

typedef struct prime_entry
{
    unsigned long  prime;
    struct prime_entry  *next;
} PRIME_ENTRY;

typedef struct
{
    PRIME_ENTRY *first;
    PRIME_ENTRY *last;
} PRIME_LIST;

static const unsigned long FIRST_PRIME_NUMBER = 2;


PRIME_ENTRY *add_prime_to_list(volatile PRIME_LIST *list, PRIME_ENTRY *previous_entry, unsigned long prime);
void insert_prime_list(PRIME_ENTRY *start_entry, PRIME_LIST new_list);

PRIME_ENTRY *get_next_entry(PRIME_ENTRY *entry);
void output_list(PRIME_LIST list);

#endif
