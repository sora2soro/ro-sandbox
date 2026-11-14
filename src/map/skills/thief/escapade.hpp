// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#pragma once

#include "../skill_impl.hpp"

// [RO Sandbox] Escapade (TF_ESCAPADE, Phase 6)
class SkillEscapade : public SkillImpl {
public:
	SkillEscapade();

	void castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const override;
};
