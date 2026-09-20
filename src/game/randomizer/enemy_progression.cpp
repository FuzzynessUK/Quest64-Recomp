#include <algorithm>
#include <cmath>
#include <cstdio>

#include "enemy_progression.h"
#include "enemy_progression_data.h"

namespace data = merrow::progression;
using zelda64::randomizer::Options;
using zelda64::randomizer::progression::Factors;
using zelda64::randomizer::progression::Plan;
using zelda64::randomizer::progression::Stat;

namespace {
    Plan active_plan;

    constexpr int tier_count = 8;

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

    // Sheet: Settings B4-B6. Any set may serve any area; scaling is full.
    constexpr int spread_down = 7;
    constexpr int spread_up = 7;
    constexpr double scaling_k = 1.0;

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
}

std::vector<int> zelda64::randomizer::progression::candidate_tables(int area, const Options& options) {
    std::vector<int> out;
    const data::AreaInfo& here = data::areas[static_cast<size_t>(area)];
    (void)options;
    int low = std::max(1, here.tier - spread_down);
    int high = std::min(tier_count, here.tier + spread_up);

    for (int table = 0; table < static_cast<int>(data::table_monsters.size()); table++) {
        std::vector<int> tiers = file_tiers(table);
        if (tiers.empty()) continue;
        bool in_window = false;
        for (int id : data::table_monsters[static_cast<size_t>(table)]) {
            if (id < 0) continue;
            const data::MonsterInfo& m = data::monsters[static_cast<size_t>(id)];
            int native = m.home_area >= 0 ? data::areas[static_cast<size_t>(m.home_area)].tier : here.tier;
            if (native >= low && native <= high) in_window = true;
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
    Plan plan;
    plan.enabled = true;
    (void)options;
    double k = scaling_k;
    std::vector<double> guard = guards();

    char line[256];
    std::snprintf(line, sizeof(line), "  Enemy Randomizer: any set in any area, stats and damage scaled to the area (k = %.1f).\n", k);
    spoiler += line;

    for (int a = 0; a < area_count(); a++) {
        const data::AreaInfo& here = data::areas[static_cast<size_t>(a)];
        int table = table_per_area[static_cast<size_t>(a)];
        for (int raw = here.first_raw; raw <= here.last_raw; raw++) {
            plan.table_index[static_cast<size_t>(raw)] = table;
        }

        std::vector<Factors>& factors = plan.by_map[static_cast<size_t>(here.map_id)];
        factors.clear();
        std::snprintf(line, sizeof(line), "  %s (tier %d, file %d, guard x%.2f):\n", here.name, here.tier, table, guard[static_cast<size_t>(a)]);
        spoiler += line;

        for (int id : data::table_monsters[static_cast<size_t>(table)]) {
            Factors f;
            if (id >= 0 && data::monsters[static_cast<size_t>(id)].home_area >= 0) {
                const data::AreaInfo& home = data::areas[static_cast<size_t>(data::monsters[static_cast<size_t>(id)].home_area)];
                // Sheet rule 5: (dest / home) ^ k per stat, EXP with k = 1,
                // then rule 6's guard on everything.
                auto ratio = [&](int i, double exponent) {
                    double r = home.avg[i] > 0.0 ? here.avg[i] / home.avg[i] : 1.0;
                    return std::pow(r, exponent) * guard[static_cast<size_t>(a)];
                };
                f.hp = ratio(0, k);
                f.atk = ratio(1, k);
                f.def = ratio(2, k);
                f.agi = ratio(3, k);
                f.exp = ratio(4, 1.0);
                f.dmg = std::sqrt(f.hp * f.atk);
                std::snprintf(line, sizeof(line), "    %-18s HP x%.2f ATK x%.2f DEF x%.2f AGI x%.2f EXP x%.2f DMG x%.2f\n",
                    data::monsters[static_cast<size_t>(id)].name, f.hp, f.atk, f.def, f.agi, f.exp, f.dmg);
                spoiler += line;
            }
            factors.push_back(f);
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

double zelda64::randomizer::progression::factor(int map_id, int entry, Stat stat) {
    if (!active_plan.enabled || map_id < 0 || map_id >= static_cast<int>(active_plan.by_map.size())) {
        return 1.0;
    }
    const std::vector<Factors>& factors = active_plan.by_map[static_cast<size_t>(map_id)];
    if (entry < 0 || entry >= static_cast<int>(factors.size())) {
        return 1.0;
    }
    const Factors& f = factors[static_cast<size_t>(entry)];
    switch (stat) {
        case Stat::HP: return f.hp;
        case Stat::ATK: return f.atk;
        case Stat::DEF: return f.def;
        case Stat::AGI: return f.agi;
        case Stat::EXP: return f.exp;
        case Stat::DMG: return f.dmg;
    }
    return 1.0;
}
