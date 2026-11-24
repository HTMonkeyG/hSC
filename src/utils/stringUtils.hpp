#ifndef __UTILS_STRINGUTILS_HPP__
#define __UTILS_STRINGUTILS_HPP__

#include <string.h>
#include "utils/types.hpp"

namespace StringUtils {

inline cstring StrDup(cstring s) {
  size_t l = strlen(s) + 1;
  char *result = new char[l];
  strncpy(result, s, l);
  return result;
}

}

#endif
