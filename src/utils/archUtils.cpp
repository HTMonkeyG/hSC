#include <string.h>
#include <hde64.h>
#include "utils/archUtils.hpp"

// Shorthands for the nested types. Members of ArchX64 pick these up from
// class scope; the helpers below are outside it.
using Insn = ArchX64::Insn;
using Imm = ArchX64::Imm;
using ImmKind = ArchX64::ImmKind;

static u64 s_ReadLE(
  const u08 *p,
  u32 width
) {
  u64 v = 0;
  for (u32 i = 0; i < width; ++i) {
    v |= (u64)p[i] << (i * 8);
  }
  return v;
}

static i64 s_SignExtend(
  u64 v,
  u32 width
) {
  const u32 bits = width * 8;
  if (bits >= 64) {
    return (i64)v;
  }
  const u64 sign = (u64)1 << (bits - 1);
  return (i64)((v ^ sign) - sign);
}

static void s_PushImm(
  Insn *out,
  ImmKind kind,
  u32 offset,
  u32 width,
  bool isSigned,
  const u08 *code
) {
  if (out->immCount >= ArchX64::kMaxImms) {
    return;
  }

  const u64 raw = s_ReadLE(code + offset, width);

  Imm &imm = out->imms[out->immCount++];
  imm.kind = kind;
  imm.offset = (u08)offset;
  imm.width = (u08)width;
  imm.isSigned = isSigned;
  imm.value = isSigned ? s_SignExtend(raw, width) : (i64)raw;
}

static u32 s_DispWidth(
  const hde64s &hs
) {
  if (hs.flags & F_DISP8) return 1;
  if (hs.flags & F_DISP16) return 2;
  if (hs.flags & F_DISP32) return 4;
  return 0;
}

// Total bytes of immediate operands trailing the displacement. F_IMM16 and
// F_IMM8 can both be set at once (ENTER imm16, imm8), which is exactly the
// case where hde64's `imm` union loses one of the two values.
static u32 s_ImmBytes(
  const hde64s &hs
) {
  u32 n = 0;
  if (hs.flags & F_IMM64) n += 8;
  if (hs.flags & F_IMM32) n += 4;
  if (hs.flags & F_IMM16) n += 2;
  if (hs.flags & F_IMM8) n += 1;
  return n;
}

static void s_ClassifyFlow(
  const hde64s &hs,
  Insn *out
) {
  if (hs.opcode2) {
    // 0F 80..8F: Jcc rel32.
    out->isBranch = (hs.opcode2 & 0xf0) == 0x80;
    return;
  }

  switch (hs.opcode) {
    case 0xe8:
      out->isCall = true;
      out->isBranch = true;
      break;
    case 0xe9:
    case 0xeb:
    case 0xe3:
      out->isBranch = true;
      break;
    case 0xc2:
    case 0xc3:
    case 0xca:
    case 0xcb:
      out->isRet = true;
      break;
    case 0xff:
      // /2 and /3 are CALL, /4 and /5 are JMP.
      if (hs.modrm_reg == 2 || hs.modrm_reg == 3) {
        out->isCall = true;
        out->isBranch = true;
      } else if (hs.modrm_reg == 4 || hs.modrm_reg == 5) {
        out->isBranch = true;
      }
      break;
    default:
      // 70..7F: Jcc rel8. E0..E2: LOOPcc.
      if ((hs.opcode & 0xf0) == 0x70 || (hs.opcode & 0xfc) == 0xe0) {
        out->isBranch = true;
      }
      break;
  }
}

namespace ArchX64 {

u32 GetMaxInsnLength() {
  return 15;
}

u32 AlignLength(
  const void *code,
  uintptr_t addr,
  u32 minLength
) {
  const u08 *p = (const u08 *)code;
  u32 total = 0;

  while (total < minLength) {
    Insn insn;
    const u32 len = Decode(p + total, addr + total, &insn);
    if (len == 0) {
      return 0;
    }
    total += len;
  }

  return total;
}

u32 Decode(
  const void *code,
  uintptr_t addr,
  Insn *out
) {
  memset(out, 0, sizeof(*out));

  hde64s hs;
  const u32 len = hde64_disasm(code, &hs);
  if (len == 0 || (hs.flags & F_ERROR)) {
    return 0;
  }

  const u08 *bytes = (const u08 *)code;
  out->length = (u08)len;

  const u32 dispWidth = s_DispWidth(hs);
  const u32 immBytes = s_ImmBytes(hs);

  // Everything before the displacement is prefixes, opcode, ModRM and SIB,
  // whose combined size hde64 does not report directly. The displacement and
  // immediates are the instruction tail, so their offsets fall out of the
  // total length.
  u32 offset = len - immBytes - dispWidth;

  // The SIB scale is a 2-bit field at the top of the SIB byte, which sits
  // immediately before the displacement. It encodes the address stride
  // (1, 2, 4 or 8); report the decoded stride rather than the raw field. It
  // has no byte extent of its own, so `width` stays 0 like the AArch64
  // bit-field immediates. Emitted first to follow the byte order: SIB before
  // displacement before immediates.
  if (hs.flags & F_SIB) {
    if (out->immCount < ArchX64::kMaxImms) {
      Imm &imm = out->imms[out->immCount++];
      imm.kind = kImmKind_SibScale;
      imm.offset = (u08)(offset - 1);
      imm.width = 0;
      imm.isSigned = false;
      imm.value = (i64)1 << hs.sib_scale;
    }
  }

  // ModRM mod=00 rm=101 is RIP-relative on x86-64. hde64 does not set
  // F_RELATIVE for it -- that flag only covers rel8/rel32 immediates -- yet
  // this is the form behind "lea rax, [rip+X]" and "call [rip+X]", so it has
  // to be reported as relative too.
  const bool isRipRel =
    (hs.flags & F_MODRM) && hs.modrm_mod == 0 && hs.modrm_rm == 5;

  if (dispWidth != 0) {
    s_PushImm(
      out,
      isRipRel ? kImmKind_Rel : kImmKind_Disp,
      offset,
      dispWidth,
      true,
      bytes);
    offset += dispWidth;
  }

  // A relative operand is always the sole immediate: hde64 reaches rel8/rel32
  // through paths that emit no other immediate.
  const bool isRel = (hs.flags & F_RELATIVE) != 0;
  const ImmKind immKind = isRel ? kImmKind_Rel : kImmKind_Imm;

  // Emitted widest-first, matching the order hde64 consumes them.
  static const struct { u32 flag; u32 width; } kImmFields[] = {
    { F_IMM64, 8 },
    { F_IMM32, 4 },
    { F_IMM16, 2 },
    { F_IMM8,  1 },
  };

  for (const auto &field: kImmFields) {
    if (!(hs.flags & field.flag)) {
      continue;
    }
    s_PushImm(out, immKind, offset, field.width, isRel, bytes);
    offset += field.width;
  }

  s_ClassifyFlow(hs, out);

  // Both forms are relative to the end of the instruction. There is at most
  // one relative field per instruction, but it is not necessarily the last
  // immediate: "mov dword ptr [rip+X], 0x1234" puts the RIP displacement
  // ahead of the imm32.
  for (u32 i = 0; i < out->immCount; ++i) {
    if (out->imms[i].kind == kImmKind_Rel) {
      out->target = addr + len + (uintptr_t)out->imms[i].value;
      break;
    }
  }

  return len;
}

} // namespace ArchX64
