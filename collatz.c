#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <time.h>
#include "collatz.h"

struct LRU *lru_cache = NULL;
struct LFU *lfu_cache = NULL;
int current_cache_size = 0;

void cache_init(int size_of_cache, const char *policy) {
    current_cache_size = size_of_cache;
    lfu_cache = (struct LFU *)malloc(current_cache_size * sizeof(struct LFU));
    lru_cache = (struct LRU *)malloc(current_cache_size * sizeof(struct LRU));

    if (lfu_cache == NULL || lru_cache == NULL) {
        printf("Memory allocation failed! :(\n");
        exit(1);  
    }

    printf("Initialized cache with size: %d for policy: %s\n", current_cache_size, policy);

    if (strcmp(policy, "LFU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            lfu_cache[i].key = -1;
            lfu_cache[i].value = 0;
            lfu_cache[i].frequency = 0;
        }
    } else if (strcmp(policy, "LRU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            lru_cache[i].key = -1;
            lru_cache[i].value = 0;
            lru_cache[i].recentlyUsed = 0;
        }
    } else {
        printf("Running without cache\n");
    }
}

bool cache_has(int number, const char *policy) {
    if (strcmp(policy, "LFU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lfu_cache[i].key == number) {
                lfu_cache[i].frequency++;
                return true;
            }
        }
    } else if (strcmp(policy, "LRU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lru_cache[i].key == number) {
                lru_cache[i].recentlyUsed = 0;
                for (int j = 0; j < current_cache_size; j++) {
                    if (j != i) {
                        lru_cache[j].recentlyUsed++;
                    }
                }
                return true;
            }
        }
    }
    
    return false;
}

int cache_value_for(int number, const char *policy) {
    if (strcmp(policy, "LFU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lfu_cache[i].key == number) {
                return lfu_cache[i].value;
            }
        }
    } else if (strcmp(policy, "LRU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lru_cache[i].key == number) {
                return lru_cache[i].value;
            }
        }
    }
    return 0;
}

void cache_insert(int candidate, int value, const char *policy) {
    if (strcmp(policy, "LFU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lfu_cache[i].key == -1) {  
                lfu_cache[i].key = candidate;
                lfu_cache[i].value = value;
                lfu_cache[i].frequency = 1;
                return;  
            }
        }

        int leastFrequentIndex = 0;
        for (int i = 1; i < current_cache_size; i++) {
            if (lfu_cache[i].frequency < lfu_cache[leastFrequentIndex].frequency) {
                leastFrequentIndex = i;  
            }
        }

        lfu_cache[leastFrequentIndex].key = candidate;
        lfu_cache[leastFrequentIndex].value = value;
        lfu_cache[leastFrequentIndex].frequency = 1;  
    } 
    else if (strcmp(policy, "LRU") == 0) {
        for (int i = 0; i < current_cache_size; i++) {
            if (lru_cache[i].key == -1) {  
                lru_cache[i].key = candidate;
                lru_cache[i].value = value;
                lru_cache[i].recentlyUsed = 0; 
                return; 
            }
        }

        int oldestIndex = 0;
        for (int i = 1; i < current_cache_size; i++) {
            if (lru_cache[i].recentlyUsed > lru_cache[oldestIndex].recentlyUsed) {
                oldestIndex = i;  
            }
        }

        lru_cache[oldestIndex].key = candidate;
        lru_cache[oldestIndex].value = value;
        lru_cache[oldestIndex].recentlyUsed = 0;  
    }
}

void cache_free(void) {
    free(lfu_cache);
    free(lru_cache);
    lfu_cache = NULL;
    lru_cache = NULL;
}

int collatz_i(int numbersToTest, int MIN, int MAX, const char *cacheType) {
    if (MIN >= MAX) {
        printf("MIN number is greater than or equal to MAX: (MIN = %d)\n", MIN);
        return 1;
    }

    FILE *fptr = fopen(".csv", "w");
    if (fptr == NULL) {
        printf("Error opening file pointer!\n");
        return 1;
    }

    fprintf(fptr, "Random Number,Steps,Numbers to Test,MIN,MAX\n");
    printf("Numbers to test: %d\n\n", numbersToTest);

    int cacheHits = 0;
    int cacheMisses = 0;

    for (int i = 0; i < numbersToTest; i++) {
        int randomNumber = rand() % (MAX - MIN + 1) + MIN;
        int originalNumber = randomNumber;
        int steps = 0;

        printf("Number chosen: %d\n", randomNumber);

        if (cache_has(randomNumber, cacheType)) {
            steps = cache_value_for(randomNumber, cacheType);  
            printf("%d steps (cached) to reach 1\n\n", steps);
            cacheHits++;
        } else {
            cacheMisses++;
            while (randomNumber != 1) {
                steps += 1;

                if (randomNumber % 2 == 0) {
                    randomNumber /= 2;
                } else {
                    randomNumber = 3 * randomNumber + 1;
                }
            }

            printf("%d steps to reach 1\n\n", steps);
            cache_insert(originalNumber, steps, cacheType);
        }

        fprintf(fptr, "%d,%d,%d,%d,%d\n", originalNumber, steps, numbersToTest, MIN, MAX);
    }

    double hitPercentage = (double)cacheHits / (cacheHits + cacheMisses) * 100.0;
    printf("Cache Hit Percentage: %.2f%%\n", hitPercentage);

    fclose(fptr);
    return 0;
}

int main(int argc, char *argv[]) { 
    int numbersToTest = 0;  
    int MIN = 0;
    int MAX = 0;
    int cacheSize = 0;       
    srand(time(NULL));

    sscanf(argv[1], "%d", &numbersToTest);
    sscanf(argv[2], "%d", &MIN);
    sscanf(argv[3], "%d", &MAX);
    char *cacheType = argv[4];
    sscanf(argv[5], "%d", &cacheSize);

    cache_init(cacheSize, cacheType);
    collatz_i(numbersToTest, MIN, MAX, cacheType);
    cache_free();
    
    return 0;
}
