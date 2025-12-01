#include "includes/htmod.h"
#include "hsc.h"

static const HTAsmSig sigE8_GlobalShaderUniforms_Singleton = {
  .sig =
    "48 8D 15 ?  ?  ?  ?  48 89 C1 41 B9 ?  ?  ?  ?  "
    "E8 ?  ?  ?  ?  E8 ?  ?  ?  ?  41 B8 ?  ?  ?  ?",
  .indirect = HT_SCAN_E8,
  .offset = 0x15
};
static const HTAsmSig sigE8_GlobalShaderUniforms_m_FindUniform = {
  .sig =
    "EA 4C 89 F1 E8 ?  ?  ?  ?  48 85 C0 ",
  .indirect = HT_SCAN_E8,
  .offset = 0x04
};

static PFN_GlobalShaderUniforms_Singleton fn_GlobalShaderUniforms_Singleton = NULL;
static PFN_GlobalShaderUniforms_m_FindUniform fn_GlobalShaderUniforms_m_FindUniform = NULL;

i08 hscInitAllCalls() {
  fn_GlobalShaderUniforms_m_FindUniform = (PFN_GlobalShaderUniforms_m_FindUniform)HTSigScan(
    &sigE8_GlobalShaderUniforms_m_FindUniform);
  fn_GlobalShaderUniforms_Singleton = (PFN_GlobalShaderUniforms_Singleton)HTSigScan(
    &sigE8_GlobalShaderUniforms_Singleton);

  return 1;
}

void *hscGetSkyUniform(
  const char *name
) {
  void *gsus;

  if (!fn_GlobalShaderUniforms_Singleton || !name)
    return NULL;

  gsus = fn_GlobalShaderUniforms_Singleton();
  if (!gsus)
    return NULL;

  SkyUniform *u = fn_GlobalShaderUniforms_m_FindUniform(
    gsus,
    name);
  if (
    !u
    || u->type == 3
    || !u->pValue
  )
    return NULL;

  return u->pValue;
}
