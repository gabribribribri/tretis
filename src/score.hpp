#pragma once

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <cassert>
#include <cstdint>

enum class LinesClear { None, Single, Double, Triple, Tretis };

struct ScoreEvent {
    bool t_spin {};
    bool mini_t_spin {};
    bool b2b_bonus {};
    bool next_level {};
    LinesClear lines_clear {};
    uint32_t score_added {};
};

class Score {
private:
    uint32_t score = 0;
    uint32_t level = 1;
    uint32_t lines = 0;
    uint32_t lines_to_clear = 5 * level;

    bool just_rotated = false;
    bool rotation_point_5_used = false;  // Only for T of course

    bool has_events_to_report = false;

public:
    sf::String score_str;
    sf::String level_str;
    sf::String lines_str;

    ScoreEvent score_event;

private:
    void add_score(uint32_t n);

    void add_lines(uint32_t n);

    void set_lines_to_clear(uint32_t n);

    void add_level(uint32_t n);

public:
    void did_just_rotate();

    void did_just_move();

    void using_rotation_point_5();

    bool do_we_have_events_to_report();

    void reset_score_event();

    void set_default_values();

    void update_strings();

    void report_score(int num_cleared_lines, bool t_spin, bool mini_t_spin);

    void add_soft_drop();

    void add_hard_drop(int length);

    sf::Time get_drop_speed_from_level();

private:
    Score();
    ;
    ~Score() = default;

public:
    static Score& Get();

    Score(Score const&) = delete;
    Score(Score&&) = delete;
    Score operator=(Score) = delete;
};
