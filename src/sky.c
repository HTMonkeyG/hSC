#include "includes/htmodloader.h"
#include "hsc.h"

typedef i08 (__fastcall *PFN_CollisionGeoBarn_sphereCast)(
  void *, v4f *, v4f *, float, v4f *, RaycastResult *);
typedef void *(__fastcall *PFN_GlobalShaderUniforms_singleton)(
  void);
typedef ShaderUniform *(__fastcall *PFN_GlobalShaderUniforms__findUniform)(
  void*, const char *);

static const HTAsmSig sigE8_CollisionGeoBarn_SphereCast = {
  .sig =
    "28 DE E8 ?  ?  ?  ?  41 08",
  .indirect = HT_SCAN_E8,
  .offset = 0x02
};
static const HTAsmSig sigE8_GlobalShaderUniforms_Singleton = {
  .sig =
    "48 8D 15 ?  ?  ?  ?  48 89 C1 41 B9 ?  ?  ?  ?  "
    "E8 ?  ?  ?  ?  E8 ?  ?  ?  ?  41 B8 ",
  .indirect = HT_SCAN_E8,
  .offset = 0x15
};
static const HTAsmSig sigE8_GlobalShaderUniforms_m_FindUniform = {
  .sig =
    "EA 4C 89 F1 E8 ?  ?  ?  ?  48 85 C0 ",
  .indirect = HT_SCAN_E8,
  .offset = 0x04
};

static PFN_CollisionGeoBarn_sphereCast fn_CollisionGeoBarn_sphereCast = NULL;
static PFN_GlobalShaderUniforms_singleton fn_GlobalShaderUniforms_singleton = NULL;
static PFN_GlobalShaderUniforms__findUniform fn_GlobalShaderUniforms__findUniform = NULL;

i08 CollisionGeoBarn_sphereCast(
  v4f start,
  v4f end,
  float radius,
  v4f *filter,
  RaycastResult *result
) {
  if (!fn_CollisionGeoBarn_sphereCast || !gCollisionGeoBarn)
    return 0;
  return fn_CollisionGeoBarn_sphereCast(
    gCollisionGeoBarn,
    &start,
    &end,
    radius,
    filter,
    result);
}

void *GlobalShaderUniforms__findUniform(
  const char *name
) {
  if (
    !fn_GlobalShaderUniforms__findUniform
    || !fn_GlobalShaderUniforms_singleton
  )
    return NULL;

  void *gsus = fn_GlobalShaderUniforms_singleton();
  if (!gsus)
    return NULL;

  ShaderUniform *uniform = fn_GlobalShaderUniforms__findUniform(
    gsus, "u_viewProj");
  if (
    !uniform
    || uniform->type == 3
  )
    return NULL;

  return uniform->pValue;
}

i08 hscFindSkyFunctions() {
  fn_CollisionGeoBarn_sphereCast = (PFN_CollisionGeoBarn_sphereCast)HTSigScan(
    &sigE8_CollisionGeoBarn_SphereCast);
  fn_GlobalShaderUniforms__findUniform = (PFN_GlobalShaderUniforms__findUniform)HTSigScan(
    &sigE8_GlobalShaderUniforms_m_FindUniform);
  fn_GlobalShaderUniforms_singleton = (PFN_GlobalShaderUniforms_singleton)HTSigScan(
    &sigE8_GlobalShaderUniforms_Singleton);

  return 1;
}
