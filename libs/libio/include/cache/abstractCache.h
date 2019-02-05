#ifndef CVUTILS_CACHE_ABSTRACT_CACHE_H
#define CVUTILS_CACHE_ABSTRACT_CACHE_H

#include <cstddef>

namespace cvutils
{
namespace cache
{
template <typename Key, typename Value>
class AbstractCache
{
public:
    virtual ~AbstractCache() { }

    // not const because it might add something to the cache
    virtual Value get(const Key& key) = 0;
    

};
} // namespace cache
} // namespace cvutils


#endif //CVUTILS_CACHE_ABSTRACT_CACHE_H
