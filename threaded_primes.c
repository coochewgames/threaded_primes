#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "prime_list.h"

typedef struct
{
    PRIME_ENTRY  *entry;
    unsigned long  previous_number;
    unsigned long  end_number;
} THREAD_PARAMS;


volatile PRIME_LIST principle_list;
static unsigned long NUMBER_RANGE_PER_THREAD = 1000UL;

static unsigned long calculate_principal_primes(unsigned long previous_number, unsigned long end_number);
static void *calculate_primes_in_range(void *params);
static bool is_prime(unsigned long number_to_check);


int main(void)
{
    clock_t start_time;
    clock_t end_time;
    double cpu_time_used;

    start_time = clock();

    PRIME_ENTRY *start_entry = add_prime_to_list(&principle_list, NULL, FIRST_PRIME_NUMBER);
    unsigned long last_number_checked = calculate_principal_primes(1, NUMBER_RANGE_PER_THREAD);

    while (principle_list.last->prime < 1000000UL)
    {
        unsigned long thread_prime_limit = principle_list.last->prime * 2UL;
        unsigned long thread_range = thread_prime_limit - last_number_checked;
        unsigned number_threads = (thread_range / NUMBER_RANGE_PER_THREAD) + 1;

        pthread_t *thread_list = calloc(number_threads, sizeof(pthread_t));

        printf("Number of threads:%u  Thread limit:%lu\n", number_threads, thread_prime_limit);

        for (unsigned thread_count = 0; thread_count < number_threads; thread_count++)
        {
            pthread_t thread_id;
            THREAD_PARAMS params;

            params.entry = principle_list.last;
            params.previous_number = last_number_checked + (NUMBER_RANGE_PER_THREAD * (unsigned long)thread_count);
            params.end_number = last_number_checked + (NUMBER_RANGE_PER_THREAD * (unsigned long)(thread_count + 1));

            if (params.end_number > thread_prime_limit)
            {
                params.end_number = thread_prime_limit;
            }

            pthread_create(&thread_id, NULL, calculate_primes_in_range, &params);

            *(thread_list + thread_count) = thread_id;
            pthread_join(thread_id, NULL);
        }

        for (unsigned thread_count = 0; thread_count < number_threads; thread_count++)
        {
            pthread_join(*(thread_list + thread_count), NULL);
        }

        free(thread_list);
        last_number_checked = ++thread_prime_limit;
    }

    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    output_list(principle_list);
    printf("Time in secs: %lf\n", cpu_time_used);
    exit(0);
}

static unsigned long calculate_principal_primes(unsigned long previous_number, unsigned long end_number)
{
    PRIME_ENTRY *new_entry = NULL;
    unsigned long next_number_to_check = previous_number;
    unsigned long last_number_checked = 0;

    while((next_number_to_check += 2LL) < end_number)
    {
        if (is_prime(next_number_to_check) == true)
        {
            new_entry = add_prime_to_list(&principle_list, new_entry, next_number_to_check);
        }

        last_number_checked = next_number_to_check;
    }

    return last_number_checked;
}

static void *calculate_primes_in_range(void *thread_params)
{
    THREAD_PARAMS params = *(THREAD_PARAMS *)thread_params;

    unsigned long next_number_to_check = params.previous_number;
    PRIME_LIST range_list;
    PRIME_ENTRY *new_entry = NULL;
    bool is_last = (params.entry->next == NULL) ? true : false;

    while ((next_number_to_check += 2UL) < params.end_number)
    {
        if (is_prime(next_number_to_check) == true)
        {
            new_entry = add_prime_to_list(&range_list, new_entry, next_number_to_check);
        }
    }

    //  Make thread-safe
    insert_prime_list(params.entry, range_list);

    if (is_last == true)
    {
        principle_list.last = range_list.last;
    }

    pthread_exit(NULL);
}

static bool is_prime(unsigned long number_to_check)
{
    PRIME_ENTRY *entry = principle_list.first;
    bool prime_found = true;

    do
    {
        if (number_to_check % entry->prime == 0)
        {
            prime_found = false;
            break;
        }

        entry = entry->next;
    } while (entry != NULL);

    return prime_found;
}
