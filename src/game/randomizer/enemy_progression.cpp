#include <algorithm>
#include <cmath>
#include <cstdio>

#include "enemy_progression.h"
#include "enemy_progression_data.h"

namespace data = merrow::progression;
using zelda64::randomizer::Options;
using zelda64::randomizer::progression::Plan;
using zelda64::randomizer::progression::Slot;
using zelda64::randomizer::progression::Stat;

namespace {
    Plan active_plan;

    constexpr int tier_count = 8;

    // Sheet: Settings B4-B6. Any set may serve any area; a monster's
    // deviation from its home average is compressed by the shape exponent.
    constexpr int spread_down = 7;
    constexpr int spread_up = 7;
    constexpr double shape_exponent = 0.5;

    // Sheet: Settings B21-B23. EXP is a word in the game; capped generously.
    constexpr int caps[5] = { 999, 255, 255, 255, 65535 };

    int area_count() {
        return static_cast<int>(data::areas.size());
    }

    // The tiers the monsters of a file come from.
    std::vector<int> file_tiers(int table) {
        std::vector<int> tiers;
        for (int id : data::table_monsters[static_cast<size_t>(table)]) {
            if (id < 0) continue;
            int home = data::monsters[static_cast<size_t>(id)].home_area;
            if (home >= 0) tiers.push_back(data::areas[static_cast<size_t>(home)].tier);
        }
        return tiers;
    }

    // Sheet: Areas columns M-N. budget[i] = MAX(avg_power[i], budget[i-1]).
    std::vector<double> guards() {
        std::vector<double> out;
        double budget = 0.0;
        for (const data::AreaInfo& area : data::areas) {
            budget = std::max(budget, area.avg_power);
            out.push_back(area.avg_power > 0.0 ? budget / area.avg_power : 1.0);
        }
        return out;
    }

    // Sheet rule 5 with the shape exponent, then rule 6's guard.
    int32_t rescale(int home, int dest, int stat, int32_t own) {
        static const std::vector<double> guard = guards();
        const data::AreaInfo& h = data::areas[static_cast<size_t>(home)];
        const data::AreaInfo& d = data::areas[static_cast<size_t>(dest)];
        if (h.avg[stat] <= 0.0 || own <= 0) {
            return own;
        }
        double shape = std::pow(static_cast<double>(own) / h.avg[stat], shape_exponent);
        double value = std::nearbyint(d.avg[stat] * shape * guard[static_cast<size_t>(dest)]);
        return static_cast<int32_t>(std::min<double>(std::max<double>(value, 1.0), caps[stat]));
    }
}

std::vector<int> zelda64::randomizer::progression::candidate_tables(int area, const Options& options) {
    (void)options;
    std::vector<int> out;
    const data::AreaInfo& here = data::areas[static_cast<size_t>(area)];
    int low = std::max(1, here.tier - spread_down);
    int high = std::min(tier_count, here.tier + spread_up);

    for (int table = 0; table < static_cast<int>(data::table_monsters.size()); table++) {
        bool in_window = false;
        for (int tier : file_tiers(table)) {
            if (tier >= low && tier <= high) in_window = true;
        }
        // No other gate: any set may serve any tier (the scaling fits it),
        // so the dangerous and flying flags in the data are informational.
        if (in_window) out.push_back(table);
    }
    if (out.empty()) {
        out.push_back(here.vanilla_table);
    }
    return out;
}

Plan zelda64::randomizer::progression::make_plan(const std::vector<int>& table_per_area, const Options& options, std::string& spoiler) {
    (void)options;
    Plan plan;
    plan.enabled = true;
    std::vector<double> guard = guards();

    char line[256];
    std::snprintf(line, sizeof(line), "  Enemy Randomizer: any set in any area, stats scaled to the area (shape %.1f). Vanilla -> scaled HP/ATK/DEF/AGI/EXP:\n", shape_exponent);
    spoiler += line;

    for (int a = 0; a < area_count(); a++) {
        const data::AreaInfo& here = data::areas[static_cast<size_t>(a)];
        int table = table_per_area[static_cast<size_t>(a)];
        for (int raw = here.first_raw; raw <= here.last_raw; raw++) {
            plan.table_index[static_cast<size_t>(raw)] = table;
        }

        std::vector<Slot>& slots = plan.by_map[static_cast<size_t>(here.map_id)];
        slots.clear();
        std::snprintf(line, sizeof(line), "  %s (tier %d, file %d, guard x%.2f):\n", here.name, here.tier, table, guard[static_cast<size_t>(a)]);
        spoiler += line;

        for (int id : data::table_monsters[static_cast<size_t>(table)]) {
            Slot slot;
            if (id >= 0 && data::monsters[static_cast<size_t>(id)].home_area >= 0) {
                const data::MonsterInfo& m = data::monsters[static_cast<size_t>(id)];
                slot.home = m.home_area;
                slot.dest = a;
                std::snprintf(line, sizeof(line), "    %-18s %d/%d/%d/%d/%d -> %d/%d/%d/%d/%d\n", m.name,
                    m.stat[0], m.stat[1], m.stat[2], m.stat[3], m.stat[4],
                    rescale(slot.home, a, 0, m.stat[0]), rescale(slot.home, a, 1, m.stat[1]),
                    rescale(slot.home, a, 2, m.stat[2]), rescale(slot.home, a, 3, m.stat[3]),
                    rescale(slot.home, a, 4, m.stat[4]));
                spoiler += line;
            }
            slots.push_back(slot);
        }
    }
    return plan;
}

void zelda64::randomizer::progression::set_active(const Plan& plan) {
    active_plan = plan;
}

bool zelda64::randomizer::progression::active() {
    return active_plan.enabled;
}

int32_t zelda64::randomizer::progression::scaled_value(int map_id, int entry, Stat stat, int32_t own) {
    if (!active_plan.enabled || map_id < 0 || map_id >= static_cast<int>(active_plan.by_map.size())) {
        return own;
    }
    const std::vector<Slot>& slots = active_plan.by_map[static_cast<size_t>(map_id)];
    if (entry < 0 || entry >= static_cast<int>(slots.size()) || slots[static_cast<size_t>(entry)].home < 0) {
        return own;
    }
    const Slot& slot = slots[static_cast<size_t>(entry)];
    return rescale(slot.home, slot.dest, static_cast<int>(stat), own);
}
