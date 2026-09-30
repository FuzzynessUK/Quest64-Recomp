from typing import Dict, List

from BaseClasses import ItemClassification, LocationProgressType, Region, Tutorial
from Options import OptionError
from worlds.AutoWorld import WebWorld, World

from .Enemies import ENEMY_AREAS, ENEMY_IDS, ENEMY_LOCATIONS, FILE_MONSTERS, REGION_ORDER
from .Items import Q64Item, item_data_table, item_table, code_to_item_table, filler_items
from .Locations import (Q64Location, Q64LocationData, location_data_table, location_table,
                        code_to_location_table, vanilla_locations)
from .Options import Q64Options
from .Regions import regions, connections
from .Rules import set_all_rules


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
        self.plan_enemies()

    def plan_enemies(self) -> None:
        """Where every monster is, which the enemy locations' regions follow.

        Enemy Randomizer off: where the game puts them, as the workbook says.
        On: decided here and sent to the game in slot_data, which builds its
        encounter packs to match - each area gets one of the six monster
        files and a list of that file's monsters, sized to what the area's
        packs can hold. A monster is logically in the earliest region of any
        area it appears in; one that appears nowhere is not a location.
        """
        self.enemy_plan = None
        self.enemy_region: Dict[int, str] = {}
        if not self.options.enemy_randomizer:
            for gid, name in ENEMY_LOCATIONS.items():
                self.enemy_region[gid] = location_data_table[name].region
            return

        rnd = self.random
        tables = [-1] * len(ENEMY_AREAS)
        if self.options.ensure_all_enemies:
            # Every file first gets an area before Mammon's World with room
            # for all of it, biggest file first so the few big areas go to
            # the files that need them.
            free = [a for a, (_, region, _) in enumerate(ENEMY_AREAS) if region != "Endgame"]
            files = sorted(range(len(FILE_MONSTERS)), key=lambda f: (-len(FILE_MONSTERS[f]), rnd.random()))
            for f in files:
                fits = [a for a in free if ENEMY_AREAS[a][2] >= len(FILE_MONSTERS[f])]
                if not fits:
                    raise OptionError(f"Quest 64: no area left with room for monster file {f}")
                a = rnd.choice(fits)
                tables[a] = f
                free.remove(a)
        previous = -1
        for a in range(len(ENEMY_AREAS)):
            if tables[a] < 0:
                # Any file, but not the one the area before has, so
                # neighbouring areas look different.
                tables[a] = rnd.choice([f for f in range(len(FILE_MONSTERS)) if f != previous])
            previous = tables[a]

        rosters: List[List[int]] = []
        for a, (_, region, capacity) in enumerate(ENEMY_AREAS):
            entries = list(range(len(FILE_MONSTERS[tables[a]])))
            if capacity < len(entries):
                entries = sorted(rnd.sample(entries, capacity))
            rosters.append(entries)
            for entry in entries:
                gid = FILE_MONSTERS[tables[a]][entry]
                known = self.enemy_region.get(gid)
                if known is None or REGION_ORDER.index(region) < REGION_ORDER.index(known):
                    self.enemy_region[gid] = region
        self.enemy_plan = {"tables": tables, "rosters": rosters}

    def location_exists(self, name: str, data: Q64LocationData) -> bool:
        if not data.can_create(self.options):
            return False
        return data.group != "enemy" or ENEMY_IDS[name] in self.enemy_region

    def location_region(self, name: str, data: Q64LocationData) -> str:
        return self.enemy_region[ENEMY_IDS[name]] if data.group == "enemy" else data.region

    def create_item(self, name: str) -> Q64Item:
        data = item_data_table[name]
        return Q64Item(name, data.type, data.code, self.player)

    def get_filler_item_name(self) -> str:
        return self.random.choice(filler_items)

    def create_regions(self) -> None:
        for region_name in regions:
            self.multiworld.regions.append(Region(region_name, self.player, self.multiworld))

        # Named "<from> to <to>", the name Rules.py sets each rule by.
        for source, target in connections:
            self.multiworld.get_region(source, self.player).connect(
                self.multiworld.get_region(target, self.player), f"{source} to {target}")

        for location_name, location_data in location_data_table.items():
            if not self.location_exists(location_name, location_data):
                continue
            region_name = self.location_region(location_name, location_data)
            region = self.multiworld.get_region(region_name, self.player)
            region.add_locations({location_name: location_data.address}, Q64Location)
            # A monster only met in Mammon's World is beaten after the final
            # portal, when nothing is left to unlock: filler only.
            if location_data.group == "enemy" and region_name == "Endgame":
                self.multiworld.get_location(location_name, self.player).progress_type = \
                    LocationProgressType.EXCLUDED

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
            if self.location_exists(location, location_data_table[location])
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
            1 for name, data in location_data_table.items() if self.location_exists(name, data)
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
            count = data.num_exist
            if data.type == ItemClassification.useful:   # the wings
                count = min(count, self.options.wings_in_pool.value)
            for _ in range(count):
                pool.append(self.create_item(name))

        # One Level Up per spirit when spirits are checks, plus however many
        # extra the yaml asked for. The extras have nothing behind them in
        # the game world - they just fire the element-choice screen when they
        # arrive - so they are capped only by the room left in the pool.
        wanted = (level_ups if self.options.spiritsanity else 0)
        wanted += self.options.extra_level_ups.value
        for _ in range(min(wanted, max(0, open_locations - len(pool)))):
            pool.append(self.create_item("Level Up"))

        # Too many items for the locations in play: shed the ones no rule
        # waits on - filler, then useful (the wings), then Level Ups, which
        # are progression in name only. The gate items and the Souls are
        # what the seed is beaten with, so they are never dropped; if they
        # alone do not fit, the options cannot make a winnable seed.
        def expendable(item: Q64Item) -> int:
            if item.classification == ItemClassification.filler:
                return 0
            if item.classification == ItemClassification.useful:
                return 1
            if item.name == "Level Up":
                return 2
            return 3

        excess = len(pool) - open_locations
        if excess > 0:
            order = sorted(range(len(pool)), key=lambda i: expendable(pool[i]))
            drop = set(i for i in order[:excess] if expendable(pool[i]) < 3)
            if len(drop) < excess:
                needed = sum(1 for item in pool if expendable(item) == 3)
                raise OptionError(
                    f"Quest 64: player {self.player_name} needs {needed} locations for the "
                    f"items the seed is won with, but the options leave only {open_locations}. "
                    f"Switch on more of the *sanity options.")
            pool = [item for i, item in enumerate(pool) if i not in drop]
        while len(pool) < open_locations:
            pool.append(self.create_item(self.get_filler_item_name()))

        self.multiworld.itempool += pool

    def set_rules(self) -> None:
        set_all_rules(self)

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
            # Every location id this slot has. The game's client (APCpp) is
            # not given the server's missing_locations, and it needs to know
            # which chests, spirits and givers are checks in this seed.
            "locations": sorted(loc.address for loc in self.multiworld.get_locations(self.player)
                                if loc.address is not None),
            # The game's own settings from the yaml, and the seed its
            # randomizer rolls them with, so every session of this slot
            # plays the same shuffle.
            "rando_seed": self.random.getrandbits(31),
            # Enemy Randomizer: the file each area uses and which of its
            # monsters must appear there, which the logic above was built
            # on. The game builds its packs from this instead of rolling
            # its own.
            "enemy_plan": self.enemy_plan or {},
            # The monsters the "all monsters" portal counts: every kind met
            # before Mammon's World, since one only found behind the portal
            # could never open it.
            "portal_monsters": sorted(gid for gid, region in self.enemy_region.items()
                                      if region != "Endgame"),
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
                "jp_boss_mp_rewards": self.options.jp_boss_mp_rewards.value,
                "double_exp": self.options.double_exp.value,
                "fast_mp_recovery": self.options.fast_mp_recovery.value,
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
