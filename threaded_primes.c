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
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

static unsigned long NUMBER_RANGE_PER_THREAD = 1000UL;
static unsigned long MAX_NUMBER_OF_THREADS = 128UL;


static inline double calc_time(struct timespec start_time);
static unsigned long calculate_principal_primes(unsigned long previous_number, unsigned long end_number);
static void *calculate_primes_in_range(void *params);
static bool is_prime(unsigned long number_to_check);


int main(void)
{
    struct timespec start_time;

    clock_gettime(CLOCK_REALTIME, &start_time);

    pthread_mutex_init(&list_mutex, NULL);

    PRIME_ENTRY *start_entry = add_prime_to_list(&principle_list, NULL, FIRST_PRIME_NUMBER);
    unsigned long last_number_checked = calculate_principal_primes(1, NUMBER_RANGE_PER_THREAD);

    while (principle_list.last->prime < 3000000UL)
    {
        unsigned long number_range_per_thread = NUMBER_RANGE_PER_THREAD;
        unsigned long thread_prime_limit = principle_list.last->prime * 2UL;
        unsigned long thread_range = thread_prime_limit - last_number_checked;
        unsigned number_threads = (thread_range / number_range_per_thread) + 1;

        if (number_threads > MAX_NUMBER_OF_THREADS)
        {
            number_threads = MAX_NUMBER_OF_THREADS;
            number_range_per_thread = thread_range / (number_threads - 1);

            //  This number needs to be even
            if (number_range_per_thread % 2 == 1)
            {
                number_range_per_thread--;
            }
        }

        pthread_t *thread_list = calloc(number_threads, sizeof(pthread_t));
        THREAD_PARAMS *params_list = calloc(number_threads, sizeof(THREAD_PARAMS));

        printf("Threads:%u  Thread limit:%lu  range:%lu  Time in secs: %lf\n",
            number_threads,
            thread_prime_limit,
            number_range_per_thread,
            calc_time(start_time));

        for (unsigned thread_count = 0; thread_count < number_threads; thread_count++)
        {
            pthread_t thread_id;
            THREAD_PARAMS *params = (params_list + thread_count);

            params->entry = principle_list.last;
            params->previous_number = last_number_checked + (number_range_per_thread * (unsigned long)thread_count);
            params->end_number = last_number_checked + (number_range_per_thread * (unsigned long)(thread_count + 1));

            if (params->end_number > thread_prime_limit)
            {
                params->end_number = thread_prime_limit;
            }

            pthread_create(&thread_id, NULL, calculate_primes_in_range, params);

            *(thread_list + thread_count) = thread_id;
        }

        for (unsigned thread_count = 0; thread_count < number_threads; thread_count++)
        {
            pthread_join(*(thread_list + thread_count), NULL);
        }

        free(thread_list);
        free(params_list);
        last_number_checked = ++thread_prime_limit;
    }

    output_list(principle_list);
    printf("Time in secs: %lf\n", calc_time(start_time));

    pthread_mutex_destroy(&list_mutex);
    free_list(principle_list);

    exit(0);
}

static inline double calc_time(struct timespec start_time)
{
    struct timespec end_time;

    clock_gettime(CLOCK_REALTIME, &end_time);

    long seconds = end_time.tv_sec - start_time.tv_sec;
    long nanoseconds = end_time.tv_nsec - start_time.tv_nsec;

    return (double)seconds + ((double)nanoseconds * 1e-9);    
}

static unsigned long calculate_principal_primes(unsigned long previous_number, unsigned long end_number)
{
    PRIME_ENTRY *new_entry = principle_list.last;
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

    while ((next_number_to_check += 2UL) < params.end_number)
    {
        if (is_prime(next_number_to_check) == true)
        {
            new_entry = add_prime_to_list(&range_list, new_entry, next_number_to_check);
        }
    }

    if (range_list.first != NULL)
    {
        PRIME_ENTRY *last_entry_next = NULL;

        pthread_mutex_lock(&list_mutex);
        last_entry_next = insert_prime_list(params.entry, range_list);
        pthread_mutex_unlock(&list_mutex);

        printf("Highest prime:%lu \n", range_list.last->prime);

        if (last_entry_next == NULL)
        {
            principle_list.last = range_list.last;
        }
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
    } while (entry != NULL && entry->prime < (number_to_check / 2UL));

    return prime_found;
}
