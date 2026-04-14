#pragma once

#include "SFML/Graphics.hpp"
#include "blocks.hpp"

class Displayer {
public:
    static Displayer& Get();

    void draw_game_on(sf::RenderTarget& target) const;

    void update_texts();

    Displayer(Displayer const&) = delete;
    Displayer(Displayer&&) = delete;
    Displayer& operator=(Displayer const&) = delete;
    Displayer& operator=(Displayer const&&) = delete;

private:
    // Font
    sf::Font text_font;

    // Delimiter boxes
    sf::RectangleShape whole_game_delimiter = sf::RectangleShape(GAME_DELIMITER_SIZE);
    sf::RectangleShape hold_piece_delimiter = sf::RectangleShape(HOLD_PIECE_DELIMITER_SIZE);

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

    // Indicators
    bool t_spin_indicator_flag = false;
    bool mini_indicator_flag = false;
    bool b2b_indicator_flag = false;
    bool line_clear_indicator_flag = false;
    bool score_added_indicator_flag = false;

    // Pretty lines
    std::array<sf::RectangleShape, GRID_WIDTH + 1> vertical_cell_lines;     // in constructor
    std::array<sf::RectangleShape, GRID_HEIGHT + 1> horizontal_cell_lines;  // in constructor

    sf::Clock indicators_clock;

    /// METHODS ///


    sf::Uint8 fade_texts_progression();

    ~Displayer() = default;
    Displayer();
};
