#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cassert>
#include <iostream>
#include <random>

#include "blocks.hpp"

const int GRID_HEIGHT = 20;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;
const Coo CENTER_START_POINT = Coo { 4, 1 };

const sf::Color EMPTY_CELL_COLOR = sf::Color::Black;

const Coo UP = Coo { 0, -1 };
const Coo LEFT = Coo { -1, 0 };
const Coo RIGHT = Coo { 1, 0 };
const Coo DOWN = Coo { 0, 1 };

// why is it static ?
static std::random_device random_device;
static std::mt19937 rng(random_device());
static std::uniform_int_distribution<> allblock_distrib(
    0, Blocks::ALL_BLOCKS.size()-1);

class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH>
        val;  // initialized in constructor

    Coo block_center = CENTER_START_POINT;
    // 0: UP
    // 1: RIGHT
    // 2: DOWN
    // 3: LEFT TODO: Make this shit less suckable
    int rotation = 0;
    int allblocks_index = 0;

public:
    Grid() {
        for (sf::RectangleShape& cell : val) {
            cell = sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setFillColor(sf::Color::Black);
        }
    }

    Block get_block_relative_cells() {
        return Blocks::ALL_BLOCKS[allblocks_index][rotation];
    }

    sf::Color get_block_color() { return Blocks::BLOCK_COLOR[allblocks_index]; }

    void set_cells_positions(int x_offset, int y_offset) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                at(x, y).setPosition(sf::Vector2f(x_offset + x * CELL_SIZE,
                                                  y_offset + y * CELL_SIZE));
            }
        }
    }

    bool is_block_movable_to(Coo direction) {
        for (Coo block_cell : get_block_relative_cells()) {
            Coo new_pos = block_center + block_cell + direction;
            if (new_pos.x < 0 || new_pos.x >= GRID_WIDTH || new_pos.y < 0 ||
                new_pos.y >= GRID_HEIGHT) {
                return false;
            }
            if (at(new_pos).getFillColor() != EMPTY_CELL_COLOR) {
                return false;
            }
        }
        return true;
    }

    void move_block_down() {
        if (is_block_movable_to(DOWN)) {
            block_center.y += 1;
        } else {
            place_block();
            select_new_block();
        }
    }

    void select_new_block() {
        block_center = CENTER_START_POINT;
        rotation = 0;
        allblocks_index = allblock_distrib(rng); // since .size() is like 7, it is accessing very ugly memory and breaking everything
    }

    void place_block() {
        for (Coo cell : get_block_relative_cells()) {
            at(block_center + cell).setFillColor(get_block_color());
        }
    }

    void remove_block() {
        for (Coo cell : get_block_relative_cells()) {
            at(block_center + cell).setFillColor(EMPTY_CELL_COLOR);
        }
    }

    sf::RectangleShape& at(int x, int y) {
        assert(x >= 0 && x < GRID_WIDTH);
        assert(y >= 0 && y < GRID_HEIGHT);
        return val[y * GRID_WIDTH + x];
    }

    sf::RectangleShape& at(Coo coo) {
        assert(coo.x >= 0);
        assert(coo.x < GRID_WIDTH);
        assert(coo.y >= 0);
        assert(coo.y < GRID_HEIGHT);
        return val[coo.y * GRID_WIDTH + coo.x];
    }
};
