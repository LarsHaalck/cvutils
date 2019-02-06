#ifndef CVUTILS_ABSTRACT_FETCHER_H
#define CVUTILS_ABSTRACT_FETCHER_H

namespace cvutils
{
namespace detail
{
template <typename Key, typename Value>
class AbstractFetcher
{
public:
    virtual ~AbstractFetcher() {}
    virtual size_t size() const = 0;
    virtual Value get(const Key& key) const = 0;
};
} // namespace detail
} // namespace cvutils

#endif //CVUTILS_ABSTRACT_FETCHER_H

