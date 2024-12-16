#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "collatz.h"

// Define LRU and LFU cache structs as per your existing logic.
struct LRU *lru_cache = NULL;
struct LFU *lfu_cache = NULL;
int current_cache_size = 0;

void cache_init(int size_of_cache, const char *policy)
{
    current_cache_size = size_of_cache;
    lfu_cache = (struct LFU *)malloc(current_cache_size * sizeof(struct LFU));
    lru_cache = (struct LRU *)malloc(current_cache_size * sizeof(struct LRU));

    if (lfu_cache == NULL || lru_cache == NULL)
    {
        printf("Memory allocation failed! :(\n");
        exit(1);
    }

    printf("Initialized cache with size: %d for policy: %s\n", current_cache_size, policy);

    if (strcmp(policy, "LFU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            lfu_cache[i].key = -1;
            lfu_cache[i].value = 0;
            lfu_cache[i].frequency = 0;
        }
    }
    else if (strcmp(policy, "LRU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            lru_cache[i].key = -1;
            lru_cache[i].value = 0;
            lru_cache[i].recentlyUsed = 0;
        }
    }
    else
    {
        printf("Running without cache\n");
    }
}

bool cache_has(int number, const char *policy)
{
    if (strcmp(policy, "LFU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lfu_cache[i].key == number)
            {
                lfu_cache[i].frequency++;
                return true;
            }
        }
    }
    else if (strcmp(policy, "LRU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lru_cache[i].key == number)
            {
                lru_cache[i].recentlyUsed = 0;
                for (int j = 0; j < current_cache_size; j++)
                {
                    if (j != i)
                    {
                        lru_cache[j].recentlyUsed++;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

int cache_value_for(int number, const char *policy)
{
    if (strcmp(policy, "LFU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lfu_cache[i].key == number)
            {
                return lfu_cache[i].value;
            }
        }
    }
    else if (strcmp(policy, "LRU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lru_cache[i].key == number)
            {
                return lru_cache[i].value;
            }
        }
    }
    return 0;
}

void cache_insert(int candidate, int value, const char *policy)
{
    if (strcmp(policy, "LFU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lfu_cache[i].key == -1)
            {
                lfu_cache[i].key = candidate;
                lfu_cache[i].value = value;
                lfu_cache[i].frequency = 1;
                return;
            }
        }

        int leastFrequentIndex = 0;
        for (int i = 1; i < current_cache_size; i++)
        {
            if (lfu_cache[i].frequency < lfu_cache[leastFrequentIndex].frequency)
            {
                leastFrequentIndex = i;
            }
        }

        lfu_cache[leastFrequentIndex].key = candidate;
        lfu_cache[leastFrequentIndex].value = value;
        lfu_cache[leastFrequentIndex].frequency = 1;
    }
    else if (strcmp(policy, "LRU") == 0)
    {
        for (int i = 0; i < current_cache_size; i++)
        {
            if (lru_cache[i].key == -1)
            {
                lru_cache[i].key = candidate;
                lru_cache[i].value = value;
                lru_cache[i].recentlyUsed = 0;
                return;
            }
        }

        int oldestIndex = 0;
        for (int i = 1; i < current_cache_size; i++)
        {
            if (lru_cache[i].recentlyUsed > lru_cache[oldestIndex].recentlyUsed)
            {
                oldestIndex = i;
            }
        }

        lru_cache[oldestIndex].key = candidate;
        lru_cache[oldestIndex].value = value;
        lru_cache[oldestIndex].recentlyUsed = 0;
    }
}

void cache_free(void)
{
    free(lfu_cache);
    free(lru_cache);
    lfu_cache = NULL;
    lru_cache = NULL;
}

int collatz_steps(int number)
{
    int steps = 0;
    while (number != 1)
    {
        if (number % 2 == 0)
        {
            number /= 2;
        }
        else
        {
            number = 3 * number + 1;
        }
        steps++;
    }
    return steps;
}

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Usage: %s <N> <MIN> <MAX> <cache_policy> <cache_size>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int MIN = atoi(argv[2]);
    int MAX = atoi(argv[3]);
    char *cache_policy = argv[4];
    int cache_size = atoi(argv[5]);

    if (MIN >= MAX)
    {
        printf("MIN should be less than MAX\n");
        return 1;
    }

    srand(time(NULL));
    FILE *fptr = fopen(".csv", "w");
    if (fptr == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    // Write header row
    fprintf(fptr, "Random Number, Steps, Numbers to Test, MIN, MAX\n");

    cache_init(cache_size, cache_policy);

    int cache_hits = 0;
    int cache_misses = 0;

    for (int i = 0; i < N; i++)
    {
        int randomNumber = rand() % (MAX - MIN + 1) + MIN;
        int steps = 0;

        if (cache_has(randomNumber, cache_policy))
        {
            steps = cache_value_for(randomNumber, cache_policy);
            cache_hits++;
        }
        else
        {
            steps = collatz_steps(randomNumber);
            cache_insert(randomNumber, steps, cache_policy);
            cache_misses++;
        }

        fprintf(fptr, "%d,%d,%d,%d,%d\n", randomNumber, steps, N, MIN, MAX);
    }

    double hit_percentage = (double)cache_hits / (cache_hits + cache_misses) * 100.0;
    printf("Cache Hit Percentage: %.2f%%\n", hit_percentage);

    fclose(fptr);
    cache_free();
    return 0;
}
