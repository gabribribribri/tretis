#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <array>

#include "SFML/Graphics.hpp"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;

class Tretis {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH> grid;
    sf::RenderWindow render_window { sf::VideoMode(800, 1200), "Tretis" };

    static Tretis& Get() {
        return tretis;
    }

    void gameloop() {
        for (;;) {
            for (int y = 0; y < GRID_HEIGHT; y+=50) {
                for (int x = 0; x < GRID_WIDTH; x+=50) {
                    sf::RectangleShape& cell = grid[y*GRID_WIDTH+x];
                    cell.setPosition(sf::Vector2f(x*50, y*50));
                    cell.setFillColor(sf::Color::Blue);
                    cell.setSize(sf::Vector2f(50, 50));
                    render_window.draw(cell);
                }
            }
        }
    }

private:
    // ~Tretis() = delete;
    Tretis();
    Tretis(const Tretis&) = delete;
    Tretis(Tretis&&) = delete;
    void operator=(const Tretis&) = delete;
    static Tretis tretis;
};

Tretis Tretis::tretis;
