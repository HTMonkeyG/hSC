#ifndef __ELYTRA_H__
#define __ELYTRA_H__

/**
 * This file contains basic minecraft elytra physics, and this is a template
 * for developing your own FPV physics.
 */

#include "internal.h"
#include "fpv/fpv.h"

#define FPVELYTRA_ROLL (0x01)
#define FPVELYTRA_SMOOTH (0x02)

#ifdef __cplusplus
extern "C" {
#endif

/** Import the struct. */
extern HscFpv gElytra;

/**
 * Initialize the HscFpv struct.
 * 
 * @param pos Initial camera pos.
 * @param rot Initial camera rotation.
 * @param flags Customized initialize flags.
 */
HscFpv *hscFpvElytraInit(
  v4f pos,
  v4f rot,
  i32 flags);

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
HscFpv *hscFpvElytraUpdate(
  v4f mDelta,
  v4f fDelta,
  f32 timeElapsed);

/**
 * Enable or disable barrel roll in elytra mode.
 * 
 * @param enable True to enable.
 */
void hscFpvElytraEnableRoll(
  i08 enable);

/**
 * Enable or disable smooth in elytra mode.
 * 
 * @param enable True to enable.
 */
void hscFpvElytraEnableSmooth(
  i08 enable);

#ifdef __cplusplus
}
#endif

#endif
