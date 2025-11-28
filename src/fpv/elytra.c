#include <math.h>
#include "fpv/fpv.h"
#include "fpv/elytra.h"

static const v4f size = {0.1f, 0.1f, 0.1f, 0.1f};

HscFpv gElytra = {0};

static v4f calculateViewVector(v4f rot) {
  f32 cy = cosf(rot.x)
    , sy = sinf(rot.x)
    , cp = cosf(rot.y)
    , sp = sinf(rot.y);
  return v4fnew(-cp * sy, sp, -cp * cy, 0);
}

/**
 * Using keyboard input and fireworks physics logic to accelerate fpv.
 * 
 * @param mDelta Movement delta, directly from keyboard.
 * @param view The view vector of the moving point.
 * @param timeElapsed Time elapsed since last frame.
 */
static inline void elytraFirework(
  v4f mDelta,
  v4f view,
  f32 timeElapsed
) {
  v4f velocity, tmp1;

  // Convert m/s to m/gt.
  velocity = v4fscale(gElytra.vel, 1 / 20.0f);

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
 * 
 * @param view The view vector of the moving point.
 * @param timeElapsed Time elapsed since last frame.
 */
static inline void elytraTravel(
  v4f view,
  f32 timeElapsed
) {
  v4f vel, tmpVec, deltaVel;
  f32 timeGt = timeElapsed * 20.0f
    , viewXZLen = sqrtf(view.x * view.x + view.z * view.z)
    // Equals to cosf(gElytra.rot.y) * cosf(gElytra.rot.y).
    , cosPitch2 = viewXZLen * viewXZLen
    , sinPitch = view.y
    // The change in velocity caused by gravity, in m/gt.
    , gravity = -0.08f * timeGt
    , horizonSpeed, friction, tmp;
  AABB_t aabb;

  // Convert to m/gt.
  vel = v4fscale(gElytra.vel, 1 / 20.0f);
  vel.y += gravity * (1.0f - 0.75f * cosPitch2);

  horizonSpeed = sqrtf(vel.x * vel.x + vel.z * vel.z);

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

    if (sinPitch > 0.0f) {
      // Apply speed improvement.
      tmp = horizonSpeed * sinPitch * 0.04f * timeGt;
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
  if (hscFpvCheckCollision(&aabb, &gElytra.vel, timeElapsed)) {
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

HscFpv *hscFpvElytraInit(
  v4f pos,
  v4f rot,
  i32 flags
) {
  memset(&gElytra, 0, sizeof(HscFpv));
  if (flags & FPVRST_POS)
    gElytra.pos = pos;
  if (flags & FPVRST_ROT)
    gElytra.rot = rot;

  eulerToRotationXYZ(gElytra.rot, gElytra.matrix.rows);
  gElytra.matrix.row4 = gElytra.pos;

  return &gElytra;
}

HscFpv *hscFpvElytraUpdate(
  v4f mDelta,
  v4f fDelta,
  f32 timeElapsed
) {
  v4f view, factor, dRot;
  m44 dRotMat = {0};
  const v4f rotationFriction1 = { 1.0f, 1.0f, 1.0f, 1.0f }
    , rotationFriction2 = { 0.1f, 0.1f, 0.1f, 0.1f };
  
  // Convert fDelta from rad to rad/s.
  fDelta = v4fscale(fDelta, 1 / timeElapsed);

  // Apply smooth effect.
  if (gElytra.flags & FPVELYTRA_SMOOTH) {
    factor = v4fseleq(v4fabs(fDelta), rotationFriction1, rotationFriction2);
    // Accelerate to fDelta (rad/s) with aacc = k * (fDelta - avel). The
    // factor, or k, is different when fDelta equals 0.
    gElytra.aacc = v4fmul(v4fsub(fDelta, gElytra.avel), factor);
    gElytra.avel = v4fadd(gElytra.avel, v4fscale(gElytra.aacc, timeElapsed));
  } else {
    // Set the angular velocity directly.
    gElytra.aacc = V4FZERO;
    gElytra.avel = fDelta;
  }

  // Calculate the rotation delta from angular velocity.
  dRot = v4fscale(gElytra.avel, timeElapsed);

  // Update rotation.
  if (gElytra.flags & FPVELYTRA_ROLL) {
    // Enable full direction movement, like Do-a-barrel-roll.
    eulerToRotationXYZ(dRot, dRotMat.rows);
    m44mul(&gElytra.matrix, &dRotMat, &gElytra.matrix);
    view = v4fscale(gElytra.matrix.row3, -1.0f);
  } else {
    // Vanilla minecraft elytra facings.
    gElytra.rot = v4fadd(gElytra.rot, dRot);
    gElytra.rot.y = m_clamp(gElytra.rot.y, -PI_F * 0.4975f, PI_F * 0.4975f);
    gElytra.rot.z = gElytra.rot.w = 0;
    view = calculateViewVector(gElytra.rot);
    // The physical update does not modify the rotation, so we can calculate
    // the matrix before it.
    eulerToRotationXYZ(gElytra.rot, gElytra.matrix.rows);
  }

  // Update physical state.
  elytraFirework(mDelta, view, timeElapsed);
  elytraTravel(view, timeElapsed);

  // Update position.
  gElytra.matrix.row4 = gElytra.pos;

  return &gElytra;
}

void hscFpvElytraEnableRoll(
  i08 enable
) {
  if (enable)
    gElytra.flags |= FPVELYTRA_ROLL;
  else
    gElytra.flags &= ~FPVELYTRA_ROLL;
}

void hscFpvElytraEnableSmooth(
  i08 enable
) {
  if (enable)
    gElytra.flags |= FPVELYTRA_SMOOTH;
  else
    gElytra.flags &= ~FPVELYTRA_SMOOTH;
}
