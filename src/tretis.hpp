#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

#include "grid.hpp"
#include "time.hpp"

class Tretis {
public:
    Grid grid;
    sf::RenderWindow render_window { sf::VideoMode(1200, 1200), "Tretis" };

private:
    Chronometre frame_time { TIME_PER_FRAME };
    Chronometre crbl_fall_by_one_countdown { BASE_BLOCK_FALL_BY_ONE};

public:
    static Tretis& Get() { return tretis; }

    void gameloop() {
        while (render_window.isOpen()) {
            // DEBUG
            debug_fps_cout();

            // GAME LOGIC
            handle_events();
            if (crbl_fall_by_one_countdown.has_time_passed()) {
                grid.move_crbl_down_or_place();
            }

            // DRAWING
            render_window.clear(sf::Color(64, 64,64));
            draw_grid_with_crbl();
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

    void draw_grid_with_crbl() {
        // the place block remove block thing is very ugly.
        // I hope to find a better way in the future
        grid.place_crbl_on_grid();
        for (sf::RectangleShape& cell : grid.val) {
            render_window.draw(cell);
        }
        grid.remove_crbl_of_grid();
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
                            grid.move_crbl_down_or_place();
                            break;
                        case sf::Keyboard::Left:
                        case sf::Keyboard::A:
                            grid.move_crbl(MOVE_LEFT);
                            break;
                        case sf::Keyboard::Right:
                        case sf::Keyboard::D:
                            grid.move_crbl(MOVE_RIGHT);
                            break;
                        case sf::Keyboard::Up:
                        case sf::Keyboard::W:
                            grid.rotate_block(1);
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
        grid.set_cells_positions(0, 0);
    }


private:
    ~Tretis() = default;
    Tretis() = default;
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(const Tretis&) = delete;
    static Tretis tretis;
};

// shut up I hate ODR
Tretis Tretis::tretis;
