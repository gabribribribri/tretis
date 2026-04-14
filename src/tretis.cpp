#include "tretis.hpp"

#include <cmath>
#include "SFML/Window/Keyboard.hpp"
#include "blocks.hpp"
#include "grid.hpp"
#include "logging.hpp"
#include "movements.hpp"
#include "score.hpp"
#include "selection.hpp"
#include "time.hpp"

using std::max;

void Tretis::gameloop() {
    while (render_window.isOpen()) {
        /// DEBUG ///
        debug_fps_cout();

        /// GAME LOGIC ///
        handle_events();
        Movements::Get().ping();
        update_texts();  // I hope I'll find a better way...
        Grid::Get().adjust_everything_if_moved();

        /// DRAWING ///
        render_window.clear(sf::Color::Black);
        draw_game();
        render_window.display();
    }
}

void Tretis::debug_fps_cout() const {
    static int fis = 0;
    static sf::Clock cl;
    fis++;
    if (cl.getElapsedTime() > sf::seconds(1.0F)) {
        Log::Info("FPS : ", fis);
        Log::Debug("tspin=", t_spin_indicator_activation,
                   " mini=", mini_indicator_activation,
                   " b2b=", b2b_indicator_activation,
                   " line_clear=", line_clear_indicator_activation,
                   " score_added=", score_added_indicator_activation);
        cl.restart();
        fis = 0;
    }
}

Tretis& Tretis::Get() {
    static Tretis instance;
    return instance;
}

void Tretis::draw_game() const {
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

    if (t_spin_indicator_activation) {
        render_window.draw(t_spin_indicator);
    }

    if (mini_indicator_activation) {
        render_window.draw(mini_indicator);
    }

    if (b2b_indicator_activation) {
        render_window.draw(b2b_indicator);
    }

    if (line_clear_indicator_activation) {
        render_window.draw(line_clear_indicator);
    }

    if (score_added_indicator_activation) {
        render_window.draw(score_added_indicator);
    }

    ///  GRID  ///

    // Draw the grid cells
    for (sf::RectangleShape const& cell : grid.get_data()) {
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

void Tretis::update_texts() {
    Score& score = Score::Get();
    ScoreEvent score_event = score.copy_score_event();

    level_value.setString(score.get_level_str());
    score_value.setString(score.get_score_str());
    lines_value.setString(score.get_lines_str());

    if (score.do_we_have_events_to_report()) {
        Log::Debug("Reporting Events !");
        indicators_clock.restart();


        t_spin_indicator_activation = score_event.t_spin or score_event.mini_t_spin;
        mini_indicator_activation = score_event.mini_t_spin;
        b2b_indicator_activation = score_event.b2b_bonus;
        line_clear_indicator_activation = score_event.lines_clear != LinesClear::None;

        switch (score_event.lines_clear) {
            case LinesClear::None:
                // lol
                break;
            case LinesClear::Single:
                line_clear_indicator.setFillColor(sf::Color::White);
                line_clear_indicator.setString("SINGLE");
                break;

            case LinesClear::Double:
                line_clear_indicator.setFillColor(sf::Color::Blue);
                line_clear_indicator.setString("DOUBLE");
                break;

            case LinesClear::Triple:
                line_clear_indicator.setFillColor(sf::Color::Red);
                line_clear_indicator.setString("TRIPLE");
                break;

            case LinesClear::Tretis:
                line_clear_indicator.setFillColor(sf::Color::Cyan);
                line_clear_indicator.setString("TRETIS");
                break;
        }

        score_added_indicator_activation = true;
        score_added_indicator.setString(std::format("+{}", score_event.score_added));

        // Don't forget to clean the events after !
        score.reset_score_event();
    }

    sf::Uint8 text_fading = fade_texts_progression();
    if (text_fading == 0) {
        b2b_indicator_activation = false;
        mini_indicator_activation = false;
        line_clear_indicator_activation = false;
        score_added_indicator_activation = false;
        t_spin_indicator_activation = false;
    } else {        
        score_added_indicator.setFillColor(sf::Color(255, 255, 255, text_fading));

        sf::Color line_clear_color = line_clear_indicator.getFillColor();
        line_clear_color.a = text_fading;
        line_clear_indicator.setFillColor(line_clear_color);

        b2b_indicator.setFillColor(sf::Color(255, 255, text_fading));
        mini_indicator.setFillColor(sf::Color(255, 0, 255, text_fading));
        t_spin_indicator.setFillColor(sf::Color(255, 0, 255, text_fading));

    }

}

sf::Uint8 Tretis::fade_texts_progression() {
    // ugly line
    return static_cast<sf::Uint8>(max(1 - indicators_clock.getElapsedTime().asSeconds(), 0.0F) * 255.);
}

void Tretis::handle_events() {
    Grid& grid = Grid::Get();
    Movements& movements = Movements::Get();

    sf::Event event {};

    while (render_window.pollEvent(event)) {
        // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
        switch (event.type) {
            case sf::Event::Closed:
                render_window.close();
                break;
            case sf::Event::Resized:
                resize_window(static_cast<float>(event.size.width),
                              static_cast<float>(event.size.height));
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
                    // Select your Tretomino (for debug purposes)
                    case sf::Keyboard::Num0: grid.select_new_crbl(Tretomino::T); break;
                    case sf::Keyboard::Num1: grid.select_new_crbl(Tretomino::O); break;
                    case sf::Keyboard::Num2: grid.select_new_crbl(Tretomino::I); break;
                    case sf::Keyboard::Num3: grid.select_new_crbl(Tretomino::L); break;
                    case sf::Keyboard::Num4: grid.select_new_crbl(Tretomino::J); break;
                    case sf::Keyboard::Num5: grid.select_new_crbl(Tretomino::S); break;
                    case sf::Keyboard::Num6: grid.select_new_crbl(Tretomino::Z); break;
                        
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
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
}

void Tretis::resize_window(float screen_width, float screen_height) {
    // I finallly mother flipping did this
    // I am having a stroke at the moment
    const float screen_ratio = (screen_width / screen_height);
    const float game_delimiter_ratio = GAME_DELIMITER_SIZE.x / GAME_DELIMITER_SIZE.y;
    const float zoom = 0.9;
    float view_height = NAN;
    float view_width = NAN;

    if (screen_ratio >= game_delimiter_ratio) {
        // Screen is thinner than Game Delimiter
        view_height = GAME_DELIMITER_SIZE.y;
        view_width = GAME_DELIMITER_SIZE.y * screen_ratio;
    } else {
        // Screen is thicker than Game Delimiter
        view_width = GAME_DELIMITER_SIZE.x;
        view_height = GAME_DELIMITER_SIZE.x / screen_ratio;
    }

    view_width = std::max(view_width, screen_width);
    view_height = std::max(view_height, screen_height);
    view_width /= zoom;
    view_height /= zoom;

    float rectLeft = (-view_width / 2) + (GAME_DELIMITER_SIZE.x / 2);
    float rectTop = (-view_height / 2) + (GAME_DELIMITER_SIZE.y / 2);

    sf::FloatRect visibleArea(rectLeft, rectTop, view_width, view_height);
    render_window.setView(sf::View(visibleArea));
}

void Tretis::hard_drop_ifnlocked() {
    if (Movements::Get().is_hard_drop_locked()) {
        return;
    }
    Movements::Get().set_hard_drop_lock(true);
    Movements::Get().restart_crbl_fall_by_one_countdown();
    Grid::Get().hard_drop();
}

Tretis::Tretis() {
    /// SETTING FRAMERATE ///
    render_window.setFramerateLimit(FRAME_PER_SECOND);

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
    for (auto [i, line] : vertical_cell_lines | std::ranges::views::enumerate) {
        auto i_f = static_cast<float>(i);
        line = sf::RectangleShape(sf::Vector2f(8, (GRID_HEIGHT * CELL_SIZE) + 4));
        line.setOrigin(GRID_ORIGIN);
        line.setPosition(
            sf::Vector2f((i_f * CELL_SIZE) - (BETWEEN_CELL_LINE_THICKNESS / 2),
                         -BETWEEN_CELL_LINE_THICKNESS / 2));
        line.setFillColor(BETWEEN_CELL_LINE_COLOR);
    }

    // Horizontal Lines initialization
    for (auto [i, line] :
         horizontal_cell_lines | std::ranges::views::enumerate) {
        auto i_f = static_cast<float>(i);
        line = sf::RectangleShape(sf::Vector2f((GRID_WIDTH * CELL_SIZE) + 4, 8));
        line.setOrigin(GRID_ORIGIN);
        line.setPosition(sf::Vector2f(-1, (i_f * CELL_SIZE) - 4));
        line.setFillColor(BETWEEN_CELL_LINE_COLOR);
    }

    /// TEXT RELATED INITIALIZATION ///

    if (!text_font.loadFromFile("apercumovistarbold.ttf")) {
        Log::Error("Could not load font ! Aborting !");
        std::quick_exit(1);
    }

    level_title.setFont(text_font);
    level_title.setFillColor(sf::Color::White);
    level_title.setString("LEVEL");
    level_title.setCharacterSize(TITLE_SIZE);
    level_title.setPosition(TEXT_POS.x, TEXT_POS.y);

    level_value.setFont(text_font);
    level_value.setFillColor(sf::Color::White);
    level_value.setString(Score::Get().get_level_str());
    level_value.setCharacterSize(TITLE_SIZE);
    level_value.setPosition(TEXT_POS.x + 640, TEXT_POS.y);

    score_title.setFont(text_font);
    score_title.setFillColor(sf::Color::White);
    score_title.setString("SCORE");
    score_title.setCharacterSize(TITLE_SIZE);
    score_title.setPosition(TEXT_POS.x, TEXT_POS.y + 120);

    score_value.setFont(text_font);
    score_value.setFillColor(sf::Color::White);
    score_value.setString(Score::Get().get_score_str());
    score_value.setCharacterSize(VALUE_SIZE);
    score_value.setPosition(TEXT_POS.x, TEXT_POS.y + 200);

    lines_title.setFont(text_font);
    lines_title.setFillColor(sf::Color::White);
    lines_title.setString("LINES");
    lines_title.setCharacterSize(TITLE_SIZE);
    lines_title.setPosition(TEXT_POS.x, TEXT_POS.y + 360);

    lines_value.setFont(text_font);
    lines_value.setFillColor(sf::Color::White);
    lines_value.setString(Score::Get().get_lines_str());
    lines_value.setCharacterSize(VALUE_SIZE);
    lines_value.setPosition(TEXT_POS.x, TEXT_POS.y + 440);

    score_added_indicator.setFont(text_font);
    score_added_indicator.setFillColor(sf::Color::Magenta);
    score_added_indicator.setString("");
    score_added_indicator.setCharacterSize(120);
    score_added_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 600);

    line_clear_indicator.setFont(text_font);
    line_clear_indicator.setFillColor(sf::Color::Magenta);
    line_clear_indicator.setString("Mini");
    line_clear_indicator.setCharacterSize(120);
    line_clear_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 720);
    line_clear_indicator.setStyle(sf::Text::Bold);

    mini_indicator.setFont(text_font);
    mini_indicator.setFillColor(sf::Color::Magenta);
    mini_indicator.setString("Mini");
    mini_indicator.setCharacterSize(60);
    mini_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1000);

    t_spin_indicator.setFont(text_font);
    t_spin_indicator.setFillColor(sf::Color::Magenta);
    t_spin_indicator.setString("T-Spin");
    t_spin_indicator.setCharacterSize(200);
    t_spin_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1080);

    b2b_indicator.setFont(text_font);
    b2b_indicator.setFillColor(sf::Color::Yellow);
    b2b_indicator.setString("B2B Bonus");
    b2b_indicator.setCharacterSize(60);
    b2b_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1280);
}
