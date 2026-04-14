#include "displayer.hpp"
#include <filesystem>
#include "grid.hpp"
#include "score.hpp"
#include "selection.hpp"
#include "logging.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

namespace {

std::string get_exe_dir() {
    char path[1024];  // NOLINT shut the fuck up
    uint32_t size = sizeof(path);

#ifdef _WIN32
    GetModuleFileNameA(NULL, path, size);
#elif __APPLE__
    _NSGetExecutablePath(path, &size);
#else
    ssize_t count = ::readlink("/proc/self/exe", path, size);  // NOLINT I swear I have murderous intents
    if (count != -1) {
        path[count] = '\0';  // NOLINT I am going to send your grandparents to a pig farm in New Zealand
    }
#endif
    std::filesystem::path p(path);
    return p.parent_path().string();  // Returns the folder containing the exe
}

}  // namespace

Displayer& Displayer::Get() {
    static Displayer instance;
    return instance;
}

void Displayer::draw_game_on(sf::RenderTarget& target) const {
    Grid& grid = Grid::Get();
    Selection& selection = Selection::Get();

    /// CLEAR ///
    target.clear();

    /// HUD ///

    // Draw the whole game delimiter (Nothing should be drawn outside of it)
    target.draw(whole_game_delimiter);

    // Draw the hold piece delimiter
    target.draw(hold_piece_delimiter);

    // Draw the hold piece Tretomino shape
    if (selection.hold_tretomino.has_value()) {
        for (auto const& cell : selection.hold_shape.shape) {
            target.draw(cell);
        }
    }

    /// TEXT ///
    target.draw(level_title);
    target.draw(level_value);
    target.draw(score_title);
    target.draw(score_value);
    target.draw(lines_title);
    target.draw(lines_value);
    if (t_spin_indicator_flag) {
        target.draw(t_spin_indicator);
    }
    if (mini_indicator_flag) {
        target.draw(mini_indicator);
    }
    if (b2b_indicator_flag) {
        target.draw(b2b_indicator);
    }
    if (line_clear_indicator_flag) {
        target.draw(line_clear_indicator);
    }
    if (score_added_indicator_flag) {
        target.draw(score_added_indicator);
    }

    ///  GRID  ///

    // Draw the grid cells
    for (sf::RectangleShape const& cell : grid.get_data()) {
        target.draw(cell);
    }

    // Draw the phantom block
    if (grid.is_phantom_enabled()) {
        for (sf::RectangleShape const& cell : grid.get_phbl_shapes()) {
            target.draw(cell);
        }
    }

    // Draw the current block
    for (sf::RectangleShape const& cell : grid.get_crbl_shapes()) {
        target.draw(cell);
    }

    // Draw Separation lines
    for (sf::RectangleShape const& line : vertical_cell_lines) {
        target.draw(line);
    }
    for (sf::RectangleShape const& line : horizontal_cell_lines) {
        target.draw(line);
    }

    /// NEXT QUEUE ///

    // Draw the next queue shapes
    for (auto const& queue_tretomino : selection.next_queue_shapes) {
        for (sf::RectangleShape const& cell : queue_tretomino.shape) {
            target.draw(cell);
        }
    }
}

void Displayer::update_texts() {
    Score& score = Score::Get();

    score_value.setString(score.get_score_str());

    if (auto score_event = score.take_score_event()) {
        level_value.setString(score.get_level_str());
        lines_value.setString(score.get_lines_str());

        indicators_clock.restart();

        t_spin_indicator_flag = score_event->t_spin or score_event->mini_t_spin;
        mini_indicator_flag = score_event->mini_t_spin;
        b2b_indicator_flag = score_event->b2b_bonus;
        line_clear_indicator_flag = score_event->lines_clear != LinesClear::None;
        score_added_indicator_flag = score_event->score_added != 0;
        score_added_indicator.setString(std::format("+{}", score_event->score_added));

        switch (score_event->lines_clear) {
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
    }

    sf::Uint8 text_fading = fade_texts_progression();
    if (text_fading == 0) {
        b2b_indicator_flag = false;
        mini_indicator_flag = false;
        line_clear_indicator_flag = false;
        score_added_indicator_flag = false;
        t_spin_indicator_flag = false;
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
sf::Uint8 Displayer::fade_texts_progression() {
    // ugly line
    return static_cast<sf::Uint8>(std::max(1 - indicators_clock.getElapsedTime().asSeconds(), 0.0F) * 255.);
}

Displayer::Displayer() {
    /// SHAPED RELATED INITIALIZATIONS ///

    // Whole game delimiter initialization
    whole_game_delimiter.setFillColor(sf::Color::Transparent);
    whole_game_delimiter.setPosition(sf::Vector2f(GAME_DELIMITER_LINE_THICHNESS, GAME_DELIMITER_LINE_THICHNESS));
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
        line.setPosition(sf::Vector2f((i_f * CELL_SIZE) - (BETWEEN_CELL_LINE_THICKNESS / 2), -BETWEEN_CELL_LINE_THICKNESS / 2));
        line.setFillColor(BETWEEN_CELL_LINE_COLOR);
    }

    // Horizontal Lines initialization
    for (auto [i, line] : horizontal_cell_lines | std::ranges::views::enumerate) {
        auto i_f = static_cast<float>(i);
        line = sf::RectangleShape(sf::Vector2f((GRID_WIDTH * CELL_SIZE) + 4, 8));
        line.setOrigin(GRID_ORIGIN);
        line.setPosition(sf::Vector2f(-1, (i_f * CELL_SIZE) - 4));
        line.setFillColor(BETWEEN_CELL_LINE_COLOR);
    }

    /// TEXT RELATED INITIALIZATION ///

    if (!text_font.loadFromFile(get_exe_dir() + "/apercumovistarbold.ttf")) {
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
    score_added_indicator.setString(Score::Get().get_score_str());
    score_added_indicator.setCharacterSize(120);
    score_added_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 600);

    line_clear_indicator.setFont(text_font);
    line_clear_indicator.setFillColor(sf::Color::White);
    line_clear_indicator.setCharacterSize(160);
    line_clear_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 720);
    line_clear_indicator.setStyle(sf::Text::Bold);

    mini_indicator.setFont(text_font);
    mini_indicator.setFillColor(sf::Color::Magenta);
    mini_indicator.setString("Mini");
    mini_indicator.setCharacterSize(60);
    mini_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1000);

    t_spin_indicator.setFont(text_font);
    t_spin_indicator.setFillColor(sf::Color::Magenta);
    t_spin_indicator.setString("T-SPIN");
    t_spin_indicator.setCharacterSize(200);
    t_spin_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1080);

    // TODO make use of this
    b2b_indicator.setFont(text_font);
    b2b_indicator.setFillColor(sf::Color::Yellow);
    b2b_indicator.setString("B2B Bonus");
    b2b_indicator.setCharacterSize(60);
    b2b_indicator.setPosition(TEXT_POS.x, TEXT_POS.y + 1280);
}
