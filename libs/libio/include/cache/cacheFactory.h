#ifndef CVUTILS_CACHE_FACTORY_H
#define CVUTILS_CACHE_FACTORY_H

#include <limits>

#include "cache/lruCache.h"
#include "cache/simpleCache.h"
#include "cache/nonCache.h"

namespace cvutils
{
namespace detail
{
template <typename Key, typename Value>
std::unique_ptr<AbstractCache<Key, Value>> createCachePtr(
    std::shared_ptr<AbstractFetcher<Key, Value>> fetcher,
    int cacheSize)
{

    if (cacheSize < 0)
    {
        // infinite cache
        return std::make_unique<SimpleCache<Key, Value>>(fetcher);
    }
    else if (cacheSize > 0) 
    {
        // use least recently used cache
        return std::make_unique<LRUCache<Key, Value>>(cacheSize, fetcher);
    }
    else 
    {
        // use no cache (Fetch always)
        return std::make_unique<NonCache<Key, Value>>(fetcher);
    }
}
} // namespace detail
} // namespace cvutils

#endif // CVUTILS_CACHE_FACTORY_H
