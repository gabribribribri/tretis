#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cassert>
#include <iostream>
#include <random>

#include "blocks.hpp"

const int GRID_HEIGHT = 22;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;
const Coo NEW_CRBL_INITIAL_CENTER_POSITION = Coo { 4, 1 };

const sf::Color EMPTY_CELL_COLOR = sf::Color::Black;

const Coo MOVE_UP = Coo { 0, -1 };
const Coo MOVE_LEFT = Coo { -1, 0 };
const Coo MOVE_RIGHT = Coo { 1, 0 };
const Coo MOVE_DOWN = Coo { 0, 1 };

// WHY IS RNG IN C++ SO AWFUL
std::random_device random_device;
std::mt19937 rng(random_device());
// I can't believe it's an inclusive range this is disgusting
std::uniform_int_distribution<> allblock_distrib(0, BlockCoos::ALL.size() - 1);

class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH> val;  // initialized in constructor

    // CRBL means CURRENT_BLOCK, the block that is falling.
    Coo crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;

    // 0: UP
    // 1: RIGHT
    // 2: DOWN
    // 3: LEFT TODO: Make this shit less suckable
    int crbl_rotation = 0;
    int allblocks_index = 0;

public:
    Grid() {
        for (sf::RectangleShape& cell : val) {
            cell = sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            cell.setFillColor(sf::Color::Black);
        }
    }

    Block get_block_relative_cells() const { return BlockCoos::ALL[allblocks_index][crbl_rotation]; }

    sf::Color get_block_color() const { return BlockColors::ALL[allblocks_index]; }

    void set_cells_positions(int x_offset, int y_offset) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                at(x, y).setPosition(sf::Vector2f(x_offset + x * CELL_SIZE, y_offset + y * CELL_SIZE));
            }
        }
    }

    void move_crbl_down_or_place() {
        if (!move_crbl(MOVE_DOWN)) {
            place_crbl_on_grid();
            select_new_crbl();
        }
    }

    bool is_crbl_movable_to_relative(Coo direction) {
        for (Coo block_cell : get_block_relative_cells()) {
            Coo new_pos = crbl_center + block_cell + direction;
            if (new_pos.x < 0 or new_pos.x >= GRID_WIDTH or new_pos.y < 0 or new_pos.y >= GRID_HEIGHT) {
                return false;
            }
            if (at(new_pos).getFillColor() != EMPTY_CELL_COLOR) {
                return false;
            }
        }
        return true;
    }

    // returns success
    bool move_crbl(Coo direction) {
        if (is_crbl_movable_to_relative(direction)) {
            crbl_center += direction;
            return true;
        }
        return false;
    }

    void rotate_block(int rotation) {
        
    }


    void select_new_crbl() {
        crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
        crbl_rotation = 0;
        // since .size() is like 7, it is accessing
        // very ugly memory and breaking everything
        allblocks_index = allblock_distrib(rng);
    }

    void place_crbl_on_grid() {
        for (Coo cell : get_block_relative_cells()) {
            at(crbl_center + cell).setFillColor(get_block_color());
        }
    }

    void remove_crbl_of_grid() {
        for (Coo cell : get_block_relative_cells()) {
            at(crbl_center + cell).setFillColor(EMPTY_CELL_COLOR);
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
