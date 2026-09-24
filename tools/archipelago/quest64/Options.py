from dataclasses import dataclass

from Options import Choice, DefaultOnToggle, PerGameCommonOptions, Range, Toggle


class Chestsanity(DefaultOnToggle):
    """Treasure chests are Archipelago checks."""
    display_name = "Chestsanity"


class Giftsanity(DefaultOnToggle):
    """NPCs who hand over an item are Archipelago checks: the gift NPCs, the
    two endgame Shannons and the wingsmiths. Each gives its check once, the
    first time you talk to them, whatever is in your bag - and no item."""
    display_name = "Giftsanity"


class WingsmithWings(Toggle):
    """The six wingsmiths still hand over their wings, on top of their
    Archipelago check. Only matters with giftsanity on; each gives its wings
    once, the first time you talk to them."""
    display_name = "Wingsmiths Give Wings"


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

    The last door of the final staircase, in Baragoon Moor, wants the
    Eletale's Book in the vanilla game. Any other choice replaces the Book:
    the door opens once the condition is met, the Book is left out of the
    pool, and the Shannon who would have handed it over is a gift check of
    her own (even with giftsanity off).

    vanilla       the Book, as the game has it

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

    With Souls on, the doors, boats and teleporters the Earth Orb, Wind
    Jade, Water Jewel and Fire Ruby open are open from the start, since the
    boss who drops one may not be there yet. The Dark Gaol Key still locks
    what it locks, and so does the Eletale's Book unless Mammon's World
    Portal replaces it.

    bosses        the seven before Mammon get Souls
    with_mammon   Mammon as well, so the last fight waits on his Soul too"""
    display_name = "Boss Souls"
    option_off = 0
    option_bosses = 1
    option_with_mammon = 2
    default = 0


class ShuffleSpells(Toggle):
    """Randomizer. Shuffles which spell each element teaches, with the spell
    damage rebalance, Invalidity, and the fixes that keep shuffled spell
    combinations from crashing."""
    display_name = "Shuffled Spells"


class EarlyHealing(Toggle):
    """Randomizer. A healing spell is guaranteed early."""
    display_name = "Early Healing"


class EnemyRandomizer(Toggle):
    """Randomizer. Any monster set can land in any area, and every monster
    is scaled to the area it appears in."""
    display_name = "Enemy Randomizer"


class ShuffleBossOrder(Toggle):
    """Randomizer. The seven bosses before Mammon swap arenas."""
    display_name = "Shuffle Boss Order"


class RandomGuiltyElement(Toggle):
    """Randomizer. Guilty takes a random element instead of his own."""
    display_name = "Randomize Guilty Element"


class FasterAreas(Toggle):
    """Randomizer. Fast Monastery, Fast Blue Cave, Fast Shamwood and Fast
    Mammon's World together: the long empty stretches are skipped."""
    display_name = "Faster Areas"


class WingsNeverExpire(Toggle):
    """Randomizer. Using a pair of wings does not use them up."""
    display_name = "Wings Never Expire"


class NoEnemyDropLimit(Toggle):
    """Randomizer. Monsters keep dropping items past the vanilla limit."""
    display_name = "No Enemy Drop Limit"


class ElementCap99(Toggle):
    """Randomizer. Each element can be raised to 99 instead of 50, by
    spirits, level-ups and Level Up items alike."""
    display_name = "Element Cap 99"


class JPHealing(Toggle):
    """Enhancements. Healing Lv2 restores 16 HP instead of 8, as in the
    Japanese release."""
    display_name = "JP Healing Amounts"


class JPMagicBarrier(Toggle):
    """Enhancements. Magic Barrier holds two turns longer, as in the
    Japanese release."""
    display_name = "JP Magic Barrier"


class JPStatUpEffect(Toggle):
    """Enhancements. A colour burst over Brian when a stat rises, as in the
    Japanese release."""
    display_name = "JP Stat Up Effect"


class ExitFromAnywhere(Toggle):
    """Enhancements. The bound Exit Spell control warps out of an area
    without the spell or the MP."""
    display_name = "Exit from Anywhere"


class FastWalking(Toggle):
    """Enhancements. Brian walks 50% faster."""
    display_name = "Fast Walking"


class TextImprovements(Toggle):
    """Enhancements. Merrow's clearer wording for a number of the game's
    messages."""
    display_name = "Text Improvements"


class TextPalette(Choice):
    """Cosmetics. The colour of the text boxes."""
    display_name = "Text Palette"
    option_off = 0
    option_random = 1
    default = 0


class StaffPalette(Choice):
    """Cosmetics. The colour of Brian's staff."""
    display_name = "Staff Palette"
    option_off = 0
    option_random = 1
    default = 0


class CloakColour(Choice):
    """Cosmetics. The colour of Brian's cloak."""
    display_name = "Cloak Colour"
    option_off = 0
    option_random = 1
    default = 0


class BrianClothes(Choice):
    """Cosmetics. The colours of Brian's clothes."""
    display_name = "Brian's Clothes"
    option_off = 0
    option_random = 1
    default = 0


class SpellPalettes(Choice):
    """Cosmetics. The colours of the spell effects."""
    display_name = "Spell Palettes"
    option_off = 0
    option_random = 1
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
    wingsmith_wings: WingsmithWings
    enemysanity: Enemysanity
    spiritsanity: Spiritsanity
    extra_level_ups: ExtraLevelUps
    shuffle_spells: ShuffleSpells
    early_healing: EarlyHealing
    enemy_randomizer: EnemyRandomizer
    shuffle_boss_order: ShuffleBossOrder
    random_guilty_element: RandomGuiltyElement
    faster_areas: FasterAreas
    wings_never_expire: WingsNeverExpire
    no_enemy_drop_limit: NoEnemyDropLimit
    element_cap_99: ElementCap99
    jp_healing: JPHealing
    jp_magic_barrier: JPMagicBarrier
    jp_stat_up_effect: JPStatUpEffect
    exit_from_anywhere: ExitFromAnywhere
    fast_walking: FastWalking
    text_improvements: TextImprovements
    text_palette: TextPalette
    staff_palette: StaffPalette
    cloak_colour: CloakColour
    brian_clothes: BrianClothes
    spell_palettes: SpellPalettes
