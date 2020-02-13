#pragma once
#include <cstdint>

class CLRUCache
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    //! \brief Constructor for the LRU software cache. 
    //!
    //! \param[in] u16CacheSize Number of lines in the cache.
    //! \param[in] u8KeySize    The size, in bytes, of a key for the cache.
    //! \param[in] u8ItemSize   The size, in bytes, of an item in the cache.
    //! \param[in] pKeybuffer   Pointer to a buffer of memory to store the keys of the cache.
    //! \param[in] pItemBuffer  Pointer to a buffer in memory to store the cache items in.
    //!
    //! \note pKeyBuffer and pItemBuffer may be NULL if you would like for the class to manage
    //!       these buffers for you on the heap.  If you wish to provide these buffers, it is
    //!       required that they both be one key/item larger in size than u16CacheSize.  This
    //!       extra space is used as a temporary buffer used for retention of an ejected item.
    ////////////////////////////////////////////////////////////////////////////////
    CLRUCache(uint16_t u16CacheSize, uint8_t u8KeySize, uint8_t u8ItemSize, void* pKeybuffer, void* pItemBuffer);

    CLRUCache(const CLRUCache& other);

    ////////////////////////////////////////////////////////////////////////////////
    //! \brief Destructor for the LRU software cache.
    //!
    //! \note If pKeyBuffer or pItemBuffer were passed to the constructor as NULL then
    //!       these buffers are managed by this class on the heap. They will be deallocated
    //!       in this case.  If you passed in these buffers to the constructor, you will
    //!       need to handle managing their memory after the class deconstructor is called.
    ////////////////////////////////////////////////////////////////////////////////
    ~CLRUCache();


    ///////////////////////////////////////////////////////////////////////////////
    //! \brief Adds an item to an LRU Software Cache
    //!
    //! This function adds an item to an LRU Software Cache.
    //!
    //! \param[in] pKey      	    Pointer to Key of Key/Value pair to add to the cache
    //! \param[in] pItem		    Pointer to Value of Key/Value pair to add to the cache
    //! \param[out] pEjectedItem    Pointer to Buffer for Ejected Item of Key/Value pair ejected from cache
    //!
    //! \note	If pEjectedItem is NULL it will not be populated upon ejection of an item
    //!
    ////////////////////////////////////////////////////////////////////////////////
    int addItemToCache(void* pKey, void* pItem, void* pEjectedItem);

    ////////////////////////////////////////////////////////////////////////////////
    //! \brief Finds an item in an LRU Cache
    //!
    //! Based on the key provided as input the function will search an LRU cache to find an item.
    //! If found the item will be marked as the youngest in the cache so it will be the most recently
    //! used and therefore least likely to be ejected upon future additions to the cache.
    //!
    //! \param[in] pKey      	Pointer to Key of Key/Value pair in the cache
    //! \param[out] pItem		Pointer to populate with Value of Key/Value pair in the cache
    //!
    ////////////////////////////////////////////////////////////////////////////////
    int findCachedItem(void* pKey, void* pItem);
    
    ////////////////////////////////////////////////////////////////////////////////
    //! \brief Removes and item from an LRU software cache
    //!
    //! This function removes an item from an LRU software cache.
    //!
    //! \param[in] pKeyToEject          Pointer to Key of Key/Value pair to eject from the cache
    //! \param[out] pEjectedItem        Pointer to Buffer for Ejected Item of Key/Value pair ejected from cache
    //!
    //! \note	If pEjectedItem is NULL, it will not be populated upon ejection of an item
    //!
    ////////////////////////////////////////////////////////////////////////////////
    int removeItemFromCache(void* pKeyToEject, void* pEjectedItem);


    const uint16_t getActiveEntries(void) const;
    const uint16_t getCacheSize(void) const;
private:
    //! \brief Size in bytes of a key
    uint8_t     u8KeySize;
    //! \brief Size in bytes of an item
    uint8_t     u8ItemSize;
    //! \brief Number of elements available in the cache
    uint16_t    u16CacheSize;
    //! \brief Current number of active entries in the cache
    uint16_t    u16CacheActiveEntries;
    //! \brief Buffer to temporarily hold a key while re-ordering the cache
    void* pTempKey;
    //! \brief Key array (each key corresponds to an item)
    void* pKeyBuffer;
    //! \brief Buffer to temporarily hold an item while re-ordering the cache
    void* pTempItem;
    //! \brief The item arrach (each item corresponds to a key in the key array)
    void* pItemBuffer;

    bool manageItemBuffer;
    bool manageKeyBuffer;


};

