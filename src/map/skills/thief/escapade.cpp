// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "escapade.hpp"

#include "map/clif.hpp"
#include "map/status.hpp"

SkillEscapade::SkillEscapade() : SkillImpl(TF_ESCAPADE) {
}

// [RO Sandbox] Cancels debuffs and briefly raises flee and move speed by 100%.
// Lasts 1s at Lv 1, +0.1s per level. Castable while hidden (AllowWhenHidden).
void SkillEscapade::castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const {
	clif_skill_nodamage(src, *target, getSkillId(), skill_lv);
	status_change_clear_buffs(target, SCCB_DEBUFFS);
	sc_start(src, target, SC_ESCAPADE, 100, skill_lv, 900 + 100 * skill_lv);
}
