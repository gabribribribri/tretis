#include <SFML/System/String.hpp>
#include <SFML/System/Time.hpp>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <format>
#include <string>

#include "logging.hpp"

class Score {
private:
    uint32_t score = 0;
    uint32_t level = 1;
    uint32_t lines = 0;
    uint32_t lines_to_clear = 5 * level;

    bool just_rotated = false;
    bool rotation_point_5_used = false;  // Only for T of course

public:
    sf::String score_str;
    sf::String level_str;
    sf::String lines_str;

private:
    void add_score(uint32_t n) {
        score += n;
        score_str = std::to_string(score);
    }

    void add_lines(uint32_t n) {
        lines += n;
        lines_str = std::format("{} / {}", lines, lines_to_clear);
    }
    void set_lines_to_clear(uint32_t n) {
        lines_to_clear = n;
        lines_str = std::format("{} / {}", lines, lines_to_clear);
    }

    void add_level(uint32_t n) {
        level += n;
        level_str = std::to_string(level);
    }


public:
    void did_just_rotate() { just_rotated = true; }

    void did_just_move() { just_rotated = false; }

    void using_rotation_point_5() { rotation_point_5_used = true; }

    void set_default_values() {
        score = 0;
        level = 1;
        lines = 0;
        lines_to_clear = 5 * level;
        update_strings();
    }

    void update_strings() {
        score_str = std::to_string(score);
        level_str = std::to_string(level);
        lines_str = std::format("{} / {}", lines, lines_to_clear);
    }

    void create_score_report(int num_cleared_lines, bool t_spin,
                             bool mini_t_spin) {
        assert(num_cleared_lines >= 1 && num_cleared_lines <= 4);
        assert(not(t_spin and mini_t_spin));

        if (just_rotated and (t_spin or rotation_point_5_used)) {
            // T-SPIN !
            Log::Debug("T-Spin detected with ", num_cleared_lines,
                       " lines cleared !");
            switch (num_cleared_lines) {
                case 0:
                    add_lines(4);
                    add_score(400 * level);
                    break;
                case 1:
                    add_lines(8);
                    add_score(800 * level);
                    break;
                case 2:
                    add_lines(12);
                    add_score(1200 * level);
                    break;
                case 3:
                    add_lines(16);
                    add_score(1600 * level);
                    break;
                case 4:
                default:
                    Log::Error("Number of lines cleared received is ",
                               num_cleared_lines, " in a T-Spin");
                    break;
            }
        } else if (just_rotated and mini_t_spin) {
            // MINI T-SPIN !
            Log::Debug("Mini T-Spin detected with ", num_cleared_lines,
                       " lines cleared !");
            switch (num_cleared_lines) {
                case 0:
                    add_lines(1);
                    add_score(100 * level);
                    break;
                case 1:
                    add_lines(2);
                    add_score(200 * level);
                    break;
                case 2:
                case 3:
                case 4:
                default:
                    Log::Error("Number of lines cleared received is ",
                               num_cleared_lines, " in a Mini T-Spin");
                    break;
            }
        } else {
            // No t-spin...
            switch (num_cleared_lines) {
                case 1:
                    add_lines(1);
                    add_score(100 * level);
                    break;
                case 2:
                    add_lines(3);
                    add_score(300 * level);
                    break;
                case 3:
                    add_lines(5);
                    add_score(500 * level);
                    break;
                case 4:
                    add_lines(8);
                    add_score(800 * level);
                    break;
                case 0:
                default:
                    Log::Error("Number of lines cleared received is ",
                               num_cleared_lines);
                    break;
            }
        }
        Log::Debug("Score updated to ", score);
    }

    void add_soft_drop() { add_score(1); }

    void add_hard_drop(int length) { add_score(2 * length); }

    sf::Time get_drop_speed_from_level() {
        return sf::seconds(std::powf(0.8 - ((level - 1) * 0.007), level - 1));
    }

private:
    Score() {
        set_default_values();
        update_strings();
    };
    ~Score() = default;

public:
    static Score& Get() {
        static Score instance;
        return instance;
    }
    Score(Score const&) = delete;
    Score(Score&&) = delete;
    Score operator=(Score) = delete;
};
