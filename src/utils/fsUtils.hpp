#ifndef __UTILS_FSUTILS_HPP__
#define __UTILS_FSUTILS_HPP__

#include "utils/types.hpp"

namespace FsUtils {

TgcString ReadFileRaw(const TgcWString &path);
TgcString ReadFileUtf8(const TgcWString &path);
bool WriteFileUtf8(const TgcWString &path, const TgcString &data);

bool EnsureDirectory(const TgcWString &path);

}

#endif
