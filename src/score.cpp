#include "score.hpp"
#include <cmath>
#include <format>
#include <string>
#include "SFML/System/String.hpp"
#include "logging.hpp"

void Score::add_score(uint32_t n) {
    score += n;
    score_str = std::to_string(n);
    score_event.value().score_added = n;
}

void Score::add_lines(uint32_t n) {
    lines += n;
    lines_str = std::format("{} / {}", lines, lines_to_clear);
}

void Score::set_lines_to_clear(uint32_t n) {
    lines_to_clear = n;
    lines_str = std::format("{} / {}", lines, lines_to_clear);
}

void Score::add_level(uint32_t n) {
    level += n;
    level_str = std::to_string(level);
}

void Score::did_t_just_rotate() { t_just_rotated = true; }

void Score::did_just_move() { t_just_rotated = false; }

void Score::did_t_rotation_point_5() { t_rotation_point_5_used = true; }

void Score::did_t_spin_sides() { t_spin_sides = true; }

void Score::did_t_spin_mini_sides() { t_spin_mini_sides = true; }

std::optional<ScoreEvent> Score::take_score_event() {
    // I wanted to std::move but it told me it's useless
    auto ret = score_event;
    score_event.reset();
    return ret;
}
void Score::set_default_values() {
    score = 0;
    level = 1;
    lines = 0;
    lines_to_clear = 5 * level;
    update_strings();
}

void Score::update_strings() {
    score_str = std::to_string(score);
    level_str = std::to_string(level);
    lines_str = std::format("{} / {}", lines, lines_to_clear);
}

void Score::report_score(int num_cleared_lines) {
    // waiting for C++26 contracts...
    // assert(not(t_spin_sides and mini_t_spin));

    // Construct the score_event
    score_event.emplace();

    if (t_just_rotated and (t_spin_sides or t_rotation_point_5_used)) {
        // T-SPIN !
        Log::Debug("T-Spin detected with ", num_cleared_lines, " lines cleared !");
        score_event->t_spin = true;
        switch (num_cleared_lines) {
            case 0:
                add_lines(4);
                add_score(400 * level);
                score_event->lines_clear = LinesClear::None;
                break;
            case 1:
                add_lines(8);
                add_score(800 * level);
                score_event->lines_clear = LinesClear::Single;
                break;
            case 2:
                add_lines(12);
                add_score(1200 * level);
                score_event->lines_clear = LinesClear::Double;
                break;
            case 3:
                add_lines(16);
                add_score(1600 * level);
                score_event->lines_clear = LinesClear::Triple;
                break;
            case 4:
            default:
                Log::Error("Number of lines cleared received is ", num_cleared_lines, " in a T-Spin");
                break;
        }
    } else if (t_just_rotated and t_spin_mini_sides) {
        // MINI T-SPIN !
        Log::Debug("Mini T-Spin detected with ", num_cleared_lines, " lines cleared !");
        score_event->mini_t_spin = true;
        switch (num_cleared_lines) {
            case 0:
                add_lines(1);
                add_score(100 * level);
                score_event->lines_clear = LinesClear::None;
                break;
            case 1:
                add_lines(2);
                add_score(200 * level);
                score_event->lines_clear = LinesClear::Single;
                break;
            case 2:
            case 3:
            case 4:
            default:
                Log::Error("Number of lines cleared received is ", num_cleared_lines, " in a Mini T-Spin");
                break;
        }
    } else {
        // No t-spin...
        switch (num_cleared_lines) {
            case 0:
                break;
            case 1:
                add_lines(1);
                add_score(100 * level);
                score_event->lines_clear = LinesClear::Single;
                break;
            case 2:
                add_lines(3);
                add_score(300 * level);
                score_event->lines_clear = LinesClear::Double;
                break;
            case 3:
                add_lines(5);
                add_score(500 * level);
                score_event->lines_clear = LinesClear::Triple;
                break;
            case 4:
                add_lines(8);
                add_score(800 * level);
                score_event->lines_clear = LinesClear::Tretis;
                break;
            default:
                Log::Error("Number of lines cleared received is ", num_cleared_lines);
                break;
        }
    }
    Log::Debug("Score updated to ", score);

    clear_t_spin_flags();
}

void Score::clear_t_spin_flags() {
    t_just_rotated = false;
    t_rotation_point_5_used = false;
    t_spin_sides = false;
    t_spin_mini_sides = false;
}

void Score::add_soft_drop() {
    score += 1;
    score_str = std::to_string(score);
}

void Score::add_hard_drop(int length) {
    score += length * 2;
    score_str = std::to_string(score);
}

sf::Time Score::get_drop_speed_from_level() const {
    auto base = static_cast<float>(0.8 - ((level - 1) * 0.007));
    auto exp = static_cast<float>(level - 1);
    return sf::seconds(std::powf(base, exp));
}

sf::String const& Score::get_score_str() const { return score_str; };

sf::String const& Score::get_level_str() const { return level_str; };

sf::String const& Score::get_lines_str() const { return lines_str; };

Score::Score() {
    set_default_values();
}

Score& Score::Get() {
    static Score instance;
    return instance;
}
