// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "hateraid.hpp"

#include "map/status.hpp"

SkillHateRaid::SkillHateRaid() : WeaponSkillImpl(RG_HATERAID) {
}

// [RO Sandbox] Elite: a 4-hit combo; the hits divest helm, shield, armor and
// weapon in order, each guaranteed and lasting (10 + level) seconds. Chemical
// protection still blocks the divests. The shared elite cooldown waits for
// Phase 12; its own 60s cooldown comes from skill_db.
void SkillHateRaid::castendDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const {
	WeaponSkillImpl::castendDamageId(src, target, skill_lv, tick, flag);

	status_change *tsc = status_get_sc(target);
	int32 duration = (10 + skill_lv) * 1000;
	const sc_type strip_sc[4] = { SC_STRIPHELM, SC_STRIPSHIELD, SC_STRIPARMOR, SC_STRIPWEAPON };
	const sc_type protect_sc[4] = { SC_CP_HELM, SC_CP_SHIELD, SC_CP_ARMOR, SC_CP_WEAPON };

	for (int32 i = 0; i < 4; i++) {
		if (tsc != nullptr && tsc->getSCE(protect_sc[i]) != nullptr)
			continue;
		sc_start(src, target, strip_sc[i], 100, skill_lv, duration);
	}
}

void SkillHateRaid::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
	base_skillratio += 300 + 200 * skill_lv; // 4 hits of 100% + 50% x level
}
