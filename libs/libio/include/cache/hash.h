#ifndef CVUTILS_HASH_H
#define CVUTILS_HASH_H

#include <utility>

namespace std
{
  template <> struct hash<std::pair<size_t, size_t>>
  {
    size_t operator()(const std::pair<size_t, size_t>& ids) const
    {
        return hash<size_t>()(ids.first) ^ hash<size_t>()(ids.second);
    }
  };
}
#endif // CVUTILS_HASH_H
