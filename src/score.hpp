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

public:
    uint32_t score = 0;
    uint32_t level = 1;
    uint32_t lines = 0;
    uint32_t lines_to_clear = 5 * level;

    sf::String score_str;
    sf::String level_str;
    sf::String lines_str;

private:

public:
    void set_default_values() {
        score = 0;
        level = 1;
        lines = 0;
        lines_to_clear = 5 * level;
    }

    void update_strings() {
        score_str = std::to_string(score);
        level_str = std::to_string(level);
        lines_str = std::format("{} / {}", lines, lines_to_clear);
    }

    void add_to_score(int num_cleared_lines) {
        assert(num_cleared_lines >= 1 && num_cleared_lines <= 4);
        switch (num_cleared_lines) {
            case 1:
                lines += 1;
                score += 100 * level;
                break;
            case 2:
                lines += 3;
                score += 300 * level;
                break;
            case 3:
                lines += 5;
                score += 500 * level;
                break;
            case 4:
                lines += 8;
                score += 800 * level;
                break;
            default:
                Log::Error("Number of lines cleared received is ",
                           num_cleared_lines);
                break;
        }
        Log::Debug("Score updated to ", score);
        update_strings();
    }

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
