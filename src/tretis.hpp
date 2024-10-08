#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>

#include "grid.hpp"
#include "time.hpp"

class Tretis {
public:
    Grid grid;
    sf::RenderWindow render_window { sf::VideoMode(1200, 1200), "Tretis" };

private:
    TimeHandler frame_time { TIME_PER_FRAME };

public:
    static Tretis& Get() { return tretis; }

    void gameloop() {
        while (render_window.isOpen()) {
            print_fps();
            render_window.clear(sf::Color(64, 64,64));
            handle_events();
            draw_grid();
            render_window.display();
            frame_time.wait_until_time_has_passed();
        }
    }

    void print_fps() {
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
        grid.place_block();
        for (sf::RectangleShape& cell : grid.val) {
            render_window.draw(cell);
        }
        grid.remove_block();
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
