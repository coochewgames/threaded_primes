# threaded_primes

## Compile

Uses a bin directory for simplicity in adding files to git.

### Clang
cc -g threaded_primes.c prime_list.c -o bin/threaded_primes -lpthread

### gcc
gcc -g threaded_primes.c prime_list.c -o bin/threaded_primes -pthread
