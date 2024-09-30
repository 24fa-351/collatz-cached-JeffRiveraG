#ifndef COLLATZ_H
#define COLLATZ_H

#include <stdbool.h>
#include <stdint.h>

struct LFU {
    int key;
    int value;
    int frequency;
};

struct LRU {
    int key;
    int value;
    int recentlyUsed;
};

// Initialize the cache with a specific size and policy (LRU or LFU)
void cache_init(int size_of_cache, const char *policy);

// Check if a number exists in the cache (returns true if found)
bool cache_has(int number, const char *policy);

// Retrieve the cached value for a number (only valid if cache_has returned true)
int cache_value_for(int number, const char *policy);

// Insert a new number and its value (number of Collatz steps) into the cache
void cache_insert(int candidate, int value, const char *policy);

// Free the memory allocated for the cache
void cache_free(void);

#endif // CACHE_H
