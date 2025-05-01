#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <format>
#include <ranges>

#include "blocks.hpp"
#include "score.hpp"
#include "grid.hpp"
#include "logging.hpp"
#include "movements.hpp"
#include "selection.hpp"
#include "tretomino_render_shape.hpp"

class Tretis {
private:
    // Render Engine
    mutable sf::RenderWindow render_window {
        sf::VideoMode(1280, 720),
        "Tretis",
    };
    Chronometre frame_time { TIME_PER_FRAME };

    // Renderee
    sf::RectangleShape whole_game_delimiter =
        sf::RectangleShape(GAME_DELIMITER_SIZE);

    sf::RectangleShape hold_piece_delimiter =
        sf::RectangleShape(HOLD_PIECE_DELIMITER_SIZE);

    std::array<sf::RectangleShape, GRID_WIDTH + 1>
        vertical_cell_lines;  // in constructor

    std::array<sf::RectangleShape, GRID_HEIGHT + 1>
        horizontal_cell_lines;  // in constructor

    // Various text related thangs
    sf::Font text_font;

    sf::Text level_title;
    sf::Text level_value;

    sf::Text score_title;
    sf::Text score_value;

    sf::Text lines_title;
    sf::Text lines_value;

public:
    static Tretis& Get() {
        static Tretis instance;
        return instance;
    }

    void gameloop() {
        while (render_window.isOpen()) {
            /// DEBUG ///
            debug_fps_cout();

            /// GAME LOGIC ///
            handle_events();
            Movements::Get().ping();
            Grid::Get().adjust_everything_if_moved();

            /// DRAWING ///
            render_window.clear(sf::Color::Black);
            draw_game();
            render_window.display();

            /// WAITING ///
            frame_time.wait_until_time_has_passed();
        }
    }

private:
    void debug_fps_cout() {
        static int fis = 0;
        static sf::Clock cl;
        fis++;
        if (cl.getElapsedTime() > sf::seconds(1.0f)) {
            Log::Info("FPS : ", fis);
            cl.restart();
            fis = 0;
        }
    }

    void draw_game() const {
        Grid& grid = Grid::Get();
        Selection& selection = Selection::Get();

        /// HUD ///

        // Draw the whole game delimiter (Nothing should be drawn outside of it)
        render_window.draw(whole_game_delimiter);

        // Draw the hold piece delimiter
        render_window.draw(hold_piece_delimiter);

        // Draw the hold piece Tretomino shape
        if (selection.hold_tretomino.has_value()) {
            for (auto const& cell : selection.hold_shape.shape) {
                render_window.draw(cell);
            }
        }

        /// TEXT ///

        render_window.draw(level_title);
        render_window.draw(level_value);
        render_window.draw(score_title);
        render_window.draw(score_value);
        render_window.draw(lines_title);
        render_window.draw(lines_value);

        ///  GRID  ///

        // Draw the grid cells
        for (sf::RectangleShape& cell : grid.val) {
            render_window.draw(cell);
        }

        // Draw the phantom block
        if (grid.is_phantom_enabled()) {
            for (sf::RectangleShape const& cell : grid.get_phbl_shapes()) {
                render_window.draw(cell);
            }
        }

        // Draw the current block
        for (sf::RectangleShape const& cell : grid.get_crbl_shapes()) {
            render_window.draw(cell);
        }

        // Draw Separation lines
        for (sf::RectangleShape const& line : vertical_cell_lines) {
            render_window.draw(line);
        }
        for (sf::RectangleShape const& line : horizontal_cell_lines) {
            render_window.draw(line);
        }

        /// NEXT QUEUE ///

        // Draw the next queue shapes
        for (auto const& queue_tretomino : selection.next_queue_shapes) {
            for (sf::RectangleShape const& cell : queue_tretomino.shape) {
                render_window.draw(cell);
            }
        }
    }

    void handle_events() {
        Grid& grid = Grid::Get();
        Movements& movements = Movements::Get();

        sf::Event event;

        while (render_window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    render_window.close();
                    break;
                case sf::Event::Resized:
                    resize_window(event.size.width, event.size.height);
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Down:
                        case sf::Keyboard::S:
                            movements.go_vertical();
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                            movements.go_lateral(MOVE_LEFT);
                            break;
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            movements.go_lateral(MOVE_RIGHT);
                            break;
                        case sf::Keyboard::Z:
                            grid.super_rotate_block(false);
                            break;
                        case sf::Keyboard::Up:
                        case sf::Keyboard::W:
                        case sf::Keyboard::X:
                            grid.super_rotate_block(true);
                            break;
                        case sf::Keyboard::P:
                            grid.switch_phantom_block();
                            break;
                        case sf::Keyboard::Space:
                            // I really wonder HOW to connect everything up !
                            hard_drop_ifnlocked();
                            break;
                        case sf::Keyboard::C:
                            grid.hold_crbl_ifnlocked();
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::Down:
                        case sf::Keyboard::S:
                            movements.stop_vertical();
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            movements.stop_lateral();
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void resize_window(float screen_width, float screen_height) {
        // I finallly mother flipping did this
        // I am having a stroke at the moment
        float screen_ratio = screen_width / screen_height;
        float view_height, view_width;

        if (screen_ratio >= GAME_DELIMITER_SIZE.x / GAME_DELIMITER_SIZE.y) {
            view_height = GAME_DELIMITER_SIZE.y;
            view_width = GAME_DELIMITER_SIZE.y * screen_ratio;
        } else {
            view_width = GAME_DELIMITER_SIZE.x;
            view_height = GAME_DELIMITER_SIZE.x / screen_ratio;
        }

        view_width = std::max(view_width, screen_width);
        view_height = std::max(view_height, screen_height);

        float rectLeft = -view_width / 2 + GAME_DELIMITER_SIZE.x / 2;
        float rectTop = -view_height / 2 + GAME_DELIMITER_SIZE.y / 2;

        sf::FloatRect visibleArea(rectLeft, rectTop, view_width, view_height);
        render_window.setView(sf::View(visibleArea));
    }

    void hard_drop_ifnlocked() {
        if (not Grid::Get().hard_drop_locked) {
            Grid::Get().hard_drop_locked = true;
            Movements::Get().crbl_fall_by_one_countdown.restart();
            Grid::Get().hard_drop();
        }
    }

public:
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(const Tretis&) = delete;

private:
    ~Tretis() = default;
    Tretis() {
        /// SHAPED RELATED INITIALIZATIONS ///

        // Whole game delimiter initialization
        whole_game_delimiter.setFillColor(sf::Color::Transparent);
        whole_game_delimiter.setPosition(sf::Vector2f(
            GAME_DELIMITER_LINE_THICHNESS, GAME_DELIMITER_LINE_THICHNESS));
        whole_game_delimiter.setOutlineColor(BETWEEN_CELL_LINE_COLOR);
        whole_game_delimiter.setOutlineThickness(GAME_DELIMITER_LINE_THICHNESS);

        // Hold Piece delimiter initialization
        hold_piece_delimiter.setFillColor(sf::Color::Transparent);
        hold_piece_delimiter.setPosition(HOLD_PIECE_DELIMITER_POS);
        hold_piece_delimiter.setOutlineColor(BETWEEN_CELL_LINE_COLOR);
        hold_piece_delimiter.setOutlineThickness(GAME_DELIMITER_LINE_THICHNESS);

        // Vertical Lines initialization
        for (auto [i, line] :
             vertical_cell_lines | std::ranges::views::enumerate) {
            line = sf::RectangleShape(
                sf::Vector2f(2, GRID_HEIGHT * CELL_SIZE + 1));
            line.setOrigin(GRID_ORIGIN);
            line.setPosition(
                sf::Vector2f(i * CELL_SIZE - BETWEEN_CELL_LINE_THICKNESS / 2,
                             -BETWEEN_CELL_LINE_THICKNESS / 2));
            line.setFillColor(BETWEEN_CELL_LINE_COLOR);
        }

        // Horizontal Lines initialization
        for (auto [i, line] :
             horizontal_cell_lines | std::ranges::views::enumerate) {
            line =
                sf::RectangleShape(sf::Vector2f(GRID_WIDTH * CELL_SIZE + 1, 2));
            line.setOrigin(GRID_ORIGIN);
            line.setPosition(sf::Vector2f(-1, i * CELL_SIZE - 1));
            line.setFillColor(BETWEEN_CELL_LINE_COLOR);
        }

        /// TEXT RELATED INITIALIZATION ///

        if (!text_font.loadFromFile("apercumovistarbold.ttf")) {
            Log::Error("Could not load font ! Aborting !");
            exit(1);
        }

        level_title.setFont(text_font);
        level_title.setFillColor(sf::Color::White);
        level_title.setString("LEVEL");
        level_title.setCharacterSize(TITLE_SIZE);
        level_title.setPosition(TEXT_POS.x, TEXT_POS.y);

        level_value.setFont(text_font);
        level_value.setFillColor(sf::Color::White);
        level_value.setString(Score::Get().level_str);
        level_value.setCharacterSize(TITLE_SIZE);
        level_value.setPosition(TEXT_POS.x + 160, TEXT_POS.y);

        score_title.setFont(text_font);
        score_title.setFillColor(sf::Color::White);
        score_title.setString("SCORE");
        score_title.setCharacterSize(TITLE_SIZE);
        score_title.setPosition(TEXT_POS.x, TEXT_POS.y + 30);

        score_value.setFont(text_font);
        score_value.setFillColor(sf::Color::White);
        score_value.setString(Score::Get().score_str);
        score_value.setCharacterSize(VALUE_SIZE);
        score_value.setPosition(TEXT_POS.x, TEXT_POS.y + 50);

        lines_title.setFont(text_font);
        lines_title.setFillColor(sf::Color::White);
        lines_title.setString("LINES");
        lines_title.setCharacterSize(TITLE_SIZE);
        lines_title.setPosition(TEXT_POS.x, TEXT_POS.y + 90);

        lines_value.setFont(text_font);
        lines_value.setFillColor(sf::Color::White);
        lines_value.setString(Score::Get().lines_str);
        lines_value.setCharacterSize(VALUE_SIZE);
        lines_value.setPosition(TEXT_POS.x, TEXT_POS.y + 110);
    };
};
