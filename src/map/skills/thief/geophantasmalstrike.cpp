// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "geophantasmalstrike.hpp"

#include "map/clif.hpp"
#include "map/status.hpp"
#include "map/unit.hpp"

SkillGeoPhantasmalStrike::SkillGeoPhantasmalStrike() : WeaponSkillImpl(RG_GEOPHANTASM) {
}

// [RO Sandbox] Only usable while hidden: blinks behind the target, strikes, and
// enchants the blade for 10 seconds. Each weapon hit while enchanted adds a
// stack (max 5): +2% damage per stack for the Rogue (battle.cpp) and -3%
// ASPD/move speed per stack on the target (skill.cpp, status.cpp).
void SkillGeoPhantasmalStrike::castendDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const {
	map_session_data *sd = BL_CAST(BL_PC, src);
	status_change *sc = status_get_sc(src);

	if (sc == nullptr || sc->getSCE(SC_HIDING) == nullptr) {
		if (sd != nullptr)
			clif_skill_fail(*sd, getSkillId());
		return;
	}

	// Blink behind the target (the cell away from its facing)
	uint8 dir = unit_getdir(target);
	int16 blink_x = target->x - dirx[dir];
	int16 blink_y = target->y - diry[dir];

	if (unit_movepos(src, blink_x, blink_y, 1, true))
		clif_slide(*src, blink_x, blink_y);

	WeaponSkillImpl::castendDamageId(src, target, skill_lv, tick, flag);

	status_change_end(src, SC_HIDING); // the strike reveals the Rogue
	sc_start(src, src, SC_GEOBLADE, 100, 0, 10000);
}

void SkillGeoPhantasmalStrike::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
	base_skillratio += 50 + 30 * skill_lv; // 150% + 30% x level
}
