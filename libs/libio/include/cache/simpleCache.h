#ifndef CVUTILS_SIMPLE_CACHE_H
#define CVUTILS_SIMPLE_CACHE_H

#include <memory>
#include <unordered_map>

#include "fetch/abstractFetcher.h"

namespace cvutils
{
namespace detail
{
template <typename Key, typename Value>
class SimpleCache : public AbstractCache<Key, Value>
{
private:
    std::unordered_map<Key, Value> mMap;
    std::shared_ptr<AbstractFetcher<Key, Value>> mFetcher;
public:
    SimpleCache(std::shared_ptr<AbstractFetcher<Key, Value>> fetcher)
        : mMap()
        , mFetcher(std::move(fetcher))
    {
        mMap.reserve(fetcher->size());
    }

    // always cache no matter what
    Value get(const Key& key)
    {
        #pragma omp critical
        {
            if (!mMap.count(key))
                mMap[key] = mFetcher->get(key);
        }

        return mMap[key];
    }
};
} // namespace detail
} // namespace cvutils

#endif //CVUTILS_SIMPLE_CACHE_H
