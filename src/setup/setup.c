#include "includes/htmodloader.h"
#include "aliases.h"
#include "hsc.h"

// ----------------------------------------------------------------------------
// This file contains all signature codes of the in-game functions hooked or
// called in the plugin.
// ----------------------------------------------------------------------------

static const NamedSig sigE8_SkyCameraProp__updateParams = {
  .s = {
    .sig =
      "48 89 F1 48 89 FA E8 ?  ?  ?  ?  4C 8D 85 ?  ?  "
      "?  ?  48 89 F1 48 89 FA",
    .indirect = HT_SCAN_E8,
    .offset = 0x06
  },
  .name = "SkyCameraProp::_updateParams()",
};
static const NamedSig sigE8_SkyCameraProp_update = {
  .s = {
    .sig =
      "80 ?  ?  ?  ?  ?  01 ?  ?  48 89 ?  48 89 ?  E8 "
      "?  ?  ?  ?  E9 ?  ?  ?  ?  48 8B ",
    .indirect = HT_SCAN_E8,
    .offset = 0x0F
  },
  .name = "SkyCameraProp::update()",
};
static const NamedSig sigE8_CollisionGeoBarn_raycast = {
  .s = {
    .sig =
      "C5 ?  ?  ?  E8 ?  ?  ?  ?  ?  89 ?  84 C0 74 ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x04
  },
  .name = "CollisionGeoBarn::raycast()"
};
static const NamedSig sig_WhiskerCamera_update = {
  .s = {
    .sig =
      "48 89 CE 48 83 BA ?  ?  ?  ?  ?  0F 84 ?  ?  ?  "
      "?  48 89 D7 48 8B 8A ?  ?  ?  ?  48 8D 9C 24 ",
    .indirect = HT_SCAN_DIRECT,
    .offset = -0x6F
  },
  .name = "WhiskerCamera::update()"
};
static const NamedSig sig_SkyCamera_update = {
  .s = {
    .sig =
      "55 56 57 48 83 EC 40 48 8D 6C 24 ?  48 89 CE C5 "
      "FA 10 05 ?  ?  ?  ?  C5 F8 29 41 ?  C5 FB 10 05 "
      "?  ?  ?  ?  C5 F8 29 41 ?  48 8D",
    .indirect = HT_SCAN_DIRECT,
    .offset = 0x00
  },
  .name = "SkyCamera::update()"
};
static const NamedSig sigE8_Input_getMouseDeltaPx = {
  .s = {
    .sig =
      "48 8D 55 E0 E8 ?  ?  ?  ?  48 8B 4E 68 48 8D 55 "
      "F0 E8 ? ? ? ? ",
    .indirect = HT_SCAN_E8,
    .offset = 0x11
  },
  .name = "Input::getMouseDeltaPx()"
};

const NamedSig *const RequiredFn[REQUIRED_FUNC_COUNT] = {
  &sigE8_SkyCameraProp__updateParams,
  NULL,
  NULL,
  &sigE8_SkyCameraProp_update,
  NULL,
  &sigE8_CollisionGeoBarn_raycast,
  &sig_WhiskerCamera_update,
  &sig_SkyCamera_update,
  &sigE8_Input_getMouseDeltaPx
};

i08 hscSetupFuncWithSig(
  FuncAddresses *functions
) {
  i08 r = 1;
  i32 length;
  const NamedSig *sig;

  LOGI("Scaning functions...\n");

  if (!functions)
    return 0;

  length = sizeof(RequiredFn) / sizeof(NamedSig *);
  for (i32 i = 0; i < length; i++) {
    sig = RequiredFn[i];
    if (!sig)
      continue;

    functions->functions[i] = HTSigScan(&sig->s);

    if (functions->functions[i])
      LOGI("Found %s: 0x%p\n", sig->name, functions->functions[i]);
    else {
      LOGE("Scan %s failed!\n", sig->name);
      r = 0;
    }
  }
  
  return r;
}
