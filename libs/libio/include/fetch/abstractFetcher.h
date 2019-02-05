#ifndef CVUTILS_FETCH_ABSTRACT_FETCHER_H
#define CVUTILS_FETCH_ABSTRACT_FETCHER_H

namespace cvutils
{
namespace fetch
{
template <typename Key, typename Value>
class AbstractFetcher
{
public:
    virtual ~AbstractFetcher() {}
    virtual size_t size() const = 0;
    virtual Value get(const Key& key) const = 0;
};
} // namespace fetch
} // namespace cvutils

#endif //CVUTILS_FETCH_ABSTRACT_FETCHER_H

