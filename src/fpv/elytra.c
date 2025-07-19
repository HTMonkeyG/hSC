#include <math.h>

#include <stdio.h>

#include "elytra.h"
#include "mth/vector.h"
#include "fpv/fpv.h"
#include "mth/macros.h"

static const v4f size = {0.1f, 0.1f, 0.1f, 0.1f};

FPV_t gElytra = {0};

static v4f calculateViewVector(v4f rot) {
  f32 cy = cosf(rot.x)
    , sy = sinf(rot.x)
    , cp = cosf(rot.y)
    , sp = sinf(rot.y);
  return v4fnew(-cp * sy, sp, -cp * cy, 0);
}

/**
 * Accelerate fpv with keyboard inputs.
 */
static inline void elytraFirework(v4f mDelta, f32 timeElapsed) {
  v4f view = calculateViewVector(gElytra.rot)
    // Convert m/s to m/gt.
    , velocity = v4fscale(gElytra.vel, 1 / 20.0f)
    , tmp1;

  // Acceleration, in m/gt^2.
  // view * 0.1 + (view * 1.5 - velocity) * 0.5
  tmp1 = v4fadd(
    v4fscale(view, 0.1f),
    v4fscale(
      v4fsub(
        v4fscale(view, 1.5f),
        velocity
      ),
      0.5f
    )
  );
  // Convert m/gt^2 to m/s^2.
  tmp1 = v4fscale(tmp1, 400.0f);

  // Apply accelerate.
  gElytra.vel = v4fadd(gElytra.vel, v4fscale(tmp1, mDelta.z * timeElapsed));
  // When pressing space in MCBE, the elytra will be given an upward
  // acceleration.
  gElytra.vel.y += 48.0f * mDelta.y * timeElapsed;
}

/**
 * Move fpv with elytra physics.
 * 
 * All of the calculations is done under the time unit of gt, and then
 * convert to second.
 */
static inline void elytraTravel(f32 timeElapsed) {
  v4f view = calculateViewVector(gElytra.rot)
    // Convert to m/gt.
    , vel = v4fscale(gElytra.vel, 1 / 20.0f)
    , tmpVec, deltaVel;
  f32 timeGt = timeElapsed * 20.0f
    , viewXZLen = sqrtf(view.x * view.x + view.z * view.z)
    , horizonSpeed = sqrtf(vel.x * vel.x + vel.z * vel.z)
    , cosPitch2 = cosf(gElytra.rot.y)
    // Gravitational acceleration.
    , gravity = -0.08f * timeGt
    , friction, tmp;
  AABB_t aabb;

  cosPitch2 *= cosPitch2;
  vel.y += gravity * (1.0f - 0.75f * cosPitch2);

  // In order to adapt different update frequency in different FPS, we
  // multiplied `timeGt` in every accelerate operation, which means treating
  // each speed addition/subtraction as an implicit acceleration with a time
  // of 1gt.
  if (viewXZLen > 0.0f) {
    if (vel.y < 0) {
      // Apply diving forward speed.
      tmp = vel.y * -0.1f * cosPitch2 * timeGt;
      vel = v4fadd(
        vel,
        v4fnew(view.x * tmp / viewXZLen, tmp, view.z * tmp / viewXZLen, 0));
    }

    if (gElytra.rot.y > 0.0f) {
      // Apply speed improvement.
      tmp = horizonSpeed * sinf(gElytra.rot.y) * 0.04f * timeGt;
      vel = v4fadd(
        v4fnew(-view.x * tmp / viewXZLen, tmp * 3.2f, -view.z * tmp / viewXZLen, 0),
        vel);
    }

    // Apply gliding descent acceleration.
    tmpVec = v4fnew(
      view.x / viewXZLen * horizonSpeed - vel.x,
      0,
      view.z / viewXZLen * horizonSpeed - vel.z,
      0);
    tmpVec = v4fscale(tmpVec, timeGt * 0.1f);
    vel = v4fadd(vel, tmpVec);
  }

  // Apply air resistance.
  tmpVec = v4fmul(vel, v4fscale(v4fnew(0.01f, 0.02f, 0.01f, 0), timeGt));
  gElytra.vel = v4fscale(v4fsub(vel, tmpVec), 20.0f);

  // Check collision.
  aabb_fromCenter(&aabb, gElytra.pos, size);
  deltaVel = gElytra.vel;
  if (fpv_checkCollision(&aabb, &gElytra.vel, timeElapsed)) {
    deltaVel = v4fnormalize(v4fsub(gElytra.vel, deltaVel));
    // Calculate the pressure using the momentum theorem.
    friction = v4flen(deltaVel) / timeElapsed * 0.01f * timeElapsed;
    // Simulate static friction.
    if (v4flen(gElytra.vel) > friction)
      gElytra.vel = v4fsub(
        gElytra.vel,
        v4fscale(v4fnormalize(gElytra.vel), friction));
    else
      gElytra.vel = V4FZERO;
    if (deltaVel.y);
  }

  // Do movement.
  gElytra.pos = v4fadd(gElytra.pos, v4fscale(gElytra.vel, timeElapsed));
}

FPV_t *fpvElytra_init(v4f pos, v4f rot, i32 flags) {
  memset(&gElytra, 0, sizeof(FPV_t));
  if (flags & FPVRST_POS)
    gElytra.pos = pos;
  if (flags & FPVRST_ROT)
    gElytra.rot = rot;
  return &gElytra;
}

/**
 * Minecraft's elytra physics, reproduce from the description in Chinese
 * Minecraft Wiki (https://zh.minecraft.wiki/w/%E9%9E%98%E7%BF%85).
 * 
 * The time unit in Minecraft's code is 'gt', 1/20 of 1 second, but the time
 * unit in our codes is 'second', so we have added a lot of unit conversions
 * to make the operation feels the same as Minecraft.
 * 
 * We assume that this function is only executed once in every physic frame.
 * 
 * @param mDelta Movement delta, directly from keyboard.
 * @param fDelta Rotation delta, in the unit of 'rad'.
 * @param timeElapsed Time elapsed since last frame.
 */
FPV_t *fpvElytra_update(v4f mDelta, v4f fDelta, f32 timeElapsed) {
  // Update rotation.
  gElytra.rot = v4fadd(gElytra.rot, fDelta);
  gElytra.rot.y = clamp(gElytra.rot.y, -PI_F * 0.4975f, PI_F * 0.4975f);
  gElytra.rot.z = gElytra.rot.w = 0;

  elytraFirework(mDelta, timeElapsed);
  elytraTravel(timeElapsed);

  return &gElytra;
}
