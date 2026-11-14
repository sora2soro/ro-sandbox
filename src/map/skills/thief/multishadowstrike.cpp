// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "multishadowstrike.hpp"

#include "map/status.hpp"

SkillMultiShadowStrike::SkillMultiShadowStrike() : SkillImplRecursiveDamageSplash(AS_MULTISHADOW) {
}

// [RO Sandbox] Elite: leaps into the shadows (2 seconds of invulnerability) and
// strikes every enemy in a 7x7 area around the target cell. Targets marked by
// Grim Reaper's Mark always take critical strikes (battle.cpp). The shared
// elite cooldown waits for Phase 12; its own 60s cooldown comes from skill_db.
void SkillMultiShadowStrike::castendPos2(block_list* src, int32 x, int32 y, uint16 skill_lv, t_tick tick, int32& flag) const {
	sc_start(src, src, SC_INVINCIBLE, 100, skill_lv, 2000);
	SkillImplRecursiveDamageSplash::castendPos2(src, x, y, skill_lv, tick, flag);
}

void SkillMultiShadowStrike::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
	base_skillratio += 80 * skill_lv; // 100% + 80% x level
}

int16 SkillMultiShadowStrike::getSplashSearchSize(block_list* src, uint16 skill_lv) const {
	return 3; // 7x7
}
