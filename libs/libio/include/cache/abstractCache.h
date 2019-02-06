#ifndef CVUTILS_ABSTRACT_CACHE_H
#define CVUTILS_ABSTRACT_CACHE_H

namespace cvutils
{
namespace detail
{
template <typename Key, typename Value>
class AbstractCache
{
public:
    virtual ~AbstractCache() { }

    // not const because it might add something to the cache
    virtual Value get(const Key& key) = 0;
    

};
} // namespace detail
} // namespace cvutils


#endif //CVUTILS_ABSTRACT_CACHE_H
