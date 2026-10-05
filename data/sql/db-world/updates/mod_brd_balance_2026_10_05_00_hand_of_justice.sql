-- mod-brd-balance: Hand of Justice back on General Angerforge.
--
-- Stock AzerothCore has Hand of Justice (11815) in Emperor Dagran Thaurissan's boss loot
-- (reference 35014, which only the Emperor uses) and Force of Will (11810) on General Angerforge.
-- This swaps them back to the older layout: each trinket takes the other's place in the same
-- equal-chance group, so neither boss's loot changes otherwise. Idempotent.
-- Undo with data/sql/uninstall/mod_brd_balance_uninstall_world.sql.

-- General Angerforge: Force of Will -> Hand of Justice
DELETE FROM `creature_loot_template` WHERE `Entry` = 9033 AND `Item` IN (11810, 11815) AND `Reference` = 0;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(9033, 11815, 0, 0, 0, 1, 1, 1, 1, 'General Angerforge - Hand of Justice');

-- Emperor Dagran Thaurissan (reference 35014): Hand of Justice -> Force of Will
DELETE FROM `reference_loot_template` WHERE `Entry` = 35014 AND `Item` IN (11810, 11815) AND `Reference` = 0;
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(35014, 11810, 0, 0, 0, 1, 1, 1, 1, 'Force of Will');
