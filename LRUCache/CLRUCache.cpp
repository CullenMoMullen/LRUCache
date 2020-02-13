#include "CLRUCache.h"
#include <malloc.h>
#include <cassert>
#include <stdlib.h>
#include <xmemory>

CLRUCache::CLRUCache(uint16_t u16CacheSize, uint8_t u8KeySize, uint8_t u8ItemSize, void* pKeybuffer = NULL,  void* pItemBuffer = NULL)
{
    this->u16CacheSize = u16CacheSize;
    this->manageItemBuffer = false;
    this->manageKeyBuffer = false;

    this->u8KeySize = u8KeySize;
    if (pKeyBuffer == NULL) {
        this->pKeyBuffer = malloc(u8KeySize * (u16CacheSize + 1));
        assert(this->pKeyBuffer != NULL);
        this->manageKeyBuffer = true;
    }
    else {
        this->pKeyBuffer = pKeybuffer;
    }

    this->u8ItemSize = u8ItemSize;
    if (pItemBuffer == NULL) {
        this->pItemBuffer = malloc(u8ItemSize * (u16CacheSize + 1));
        assert(this->pItemBuffer != NULL);
        this->manageItemBuffer = true;
    }
    else {
        this->pItemBuffer = pItemBuffer;
    }

    this->u16CacheActiveEntries = 0;
    
    //point the temp key and item to last position in allocated memory, for this reason
    //if you self manage these two buffers, they must be one item larger than the keySize or
    //itemSize respectively.
    this->pTempItem = (void*)((char*)this->pItemBuffer + (u16CacheSize * u8ItemSize));
    this->pTempKey = (void*)((char*)this->pKeyBuffer + (u16CacheSize * u8KeySize));
}

CLRUCache::CLRUCache(const CLRUCache& other)
    : CLRUCache(other.u16CacheSize, other.u8KeySize, other.u8ItemSize, NULL, NULL)
{}

CLRUCache::~CLRUCache() {
    if (this->manageItemBuffer) {
        free(this->pItemBuffer);
    }
    if (this->manageKeyBuffer) {
        free(this->pKeyBuffer);
    }
}

int CLRUCache::addItemToCache(void* pKey, void* pItem, void* pEjectedItem)
{
    if (findCachedItem(pKey, NULL) == 0) {
        if (pEjectedItem) {
            //copy the old most recently used item to pEjectedItem
            memcpy(pEjectedItem, this->pItemBuffer, u8ItemSize);
        }
        //overwrite most recently used item which was set during findCachedItem()
        memcpy(this->pItemBuffer, pItem, u8ItemSize);
        return 0;
    }
    //Item is not currently in the cache
    if (this->u16CacheActiveEntries) {
        //we currently have active entries, so we need to move all entries down by one
        //we also know the key doesn't already exist
        memmove((char*)this->pKeyBuffer + u8KeySize, this->pKeyBuffer, this->u8KeySize * this->u16CacheActiveEntries);
        memmove((char*)this->pItemBuffer + u8ItemSize, this->pItemBuffer, this->u8ItemSize * this->u16CacheActiveEntries);
    }
    
    //write new key and item into top position
    memcpy(this->pKeyBuffer, pKey, this->u8KeySize);
    memcpy(this->pItemBuffer, pItem, this->u8ItemSize);
    
    //Was cache already full?
    if (this->u16CacheActiveEntries == this->u16CacheSize) {
        //this requires that we allocated enough space for cache size + 1, the LRU item will have
        //been moved into the pTempItem location during the memmove above, if ActiveEnties equals size
        if (pEjectedItem) {
            memcpy(pEjectedItem, (char*)this->pTempItem, this->u8ItemSize);
        }
    }
    else if(this->u16CacheActiveEntries < this->u16CacheSize) {
        //cache is not full so increase active entries
        this->u16CacheActiveEntries++;
    }
    return 0;
}

int CLRUCache::findCachedItem(void* pKey, void* pItem)
{
    char* currentKeyAddress = (char*)pKeyBuffer;
    char* currentItemAddress = (char*)pItemBuffer;

    for (int i = 0; i < this->u16CacheActiveEntries; i++) {
        if (memcmp(currentKeyAddress, pKey, u8KeySize) == 0) {
            if (pItem) {
                memcpy(pItem, currentItemAddress, u8ItemSize);
            }
            //move entry to top so it is the most recently used
            if (i) {
                memcpy(this->pTempKey, currentKeyAddress, u8KeySize);
                memcpy(this->pTempItem, currentItemAddress, u8ItemSize);

                //move all entries down to make room at top
                memmove((char*)this->pKeyBuffer + u8KeySize, this->pKeyBuffer, this->u8KeySize * i);
                memmove((char*)this->pItemBuffer + u8ItemSize, this->pItemBuffer, this->u8ItemSize * i);

                memcpy(this->pKeyBuffer, this->pTempKey, this->u8KeySize);
                memcpy(this->pItemBuffer, this->pTempItem, this->u8ItemSize);
            }
            return 0;
        }
        currentKeyAddress += u8KeySize;
        currentItemAddress += u8ItemSize;
    }
    return -1;
}

int CLRUCache::removeItemFromCache(void* pKeyToEject, void* pEjectedItem)
{
    int i;
    char* currentKeyAddress = (char*)this->pKeyBuffer;
    char* currentItemAddress = (char*)this->pItemBuffer;
    for (i = 0; i < this->u16CacheActiveEntries; i++) {
        if (memcmp(currentKeyAddress, pKeyToEject, u8KeySize) == 0) {
            if (pEjectedItem) {
                memcpy(pEjectedItem, currentItemAddress, u8ItemSize);
            }
            if (i < (this->u16CacheActiveEntries - 1)) {
                //move the rest of the items up
                memmove(currentKeyAddress, currentKeyAddress + u8KeySize, u8KeySize * (this->u16CacheActiveEntries - i - 1));
                memmove(currentItemAddress, currentItemAddress + u8ItemSize, u8ItemSize * (this->u16CacheActiveEntries - i - 1));
            }
            this->u16CacheActiveEntries--;
            return 0;
        }
        currentItemAddress += u8ItemSize;
        currentKeyAddress += u8KeySize;
    }
    return -1;
}

const uint16_t CLRUCache::getActiveEntries(void) const
{
    return this->u16CacheActiveEntries;
}

const uint16_t CLRUCache::getCacheSize(void) const
{
    return this->u16CacheSize;
}
