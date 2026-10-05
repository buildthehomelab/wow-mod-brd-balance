-- mod-dungeon-brd: Iron Hall flames (Magmus).
--
-- Hooks the Ironhand Guardians' Gout of Flame up to the module's spell scripts: 15529 (the 10 s
-- aura) won't start outside the Magmus fight, and 15538 (the damage tick) only hits a strip in
-- front of the statue instead of everything within 30 yards. Neither spell has a script in stock
-- AzerothCore. Idempotent.
-- Undo with data/sql/uninstall/mod_dungeon_brd_uninstall_world.sql.

DELETE FROM `spell_script_names` WHERE `spell_id` IN (15529, 15538) AND `ScriptName` LIKE 'spell_dungeon_brd_%';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(15529, 'spell_dungeon_brd_gout_of_flame'),
(15538, 'spell_dungeon_brd_gout_of_flame_damage');
