# Dungeon: Blackrock Depths

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
respawns (`DungeonBrd.MinRespawnSeconds`). The Reservists now match the Flame Keepers: the room
still refills behind a slow group, just not while you're still looting. Longer timers, like the
2 hours on the rest of BRD's trash and the bosses' timers, are never shortened.
Scripted summons (the Reservists General Angerforge calls in, the Ring of Law waves) are left alone,
and the Grim Guzzler event NPCs and the Brewfest brewer keep their stock timers. The world database
isn't changed: the respawn delay is set on each creature as it's added to the map, so turning the
module off restores stock behavior.

## Hand of Justice on General Angerforge

**Hand of Justice** used to drop from General Angerforge, until later vanilla moved it to Emperor
Dagran Thaurissan and gave Angerforge **Force of Will** instead. This module moves them back:
Angerforge drops Hand of Justice and the Emperor drops Force of Will.

Each trinket takes the other's slot in the same loot group, so the odds barely move. Angerforge
drops one of 5 equal items (20% each). The Emperor drops 2 items from a pool of 11, which works
out to about 19% for each regular item. Nothing else in either boss's loot changes, and no
other creature drops either trinket.

This part is a world database update (`data/sql/db-world/updates`), applied automatically the next
time the worldserver starts. The loot swap isn't covered by `DungeonBrd.Enable`. To undo it after removing the
module, run `data/sql/uninstall/mod_dungeon_brd_uninstall_world.sql` on the world database by hand.

## Iron Hall flames (Magmus)

Six Ironhand Guardian statues line the Iron Hall in three rows of two, and during the Magmus fight
they breathe fire (**Gout of Flame**). The fire is meant to shoot straight across the hall, so you
fight Magmus in the gaps between the rows. In stock AzerothCore the damage hits every enemy within
30 yards of a statue instead. The rows are 45 yards apart, so the circles cover the whole hall and
there's nowhere safe to stand.

This module limits each statue's damage to a strip straight out in front of it, 10 yards wide
(`DungeonBrd.IronHallFlameWidth`) and still 30 yards long, so the two statues in a row burn the
line between them and the space between rows is safe. It also switches the statues off when
Magmus dies; stock AzerothCore leaves them firing for the rest of the run.

The fire's look is up to the client, so it may not line up with where the damage lands. Trust the
statue's facing: the strip runs straight out from the statue across the hall.

This needs the module's world database update (two `spell_script_names` rows), applied
automatically on the next worldserver start and removed by the uninstall SQL.

## Patch Notes: Blackrock Depths

Category: Dungeons

- **Anvilrage Reservists** in the Lyceum now take 5 minutes to respawn, the same as the
  **Shadowforge Flame Keepers**, instead of returning about a minute after they die.
- **Hand of Justice** drops from **General Angerforge** again. **Emperor Dagran Thaurissan** now
  drops **Force of Will** in its place.
- The **Ironhand Guardians'** **Gout of Flame** in the Iron Hall now shoots straight across the
  hall instead of burning everything within 30 yards, so the gaps between the rows of statues are
  safe during **Magmus**.
- The Ironhand Guardians stop breathing fire once Magmus is dead.

> The Lyceum is supposed to keep you moving, but Reservists coming back before you'd finished
> looting them was too much.

## Installation

Clone it into your AzerothCore `modules` folder, **as `mod-dungeon-brd`**. AzerothCore derives the
module's loader name from the folder name:

```bash
cd azerothcore-wotlk/modules
git clone https://github.com/buildthehomelab/wow-mod-dungeon-brd.git mod-dungeon-brd
```

Re-run CMake, rebuild the worldserver, and copy `conf/mod_dungeon_brd.conf.dist` to
`mod_dungeon_brd.conf` in your config directory. Its world database updates (the loot swap and the
Iron Hall spell scripts) are applied automatically on the next worldserver start.

To check that it's loaded, look for this line in the worldserver log at startup:

```
mod-dungeon-brd: enabled, minimum respawn 300s, 3 ignored entries, Iron Hall flame width 10
```

## Configuration

| Setting | Default | What it does |
| --- | --- | --- |
| `DungeonBrd.Enable` | `1` | Master switch. |
| `DungeonBrd.MinRespawnSeconds` | `300` | Minimum respawn time (seconds) for BRD creatures. `0` turns it off. |
| `DungeonBrd.IgnoreEntries` | `"9537,9541,28067"` | Creature entries that keep their stock respawn (Hurley Blackbreath, Blackbreath Crony, Dark Iron Brewer). |
| `DungeonBrd.IronHallFlameWidth` | `10` | Width in yards of each Ironhand Guardian's flame strip during Magmus. `0` keeps the stock 30-yard circle. |

## What the respawn change touches

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
