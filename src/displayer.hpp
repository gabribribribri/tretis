#pragma once

#include "SFML/Graphics.hpp"
#include "blocks.hpp"

/// CONSTANTS ///
// NOLINTBEGIN(cert-err58-cpp)
const int LEFT_OFFSET = 320;
const sf::Vector2f GRID_ORIGIN = sf::Vector2f(-1320.0, -200.0); 
const int CELL_SIZE = 140;
const sf::Vector2f GAME_DELIMITER_SIZE = sf::Vector2f(3920, 3480);
const float BLUR_SCALE_FACTOR = 8.0;
const float BETWEEN_CELL_LINE_THICKNESS = 8.0;
const float GAME_DELIMITER_LINE_THICHNESS = 16.0;
const sf::Vector2f HOLD_PIECE_DELIMITER_SIZE = sf::Vector2f(680, 680);
const sf::Vector2f HOLD_PIECE_DELIMITER_POS = sf::Vector2f(LEFT_OFFSET, 200 + BETWEEN_CELL_LINE_THICKNESS);
const sf::Vector2f NEXT_QUEUE_POS = sf::Vector2f(3000, 200);
const float NEXT_QUEUE_HEIGHT = 3080;

// Text related
const int TITLE_SIZE = 80;
const int VALUE_SIZE = 132;
const sf::Vector2f TEXT_POS = sf::Vector2f( LEFT_OFFSET, HOLD_PIECE_DELIMITER_SIZE.y + HOLD_PIECE_DELIMITER_POS.y + 80);
// NOLINTEND(cert-err58-cpp)

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
