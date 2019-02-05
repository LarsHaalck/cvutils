#ifndef CVUTILS_CACHE_SIMPLE_CACHE_H
#define CVUTILS_CACHE_SIMPLE_CACHE_H

#include <memory>
#include <unordered_map>

#include "../fetch/abstractFetcher.h"

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
class SimpleCache : public AbstractCache<Key, Value>
{
private:
    std::unordered_map<Key, Value> mMap;
    std::shared_ptr<fetch::AbstractFetcher<Key, Value>> mFetcher;
public:
    SimpleCache(std::shared_ptr<fetch::AbstractFetcher<Key, Value>> fetcher)
        : mMap()
        , mFetcher(std::move(fetcher))
    {
        mMap.reserve(fetcher->size());
    }

    // always cache no matter what
    Value get(const Key& key)
    {
        if (!mMap.count(key))
            mMap[key] = mFetcher->get(key);

        return mMap[key];
    }
};
} // namespace cache
} // namespace cvutils

#endif
