-- mod-dungeon-brd: undo the module's world database changes. Run it by hand on the world
-- database after removing the module. It isn't applied automatically: AzerothCore only runs the
-- module's db-world folder.
--
-- Puts Hand of Justice (11815) back in Emperor Dagran Thaurissan's loot (reference 35014) and
-- Force of Will (11810) back on General Angerforge, as stock AzerothCore has them. The respawn
-- change needs no undo: it is never written to the database. Idempotent: safe to run again.

DELETE FROM `creature_loot_template` WHERE `Entry` = 9033 AND `Item` IN (11810, 11815) AND `Reference` = 0;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(9033, 11810, 0, 0, 0, 1, 1, 1, 1, 'General Angerforge - Force of Will');

DELETE FROM `reference_loot_template` WHERE `Entry` = 35014 AND `Item` IN (11810, 11815) AND `Reference` = 0;
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(35014, 11815, 0, 0, 0, 1, 1, 1, 1, 'Hand of Justice');
