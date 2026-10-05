# BRD Balance

An [AzerothCore](https://www.azerothcore.org/) (WotLK 3.3.5a) module with balance tweaks for
Blackrock Depths.

## Lyceum respawns

The Lyceum is the hall after the Summoners' Tomb (the Seven), where you kill Shadowforge Flame
Keepers for the torches that light the braziers and open the way to Magmus. The room is meant to
respawn quickly so a group has to keep moving, but in stock AzerothCore it overdoes it. Its
roughly 200 Anvilrage Reservists have a spawn time of 0, so each one is back as soon as its corpse
decays, about a minute after it dies (normal-rank corpse decay is 60 seconds). The four Flame
Keepers are on 5 minutes.

With this module, every creature spawned in Blackrock Depths waits at least 5 minutes before it
respawns (`BrdBalance.MinRespawnSeconds`). The Reservists now match the Flame Keepers: the room
still refills behind a slow group, just not while you're still looting. Longer timers, like the
2 hours on the rest of BRD's trash and the bosses' timers, are never shortened.
Scripted summons (the Reservists General Angerforge calls in, the Ring of Law waves) are left alone,
and the Grim Guzzler event NPCs and the Brewfest brewer keep their stock timers. The world database
isn't changed: the respawn delay is set on each creature as it's added to the map, so turning the
module off restores stock behavior.

## Patch Notes: Blackrock Depths Balance

Category: Dungeons

- **Anvilrage Reservists** in the Lyceum now take 5 minutes to respawn, the same as the
  **Shadowforge Flame Keepers**, instead of returning about a minute after they die.

> The Lyceum is supposed to keep you moving, but Reservists coming back before you'd finished
> looting them was too much.

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
mod-brd-balance: enabled, minimum respawn 300s, 3 ignored entries
```

## Configuration

| Setting | Default | What it does |
| --- | --- | --- |
| `BrdBalance.Enable` | `1` | Master switch. |
| `BrdBalance.MinRespawnSeconds` | `300` | Minimum respawn time (seconds) for BRD creatures. `0` turns it off. |
| `BrdBalance.IgnoreEntries` | `"9537,9541,28067"` | Creature entries that keep their stock respawn (Hurley Blackbreath, Blackbreath Crony, Dark Iron Brewer). |

## What it changes

In the server's world database (map 230), the spawns with short respawns are:

| Creature | Entry | Spawns | Stock respawn |
| --- | --- | --- | --- |
| Anvilrage Reservist | 8901 | 236 (199 in the Lyceum) | 0 (back when the 60 s corpse decays) |
| Shadowforge Flame Keeper | 9956 | 4 | 300 s |
| Anvilrage Guardsman | 8891 | 1 | 300 s |
| Hurley Blackbreath / Blackbreath Crony | 9537 / 9541 | 4 | 300 s (ignored by default) |
| Dark Iron Brewer | 28067 | 1 | 180 s (ignored by default) |

## License

MIT
