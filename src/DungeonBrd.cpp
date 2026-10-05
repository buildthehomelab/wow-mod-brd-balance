/*
 * mod-dungeon-brd
 *
 * Balance tweaks for Blackrock Depths (map 230).
 *
 * Respawns: the Lyceum (the hall after the Summoners' Tomb, where you kill Shadowforge Flame
 * Keepers for the torches) is meant to respawn quickly to keep a group moving, but its ~200
 * Anvilrage Reservists have a spawn time of 0. They come back as soon as their corpse decays,
 * which for normal-rank mobs is a minute after death. The four Flame Keepers are on 5 minutes,
 * which is the default minimum here.
 *
 * Every BRD creature spawned from the database whose spawn time is below MinRespawnSeconds gets
 * that as its respawn delay instead. Spawn times above it are never lowered, scripted summons are
 * left alone, and IgnoreEntries keeps event NPCs (the Grim Guzzler's Hurley Blackbreath and his
 * cronies, the Brewfest brewer) on their stock timers. The world database is not changed.
 *
 * Iron Hall flames: during Magmus the six Ironhand Guardian statues cast Gout of Flame (15529), a
 * 10 s self aura that triggers 15538 every second. 15538 targets every enemy within 30 yards of
 * the statue (SRC_AREA_ENEMY), and the statues sit 45 yards apart in rows of two, so the whole hall
 * burns and there is no safe spot. The flames are meant to shoot straight across the hall, so the
 * spell script keeps only targets in a strip IronHallFlameWidth wide in front of each statue. Stock
 * AzerothCore also never turns the statues off when Magmus dies (TYPE_IRON_HALL DONE only opens the
 * throne door), so they keep firing for the rest of the run; both spells now do nothing unless the
 * Magmus encounter is in progress.
 *
 * Released under the MIT License.
 */

#include "Config.h"
#include "Creature.h"
#include "CreatureData.h"
#include "Log.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"

#include <algorithm>
#include <list>

#include <sstream>
#include <string>
#include <unordered_set>

namespace
{
    constexpr uint32 BRD_MAP_ID = 230;
    constexpr uint32 BRD_TYPE_IRON_HALL = 6;  // TYPE_IRON_HALL in blackrock_depths.h (Magmus)

    struct Config
    {
        bool enabled = true;
        uint32 minRespawn = 300;  // seconds
        float ironHallFlameWidth = 10.0f;  // yards, 0 = stock 30 yd circle
        std::unordered_set<uint32> ignoredEntries;
    };

    Config config;

    std::unordered_set<uint32> ParseEntries(std::string const& list)
    {
        std::unordered_set<uint32> entries;
        std::stringstream stream(list);
        std::string token;
        while (std::getline(stream, token, ','))
        {
            try
            {
                if (token.find_first_not_of(" \t") != std::string::npos)
                    entries.insert(static_cast<uint32>(std::stoul(token)));
            }
            catch (...)
            {
                LOG_ERROR("server.loading", "mod-dungeon-brd: ignoring bad IgnoreEntries value '{}'", token);
            }
        }
        return entries;
    }
}

class DungeonBrdWorldScript : public WorldScript
{
public:
    DungeonBrdWorldScript() : WorldScript("DungeonBrdWorldScript", { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        config.enabled = sConfigMgr->GetOption<bool>("DungeonBrd.Enable", true);
        config.minRespawn = sConfigMgr->GetOption<uint32>("DungeonBrd.MinRespawnSeconds", 300);
        config.ignoredEntries = ParseEntries(sConfigMgr->GetOption<std::string>("DungeonBrd.IgnoreEntries", "9537,9541,28067"));
        config.ironHallFlameWidth = std::max(0.0f, sConfigMgr->GetOption<float>("DungeonBrd.IronHallFlameWidth", 10.0f));

        LOG_INFO("server.loading", "mod-dungeon-brd: {}, minimum respawn {}s, {} ignored entries, Iron Hall flame width {}",
            config.enabled ? "enabled" : "disabled", config.minRespawn, config.ignoredEntries.size(), config.ironHallFlameWidth);
    }
};

class DungeonBrdCreatureScript : public AllCreatureScript
{
public:
    DungeonBrdCreatureScript() : AllCreatureScript("DungeonBrdCreatureScript") { }

    // Runs once per creature object. In compatibility respawn mode the same object respawns and
    // keeps the delay; in dynamic mode a fresh creature is added (and adjusted) on every respawn.
    void OnCreatureAddWorld(Creature* creature) override
    {
        if (!config.enabled || !config.minRespawn || creature->GetMapId() != BRD_MAP_ID)
            return;

        // summons have no spawn row; their lifetime belongs to whatever script made them
        CreatureData const* data = creature->GetCreatureData();
        if (!creature->GetSpawnId() || !data)
            return;

        if (data->spawntimesecs >= config.minRespawn || creature->GetRespawnDelay() >= config.minRespawn)
            return;

        if (config.ignoredEntries.count(creature->GetEntry()))
            return;

        creature->SetRespawnDelay(config.minRespawn);
    }
};

namespace
{
    // The statues only belong to the Magmus fight. Stock never switches them off after he dies.
    bool IronHallInProgress(WorldObject const* caster)
    {
        InstanceScript* instance = caster ? caster->GetInstanceScript() : nullptr;
        return !instance || instance->GetData(BRD_TYPE_IRON_HALL) == IN_PROGRESS;
    }
}

// 15529 - Gout of Flame (Ironhand Guardian, 10 s aura triggering 15538 every second)
class spell_dungeon_brd_gout_of_flame : public SpellScript
{
    PrepareSpellScript(spell_dungeon_brd_gout_of_flame);

    SpellCastResult CheckCast()
    {
        if (config.enabled && !IronHallInProgress(GetCaster()))
            return SPELL_FAILED_DONT_REPORT;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_dungeon_brd_gout_of_flame::CheckCast);
    }
};

// 15538 - Gout of Flame (damage): a jet straight ahead instead of a 30 yd circle around the statue
class spell_dungeon_brd_gout_of_flame_damage : public SpellScript
{
    PrepareSpellScript(spell_dungeon_brd_gout_of_flame_damage);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (!config.enabled)
            return;

        Unit* caster = GetCaster();
        if (!IronHallInProgress(caster))
        {
            targets.clear();
            return;
        }

        if (!caster || config.ironHallFlameWidth <= 0.0f)
            return;

        float halfWidth = config.ironHallFlameWidth / 2.0f;
        targets.remove_if([caster, halfWidth](WorldObject* target)
        {
            return !caster->HasInLine(target, target->GetObjectSize(), halfWidth);
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dungeon_brd_gout_of_flame_damage::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

void AddDungeonBrdScripts()
{
    new DungeonBrdWorldScript();
    new DungeonBrdCreatureScript();
    RegisterSpellScript(spell_dungeon_brd_gout_of_flame);
    RegisterSpellScript(spell_dungeon_brd_gout_of_flame_damage);
}
