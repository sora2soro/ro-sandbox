// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "rapidstrikes.hpp"

#include "map/clif.hpp"
#include "map/pc.hpp"
#include "map/status.hpp"

SkillRapidStrikes::SkillRapidStrikes() : SkillImpl(TF_RSTRIKES) {
}

// [RO Sandbox] Briefly +190% attack speed and accuracy, -75% damage dealt
// (status.cpp / battle.cpp). A Sonic Blow within the last 15 seconds empowers
// it: +2% crit and +0.1s duration per Sonic Blow level. Shares Sonic Blow's
// cooldown for Assassins.
void SkillRapidStrikes::castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const {
	map_session_data *sd = BL_CAST(BL_PC, src);
	status_change *sc = status_get_sc(src);
	int32 duration = skill_get_time(getSkillId(), skill_lv);
	int32 crit = 0;

	if (sc != nullptr) {
		status_change_entry *emp = sc->getSCE(SC_SONICEMPOWER);

		if (emp != nullptr) {
			duration += 100 * emp->val1;
			crit = 2 * emp->val1;
			status_change_end(src, SC_SONICEMPOWER);
		}
	}

	clif_skill_nodamage(src, *target, getSkillId(), skill_lv,
		sc_start2(src, target, SC_RSTRIKES, 100, skill_lv, crit, duration));

	if (sd != nullptr && pc_checkskill(sd, AS_SONICBLOW) > 0)
		skill_blockpc_start(*sd, AS_SONICBLOW, skill_get_cooldown(getSkillId(), skill_lv));
}
