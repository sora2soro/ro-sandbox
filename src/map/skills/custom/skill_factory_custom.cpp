// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "skill_factory_custom.hpp"

#include "map/clif.hpp"

#include "../skill_impl.hpp"
#include "../swordman/bash.hpp"

class SkillCustomBash;

// [RO Sandbox] Phase 6: castable no-effect stubs for the new sandbox skills,
// so each one can be learned and cast in game before its real effect lands
// (one PR per skill). IDs and targeting live in tools/sandbox/new_skills.csv;
// passive skills are never cast, so they need no entry here.
class SkillSandboxSelfStub : public SkillImpl {
public:
	explicit SkillSandboxSelfStub(e_skill skill_id) : SkillImpl(skill_id) {}

	void castendNoDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const override {
		clif_skill_nodamage(src, *target, getSkillId(), skill_lv);
	}
};

class SkillSandboxGroundStub : public SkillImpl {
public:
	explicit SkillSandboxGroundStub(e_skill skill_id) : SkillImpl(skill_id) {}

	void castendPos2(block_list* src, int32 x, int32 y, uint16 skill_lv, t_tick tick, int32& flag) const override {
		// No effect yet.
	}
};

std::unique_ptr<const SkillImpl> SkillFactoryCustom::create(const e_skill skill_id) const {
	// [RO Sandbox] Phase 6 new skills: stubs until each effect PR.
	switch( skill_id ){
		case TF_RSTRIKES:
		case TF_ESCAPADE:
		case RG_SKILLLOCK:
		case RG_BACKDOOR:
			return std::make_unique<SkillSandboxSelfStub>(skill_id);

		case TF_TWINNEEDLE:
		case RG_HATERAID:
		case RG_GEOPHANTASM:
			return std::make_unique<WeaponSkillImpl>(skill_id);

		case AS_MULTISHADOW:
		case RG_CORROSIVE:
			return std::make_unique<SkillSandboxGroundStub>(skill_id);

		default:
			break;
	}

#if 0
	switch( skill_id ){
		case SM_BASH:
			return std::make_unique<SkillCustomBash>();

		default:
			return nullptr;
	}
#endif

	return nullptr;
}

class SkillCustomBash : public SkillBash{
	void calculateSkillRatio(const Damage* wd, const block_list* src, const block_list* target, uint16 skill_lv, int32& base_skillratio, int32 mflag) const override{
		// Normal Bash:
		// Base 100% + 30% per level
		// base_skillratio += 30 * skill_lv;

		// But my custom Bash Skill is stronger:
		// Base 100% + 300% per level
		base_skillratio += 300 * skill_lv;
	}
};
