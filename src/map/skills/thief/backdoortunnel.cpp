// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "backdoortunnel.hpp"

#include "map/clif.hpp"
#include "map/status.hpp"
#include "map/unit.hpp"

SkillBackdoorTunnel::SkillBackdoorTunnel() : SkillImpl(RG_BACKDOOR) {
}

// [RO Sandbox] Hiding marks the cell where it was cast (hiding.cpp). This
// returns the Rogue to that mark if it is within (5 + 2 x level) cells, and a
// hidden Rogue stays hidden (AllowWhenHidden, Hiding is left untouched).
void SkillBackdoorTunnel::castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const {
	map_session_data *sd = BL_CAST(BL_PC, src);
	status_change *sc = status_get_sc(src);
	status_change_entry *mark = sc != nullptr ? sc->getSCE(SC_BACKDOOR) : nullptr;
	int32 range = 5 + 2 * skill_lv;

	if (mark == nullptr || mark->val3 != src->m
		|| max(abs(src->x - mark->val1), abs(src->y - mark->val2)) > range) {
		if (sd != nullptr)
			clif_skill_fail(*sd, getSkillId());
		return;
	}

	int16 mark_x = (int16)mark->val1, mark_y = (int16)mark->val2;

	status_change_end(src, SC_BACKDOOR);
	clif_skill_nodamage(src, *target, getSkillId(), skill_lv);

	if (unit_movepos(src, mark_x, mark_y, 0, false))
		clif_slide(*src, mark_x, mark_y);
}
