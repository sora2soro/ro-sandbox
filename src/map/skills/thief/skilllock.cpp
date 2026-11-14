// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "skilllock.hpp"

#include "map/clif.hpp"
#include "map/status.hpp"

SkillSkillLock::SkillSkillLock() : SkillImpl(RG_SKILLLOCK) {
}

// [RO Sandbox] Toggle: while active, the skills copied by Mimicry and Envious
// Eye are kept and cannot be overwritten. Implemented as Preserve, which both
// copy paths already respect.
void SkillSkillLock::castendNoDamageId(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32& flag) const {
	status_change *tsc = status_get_sc(target);

	if (tsc != nullptr && tsc->getSCE(SC_PRESERVE) != nullptr) {
		clif_skill_nodamage(src, *target, getSkillId(), skill_lv, status_change_end(target, SC_PRESERVE));
		return;
	}

	clif_skill_nodamage(src, *target, getSkillId(), skill_lv,
		sc_start(src, target, SC_PRESERVE, 100, skill_lv, 600000));
}
