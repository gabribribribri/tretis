#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

#include "grid.hpp"
#include "movements.hpp"

class Tretis {
private:
    sf::RenderWindow render_window { sf::VideoMode(1280, 720), "Tretis" };
    Chronometre frame_time { TIME_PER_FRAME };

public:
    static Tretis& Get() {
        static Tretis instance;
        return instance;
    }

    void gameloop() {
        while (render_window.isOpen()) {
            // DEBUG
            debug_fps_cout();

            // GAME LOGIC
            handle_events();
            Movements::Get().ping();

            // DRAWING
            render_window.clear(sf::Color::Black);
            draw_grid();
            render_window.display();

            // WAITING
            frame_time.wait_until_time_has_passed();
        }
    }

    void debug_fps_cout() {
        static int fis = 0;
        static sf::Clock cl;
        fis++;
        if (cl.getElapsedTime() > sf::seconds(1.0f)) {
            std::cout << "\nFPS: " << fis;
            cl.restart();
            fis = 0;
        }
    }

    void draw_grid() {
        // the place block remove block thing is very ugly.
        // I hope to find a better way in the future
        Grid& grid = Grid::Get();

        for (sf::RectangleShape& cell : grid.val) {
            render_window.draw(cell);
        }

        if (grid.is_phantom_enabled()) {
            for (sf::RectangleShape const& cell : grid.get_phbl_shapes()) {
                render_window.draw(cell);
            }
        }
        for (sf::RectangleShape const& cell : grid.get_crbl_shapes()) {
            render_window.draw(cell);
        }
    }

    void handle_events() {
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
                            Movements::Get().go_vertical();
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                            Movements::Get().go_lateral(MOVE_LEFT);
                            break;
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            Movements::Get().go_lateral(MOVE_RIGHT);
                            break;
                        case sf::Keyboard::Z:
                            Grid::Get().super_rotate_block(false);
                            break;
                        case sf::Keyboard::Up:
                        case sf::Keyboard::W:
                        case sf::Keyboard::X:
                            Grid::Get().super_rotate_block(true);
                            break;
                        case sf::Keyboard::P:
                            Grid::Get().switch_phantom_block();
                            break;
                        case sf::Keyboard::Space:
                            Grid::Get().hard_drop();
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::Down:
                        case sf::Keyboard::S:
                            Movements::Get().stop_vertical();
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            Movements::Get().stop_lateral();
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

    void resize_window(int width, int height) {
        sf::FloatRect visibleArea(0, 0, width, height);
        render_window.setView(sf::View(visibleArea));
        Grid::Get().set_cells_positions();
    }

public:
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(const Tretis&) = delete;

private:
    ~Tretis() = default;
    Tretis() = default;
};
