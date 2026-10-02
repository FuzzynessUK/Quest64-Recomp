from dataclasses import dataclass

from Options import Choice, DeathLink, DefaultOnToggle, PerGameCommonOptions, Range, Toggle


class Chestsanity(DefaultOnToggle):
    """Treasure chests are Archipelago checks."""
    display_name = "Chestsanity"


class Giftsanity(DefaultOnToggle):
    """NPCs who hand over an item are Archipelago checks: the gift NPCs, the
    two endgame Shannons and the wingsmiths. Each gives its check once, the
    first time you talk to them, whatever is in your bag - and no item."""
    display_name = "Giftsanity"


class WingsmithWings(DefaultOnToggle):
    """The six wingsmiths still hand over their wings, on top of their
    Archipelago check. Only matters with giftsanity on; each gives its wings
    once, the first time you talk to them."""
    display_name = "Wingsmiths Give Wings"


class WingsInPool(Choice):
    """How many of each pair of wings go in the item pool. The wings are
    handy for getting about but no rule needs them, so fewer leaves room
    for filler instead.

    none          no wings at all (the default); the wingsmiths can still
                  hand theirs over with Wingsmiths Give Wings
    one           one of each
    two           two of each of the six"""
    display_name = "Wings in Pool"
    option_none = 0
    option_one = 1
    option_two = 2
    default = 0


class Enemysanity(Toggle):
    """Defeating a kind of regular monster is an Archipelago check. One check
    per kind, sent the first time you beat one of them and never again -
    there are 67 kinds, so 67 checks, and a lot of hunting."""
    display_name = "Enemysanity"


class EnsureAllEnemies(DefaultOnToggle):
    """Only matters with the Enemy Randomizer on. Every one of the 67 kinds of
    regular monster appears somewhere before Mammon's World: each of the six
    monster sets is given at least one area with room for all of it.

    Off, a set can be left out entirely, or only partly fit the areas it
    lands in. A monster that appears nowhere is simply not a check, and the
    "all monsters" portal only counts the ones that do appear."""
    display_name = "Ensure All Enemies Appear"


class ShuffleOrbs(Toggle):
    """Off keeps the boss rewards on their bosses (the Earth Orb, Wind Jade,
    Water Jewel and Fire Ruby), and the Eletale's Book and Dark Gaol Key with
    the Shannons who give them. On shuffles them with everything else."""
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


class Traps(Choice):
    """How many traps take the place of filler (herbs, potions and the like)
    in your pool, in the style of Ocarina of Time's Ice Traps. The pool does
    not grow: a trap always replaces a filler item, never a gem, the Book,
    the key, a Soul, a pair of wings or a Level Up. Which kinds can appear
    is set by the four trap options below.

    no_traps            none
    normal              about 1 filler item in 10 is a trap
    extra               about 1 in 4
    mayhem              about half
    onslaught           every filler item
    custom_count        exactly trap_count traps (as many as there is filler for)
    custom_percentage   trap_percentage percent of the filler"""
    display_name = "Traps"
    option_no_traps = 0
    option_normal = 1
    option_extra = 2
    option_mayhem = 3
    option_onslaught = 4
    option_custom_count = 5
    option_custom_percentage = 6
    default = 0


class TrapCount(Range):
    """How many traps, when Traps is custom_count."""
    display_name = "Trap Count"
    range_start = 0
    range_end = 300
    default = 10


class TrapPercentage(Range):
    """What percentage of the filler becomes traps, when Traps is
    custom_percentage."""
    display_name = "Trap Percentage"
    range_start = 0
    range_end = 100
    default = 20


class DeathTraps(Choice):
    """Death Trap: Brian falls, the way a DeathLink death makes him, and is
    sent back to his last save point. It waits until a battle is won and its
    rewards are in hand, like DeathLink. The most brutal kind, so it has its
    own amount.

    off      none
    rare     a quarter as likely as each other kind (about 1 trap in 13
             with all four kinds on)
    normal   as likely as each other kind
    custom   exactly death_trap_count of the traps (as many as there are
             traps for); the rest come from the other kinds"""
    display_name = "Death Traps"
    option_off = 0
    option_rare = 1
    option_normal = 2
    option_custom = 3
    default = 1


class DeathTrapCount(Range):
    """How many Death Traps, when Death Traps is custom."""
    display_name = "Death Trap Count"
    range_start = 0
    range_end = 50
    default = 1


class HPTraps(DefaultOnToggle):
    """HP Trap: HP drops by half of max HP. It never kills: it stops at 1."""
    display_name = "HP Traps"


class MPTraps(DefaultOnToggle):
    """MP Trap: MP drops by half of max MP."""
    display_name = "MP Traps"


class IceTraps(DefaultOnToggle):
    """Ice Trap: Brian is frozen in a block of ice for five seconds, in the
    field or in battle. He cannot move, attack, cast, talk or open anything
    until it melts."""
    display_name = "Ice Traps"


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


class ElementCap99(Toggle):
    """Randomizer. Each element can be raised to 99 instead of 50, by
    spirits, level-ups and Level Up items alike."""
    display_name = "Element Cap 99"


class DoubleExperience(Choice):
    """Enhancements. Levels need half the experience. both: level (combat)
    experience and the hidden experience that raises HP, MP, Agility and
    Defense; stat_only: just the hidden stat experience; level_only: just
    the level experience."""
    display_name = "Double Experience"
    option_off = 0
    option_both = 1
    option_stat_only = 2
    option_level_only = 3
    default = 0


class JPHealing(DefaultOnToggle):
    """Enhancements. Healing Lv2 restores 16 HP instead of 8, as in the
    Japanese release."""
    display_name = "JP Healing Amounts"


class JPMagicBarrier(DefaultOnToggle):
    """Enhancements. Magic Barrier holds two turns longer, as in the
    Japanese release."""
    display_name = "JP Magic Barrier"


class JPBossMPRewards(DefaultOnToggle):
    """Enhancements. Beating a boss raises max MP as well as max HP and
    refills both, by the amounts the Japanese release uses (5, 5, 5, 10, 10,
    15, 15)."""
    display_name = "JP Boss MP Rewards"


class FastMPRecovery(DefaultOnToggle):
    """Enhancements. MP comes back as you walk at the fastest rate, as in
    Easy Mode."""
    display_name = "Fast MP Recovery"


class JPStatUpEffect(DefaultOnToggle):
    """Enhancements. A colour burst over Brian when a stat rises, as in the
    Japanese release."""
    display_name = "JP Stat Up Effect"


class ExitFromAnywhere(DefaultOnToggle):
    """Enhancements. The bound Exit Spell control warps out of an area
    without the spell or the MP."""
    display_name = "Exit from Anywhere"


class FastWalking(DefaultOnToggle):
    """Enhancements. Brian walks 50% faster."""
    display_name = "Fast Walking"


class TextImprovements(DefaultOnToggle):
    """Enhancements. Merrow's clearer wording for a number of the game's
    messages."""
    display_name = "Text Improvements"


class FasterAreas(DefaultOnToggle):
    """Enhancements. Fast Monastery, Fast Blue Cave, Fast Shamwood and Fast
    Mammon's World together: the long empty stretches are skipped."""
    display_name = "Faster Areas"


class WingsNeverExpire(DefaultOnToggle):
    """Enhancements. Using a pair of wings does not use them up."""
    display_name = "Wings Never Expire"


class NoEnemyDropLimit(DefaultOnToggle):
    """Enhancements. Monsters keep dropping items past the vanilla limit."""
    display_name = "No Enemy Drop Limit"


class TextPalette(Choice):
    """Cosmetics. The colour of the text boxes."""
    display_name = "Text Palette"
    option_off = 0
    option_randomized = 1
    default = 0


class StaffPalette(Choice):
    """Cosmetics. The colour of Brian's staff."""
    display_name = "Staff Palette"
    option_off = 0
    option_randomized = 1
    default = 0


class CloakColour(Choice):
    """Cosmetics. The colour of Brian's cloak."""
    display_name = "Cloak Colour"
    option_off = 0
    option_randomized = 1
    default = 0


class BrianClothes(Choice):
    """Cosmetics. The colours of Brian's clothes."""
    display_name = "Brian's Clothes"
    option_off = 0
    option_randomized = 1
    default = 0


class SpellPalettes(Choice):
    """Cosmetics. The colours of the spell effects."""
    display_name = "Spell Palettes"
    option_off = 0
    option_randomized = 1
    default = 0


class Goal(Choice):
    """What finishes the run.

    mammon      beat King Mammon
    page_hunt   find pages_required Torn Pages of the Eletale's Book. Anywhere
                from 5 to 100. The
                moment the last one is in your bag the game fades into the
                credits and the goal is done."""
    display_name = "Goal"
    option_mammon = 0
    option_page_hunt = 1
    default = 0


class PagesRequired(Range):
    """How many Torn Pages finish a Page Hunt."""
    display_name = "Pages Required"
    range_start = 5
    range_end = 100
    default = 20


class PagePlacement(Choice):
    """Where the Torn Pages can be.

    quest64_only       only in Quest 64's own locations
    all_games          anywhere in the multiworld
    other_games_only   only in the other games' locations (needs at least
                       one other game in the seed, with room for them)"""
    display_name = "Page Placement"
    option_quest64_only = 0
    option_all_games = 1
    option_other_games_only = 2
    default = 1


@dataclass
class Q64Options(PerGameCommonOptions):
    goal: Goal
    pages_required: PagesRequired
    page_placement: PagePlacement
    mammon_portal: MammonPortal
    boss_souls: BossSouls
    shuffle_orbs: ShuffleOrbs
    chestsanity: Chestsanity
    giftsanity: Giftsanity
    wingsmith_wings: WingsmithWings
    wings_in_pool: WingsInPool
    enemysanity: Enemysanity
    ensure_all_enemies: EnsureAllEnemies
    spiritsanity: Spiritsanity
    extra_level_ups: ExtraLevelUps
    traps: Traps
    trap_count: TrapCount
    trap_percentage: TrapPercentage
    death_traps: DeathTraps
    death_trap_count: DeathTrapCount
    hp_traps: HPTraps
    mp_traps: MPTraps
    ice_traps: IceTraps
    death_link: DeathLink
    shuffle_spells: ShuffleSpells
    early_healing: EarlyHealing
    enemy_randomizer: EnemyRandomizer
    shuffle_boss_order: ShuffleBossOrder
    random_guilty_element: RandomGuiltyElement
    element_cap_99: ElementCap99
    double_exp: DoubleExperience
    jp_healing: JPHealing
    jp_magic_barrier: JPMagicBarrier
    jp_boss_mp_rewards: JPBossMPRewards
    fast_mp_recovery: FastMPRecovery
    jp_stat_up_effect: JPStatUpEffect
    exit_from_anywhere: ExitFromAnywhere
    fast_walking: FastWalking
    text_improvements: TextImprovements
    faster_areas: FasterAreas
    wings_never_expire: WingsNeverExpire
    no_enemy_drop_limit: NoEnemyDropLimit
    text_palette: TextPalette
    staff_palette: StaffPalette
    cloak_colour: CloakColour
    brian_clothes: BrianClothes
    spell_palettes: SpellPalettes
