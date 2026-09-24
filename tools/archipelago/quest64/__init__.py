from typing import Dict, List

from BaseClasses import Region, Tutorial
from worlds.AutoWorld import WebWorld, World

from .Items import Q64Item, item_data_table, item_table, code_to_item_table, filler_items
from .Locations import (Q64Location, location_data_table, location_table,
                        code_to_location_table, vanilla_locations)
from .Options import Q64Options
from .Regions import region_data_table, get_exit
from .Rules import set_rules


class Q64WebWorld(WebWorld):
    theme = "grass"

    setup_en = Tutorial(
        tutorial_name="Start Guide",
        description="A guide to playing Quest 64 Recompiled in Archipelago.",
        language="English",
        file_name="guide_en.md",
        link="guide/en",
        authors=["Fuzzyness"],
    )

    tutorials = [setup_en]


class Q64World(World):
    """Quest 64, as the recompiled port plays it."""

    game = "Quest 64 Recompiled"
    web = Q64WebWorld()
    options_dataclass = Q64Options
    options: Q64Options
    location_name_to_id = location_table
    item_name_to_id = item_table

    def generate_early(self) -> None:
        # Asking for every kind of monster to be beaten means counting them,
        # and the count is the server's: a monster leaves nothing behind in
        # the save for the game to read back after a reload, the way a chest
        # or a spirit does. So the monster checks have to exist.
        if self.options.mammon_portal.value & 2:
            self.options.enemysanity.value = 1

    def create_item(self, name: str) -> Q64Item:
        data = item_data_table[name]
        return Q64Item(name, data.type, data.code, self.player)

    def get_filler_item_name(self) -> str:
        return self.random.choice(filler_items)

    def create_regions(self) -> None:
        for region_name in region_data_table:
            self.multiworld.regions.append(Region(region_name, self.player, self.multiworld))

        for region_name, region_data in region_data_table.items():
            region = self.multiworld.get_region(region_name, self.player)
            region.add_exits(get_exit(region_name))

        for location_name, location_data in location_data_table.items():
            if not location_data.can_create(self.options):
                continue
            region = self.multiworld.get_region(location_data.region, self.player)
            region.add_locations({location_name: location_data.address}, Q64Location)

    def locked_items(self) -> Dict[str, str]:
        """Item name -> location name for anything held out of the pool.

        With shuffle_orbs off the six gate items stay where the game puts
        them. If the group that location belongs to is switched off it is not
        a location at all, so the item goes back in the pool instead.
        """
        if self.options.shuffle_orbs:
            return {}
        return {
            item: location
            for item, location in vanilla_locations.items()
            if location_data_table[location].can_create(self.options)
            and item_data_table[item].can_create(self.options)
        }

    def pre_fill(self) -> None:
        for item_name, location_name in self.locked_items().items():
            location = self.multiworld.get_location(location_name, self.player)
            location.place_locked_item(self.create_item(item_name))

    def create_items(self) -> None:
        # The pool has to be exactly as big as the number of locations in
        # play, and which locations those are depends on the sanity options.
        # So: place what must exist, then pad with filler.
        locked = self.locked_items()
        open_locations = sum(
            1 for data in location_data_table.values() if data.can_create(self.options)
        ) - len(locked)

        pool: List[Q64Item] = []
        level_ups = 0
        for name, data in item_data_table.items():
            if not data.code or not data.can_create(self.options):
                continue
            if name in locked:
                continue
            if name == "Level Up":
                level_ups = data.num_exist
                continue
            for _ in range(data.num_exist):
                pool.append(self.create_item(name))

        # One Level Up per spirit when spirits are checks, plus however many
        # extra the yaml asked for. The extras have nothing behind them in
        # the game world - they just fire the element-choice screen when they
        # arrive - so they are capped only by the room left in the pool.
        wanted = (level_ups if self.options.spiritsanity else 0)
        wanted += self.options.extra_level_ups.value
        for _ in range(min(wanted, max(0, open_locations - len(pool)))):
            pool.append(self.create_item("Level Up"))

        while len(pool) > open_locations:
            pool.pop()
        while len(pool) < open_locations:
            pool.append(self.create_item(self.get_filler_item_name()))

        self.multiworld.itempool += pool

    def set_rules(self) -> None:
        set_rules(self, self.player)

    def fill_slot_data(self) -> Dict[str, object]:
        # What the game needs once it connects: which groups are checks, and
        # the id bases so it can turn a location id back into a check.
        return {
            "goal": self.options.goal.value,
            "mammon_portal": self.options.mammon_portal.value,
            "boss_souls": self.options.boss_souls.value,
            "chestsanity": bool(self.options.chestsanity),
            "giftsanity": bool(self.options.giftsanity),
            "wingsmith_wings": bool(self.options.wingsmith_wings),
            "enemysanity": bool(self.options.enemysanity),
            "spiritsanity": bool(self.options.spiritsanity),
            "shuffle_orbs": bool(self.options.shuffle_orbs),
            # The game's own settings from the yaml, and the seed its
            # randomizer rolls them with, so every session of this slot
            # plays the same shuffle.
            "rando_seed": self.random.getrandbits(31),
            "settings": {
                "shuffle_spells": self.options.shuffle_spells.value,
                "early_healing": self.options.early_healing.value,
                "enemy_randomizer": self.options.enemy_randomizer.value,
                "shuffle_boss_order": self.options.shuffle_boss_order.value,
                "random_guilty_element": self.options.random_guilty_element.value,
                "faster_areas": self.options.faster_areas.value,
                "wings_never_expire": self.options.wings_never_expire.value,
                "no_enemy_drop_limit": self.options.no_enemy_drop_limit.value,
                "element_cap_99": self.options.element_cap_99.value,
                "jp_healing": self.options.jp_healing.value,
                "jp_magic_barrier": self.options.jp_magic_barrier.value,
                "jp_stat_up_effect": self.options.jp_stat_up_effect.value,
                "exit_from_anywhere": self.options.exit_from_anywhere.value,
                "fast_walking": self.options.fast_walking.value,
                "text_improvements": self.options.text_improvements.value,
                "text_palette": self.options.text_palette.value,
                "staff_palette": self.options.staff_palette.value,
                "cloak_colour": self.options.cloak_colour.value,
                "brian_clothes": self.options.brian_clothes.value,
                "spell_palettes": self.options.spell_palettes.value,
            },
        }
