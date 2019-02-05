#ifndef CVUTILS_CACHE_CACHE_FACTORY_H
#define CVUTILS_CACHE_CACHE_FACTORY_H

#include <limits>

#include "lruCache.h"
#include "simpleCache.h"
#include "nonCache.h"

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
std::unique_ptr<AbstractCache<Key, Value>> createCachePtr(
    std::shared_ptr<cvutils::fetch::AbstractFetcher<Key, Value>> fetcher,
    size_t cacheSize)
{

    if (cacheSize == std::numeric_limits<std::size_t>::max())
    {
        // infinite cache
        return std::make_unique<SimpleCache<Key, Value>>(fetcher);
    }
    else if (cacheSize) 
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
} // namespace cache
} // namespace cvutils
#endif // CVUTILS_CACHE_CACHE_FACTORY_H
