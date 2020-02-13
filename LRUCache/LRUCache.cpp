// LRUCache.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CLRUCache.h"

#define CACHE_SIZE 10

int main()
{
    
    int8_t i;
    int retVal;
    uint32_t CacheItem;
    uint8_t CacheKey;
    uint32_t EjectedCacheItem = 0;
    CLRUCache cache(CACHE_SIZE, sizeof(uint8_t), sizeof(uint32_t), NULL, NULL);

    for (i = 0; i < CACHE_SIZE; i++) {
        CacheItem = i + 1;
        cache.addItemToCache(&i, &CacheItem, &EjectedCacheItem);
        if (EjectedCacheItem) {
            printf("Error: False Cache Item Ejection.\n");
            return -1;
        }
        if (cache.findCachedItem(&i, &CacheItem) != 0) {
            printf("Error: Expected Key (%d) Not Found In Cache.", i);
            return -1;
        }
        if (CacheItem != (i + 1)) {
            printf("Error: Incorrect value retrieved from cache (%d, %d).", i, CacheItem);
            return -1;
        }
    }
    //The cache should be full at this point with the key/value pair (0,1) being the LRU.
    if (cache.getActiveEntries() != CACHE_SIZE) {
        printf("Error: Cache should be full but is not.\n");
        return -1;
    }

    CacheKey = 11;
    CacheItem = 100;
    cache.addItemToCache(&CacheKey, &CacheItem, &EjectedCacheItem);
    if (EjectedCacheItem != 1) {
        printf("Error: Incorrect Item Ejected from Cache");
        return -1;
    }
    //remove the item we just added
    cache.removeItemFromCache(&CacheKey, &EjectedCacheItem);

    if (EjectedCacheItem != 100) {
        printf("Error: Remove Item Ejected Incorrect Value");
        return -1;
    }
    //add the key/value pair (0,1) back into the cache
    CacheKey = 0;
    CacheItem = 1;
    cache.addItemToCache(&CacheKey, &CacheItem, &EjectedCacheItem);

    for (i = 0; i < CACHE_SIZE; i++) {
        CacheKey = i;
        cache.removeItemFromCache(&CacheKey, &EjectedCacheItem);
        if (EjectedCacheItem != (i + 1)) {
            printf("Error: Remove Item Ejected Incorrect Value\n");
            return -1;
        }
        if (cache.findCachedItem(&i, &CacheItem) == 0) {
            printf("Error: Unexpected Key (%d) Found In Cache.", i);
            return -1;
        }
    }
    //cache should be empty at this point so let's check it
    if (cache.getActiveEntries() != 0) {
        printf("Error: Cache should be empty but is not.\n");
        return -1;
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
