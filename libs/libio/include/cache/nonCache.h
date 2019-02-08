#ifndef CVUTILS_NON_CACHE_H
#define CVUTILS_NON_CACHE_H

#include <memory>

#include "fetch/abstractFetcher.h"

namespace cvutils
{
namespace detail
{
template <typename Key, typename Value>
class NonCache : public AbstractCache<Key, Value>
{
private:
    std::shared_ptr<AbstractFetcher<Key, Value>> mReader;
public:
    NonCache(std::shared_ptr<AbstractFetcher<Key, Value>> reader)
        : mReader(std::move(reader))
    {
    }

    // always fetch, no caching
    Value get(const Key& key)
    {
        return mReader->get(key);
    }
};
} // namespace detail
} // namespace cvutils

#endif //CVUTILS_NON_CACHE_H
