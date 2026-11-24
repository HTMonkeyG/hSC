#ifndef __UTILS_TYPES_HPP__
#define __UTILS_TYPES_HPP__

#include <stddef.h>
#include <stdint.h>
#include <string>

typedef int8_t i08;
typedef uint8_t u08;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef void nil;
typedef void *any;

typedef const char *cstring;
typedef std::string TgcString;
typedef std::wstring TgcWString;

template<typename T, usize N>
constexpr usize countof(const T (&)[N]) noexcept { return N; }

#endif
