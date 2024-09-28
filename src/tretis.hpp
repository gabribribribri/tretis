#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <array>
#include <iostream>


const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;

class Tretis {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH> grid;
    sf::RenderWindow render_window { sf::VideoMode(800, 1200), "Tretis" };

    static Tretis& Get() {
        return tretis;
    }

    void gameloop() {
        for (;;) {
            for (int y = 0; y < GRID_HEIGHT ; y++) {
                for (int x = 0; x < GRID_WIDTH; x++) {
                    sf::RectangleShape& cell = grid[y*GRID_WIDTH+x];
                    cell = sf::RectangleShape(sf::Vector2f(50, 50));
                    cell.setPosition(sf::Vector2f(x*CELL_SIZE, y*CELL_SIZE));
                    cell.setFillColor(sf::Color::Blue);
                    render_window.draw(cell);
                }
            }
            render_window.display();
        }
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
