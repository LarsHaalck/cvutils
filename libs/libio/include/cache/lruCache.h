#ifndef CVUTILS_CACHE_LRU_CACHE_H
#define CVUTILS_CACHE_LRU_CACHE_H

#include "abstractCache.h"
#include "../fetch/abstractFetcher.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
class LRUCache : public AbstractCache<Key, Value>
{
private:
    using listNode = std::pair<Key, Value>;
    size_t mCapacity;
    std::list<listNode> mList;
    std::unordered_map<Key, typename std::list<listNode>::iterator> mMap;
    std::shared_ptr<fetch::AbstractFetcher<Key, Value>> mReader;
public:
    LRUCache(size_t capacity, std::shared_ptr<fetch::AbstractFetcher<Key, Value>> reader)
        : mCapacity(capacity)
        , mList()
        , mMap(capacity)
        , mReader(std::move(reader))
    {

    }

    Value get(const Key& key)
    {
        // not in cache
        if (!mMap.count(key))
        {
            // cache is full
            if (mMap.size() == mCapacity)
            {
                auto node = mList.back();
                mList.pop_back();
                mMap.erase(node.first);
            }

            // let reader get a new value
            mList.push_front(std::make_pair(key, mReader->get(key)));
        }
        else
        {
            // element is in cache, retrieve and move to new position to
            // avoid re-reading it from file
            auto pairIt = mMap[key];
            auto keyValuePair = std::move(*pairIt);
            mList.erase(pairIt);
            mList.push_front(keyValuePair);
        }

        // update iterator
        mMap[key] = mList.begin();
        return mList.front().second;
    }

};
} // namespace cache
} // namespace cvutils

#endif // CVUTILS_CACHE_LRU_CACHE_H
