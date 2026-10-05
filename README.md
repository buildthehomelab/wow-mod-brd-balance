# BRD Balance

An [AzerothCore](https://www.azerothcore.org/) (WotLK 3.3.5a) module with balance tweaks for
Blackrock Depths.

## Lyceum respawns

The Lyceum is the hall after the Summoners' Tomb (the Seven), where you kill Shadowforge Flame
Keepers for the torches that light the braziers and open the way to Magmus. In stock AzerothCore
it is stocked with about 200 Anvilrage Reservists whose spawn time is 0, so each one is back as
soon as its corpse decays, about a minute after it dies (normal-rank corpse decay is 60 seconds). The four
Flame Keepers respawn after 5 minutes. A group clearing the hall finds the start of it full again
before they reach the braziers. The rest of BRD's trash respawns after 2 hours.

With this module, every creature spawned in Blackrock Depths gets at least a 2-hour respawn
(`BrdBalance.MinRespawnSeconds`). Longer timers, such as the bosses', are never shortened.
Scripted summons (the Reservists General Angerforge calls in, the Ring of Law waves) are left alone,
and the Grim Guzzler event NPCs and the Brewfest brewer keep their stock timers. The world database
isn't changed: the respawn delay is set on each creature as it's added to the map, so turning the
module off restores stock behavior.

## Patch Notes: Blackrock Depths Balance

Category: Dungeons

- **Anvilrage Reservists** and **Shadowforge Flame Keepers** in the Lyceum no longer respawn
  within minutes of being killed. They now stay dead for 2 hours, like the rest of Blackrock Depths.

> The Lyceum's 200 Reservists came back about a minute after dying, so groups often had to
> fight through the hall twice to reach the braziers.

## Installation

Clone it into your AzerothCore `modules` folder, **as `mod-brd-balance`**. AzerothCore derives the
module's loader name from the folder name:

```bash
cd azerothcore-wotlk/modules
git clone https://github.com/buildthehomelab/wow-mod-brd-balance.git mod-brd-balance
```

Re-run CMake, rebuild the worldserver, and copy `conf/mod_brd_balance.conf.dist` to
`mod_brd_balance.conf` in your config directory. The module needs no SQL.

To check that it's loaded, look for this line in the worldserver log at startup:

```
mod-brd-balance: enabled, minimum respawn 7200s, 3 ignored entries
```

## Configuration

| Setting | Default | What it does |
| --- | --- | --- |
| `BrdBalance.Enable` | `1` | Master switch. |
| `BrdBalance.MinRespawnSeconds` | `7200` | Minimum respawn time for BRD creatures. `0` turns it off. |
| `BrdBalance.IgnoreEntries` | `"9537,9541,28067"` | Creature entries that keep their stock respawn (Hurley Blackbreath, Blackbreath Crony, Dark Iron Brewer). |

## What it changes

In the server's world database (map 230), the spawns below 2 hours are:

| Creature | Entry | Spawns | Stock respawn |
| --- | --- | --- | --- |
| Anvilrage Reservist | 8901 | 236 (199 in the Lyceum) | 0 (back when the 60 s corpse decays) |
| Shadowforge Flame Keeper | 9956 | 4 | 300 s |
| Anvilrage Guardsman | 8891 | 1 | 300 s |
| Hurley Blackbreath / Blackbreath Crony | 9537 / 9541 | 4 | 300 s (ignored by default) |
| Dark Iron Brewer | 28067 | 1 | 180 s (ignored by default) |

## License

MIT
