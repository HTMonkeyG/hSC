#ifndef __UTILS_ARCHUTILS_HPP__
#define __UTILS_ARCHUTILS_HPP__

#include "utils/types.hpp"

// Arch-neutral view over a single decoded instruction, backed by hde64 on
// x86-64 (ArchX64).
//
// The point of this layer is the immediate list. A single instruction can
// carry more than one constant on both architectures, and pattern scanning
// needs every one of them located, not just the last one decoded:
//
//   x86-64  "mov dword ptr [rax+0x18], 0x1234"  -> disp8 0x18, imm32 0x1234
//   x86-64  "enter 0x10, 0x02"                  -> imm16 0x10, imm8 0x02
namespace ArchX64 {

// x86-64 tops out at three (SIB scale + disp + imm); AArch64 reaches four
// (e.g. SYS takes op1/CRn/CRm/op2). One spare slot keeps the array from
// being exact-fit.
constexpr u32 kMaxImms = 5;

enum ImmKind: u08 {
  // Literal operand encoded in the instruction (x64 imm8/16/32/64, AArch64
  // #imm operands).
  kImmKind_Imm = 0,
  // Address displacement (x64 disp8/16/32, AArch64 load/store offsets).
  kImmKind_Disp = 1,
  // Shift amount attached to another immediate (AArch64 `lsl #12` on
  // ADD_IMM, `lsl #16` on MOVK, MSL on SIMD MOVI).
  kImmKind_Shift = 2,
  // PC-relative offset. `Insn::target` holds the resolved address.
  kImmKind_Rel = 3,
  // x86-64 SIB scale: the address stride (1, 2, 4 or 8) encoded in the top
  // two bits of the SIB byte. Reported as its decoded value, not the raw
  // 2-bit field.
  kImmKind_SibScale = 4,
};

struct Imm {
  ImmKind kind;
  // Byte offset of the field from the start of the instruction. Meaningful
  // on x86-64 only, where immediates are byte-aligned and contiguous, so a
  // byte mask can cover them. Always 0 on AArch64.
  u08 offset;
  // Field width in bytes. x86-64 only; 0 on AArch64, where an immediate is
  // a bit field (sometimes split across non-adjacent runs, as in TBZ) and
  // has no byte extent. Match those by mnemonic instead of by byte mask.
  u08 width;
  bool isSigned;
  // Sign-extended when isSigned, zero-extended otherwise.
  i64 value;
};

struct Insn {
  // 0 when decoding failed. Every other field is undefined in that case.
  u08 length;
  u08 immCount;
  Imm imms[kMaxImms];

  bool isBranch;
  bool isCall;
  bool isRet;
  // Resolved absolute destination of a PC-relative branch or call, valid
  // only when a kImmKind_Rel immediate is present.
  uintptr_t target;
};

inline cstring GetTag() {
  return "x64";
}

// Longest encoding the target admits, for sizing relocation buffers.
u32 GetMaxInsnLength();

// Decodes one instruction at `code`. `addr` is the address the instruction
// will execute from, used to resolve PC-relative targets; pass the same
// value as `code` when disassembling in place.
//
// Returns the instruction length in bytes, or 0 if `code` does not decode.
u32 Decode(const void *code, uintptr_t addr, Insn *out);

// Sums instruction lengths from `code` until at least `minLength` bytes are
// covered, so a hook can relocate whole instructions. Returns 0 if any
// instruction in the range fails to decode.
u32 AlignLength(const void *code, uintptr_t addr, u32 minLength);

} // namespace ArchX64

#endif
