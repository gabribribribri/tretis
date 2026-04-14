#include "tretis.hpp"

#include <cmath>
#include "blocks.hpp"
#include "displayer.hpp"
#include "grid.hpp"
#include "logging.hpp"
#include "movements.hpp"
#include "time.hpp"

void Tretis::gameloop() {
    while (render_window.isOpen()) {
        state();
    }
}

void Tretis::state_running() {
    /// DEBUGGING ///
    debug_fps_cout();

    /// HANDLE EVENTS ///
    sf::Event event {};
    while (render_window.pollEvent(event)) {
        handle_basic_event(event);
        handle_running_event(event);
    }

    /// GAME LOGIC ///
    Movements::Get().ping();
    Displayer::Get().update_texts();  // I hate this method
    Grid::Get().adjust_everything_if_moved();

    /// DRAWING ///
    render_window.clear();
    Displayer::Get().draw_game_on(render_window);
    render_window.display();
}

void Tretis::state_pause() {
    /// DEBUGGING ///
    debug_fps_cout();

    sf::Event event {};
    while (render_window.pollEvent(event)) {
        handle_basic_event(event);
    }

    /// DRAWING ///
    render_window.clear();
    Displayer::Get().draw_game_on(render_window);
    render_window.display();
}

void Tretis::debug_fps_cout() {
    static int fis = 0;
    static sf::Clock cl;
    fis++;
    if (cl.getElapsedTime() > sf::seconds(1.0F)) {
        Log::Info("FPS : ", fis);
        // TODO Fix this
        // Log::Debug("tspin=", t_spin_indicator_flag, " mini=", mini_indicator_flag, " b2b=", b2b_indicator_flag, " line_clear=", line_clear_indicator_flag, " score_added=", score_added_indicator_flag);
        cl.restart();
        fis = 0;
    }
}

Tretis& Tretis::Get() {
    static Tretis instance;
    return instance;
}



void Tretis::handle_basic_event(sf::Event event) {
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    switch (event.type) {
        case sf::Event::Closed:
            render_window.close();
            break;
        case sf::Event::Resized:
            resize_window(event.size.width, event.size.height);
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Hyphen:
                    zoom -= 0.05;
                    resize_window(render_window.getSize().x, render_window.getSize().y);
                    break;
                case sf::Keyboard::Equal:
                    zoom += 0.05;
                    resize_window(render_window.getSize().x, render_window.getSize().y);
                    break;
                case sf::Keyboard::P:
                    if (is_paused) {
                        is_paused = false;
                        state = [this] { state_running(); };
                    } else {
                        is_paused = true;
                        state = [this] { state_pause(); };
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
}

void Tretis::handle_running_event(sf::Event event) {
    Grid& grid = Grid::Get();
    Movements& movements = Movements::Get();

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    switch (event.type) {
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
                case sf::Keyboard::BackSlash:
                    grid.switch_phantom_block();
                    break;
                case sf::Keyboard::Space:
                    // I really wonder HOW to connect everything up !
                    Grid::hard_drop_ifnlocked();
                    break;
                case sf::Keyboard::C:
                    grid.hold_crbl_ifnlocked();
                    break;
                // Select your Tretomino (for debug purposes)
                case sf::Keyboard::Num0:
                    grid.select_new_crbl(Tretomino::T);
                    break;
                case sf::Keyboard::Num1:
                    grid.select_new_crbl(Tretomino::O);
                    break;
                case sf::Keyboard::Num2:
                    grid.select_new_crbl(Tretomino::I);
                    break;
                case sf::Keyboard::Num3:
                    grid.select_new_crbl(Tretomino::L);
                    break;
                case sf::Keyboard::Num4:
                    grid.select_new_crbl(Tretomino::J);
                    break;
                case sf::Keyboard::Num5:
                    grid.select_new_crbl(Tretomino::S);
                    break;
                case sf::Keyboard::Num6:
                    grid.select_new_crbl(Tretomino::Z);
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
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
}

void Tretis::resize_window(uint32_t screen_width, uint32_t screen_height) const {
    // I finallly mother flipping did this
    // I am having a stroke at the moment
    const auto screen_width_f = static_cast<float>(screen_width);
    const auto screen_height_f = static_cast<float>(screen_height);
    const float screen_ratio = screen_width_f / screen_height_f;
    const float game_delimiter_ratio = GAME_DELIMITER_SIZE.x / GAME_DELIMITER_SIZE.y;
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

    view_width = std::max(view_width, screen_width_f) / zoom;
    view_height = std::max(view_height, screen_height_f) / zoom;

    float rectLeft = (-view_width / 2) + (GAME_DELIMITER_SIZE.x / 2);
    float rectTop = (-view_height / 2) + (GAME_DELIMITER_SIZE.y / 2);

    sf::FloatRect visibleArea(rectLeft, rectTop, view_width, view_height);
    render_window.setView(sf::View(visibleArea));
}

Tretis::Tretis() {
    /// RENDERING RELATED INIT ///
    render_window.setFramerateLimit(FRAME_PER_SECOND);
    // We add 100 pixels each side to fit the blur
    pause_rentex.create(static_cast<int>(GAME_DELIMITER_SIZE.x / BLUR_SCALE_FACTOR) + 200, static_cast<int>(GAME_DELIMITER_SIZE.y / BLUR_SCALE_FACTOR) + 200);
    pause_rentex.setView(sf::View(sf::FloatRect(-100, -100, GAME_DELIMITER_SIZE.x + 100, GAME_DELIMITER_SIZE.y + 100)));
}
