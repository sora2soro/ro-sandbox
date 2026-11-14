// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "sonicblow.hpp"

#include <config/core.hpp>

#include "map/pc.hpp"
#include "map/status.hpp"

SkillSonicBlow::SkillSonicBlow() : WeaponSkillImpl(AS_SONICBLOW) {
}

// [RO Sandbox] Shares Rapid Strikes' cooldown, and empowers the next Rapid
// Strikes cast within 15 seconds: +2% crit and +0.1s duration per level.
void SkillSonicBlow::castendDamageId(block_list* src, block_list* target, uint16 skill_lv, t_tick tick, int32& flag) const {
	WeaponSkillImpl::castendDamageId(src, target, skill_lv, tick, flag);

	map_session_data *sd = BL_CAST(BL_PC, src);

	if (sd != nullptr && pc_checkskill(sd, TF_RSTRIKES) > 0)
		skill_blockpc_start(*sd, TF_RSTRIKES, skill_get_cooldown(getSkillId(), skill_lv));
	sc_start(src, src, SC_SONICEMPOWER, 100, skill_lv, 15000);
}

void SkillSonicBlow::calculateSkillRatio(const Damage *wd, const block_list *src, const block_list *target, uint16 skill_lv, int32 &base_skillratio, int32 mflag) const {
#ifdef RENEWAL
	const status_data* tstatus = status_get_status_data(*target);

	base_skillratio += 100 + 100 * skill_lv;
	if (tstatus->hp < (tstatus->max_hp / 2))
		base_skillratio += base_skillratio / 2;
#else
	const map_session_data* sd = BL_CAST( BL_PC, src );

	base_skillratio += 200 + 50 * skill_lv;
	if (sd && pc_checkskill(sd, AS_SONICACCEL) > 0)
		base_skillratio += base_skillratio / 10;
#endif
}

void SkillSonicBlow::applyAdditionalEffects(block_list *src, block_list *target, uint16 skill_lv, t_tick tick, int32 attack_type, enum damage_lv dmg_lv) const {
	status_change *sc = status_get_sc(src);

	if (!map_flag_gvg2(target->m) && !map_getmapflag(target->m, MF_BATTLEGROUND) && sc && sc->getSCE(SC_SPIRIT) && sc->getSCE(SC_SPIRIT)->val2 == SL_ASSASIN)
		sc_start(src, target, SC_STUN, (4 * skill_lv + 20), skill_lv, skill_get_time2(getSkillId(), skill_lv)); //Link gives double stun chance outside GVG/BG
	else
		sc_start(src, target, SC_STUN, (2 * skill_lv + 10), skill_lv, skill_get_time2(getSkillId(), skill_lv));
}

void SkillSonicBlow::modifyHitRate(int16& hit_rate, const block_list* src, const block_list* target, uint16 skill_lv) const {
	const map_session_data* sd = BL_CAST( BL_PC, src );

	if(sd && pc_checkskill(sd,AS_SONICACCEL) > 0)
#ifdef RENEWAL
		hit_rate += hit_rate * 90 / 100;
#else
		hit_rate += hit_rate * 50 / 100;
#endif
}
