#pragma once

#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <cassert>
#include <cstdint>

enum class LinesClear : std::uint8_t {
    None,
    Single,
    Double,
    Triple,
    Tretis
};

/// ScoreEvent only stores relevant informations
/// to be displayed
struct ScoreEvent {
    bool t_spin {};
    bool mini_t_spin {};
    bool b2b_bonus {};
    bool next_level {};
    LinesClear lines_clear {};
    uint32_t score_added {};
};

class Score {
public:
    static Score& Get();

    void did_t_just_rotate();

    void did_just_move();

    void did_t_rotation_point_5();

    void did_t_spin_sides();

    void did_t_spin_mini_sides();

    void clear_t_spin_flags();

    bool do_we_have_events_to_report();

    void reset_score_event();

    void set_default_values();

    void update_strings();

    void report_score(int num_cleared_lines);

    void add_soft_drop();

    void add_hard_drop(int length);

    [[nodiscard]] sf::Time get_drop_speed_from_level() const;

    [[nodiscard]] ScoreEvent copy_score_event() const;

    [[nodiscard]] sf::String const& get_score_str() const;

    [[nodiscard]] sf::String const& get_level_str() const;

    [[nodiscard]] sf::String const& get_lines_str() const;

    Score(Score const&) = delete;
    Score(Score&&) = delete;
    Score operator=(Score) = delete;
    Score operator=(Score&&) = delete;

private:
    Score();
    ~Score() = default;

    uint32_t score = 0;
    uint32_t level = 1;
    uint32_t lines = 0;
    uint32_t lines_to_clear = 5 * level;

    // T-Spin flags
    bool t_just_rotated = false;
    bool t_rotation_point_5_used = false;
    bool t_spin_sides = false;
    bool t_spin_mini_sides = false;

    sf::String score_str;
    sf::String level_str;
    sf::String lines_str;

    // TODO Make this a std::optional
    bool has_events_to_report = false;
    ScoreEvent score_event;

    void add_score(uint32_t n);

    void add_lines(uint32_t n);

    void set_lines_to_clear(uint32_t n);

    void add_level(uint32_t n);
};
