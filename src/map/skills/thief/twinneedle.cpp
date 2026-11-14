// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "twinneedle.hpp"

#include "map/pc.hpp"
#include "map/status.hpp"

SkillTwinNeedle::SkillTwinNeedle() : WeaponSkillImpl(TF_TWINNEEDLE) {
}

// [RO Sandbox] Two Envenom strikes at one target. Stacking cooldown: 0.5s on
// the first cast, 1s on the second, 1.5s from the third on (max 3 stacks);
// the stacks reset after 2 seconds without a cast.
void SkillTwinNeedle::castendDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const {
	map_session_data *sd = BL_CAST(BL_PC, src);

	if (sd != nullptr) {
		status_change *sc = status_get_sc(src);
		int32 stacks = 1;

		if (sc != nullptr) {
			status_change_entry *twin = sc->getSCE(SC_TWINSTACK);

			if (twin != nullptr)
				stacks = min(twin->val1 + 1, 3);
		}

		t_tick cooldown = 500 * stacks;

		skill_blockpc_start(*sd, getSkillId(), cooldown);
		sc_start(src, src, SC_TWINSTACK, 100, stacks, 2000 + cooldown);
	}

	WeaponSkillImpl::castendDamageId(src, target, skill_lv, tick, flag);
}

void SkillTwinNeedle::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
	const map_session_data *sd = BL_CAST(BL_PC, src);
	int32 envenom_lv = sd != nullptr ? max(1, (int32)pc_checkskill(sd, TF_POISON)) : skill_lv;

	// Two Envenom (100% + 15% x Envenom level each), then +2.5% poison damage per level
	base_skillratio += 100 + 30 * envenom_lv;
	base_skillratio += base_skillratio * 5 * skill_lv / 200;
}

// -5% move speed per level for 1.5 seconds
void SkillTwinNeedle::applyAdditionalEffects(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32 attack_type, enum damage_lv dmg_lv) const {
	sc_start(src, target, SC_TWINNEEDLE, 100, skill_lv, 1500);
}
