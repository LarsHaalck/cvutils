#ifndef CVUTILS_CACHE_NON_CACHE_H
#define CVUTILS_CACHE_NON_CACHE_H

#include <memory>

#include "../fetch/abstractFetcher.h"

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
class NonCache : public AbstractCache<Key, Value>
{
private:
    std::shared_ptr<fetch::AbstractFetcher<Key, Value>> mReader;
public:
    NonCache(std::shared_ptr<fetch::AbstractFetcher<Key, Value>> reader)
        : mReader(std::move(reader))
    {
    }

    // always fetch, no caching
    Value get(const Key& key)
    {
        return mReader->get(key);
    }
};
} // namespace cache
} // namespace cvutils

#endif //CVUTILS_CACHE_NON_CACHE_H
