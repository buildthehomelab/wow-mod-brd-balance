/*
 * mod-brd-balance
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
 * Released under the MIT License.
 */

#include "Config.h"
#include "Creature.h"
#include "CreatureData.h"
#include "Log.h"
#include "ScriptMgr.h"

#include <sstream>
#include <string>
#include <unordered_set>

namespace
{
    constexpr uint32 BRD_MAP_ID = 230;

    struct Config
    {
        bool enabled = true;
        uint32 minRespawn = 300;  // seconds
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
                LOG_ERROR("server.loading", "mod-brd-balance: ignoring bad IgnoreEntries value '{}'", token);
            }
        }
        return entries;
    }
}

class BrdBalanceWorldScript : public WorldScript
{
public:
    BrdBalanceWorldScript() : WorldScript("BrdBalanceWorldScript", { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        config.enabled = sConfigMgr->GetOption<bool>("BrdBalance.Enable", true);
        config.minRespawn = sConfigMgr->GetOption<uint32>("BrdBalance.MinRespawnSeconds", 300);
        config.ignoredEntries = ParseEntries(sConfigMgr->GetOption<std::string>("BrdBalance.IgnoreEntries", "9537,9541,28067"));

        LOG_INFO("server.loading", "mod-brd-balance: {}, minimum respawn {}s, {} ignored entries",
            config.enabled ? "enabled" : "disabled", config.minRespawn, config.ignoredEntries.size());
    }
};

class BrdBalanceCreatureScript : public AllCreatureScript
{
public:
    BrdBalanceCreatureScript() : AllCreatureScript("BrdBalanceCreatureScript") { }

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

void AddBrdBalanceScripts()
{
    new BrdBalanceWorldScript();
    new BrdBalanceCreatureScript();
}
