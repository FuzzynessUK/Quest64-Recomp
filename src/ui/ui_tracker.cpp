#include <algorithm>
#include <string>
#include <vector>

#ifdef _WIN32
#include <SDL_mouse.h>
#else
#include <SDL2/SDL_mouse.h>
#endif
#include "recomp_ui.h"
#include "tracker.h"
#include "zelda_config.h"
#include "zelda_support.h"
#include "core/ui_context.h"
#include "ultramodern/ultramodern.hpp"
#include "RmlUi/Core.h"

// The tracker's two overlay windows (include/tracker.h, assets/tracker.rml),
// and the same check list again inside the Tracker tab. The windows have
// their own context, above the game: draw-only while locked; unlocked it
// takes the mouse so a window can be dragged by its title and an area opened
// or closed with a click. Everything is built once, then only classes and
// text change, and only when the game side's snapshot does. The list's look
// is assets/tracker_list.rcss, which both documents link.
namespace {
    recompui::ContextId tracker_context = recompui::ContextId::null();

    struct ItemInfo { int id; const char* name; const char* color; };
    const ItemInfo boss_items[] = {
        { 20, "Earth Orb", "#c8904c" }, { 21, "Wind Jade", "#5ad07a" },
        { 22, "Water Jewel", "#4a8cff" }, { 23, "Fire Ruby", "#ff4a3c" },
        { 24, "Eletale Book", "#b07cff" }, { 25, "Dark Gaol Key", "#a8aeb8" },
    };
    const ItemInfo wing_items[] = {
        { 14, "White", "#f4f4f4" }, { 15, "Yellow", "#ffe24a" },
        { 16, "Blue", "#4a8cff" }, { 17, "Green", "#5ad07a" },
        { 18, "Red", "#ff4a3c" }, { 19, "Black", "#2a2a30" },
    };

    zelda64::tracker::Snapshot shown;
    bool have_snapshot = false;

    std::string escape(const std::string& s) {
        std::string out;
        for (char c : s) {
            switch (c) {
                case '&': out += "&amp;"; break;
                case '<': out += "&lt;"; break;
                case '>': out += "&gt;"; break;
                default: out += c;
            }
        }
        return out;
    }

    void set_display(Rml::Element* e, bool on) {
        e->SetProperty("display", on ? "block" : "none");
    }

    // ---- the check list, by area
    class CheckList : public Rml::EventListener {
    public:
        // clickable: whether an area opens on a click right now (the overlay
        // only while unlocked; the menu always).
        bool (*clickable)() = nullptr;
        bool built = false;
        bool dirty = true;
        int found_total = 0, present_total = 0;

        void build(Rml::ElementDocument* document, Rml::Element* container) {
            const size_t areas = zelda64::tracker::area_names.size();
            open.assign(areas, -1);
            shown_open.assign(areas, 0);
            headers.assign(areas, nullptr);
            bodies.assign(areas, nullptr);
            for (size_t a = 0; a < areas; a++) {
                Rml::ElementPtr header = document->CreateElement("div");
                header->SetClass("trk-area", true);
                header->AddEventListener(Rml::EventId::Click, this);
                headers[a] = container->AppendChild(std::move(header));
                Rml::ElementPtr body = document->CreateElement("div");
                body->SetClass("trk-area-body", true);
                bodies[a] = container->AppendChild(std::move(body));
            }
            rows.assign(zelda64::tracker::checks.size(), nullptr);
            for (size_t i = 0; i < zelda64::tracker::checks.size(); i++) {
                const zelda64::tracker::CheckInfo& c = zelda64::tracker::checks[i];
                Rml::ElementPtr row = document->CreateElement("div");
                row->SetClass("trk-check", true);
                row->SetInnerRML(escape(c.label));
                rows[i] = bodies[static_cast<size_t>(c.area)]->AppendChild(std::move(row));
            }
            built = true;
            dirty = true;
        }

        // Before any snapshot, everything is listed and nothing found.
        void refresh(const zelda64::tracker::Snapshot& s, bool snapshot_valid, bool hide_done) {
            dirty = false;
            const size_t areas = zelda64::tracker::area_names.size();
            std::vector<int> present(areas, 0), found(areas, 0);
            found_total = present_total = 0;
            for (size_t i = 0; i < rows.size(); i++) {
                const zelda64::tracker::CheckInfo& c = zelda64::tracker::checks[i];
                bool here = !snapshot_valid || (i < s.present.size() && s.present[i]);
                bool got = snapshot_valid && i < s.found.size() && s.found[i];
                set_display(rows[i], here);
                rows[i]->SetClass("trk-check--missing", !got);
                if (!here) {
                    continue;
                }
                present[static_cast<size_t>(c.area)]++;
                present_total++;
                if (got) {
                    found[static_cast<size_t>(c.area)]++;
                    found_total++;
                }
            }
            int current_area = snapshot_valid ? s.area : -1;
            for (size_t a = 0; a < areas; a++) {
                bool done = present[a] > 0 && found[a] == present[a];
                bool current = static_cast<int>(a) == current_area;
                bool visible = present[a] > 0 && !(hide_done && done && !current);
                set_display(headers[a], visible);
                headers[a]->SetClass("trk-area--current", current);
                headers[a]->SetClass("trk-area--done", done && !current);
                headers[a]->SetInnerRML("<span class=\"trk-count\">" + std::to_string(found[a]) + " / " +
                    std::to_string(present[a]) + "</span>" + escape(zelda64::tracker::area_names[a]));
                bool is_open = open[a] == 1 || (open[a] == -1 && current);
                shown_open[a] = is_open ? 1 : 0;
                set_display(bodies[a], visible && is_open);
            }
        }

        void ProcessEvent(Rml::Event& event) override {
            if (clickable != nullptr && !clickable()) {
                return;
            }
            Rml::Element* header = event.GetCurrentElement();
            for (size_t a = 0; a < headers.size(); a++) {
                if (headers[a] == header) {
                    open[a] = shown_open[a] ? 0 : 1;
                    dirty = true;
                }
            }
        }

    private:
        std::vector<Rml::Element*> headers, bodies, rows;
        // Per area: -1 follows where Brian is, 0 closed, 1 open.
        std::vector<int> open;
        std::vector<uint8_t> shown_open;
    };

    // ---- the overlay windows
    bool overlay_built = false;
    bool overlay_dirty = true;
    zelda64::tracker::Options applied;
    bool applied_set = false;
    Rml::Element* item_window = nullptr;
    Rml::Element* check_window = nullptr;
    Rml::Element* check_title = nullptr;
    Rml::Element* areas_scroll = nullptr;
    std::vector<Rml::Element*> item_chips;   // boss items, then wings
    CheckList overlay_list;
    Rml::Element* wings_heading = nullptr;
    Rml::Element* wings_grid = nullptr;
    Rml::Element* notes_window = nullptr;
    Rml::Element* notes_text = nullptr;
    Rml::Element* check_resize = nullptr;   // the check list's corner, only while unlocked
    Rml::Element* notes_resize = nullptr;   // and the notes', likewise
    // Whether the note has the keyboard, which the game then does not get.
    bool notes_focused = false;

    // ---- the list in the Tracker tab
    CheckList menu_list;
    Rml::ElementDocument* menu_document = nullptr;
    Rml::Element* menu_title = nullptr;

    // ---- dragging a window by its title
    struct Drag {
        Rml::Element* window = nullptr;
        float mouse[2] = {};
        float origin[2] = {};
    } drag;

    Rml::Vector2f window_size() {
        Rml::ElementDocument* document = tracker_context.get_document();
        Rml::Vector2i size = document ? document->GetContext()->GetDimensions() : Rml::Vector2i(1, 1);
        return { static_cast<float>(std::max(size.x, 1)), static_cast<float>(std::max(size.y, 1)) };
    }

    void place(Rml::Element* window, float x, float y) {
        Rml::Vector2f size = window_size();
        Rml::Vector2f box = window->GetBox().GetSize(Rml::BoxArea::Border);
        float left = std::clamp(x * size.x, 0.0f, std::max(size.x - box.x, 0.0f));
        float top = std::clamp(y * size.y, 0.0f, std::max(size.y - std::min(box.y, size.y * 0.5f), 0.0f));
        window->SetProperty(Rml::PropertyId::Left, Rml::Property(left, Rml::Unit::PX));
        window->SetProperty(Rml::PropertyId::Top, Rml::Property(top, Rml::Unit::PX));
    }

    class DragListener : public Rml::EventListener {
        void ProcessEvent(Rml::Event& event) override {
            Rml::Element* window = event.GetCurrentElement()->GetParentNode();
            if (window == nullptr || zelda64::tracker::options().locked) {
                return;
            }
            float mx = event.GetParameter("mouse_x", 0.0f);
            float my = event.GetParameter("mouse_y", 0.0f);
            Rml::Vector2f at = window->GetAbsoluteOffset(Rml::BoxArea::Border);
            if (event.GetId() == Rml::EventId::Dragstart) {
                drag.window = window;
                drag.mouse[0] = mx;
                drag.mouse[1] = my;
                drag.origin[0] = at.x;
                drag.origin[1] = at.y;
                return;
            }
            if (drag.window != window) {
                return;
            }
            Rml::Vector2f size = window_size();
            place(window, (drag.origin[0] + mx - drag.mouse[0]) / size.x, (drag.origin[1] + my - drag.mouse[1]) / size.y);
            if (event.GetId() == Rml::EventId::Dragend) {
                drag.window = nullptr;
                // Where place() settled, so a window pushed against an edge is
                // saved where it shows.
                Rml::Vector2f now = window->GetAbsoluteOffset(Rml::BoxArea::Border);
                zelda64::tracker::Options o = zelda64::tracker::options();
                if (window == item_window) {
                    o.item_x = now.x / size.x;
                    o.item_y = now.y / size.y;
                }
                else if (window == notes_window) {
                    o.notes_x = now.x / size.x;
                    o.notes_y = now.y / size.y;
                }
                else {
                    o.check_x = now.x / size.x;
                    o.check_y = now.y / size.y;
                }
                zelda64::tracker::set_options(o);
                applied = o;
            }
        }
    };
    DragListener drag_listener;

    // The notes' corner: dragged, it sizes the window. Works locked or not.
    // Sizes are kept in dp, so they follow the window's own scale.
    class ResizeListener : public Rml::EventListener {
        float mouse[2] = {};
        float origin[2] = {};
        // The option each axis sizes, in dp; null for an axis that stays put.
        float zelda64::tracker::Options::* width_field;
        float zelda64::tracker::Options::* height_field;
        void ProcessEvent(Rml::Event& event) override {
            Rml::ElementDocument* document = tracker_context.get_document();
            float dp = document ? document->GetContext()->GetDensityIndependentPixelRatio() : 1.0f;
            float mx = event.GetParameter("mouse_x", 0.0f);
            float my = event.GetParameter("mouse_y", 0.0f);
            zelda64::tracker::Options o = zelda64::tracker::options();
            // Resizing only while the windows are unlocked.
            if (o.locked) {
                active = false;
                return;
            }
            if (event.GetId() == Rml::EventId::Dragstart) {
                mouse[0] = mx;
                mouse[1] = my;
                origin[0] = width_field ? o.*width_field : 0.0f;
                origin[1] = height_field ? o.*height_field : 0.0f;
                w = origin[0];
                h = origin[1];
                active = true;
                return;
            }
            if (!active) {
                return;
            }
            if (width_field) {
                w = std::clamp(origin[0] + (mx - mouse[0]) / dp, 140.0f, 900.0f);
            }
            if (height_field) {
                h = std::clamp(origin[1] + (my - mouse[1]) / dp, 80.0f, 1000.0f);
            }
            if (event.GetId() == Rml::EventId::Dragend) {
                active = false;
                if (width_field) o.*width_field = w;
                if (height_field) o.*height_field = h;
                zelda64::tracker::set_options(o);   // written to disk once, at the end
            }
        }
    public:
        ResizeListener(float zelda64::tracker::Options::* width, float zelda64::tracker::Options::* height)
            : width_field(width), height_field(height) {}
        // While the corner is being dragged: the size shown, not yet saved.
        bool active = false;
        float w = 0.0f, h = 0.0f;
    };
    // The notes resize both ways; the check list only in height, since it
    // scrolls.
    ResizeListener resize_listener(&zelda64::tracker::Options::notes_w, &zelda64::tracker::Options::notes_h);
    ResizeListener check_resize_listener(nullptr, &zelda64::tracker::Options::check_h);

    // The note itself: saved as it is typed; while it has focus the tracker
    // takes the keyboard, so typing does not also move Brian.
    // Whether a click, and not the controller, focused the note: the left
    // button is down right now, over the note. RmlUi focuses on the press,
    // before the press event itself, so this is asked rather than a flag set.
    // Being over it is not enough: the UI remembers what the mouse last
    // hovered and focuses that again on the next controller press.
    bool mouse_over_note() {
        if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) == 0) {
            return false;
        }
        Rml::ElementDocument* document = tracker_context.get_document();
        Rml::Element* hover = document ? document->GetContext()->GetHoverElement() : nullptr;
        for (; hover != nullptr; hover = hover->GetParentNode()) {
            if (hover == notes_text) {
                return true;
            }
        }
        return false;
    }

    class NotesListener : public Rml::EventListener {
        void ProcessEvent(Rml::Event& event) override {
            switch (event.GetId()) {
                case Rml::EventId::Change: {
                    auto* field = dynamic_cast<Rml::ElementFormControl*>(notes_text);
                    if (field != nullptr) {
                        zelda64::tracker::save_notes(field->GetValue());
                    }
                    break;
                }
                case Rml::EventId::Focus:
                    // Only a click puts the note in charge of the keyboard.
                    // With the tracker holding the mouse, a controller press
                    // makes the UI focus the first thing it can here - the
                    // note - which would take the game's controls away.
                    if (!mouse_over_note()) {
                        notes_text->Blur();
                        break;
                    }
                    notes_focused = true;
                    tracker_context.set_captures_input(true);
                    break;
                case Rml::EventId::Blur:
                    notes_focused = false;
                    tracker_context.set_captures_input(false);
                    break;
                default:
                    break;
            }
        }
    };
    NotesListener notes_listener;

    // The check list: pressed and dragged, it scrolls with the pointer, like
    // dragging a page. Locked or not, like the wheel. A click without a drag
    // still reaches the area it was on.
    class ScrollDragListener : public Rml::EventListener {
        float start_mouse = 0.0f;
        float start_scroll = 0.0f;
        void ProcessEvent(Rml::Event& event) override {
            Rml::Element* list = event.GetCurrentElement();
            float my = event.GetParameter("mouse_y", 0.0f);
            if (event.GetId() == Rml::EventId::Dragstart) {
                start_mouse = my;
                start_scroll = list->GetScrollTop();
                return;
            }
            list->SetScrollTop(start_scroll - (my - start_mouse));
        }
    };
    ScrollDragListener scroll_drag_listener;

    void build_overlay(Rml::ElementDocument* document) {
        item_window = document->GetElementById("item_tracker");
        check_window = document->GetElementById("check_tracker");
        check_title = document->GetElementById("trk_check_title");
        areas_scroll = document->GetElementById("trk_areas");
        Rml::Element* boss_grid = document->GetElementById("trk_boss_items");
        Rml::Element* wing_grid = document->GetElementById("trk_wings");
        wings_grid = wing_grid;
        wings_heading = document->GetElementById("trk_wings_heading");
        notes_window = document->GetElementById("notes_window");
        notes_text = document->GetElementById("trk_notes_text");
        notes_resize = document->GetElementById("trk_notes_resize");
        check_resize = document->GetElementById("trk_check_resize");
        if (!item_window || !check_window || !check_title || !areas_scroll || !boss_grid || !wing_grid ||
            !wings_heading || !notes_window || !notes_text || !notes_resize || !check_resize) {
            return;
        }
        check_resize->AddEventListener(Rml::EventId::Dragstart, &check_resize_listener);
        check_resize->AddEventListener(Rml::EventId::Drag, &check_resize_listener);
        check_resize->AddEventListener(Rml::EventId::Dragend, &check_resize_listener);
        notes_resize->AddEventListener(Rml::EventId::Dragstart, &resize_listener);
        notes_resize->AddEventListener(Rml::EventId::Drag, &resize_listener);
        notes_resize->AddEventListener(Rml::EventId::Dragend, &resize_listener);
        notes_text->AddEventListener(Rml::EventId::Change, &notes_listener);
        notes_text->AddEventListener(Rml::EventId::Focus, &notes_listener);
        notes_text->AddEventListener(Rml::EventId::Blur, &notes_listener);
        if (auto* field = dynamic_cast<Rml::ElementFormControl*>(notes_text)) {
            field->SetValue(zelda64::tracker::load_notes());
        }
        for (const char* id : { "trk_item_title", "trk_check_title", "trk_notes_title" }) {
            Rml::Element* title = document->GetElementById(id);
            if (title == nullptr) {
                continue;
            }
            title->AddEventListener(Rml::EventId::Dragstart, &drag_listener);
            title->AddEventListener(Rml::EventId::Drag, &drag_listener);
            title->AddEventListener(Rml::EventId::Dragend, &drag_listener);
        }
        auto add_chips = [&](Rml::Element* grid, size_t n) {
            for (size_t i = 0; i < n; i++) {
                Rml::ElementPtr chip = document->CreateElement("div");
                chip->SetClass("trk-item", true);
                item_chips.push_back(grid->AppendChild(std::move(chip)));
            }
        };
        add_chips(boss_grid, std::size(boss_items));
        add_chips(wing_grid, std::size(wing_items));
        // Areas open and close on a click whether the windows are locked or not.
        overlay_list.clickable = nullptr;
        overlay_list.build(document, areas_scroll);
        areas_scroll->AddEventListener(Rml::EventId::Dragstart, &scroll_drag_listener);
        areas_scroll->AddEventListener(Rml::EventId::Drag, &scroll_drag_listener);
        overlay_built = true;
    }

    void refresh_overlay(const zelda64::tracker::Options& o) {
        overlay_dirty = false;
        auto chip = [&](Rml::Element* e, const ItemInfo& item) {
            int n = item.id < 32 ? shown.item_counts[item.id] : 0;
            e->SetClass("trk-item--missing", n == 0);
            std::string rml = std::string("<span class=\"trk-dot\" style=\"background-color: ") + item.color +
                ";\"></span>" + escape(item.name);
            if (n > 1) {
                rml += " x" + std::to_string(n);
            }
            e->SetInnerRML(rml);
        };
        size_t k = 0;
        for (const ItemInfo& item : boss_items) chip(item_chips[k++], item);
        for (const ItemInfo& item : wing_items) chip(item_chips[k++], item);

        overlay_list.refresh(shown, have_snapshot, o.hide_done_areas);
        check_title->SetInnerRML("Checks " + std::to_string(overlay_list.found_total) + " / " +
                                 std::to_string(overlay_list.present_total));
    }

    void style_windows(const zelda64::tracker::Options& o) {
        const std::pair<Rml::Element*, int> windows[] = {
            { item_window, o.item_background }, { check_window, o.check_background }, { notes_window, o.notes_background },
        };
        for (const auto& [w, background] : windows) {
            w->SetClass("trk-window--unlocked", !o.locked);
            w->SetClass("trk-window--solid", background == 0);
            w->SetClass("trk-window--clear", background == 2);
            w->SetClass("trk-window--borderless", o.hide_borders);
            w->SetClass("trk-window--notitle", o.locked && o.hide_titles);
        }
        // Windows are only resized while unlocked; locked, the note's text
        // is the one thing that still takes a click.
        set_display(check_resize, !o.locked);
        set_display(notes_resize, !o.locked);
        set_display(wings_heading, o.show_wings);
        set_display(wings_grid, o.show_wings);
    }

    // The Tracker tab's list, whenever the config menu is up. Its container
    // is in the tab's template (assets/config_menu/tracker.rml).
    void update_menu_list(bool snapshot_changed) {
        recompui::ContextId config = recompui::get_config_context_id();
        if (!recompui::is_context_shown(config)) {
            return;
        }
        Rml::ElementDocument* document = config.get_document();
        if (document == nullptr) {
            return;
        }
        if (document != menu_document) {
            Rml::Element* container = document->GetElementById("trk_menu_checks");
            menu_title = document->GetElementById("trk_menu_title");
            if (container == nullptr || menu_title == nullptr) {
                return;
            }
            menu_document = document;
            menu_list = CheckList();
            menu_list.build(document, container);
        }
        if (snapshot_changed) {
            menu_list.dirty = true;
        }
        static bool shown_hide_done = false;
        bool hide_done = zelda64::tracker::options().hide_done_areas;
        if (hide_done != shown_hide_done) {
            shown_hide_done = hide_done;
            menu_list.dirty = true;
        }
        if (menu_list.dirty) {
            menu_list.refresh(shown, have_snapshot, hide_done);
            menu_title->SetInnerRML("Checks " + std::to_string(menu_list.found_total) + " / " +
                                    std::to_string(menu_list.present_total));
        }
    }
}

void recompui::init_tracker() {
    tracker_context = recompui::create_context(zelda64::get_asset_path("tracker.rml"));
    tracker_context.set_captures_input(false);
    tracker_context.set_captures_mouse(false);
}

void recompui::update_tracker() {
    if (tracker_context == recompui::ContextId::null()) {
        return;
    }
    const zelda64::tracker::Options& o = zelda64::tracker::options();
    bool snapshot_changed = false;
    if (zelda64::tracker::snapshot(shown, have_snapshot ? shown.version : 0)) {
        have_snapshot = true;
        snapshot_changed = true;
        overlay_dirty = true;
    }
    update_menu_list(snapshot_changed);

    // Shown for good once the game is running, windows or not: documents
    // stack in the order they are shown, and one shown after the config menu
    // was opened would sit on top of it.
    if (!ultramodern::is_game_started()) {
        return;
    }
    if (!recompui::is_context_shown(tracker_context)) {
        recompui::show_context(tracker_context, "");
        overlay_dirty = true;
    }
    Rml::ElementDocument* document = tracker_context.get_document();
    if (document == nullptr) {
        return;
    }
    if (!overlay_built) {
        build_overlay(document);
        if (!overlay_built) {
            return;
        }
    }

    // Only while a file is being played: not on the title screen.
    bool playing = have_snapshot && shown.in_game;
    bool show_items = playing && o.item_tracker;
    bool show_checks = playing && o.check_tracker;
    bool show_notes = playing && o.notes;
    static bool items_visible = false, checks_visible = false, notes_visible = false;
    bool options_changed = !applied_set || o.item_tracker != applied.item_tracker ||
        o.check_tracker != applied.check_tracker || o.notes != applied.notes || o.locked != applied.locked ||
        o.hide_done_areas != applied.hide_done_areas || o.hide_borders != applied.hide_borders ||
        o.hide_titles != applied.hide_titles || o.show_wings != applied.show_wings ||
        o.item_background != applied.item_background || o.check_background != applied.check_background ||
        o.notes_background != applied.notes_background ||
        o.item_x != applied.item_x || o.item_y != applied.item_y ||
        o.check_x != applied.check_x || o.check_y != applied.check_y ||
        o.notes_x != applied.notes_x || o.notes_y != applied.notes_y ||
        o.notes_w != applied.notes_w || o.notes_h != applied.notes_h || o.check_h != applied.check_h;
    if (options_changed || show_items != items_visible || show_checks != checks_visible ||
        show_notes != notes_visible) {
        applied = o;
        applied_set = true;
        overlay_dirty = true;
        items_visible = show_items;
        checks_visible = show_checks;
        notes_visible = show_notes;
        style_windows(o);
        set_display(item_window, show_items);
        set_display(check_window, show_checks);
        set_display(notes_window, show_notes);
        // The mouse while there is an unlocked window to use it on, and always
        // while the check list (so the wheel scrolls it) or the notes (so they
        // can be clicked into) are up, locked or not.
        tracker_context.set_captures_mouse((!o.locked && show_items) || show_checks || show_notes);
        if (!show_notes && notes_focused) {
            // Hidden while being typed in: give the keyboard back.
            notes_text->Blur();
            notes_focused = false;
            tracker_context.set_captures_input(false);
        }
        if (o.locked) {
            drag.window = nullptr;
        }
    }
    if (!show_items && !show_checks && !show_notes) {
        return;
    }
    if (overlay_dirty || overlay_list.dirty) {
        refresh_overlay(o);
    }

    // Kept in the window every frame (the window can be resized), except the
    // one being dragged, which the drag places.
    Rml::Vector2f size = window_size();
    // The list scrolls within the height set from the window's corner,
    // less its title; never taller than the screen.
    {
        float h = check_resize_listener.active ? check_resize_listener.h : o.check_h;
        bool titled = !(o.locked && o.hide_titles);
        float dp = document->GetContext()->GetDensityIndependentPixelRatio();
        float list = std::max(h - 20.0f - (titled ? 25.0f : 0.0f), 40.0f) * dp;
        areas_scroll->SetProperty(Rml::PropertyId::Height, Rml::Property(std::min(list, size.y * 0.9f), Rml::Unit::PX));
    }
    if (drag.window != item_window) {
        place(item_window, o.item_x, o.item_y);
    }
    if (drag.window != check_window) {
        place(check_window, o.check_x, o.check_y);
    }
    if (show_notes) {
        // The note fills the window below its title; the window's padding
        // (8dp 10dp) and outline (2dp) are taken off.
        float w = resize_listener.active ? resize_listener.w : o.notes_w;
        float h = resize_listener.active ? resize_listener.h : o.notes_h;
        bool titled = !(o.locked && o.hide_titles);
        notes_window->SetProperty(Rml::PropertyId::Width, Rml::Property(w, Rml::Unit::DP));
        notes_text->SetProperty(Rml::PropertyId::Width, Rml::Property(w - 24.0f - 8.0f, Rml::Unit::DP));
        notes_text->SetProperty(Rml::PropertyId::Height,
            Rml::Property(std::max(h - 20.0f - 8.0f - (titled ? 25.0f : 0.0f), 30.0f), Rml::Unit::DP));
        if (drag.window != notes_window) {
            place(notes_window, o.notes_x, o.notes_y);
        }
    }
}
