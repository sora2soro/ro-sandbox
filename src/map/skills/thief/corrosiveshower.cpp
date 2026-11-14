// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "corrosiveshower.hpp"

#include "map/status.hpp"

SkillCorrosiveShower::SkillCorrosiveShower() : SkillImplRecursiveDamageSplash(RG_CORROSIVE) {
}

// [RO Sandbox] Bow-only 5x5 volley. Targets hit lose 2% DEF per level for 10
// seconds and can have their equipment break when hit (skill.cpp, not bosses).
void SkillCorrosiveShower::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
	base_skillratio += 20 * skill_lv - 20; // 80% + 20% x level
}

void SkillCorrosiveShower::applyAdditionalEffects(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32 attack_type, enum damage_lv dmg_lv) const {
	sc_start(src, target, SC_CORROSIVE, 100, skill_lv, 10000);
}

int16 SkillCorrosiveShower::getSplashSearchSize(block_list* src, uint16 skill_lv) const {
	return 2; // 5x5
}
