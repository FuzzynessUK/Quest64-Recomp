from typing import Dict, NamedTuple, List


class Q64RegionData(NamedTuple):
    connecting_regions: List[str] = []


# Deliberately coarse. Quest 64 is close to open once you leave Melrode: the
# gates that matter are the four gems and the book, so a region here is "how
# far along the boss order you have to be", not a room.
region_data_table: Dict[str, Q64RegionData] = {
    "Menu": Q64RegionData(["Overworld"]),
    "Overworld": Q64RegionData(["Boss 1", "Early"]),
    "Boss 1": Q64RegionData(["Boss 2"]),
    "Early": Q64RegionData(["Boss 2"]),
    "Boss 2": Q64RegionData(["Boss 3"]),
    "Boss 3": Q64RegionData(["Mid", "Boss 4"]),
    "Mid": Q64RegionData(["Boss 5"]),
    "Boss 4": Q64RegionData(["Boss 5"]),
    "Boss 5": Q64RegionData(["Late", "Boss 6"]),
    "Late": Q64RegionData(["Boss 7"]),
    "Boss 6": Q64RegionData(["Boss 7"]),
    "Boss 7": Q64RegionData(["Endgame"]),
    "Endgame": Q64RegionData(["Boss 8"]),
    "Boss 8": Q64RegionData([]),
}


def get_exit(region: str) -> List[str]:
    return region_data_table[region].connecting_regions
