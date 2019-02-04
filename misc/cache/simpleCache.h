#ifndef CVUTILS_CACHE_SIMPLE_CACHE_H
#define CVUTILS_CACHE_SIMPLE_CACHE_H

#include <memory>
#include <unordered_map>

#include "fetch/abstractFetcher.h"

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
class SimpleCache : public AbstractCache<Key, Value>
{
private:
    std::unordered_map<Key, Value> mMap;
    std::unique_ptr<fetch::AbstractFetcher<Key, Value>> mReader;
public:
    SimpleCache(std::unique_ptr<fetch::AbstractFetcher<Key, Value>> reader)
        : mReader(std::move(reader))
    {
    }

    void setCapacityHint(size_t capacity) { mMap.reserve(capacity); }
    // always cache no matter what
    Value get(const Key& key)
    {
        if (!mMap.count(key))
            mMap[key] = mReader->get(key);
        return mMap[key];
    }
};
} // namespace cache
} // namespace cvutils

#endif
