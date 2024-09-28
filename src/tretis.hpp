#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <array>
#include <iostream>

#include "time.hpp"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;

class Tretis {
public:
    static Tretis& Get() { return tretis; }

    void gameloop() {
        while (render_window.isOpen()) {
            print_fps();
            render_window.clear(sf::Color::Black);
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
        for (auto& cell : grid) {
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
                    resizeWindow(event.size.width, event.size.height);
                    break;
                default:
                    break;
            }
        }
    }

    void resizeWindow(int width, int height) {
        sf::FloatRect visibleArea(0, 0, width, height);
        render_window.setView(sf::View(visibleArea));
        set_cells_positions(0, 0);
    }

    void set_cells_positions(int x_offset, int y_offset) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                grid_at(x, y).setPosition(sf::Vector2f(
                    x_offset + x * CELL_SIZE, y_offset + y * CELL_SIZE));
            }
        }
    }

    sf::RectangleShape& grid_at(int x, int y) {
        return grid[y * GRID_WIDTH + x];
    }

private:
    TimeHandler frame_time { TIME_PER_FRAME };

public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH>
        grid;  // initialized in constructor
    sf::RenderWindow render_window { sf::VideoMode(1200, 1200), "Tretis" };

private:
    ~Tretis() = default;
    Tretis() {
        for (auto& cell : grid) {
            cell = sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        }
    };
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    Tretis& operator=(const Tretis&) = delete;

    static Tretis tretis;
};

// shut up I hate ODR
Tretis Tretis::tretis;
