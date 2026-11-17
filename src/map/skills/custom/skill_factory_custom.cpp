// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "skill_factory_custom.hpp"

#include "map/clif.hpp"
#include "map/map.hpp"
#include "map/pc.hpp"
#include "map/status.hpp"

#include "../merchant/adrenalinerush.hpp"

// [RO Sandbox] Phase 6 Character Rework. The client only activates skills it was
// built with, so custom actives are reskins of existing native skills, with the
// effect branched by the caster's job. This factory runs first, so it wins over
// the donor skill's own job factory.

// Rapid Strikes: a Thief's Adrenaline Rush. A short, strong attack-speed and
// accuracy burst (+190% each) at the cost of -75% damage dealt. Other jobs keep
// normal Adrenaline Rush.
class SkillCustomAdrenalineRush : public SkillAdrenalineRush {
public:
	void castendNoDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const override {
		map_session_data* sd = BL_CAST(BL_PC, src);

		if (sd != nullptr && (sd->class_ & MAPID_FIRSTMASK) == MAPID_THIEF) {
			t_tick duration = 3000 + 600 * skill_lv; // 3.6s at Lv1 .. 6s at Lv5 (max)
			clif_skill_nodamage(src, *src, getSkillId(), skill_lv,
				sc_start(src, src, SC_RSTRIKES, 100, skill_lv, duration));
			return;
		}

		SkillAdrenalineRush::castendNoDamageId(src, target, skill_lv, tick, flag);
	}
};

std::unique_ptr<const SkillImpl> SkillFactoryCustom::create(const e_skill skill_id) const {
	switch (skill_id) {
		case BS_ADRENALINE:
			return std::make_unique<SkillCustomAdrenalineRush>();
		default:
			break;
	}

	return nullptr;
}
