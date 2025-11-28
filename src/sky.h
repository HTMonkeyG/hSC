#ifndef __SKY_H__
#define __SKY_H__

#include "mth/macros.h"
#include "mth/vector.h"
#include "mth/matrix.h"
#include "mth/euler.h"
#include "mth/aabb.h"
#include "aliases.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// [SECTION] SKY_GAME_TYPES
//-----------------------------------------------------------------------------

typedef enum {
  FIRST = 1,
  FRONT = 2,
  PLACE = 3
} SkyCameraType;

// Stores the status of the camera prop.
typedef struct {
  i64 lpVtbl;
  i32 field_8;
  i08 field_C;
  i64 field_D;
  i64 field_E;
  i08 field_15;
  i32 field_16;
  i08 field_1E;
  i32 unk_1;
  i32 field_1F;
  i32 field_23;
  i08 cameraType;
  i08 unk_2[15];
  u64 player;
  i08 unk_2_2[56];
  u64 *unk_2_3;
  u64 *unk_2_4;
  u64 *unk_2_5;
  i08 unk_2_6[13];
  v4f cameraPos;
  i08 unk_3[64];
  f32 rotateXAnim;
  f32 rotateYAnim;
  i08 unk_4[8];
  f32 rotateSpeedX;
  f32 rotateSpeedY;
  i08 unk_4_2[8];
  f32 rotateX;
  f32 rotateY;
  u64 unk_4_3;
  f32 scale;
  f32 scaleAnim;
  i08 unk_4_4[24];
  f32 focus;
  f32 focusAnim;
  u64 unk_4_5;
  f32 brightness;
  f32 brightnessAnim;
  i08 unk_5[88];
  v4f lookAt;
  i08 unk_6[40];
  f32 unk_7;
  i08 unk_8[20];
  i32 *unk_8_2;
  i08 unk_8_3[5];
} SkyCameraProp;

typedef struct {
  u32 unk_1;
  // The field-of-view in the unit of 'rad'.
  f32 fov;
  u64 unk_2;
  union {
    struct {
      // Rotate matrix (3x3) is stored separately by rows in three vectors. The
      // last component of these vectors is 0.
      v4f left;
      v4f up;
      v4f backward;
      // The last element of this vector is 1. We can consider the four vectors
      // as a single matrix (4x4).
      v4f cameraPos;
    };
    m44 mat;
  };
  char unk_4[32];
} MainCameraContext;

// The parent class of all possible camera updates.
typedef struct MainCamera_ {
  u64 *lpVtbl;
  u64 unk_1;
  u64 unk_2;
  struct MainCamera_ *prev;
  struct MainCamera_ *next;
  u64 unk_3;
  MainCameraContext context1;
  MainCameraContext context2;
  u64 active;
} MainCamera;

// The size of the following two structs is incorrect. This definition only
// contains components what we need.

// The states of the default camera of the player.
typedef struct {
  MainCamera super;
  char unk_1[56];
  v4f mouseDelta;
  char unk_2[1456];
} WhiskerCamera;

// The camera status of the players's camera accessory.
typedef struct {
  MainCamera super;
  void *prop;
  void *unk_2;
  void *unk_3;
  v4f pos;
  v4f dir;
} SkyCamera;

#define SkyCamera_getPropPtr(sc) ((SkyCameraProp *)((i08 *)(((SkyCamera *)(sc))->prop) - 0x88))

// The return value of World::interactionTest().
typedef struct {
  // Intersection point.
  v4f intersection;
  // Normal vector.
  v4f normal;
  i08 unk[0x40];
} InteractionResult;

#ifdef __cplusplus
}
#endif

#endif
