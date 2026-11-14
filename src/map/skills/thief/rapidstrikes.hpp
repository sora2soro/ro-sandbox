// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#pragma once

#include "../skill_impl.hpp"

// [RO Sandbox] Rapid Strikes (TF_RSTRIKES, Phase 6)
class SkillRapidStrikes : public SkillImpl {
public:
	SkillRapidStrikes();

	void castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const override;
};
