#ifndef __UTILS_ABIMISC_HPP__
#define __UTILS_ABIMISC_HPP__

#include <string.h>
#include <string_view>
#include <functional>
#include "utils/types.hpp"

// Force /vmg.
#pragma pointers_to_members(full_generality, virtual_inheritance)

// NOTE: The use of AbiMisc requires member pointers that do not use the virtual
// base class table, meaning the member pointed to is a direct member of its own
// class, not a member of a base class.

// ----------------------------------------------------------------------------
// [SECTION] Member Pointer Wrappers
// ----------------------------------------------------------------------------

class AbiMemDataPtr {
private:
  // Foward declaration to emit the unknown inheritance of MSVC.
  class Stub;

  using PMember = void *Stub::*;

public:
  // Data members need no this-adjustment field -- the base offset folds into
  // offset -- so the single and multiple models share one layout.
  //
  // Unlike the function layouts, the null data member pointer is -1: offset 0
  // is a real field, so zero cannot mean null.
  struct AbiPMember {
    i32 offset = -1;
    i32 vadj = 0;
    i32 vindex = 0;

    inline bool IsNull()         const noexcept { return offset == -1 && vindex == 0; }
    inline ptrdiff_t GetOffset() const noexcept { return offset; }
  };

  // Layout checks. These are the sizes the ABIs mandate; a mismatch means the
  // build is not using the ABI FLOS.hh detected.
  static_assert(
    sizeof(PMember) == sizeof(AbiPMember),
    "AbiMisc: Member variable pointer size mismatch.");

public:
  // The constructor accepts any member pointers, and converts it into a general
  // layout.
  template<typename Type, typename Class>
  explicit AbiMemDataPtr(Type Class::*p = nullptr) noexcept { m_s = reinterpret_cast<PMember>(p); }

  // Or accepts an offset, and converts it into a member pointer.
  explicit AbiMemDataPtr(i32 offset)          noexcept { memset(&m_d, 0, sizeof(m_d)); m_d.offset = offset; }

  inline AbiPMember *operator->()             noexcept { return &m_d; }
  inline const AbiPMember *operator->() const noexcept { return &m_d; }
  inline PMember Pointer()              const noexcept { return m_s; }

  // We only provide fields that exist in both ABIs.
  inline bool IsNull()                  const noexcept { return m_d.IsNull(); }
  inline uintptr_t GetOffset()          const noexcept { return m_d.GetOffset(); }

private:
  union {
    PMember m_s;
    AbiPMember m_d;
  };
};

class AbiMemFuncPtr {
private:
  // Foward declaration to emit the unknown inheritance of MSVC.
  class Stub;

  using PFNMember = void (Stub::*)(void);

public:
  // MSVC member function layout.
  // Formed against a forward declaration, so the layout carries everything.
  struct AbiPFNMember {
    uintptr_t ptr;
    i32 adj;
    i32 vadj;
    i32 vindex;

    inline bool IsNull()             const noexcept { return ptr == 0; }
    inline uintptr_t GetAddress()    const noexcept { return ptr; }
    inline ptrdiff_t GetThisAdjust() const noexcept { return adj; }
  };

  // Layout checks. These are the sizes the ABIs mandate; a mismatch means the
  // build is not using the ABI FLOS.hh detected.
  static_assert(
    sizeof(PFNMember) == sizeof(AbiPFNMember),
    "AbiMisc: Member function pointer size mismatch.");

public:
  // The constructor accepts any member pointers, and converts it into a general
  // layout.
  template<typename Class, typename Ret, typename ...Args>
  explicit AbiMemFuncPtr(Ret (Class::*p)(Args...) = nullptr) noexcept { m_s = reinterpret_cast<PFNMember>(p); }

  // Or accepts an address or void *, and converts it into a member pointer.
  explicit AbiMemFuncPtr(uintptr_t address)     noexcept { memset(&m_d, 0, sizeof(m_d)); m_d.ptr = address; }

  inline AbiPFNMember *operator->()             noexcept { return &m_d; }
  inline const AbiPFNMember *operator->() const noexcept { return &m_d; }
  inline PFNMember Pointer()              const noexcept { return m_s; }

  // We only provide fields that exist in both ABIs.
  inline bool IsNull()                    const noexcept { return m_d.IsNull(); }
  inline uintptr_t GetAddress()           const noexcept { return m_d.GetAddress(); }
  inline ptrdiff_t GetThisAdjust()        const noexcept { return m_d.GetThisAdjust(); }

  // Hash function.
  inline size_t Hash() const noexcept {
    return std::hash<std::string_view>{}(
      std::string_view(reinterpret_cast<const char *>(this), sizeof(AbiMemFuncPtr))
    );
  }

private:
  union {
    PFNMember m_s;
    AbiPFNMember m_d;
  };
};

namespace std {

template <>
struct hash<AbiMemFuncPtr> {
  std::size_t operator()(const AbiMemFuncPtr &s) const noexcept {
    return s.Hash();
  }
};

}

#endif
