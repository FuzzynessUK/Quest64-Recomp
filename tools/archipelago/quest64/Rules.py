from worlds.generic.Rules import add_rule, set_rule


# The gates the game actually has: each gem opens the way on, and the book
# and key open the endgame. Regions are chained in Regions.py, so a rule here
# only has to name the one item that region waits on.
region_requires = {
    "Boss 2": "Earth Orb",
    "Boss 3": "Wind Jade",
    "Boss 4": "Water Jewel",
    "Boss 5": "Water Jewel",
    "Mid": "Wind Jade",
    "Boss 6": "Fire Ruby",
    "Boss 7": "Fire Ruby",
    "Late": "Fire Ruby",
    "Endgame": "Eletale's Book",
    "Boss 8": "Dark Gaol Key",
}

# Each boss's own check, in story order, and the Soul that has to arrive
# before that boss is in the game at all.
boss_locations = [
    (1, "Boss - Solvaring", "Solvaring's Soul"),
    (2, "Boss - Zelse", "Zelse's Soul"),
    (3, "Boss - Nepty", "Nepty's Soul"),
    (4, "Boss - Shilf", "Shilf's Soul"),
    (5, "Boss - Fargo", "Fargo's Soul"),
    (6, "Boss - Guilty", "Guilty's Soul"),
    (7, "Boss - Beigis", "Beigis's Soul"),
    (8, "Boss - Mammon", "Mammon's Soul"),
]

# A monster's check needs no item, only the chance to meet one, so the
# regions they live in stand in for them.
monster_regions = ["Overworld", "Early", "Mid", "Late"]


def set_rules(world, player):
    mw = world.multiworld
    for region_name, item in region_requires.items():
        region = mw.get_region(region_name, player)
        for entrance in region.entrances:
            set_rule(entrance, lambda state, i=item: state.has(i, player))

    # A Soul gates the boss's check, not the region he stands in: the way on
    # runs through some of those arenas, and locking the region would wall
    # off everything past it.
    souls = world.options.boss_souls.value
    for order, location, soul in boss_locations:
        if souls >= (2 if order >= 8 else 1):
            set_rule(mw.get_location(location, player),
                     lambda state, s=soul: state.has(s, player))

    # What the portal waits on, over and above the Eletale's Book. "Every
    # boss beaten" is written as every boss's check being reachable, which
    # already accounts for his Soul if Souls are on.
    mode = world.options.mammon_portal.value
    wanted_locations = [loc for order, loc, _ in boss_locations if order < 8] if mode & 1 else []
    wanted_regions = monster_regions if mode & 2 else []
    if wanted_locations or wanted_regions:
        for entrance in mw.get_region("Endgame", player).entrances:
            add_rule(entrance, lambda state, l=tuple(wanted_locations), r=tuple(wanted_regions): (
                all(state.can_reach(name, "Location", player) for name in l)
                and all(state.can_reach(name, "Region", player) for name in r)))

    # Mammon has to be beatable, which with his Soul on means having it.
    mw.completion_condition[player] = lambda state: state.can_reach(
        "Boss - Mammon", "Location", player)
