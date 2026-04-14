#pragma once

#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <functional>

#include "SFML/Graphics/RenderTarget.hpp"
#include "time.hpp"

class Tretis {
public:
    static Tretis& Get();

    void gameloop();

    Tretis(Tretis const&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(Tretis const&) = delete;
    Tretis& operator=(Tretis const&&) = delete;

private:
    // Messy but optimal fields order
    // Time related stuff
    Chronometre frame_time { TIME_PER_FRAME };

    // Render Engine
    mutable sf::RenderWindow render_window {
        sf::VideoMode(1280, 720),
        "Tretis",
    };
    mutable sf::RenderTexture pause_rentex;
    std::function<void()> state = [this] { state_running(); };

    bool is_paused = false;

    float zoom = 0.9;

    /// METHODS ///
    static void debug_fps_cout();

    void draw_game_on(sf::RenderTarget& target) const;

    void update_texts();

    sf::Uint8 fade_texts_progression();

    void handle_basic_event(sf::Event event);

    static void handle_running_event(sf::Event event); // somehow this can be made static ?...

    void resize_window(uint32_t screen_width, uint32_t screen_height) const;

    /// GAME STATES ///
    void state_running();
    void state_pause();

    ~Tretis() = default;
    Tretis();
};
