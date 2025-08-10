#pragma once

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "blocks.hpp"
#include "time.hpp"

class Tretis {
private:
    // Messy but optimal fields order
    // Time related stuff
    sf::Clock indicators_clock;
    Chronometre frame_time { TIME_PER_FRAME };

    // Font
    sf::Font text_font;

    // Delimiter boxes
    sf::RectangleShape whole_game_delimiter =
        sf::RectangleShape(GAME_DELIMITER_SIZE);
    sf::RectangleShape hold_piece_delimiter =
        sf::RectangleShape(HOLD_PIECE_DELIMITER_SIZE);

    // Various texts
    sf::Text level_title;
    sf::Text level_value;

    sf::Text score_title;
    sf::Text score_value;

    sf::Text lines_title;
    sf::Text lines_value;

    sf::Text t_spin_indicator;
    sf::Text mini_indicator;

    sf::Text b2b_indicator;
    sf::Text line_clear_indicator;

    sf::Text score_added_indicator;

    // Render Engine
    mutable sf::RenderWindow render_window {
        sf::VideoMode(1280, 720),
        "Tretis",
    };

    // Pretty lines
    std::array<sf::RectangleShape, GRID_WIDTH + 1>
        vertical_cell_lines;  // in constructor

    std::array<sf::RectangleShape, GRID_HEIGHT + 1>
        horizontal_cell_lines;  // in constructor

    // Indicators
    bool t_spin_indicator_activation = false;
    bool mini_indicator_activation = false;
    bool b2b_indicator_activation = false;
    bool line_clear_indicator_activation = false;
    bool score_added_indicator_activation = false;

public:
    static Tretis& Get();

    void gameloop();

private:
    void debug_fps_cout();

    void draw_game() const;

    void update_texts();

    sf::Uint8 gradient_progression();

    void handle_events();

    void resize_window(float screen_width, float screen_height);

    void hard_drop_ifnlocked();

public:
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(const Tretis&) = delete;

private:
    ~Tretis() = default;
    Tretis();
    ;
};
