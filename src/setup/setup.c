#include "includes/htmodloader.h"
#include "aliases.h"
#include "hsc.h"

// ----------------------------------------------------------------------------
// This file contains all signature codes of the in-game functions hooked or
// called in the plugin.
// ----------------------------------------------------------------------------

static const NamedSig sigE8_SkyCameraProp_updateUI = {
  .s = {
    .sig =
      "4C 8D 84 24 ?  ?  ?  ?  4C 8D 8C 24 ?  ?  ?  ?  "
      "48 89 F1 48 89 FA E8 ?  ?  ?  ?  C5 FA 10 86 ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x16
  },
  .name = "SkyCameraProp::updateUI()"
};
static const NamedSig sigE8_SkyCameraProp__updateParams = {
  .s = {
    .sig =
      "C6 44 24 ?  ?  48 89 F1 48 89 FA E8 ?  ?  ?  ?  "
      "4C 8D 44 24 ?  48 89 F1 48 89 FA E8 ?  ?  ?  ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x0B
  },
  .name = "SkyCameraProp::_updateParams()",
};
static const NamedSig sigE8_SkyCameraProp_update = {
  .s = {
    .sig =
      "80 ?  ?  ?  ?  ?  ?  74 ?  48 89 ?  48 89 ?  E8 "
      "?  ?  ?  ?  E9 ?  ?  ?  ?  48 8B ?  ?  ?  ?  ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x0F
  },
  .name = "SkyCameraProp::update()",
};
static const NamedSig sigE8_Player_getCameraPos = {
  .s = {
    .sig =
      "48 8B ?  ?  48 8D ?  ?  ?  ?  ?  ?  E8 ?  ?  ?  "
      "?  C5 F8 28 ?  ?  ?  ?  ?  ?  C5 F8 29 ?  ?  ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x0C
  },
  .name = "Player::getCameraPos()"
};
static const NamedSig sigE8_CollisionGeoBarn_raycast = {
  .s = {
    .sig =
      "C5 F8 ?  ?  E8 ?  ?  ?  ?  ?  89 ?  84 C0 74 ?  ",
    .indirect = HT_SCAN_E8,
    .offset = 0x04
  },
  .name = "CollisionGeoBarn::Raycast()"
};
static const NamedSig sig_WhiskerCamera_update = {
  .s = {
    .sig =
      "49 89 ?  48 83 ?  ?  ?  ?  ?  ?  0F 84 ?  ?  ?  "
      "?  ?  89 ?  48 8B ?  ?  ?  ?  ?  48 8D ?  ?  48 "
      "89 ?  45 31 ?  45 31",
    .indirect = HT_SCAN_DIRECT,
    .offset = -0x6B
  },
  .name = "WhiskerCamera::update()"
};
static const NamedSig sig_SkyCamera_update = {
  .s = {
    .sig =
      "55 56 57 48 83 EC ?  48 8D 6C 24 ?  ?  89 ?  C5 "
      "FA 10 05 ?  ?  ?  ?  C5 F8 29 41 ?  C5 F8 28 05 "
      "?  ?  ?  ?  C5 F8 29 41 ?  48 8D ?  ?  C5 F8 28 ",
    .indirect = HT_SCAN_DIRECT,
    .offset = 0x00
  },
  .name = "SkyCamera::update()"
};
static const NamedSig sigE8_MainCamera__getDelta = {
  .s = {
    .sig =
      "48 8D ?  ?  ?  ?  ?  48 89 ?  4D 89 ?  4C 8B ?  "
      "?  ?  ?  ?  E8 ?  ?  ?  ?  80 BD",
    .indirect = HT_SCAN_E8,
    .offset = 0x14
  },
  .name = "MainCamera::_getDelta()"
};
static const NamedSig sigE8_Input_getMouseDeltaPx = {
  .s = {
    .sig =
      "48 8D ?  ?  4C 89 F1 48 89 F2 E8 ?  ?  ?  ?  48 "
      "89 74 24",
    .indirect = HT_SCAN_E8,
    .offset = 0x0A
  },
  .name = "Input::getMouseDeltaPx()"
};

const NamedSig *const RequiredFn[REQUIRED_FUNC_COUNT] = {
  &sigE8_SkyCameraProp__updateParams,
  &sigE8_SkyCameraProp_updateUI,
  NULL,
  &sigE8_SkyCameraProp_update,
  &sigE8_Player_getCameraPos,
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
