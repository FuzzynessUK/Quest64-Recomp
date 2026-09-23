from dataclasses import dataclass

from Options import Choice, DefaultOnToggle, PerGameCommonOptions, Range, Toggle


class Chestsanity(DefaultOnToggle):
    """Treasure chests are Archipelago checks."""
    display_name = "Chestsanity"


class Giftsanity(DefaultOnToggle):
    """NPCs who hand over an item are Archipelago checks: the gift NPCs, the
    two endgame Shannons and the wingsmiths."""
    display_name = "Giftsanity"


class Enemysanity(Toggle):
    """Defeating a kind of regular monster is an Archipelago check. One check
    per kind, sent the first time you beat one of them and never again -
    there are 67 kinds, so 67 checks, and a lot of hunting."""
    display_name = "Enemysanity"


class ShuffleOrbs(DefaultOnToggle):
    """Shuffle the items that open the way on: the Earth Orb, Wind Jade,
    Water Jewel and Fire Ruby, the Eletale's Book and the Dark Gaol Key.

    Turn this off to leave all six where the game puts them - the four gems
    on their bosses, the book and the key with the two Shannons - so the run
    follows the usual route and only everything else moves."""
    display_name = "Shuffle Orbs"


class Spiritsanity(DefaultOnToggle):
    """Spirits are Archipelago checks. With this on a spirit no longer raises
    an element itself - it sends a check, and the element-choice screen comes
    back as the "Level Up" item, from wherever Archipelago put it."""
    display_name = "Spiritsanity"


class ExtraLevelUps(Range):
    """Extra "Level Up" items in the pool, on top of the one per spirit.

    These have no spirit behind them - nothing is added to the game world.
    They simply arrive like any other item and bring up the element-choice
    screen, so a run can hand out more element levels than Quest 64 has
    spirits for. They take the place of filler, so the pool does not grow."""
    display_name = "Extra Level Ups"
    range_start = 0
    range_end = 99
    default = 0


class MammonPortal(Choice):
    """What it takes to open the way into Mammon's World.

    The last door of the final staircase, in Baragoon Moor, always wants the
    Eletale's Book. This asks for something on top of that:

    all_bosses    every boss before Mammon - Solvaring, Zelse, Nepty, Shilf,
                  Fargo, Guilty and Beigis - has to be beaten. They are
                  checks whatever else is switched on, so this costs nothing
                  extra to track.
    all_monsters  one of each of the 67 kinds of regular monster has to be
                  beaten. This needs enemysanity, because the count is kept
                  by the server: a monster leaves nothing behind in the save
                  to read back after a reload. Turning this on turns
                  enemysanity on with it.
    both          all of the above."""
    display_name = "Mammon's World Portal"
    option_vanilla = 0
    option_all_bosses = 1
    option_all_monsters = 2
    option_both = 3
    default = 0


class BossSouls(Choice):
    """Bosses are not in the game until their Soul turns up.

    A boss whose Soul has not arrived is not loaded and not placed: his
    arena is empty and you walk straight through it. It is the same state
    the game itself uses for a boss you have already beaten, so nothing is
    left half-there to bump into. The Soul is an Archipelago item, not
    something the game knows about, so it can be anywhere in the multiworld.

    Whether a boss is there is settled as his arena loads, so a Soul that
    arrives while you are standing in the arena takes effect when you next
    walk in.

    bosses        the seven before Mammon get Souls
    with_mammon   Mammon as well, so the last fight waits on his Soul too"""
    display_name = "Boss Souls"
    option_off = 0
    option_bosses = 1
    option_with_mammon = 2
    default = 0


class Goal(Choice):
    """What finishes the run."""
    display_name = "Goal"
    option_mammon = 0
    default = 0


@dataclass
class Q64Options(PerGameCommonOptions):
    goal: Goal
    mammon_portal: MammonPortal
    boss_souls: BossSouls
    shuffle_orbs: ShuffleOrbs
    chestsanity: Chestsanity
    giftsanity: Giftsanity
    enemysanity: Enemysanity
    spiritsanity: Spiritsanity
    extra_level_ups: ExtraLevelUps
