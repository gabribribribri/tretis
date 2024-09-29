#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>

#include "blocks.hpp"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;

class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH>
        val;  // initialized in constructor

    Coo center = Coo { 4, 1 };
    int rotation = 0;
    const Rotations& block_cells;
    sf::Color block_color = sf::Color::Blue;

public:
    Grid() : block_cells(Blocks::T) {
        for (sf::RectangleShape& cell : val) {
            cell = sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setFillColor(sf::Color::Black);
        }
    }

    void set_cells_positions(int x_offset, int y_offset) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                at(x, y).setPosition(sf::Vector2f(x_offset + x * CELL_SIZE,
                                                  y_offset + y * CELL_SIZE));
            }
        }
    }

    void place_block() {
        for (Coo cell : block_cells[rotation] ) {
            at(center.x+cell.x,center.y+cell.y).setFillColor(block_color);
        }
    }


    void remove_block() {
        for (Coo cell : block_cells[rotation] ) {
            at(center.x+cell.x,center.y+cell.y).setFillColor(sf::Color::Black);
        }
        
    }

    sf::RectangleShape& at(int x, int y) { 
        return val[y * GRID_WIDTH + x];
    }
};
