#ifndef CVUTILS_SIMPLE_CACHE_H
#define CVUTILS_SIMPLE_CACHE_H

#include <memory>
#include <unordered_map>
#include <thread>

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

    std::mutex mMutex;
public:
    SimpleCache(std::shared_ptr<AbstractFetcher<Key, Value>> fetcher)
        : mMap()
        , mFetcher(std::move(fetcher))
    {
        mMap.reserve(mFetcher->size());
    }

    // always cache no matter what
    Value get(const Key& key)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (!mMap.count(key))
            mMap[key] = mFetcher->get(key);

        return mMap[key];
    }
};
} // namespace detail
} // namespace cvutils

#endif //CVUTILS_SIMPLE_CACHE_H
