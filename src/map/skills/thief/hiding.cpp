// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "hiding.hpp"

#include "map/clif.hpp"
#include "map/pc.hpp"
#include "map/status.hpp"

SkillHiding::SkillHiding() : SkillImpl(TF_HIDING) {
}

void SkillHiding::castendNoDamageId(block_list *src, block_list *bl, uint16 skill_lv, t_tick tick, int32 &flag) const {
	sc_type type = skill_get_sc(getSkillId());
	status_change *tsc = status_get_sc(bl);
	status_change_entry *tsce = tsc ? tsc->getSCE(SC_HIDING) : nullptr;

	if (tsce) {
		clif_skill_nodamage(src, *bl, getSkillId(), -1, status_change_end(bl, type)); // Hide skill-scream animation.
		return;
	}

	clif_skill_nodamage(src, *bl, getSkillId(), -1, sc_start(src, bl, SC_HIDING, 100, skill_lv, skill_get_time(getSkillId(), skill_lv)));

	// [RO Sandbox] Backdoor Tunnel: Hiding marks the cell where it was cast
	map_session_data *sd = BL_CAST(BL_PC, src);
	if (sd != nullptr && pc_checkskill(sd, RG_BACKDOOR) > 0)
		sc_start4(src, bl, SC_BACKDOOR, 100, bl->x, bl->y, bl->m, 0, 300000);
}
