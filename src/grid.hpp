#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <ranges>

#include "blocks.hpp"

using TretominoShape = std::array<sf::RectangleShape, 4>;

/// CONSTANTS ///
const int GRID_HEIGHT = 22;
const int GRID_WIDTH = 10;
const int CELL_SIZE = 30;
const Coo NEW_CRBL_INITIAL_CENTER_POSITION = Coo { 4, 1 };

const sf::Color EMPTY_CELL_COLOR = sf::Color::Black;
const sf::Color PHANTOM_BLOCK_COLOR = sf::Color(64, 64, 64);

const Coo MOVE_UP = Coo { 0, -1 };
const Coo MOVE_LEFT = Coo { -1, 0 };
const Coo MOVE_RIGHT = Coo { 1, 0 };
const Coo MOVE_DOWN = Coo { 0, 1 };

/// WHY IS RNG IN C++ SO AWFUL ///
inline std::random_device random_device;
inline std::mt19937 rng(random_device());
// I can't believe it's an inclusive range this is disgusting
inline std::uniform_int_distribution<int> allblock_distrib(0, Tretominos::ALL.size() - 1);

/// MODULUS WITH POSITIVE REMAINDER ///
int good_mod(int lhs, int rhs) {
    // Can't believe C++ doesn't have one of those
    int res = lhs % rhs;
    return res >= 0 ? res : res + rhs;
}

sf::RectangleShape create_grid_cell() {
    sf::RectangleShape cell {sf::Vector2f(CELL_SIZE, CELL_SIZE)};
    cell.setOutlineThickness(2.0);
    cell.setOutlineColor(sf::Color(32, 32, 32));
    return cell;
}

/// GRID ///
class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH> val;  // initialized in constructor

private:
    // CRBL means CURRENT_BLOCK, the block that is falling.
    Coo crbl_center; // in constructor
    Coo phbl_center; // in constructor

    TretominoShape crbl_shape; // in constructor
    TretominoShape phbl_shape; // in constructor

    int crbl_rotation = 0;
    int allblocks_index; // in constructor

    bool phantom_enabled = true;

public:
    TretominoRotation const& get_block_relative_cells(int rotation) const {
        return Tretominos::ALL[allblocks_index][rotation];
    }

    sf::Color get_block_color() const { return BlockColors::ALL[allblocks_index]; }

    void set_cells_positions() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                at(x, y).setPosition(sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
            }
        }
    }

    /// returns true if block has been placed, false if not
    bool move_crbl_down_or_place() {
        if (!move_crbl(MOVE_DOWN)) {
            place_crbl_on_grid();
            select_new_crbl();
            return true;
        }
        return false;
    }

    bool is_block_movable_to(Coo center, int rotation) {
        for (Coo block_cell : get_block_relative_cells(rotation)) {
            Coo new_pos = center + block_cell;
            if (new_pos.x < 0 or new_pos.x >= GRID_WIDTH or new_pos.y < 0 or new_pos.y >= GRID_HEIGHT) {
                return false;
            }
            if (at(new_pos).getFillColor() != EMPTY_CELL_COLOR) {
            std::cout << (int)at(new_pos).getFillColor().r << (int)at(new_pos).getFillColor().g << (int)at(new_pos).getFillColor().b << "\n";
            std::cout << "nuhuh\n";
                return false;
            }
        }
        return true;
    }

    // returns success
    bool move_crbl(Coo direction) {
        if (is_block_movable_to(crbl_center + direction, crbl_rotation)) {
            crbl_center += direction;
            adjust_crbl_shape_position();
            adjust_phbl();
            return true;
        }
        return false;
    }

    void super_rotate_block(bool clockwise) {
        int next_rotation = get_next_rotation(clockwise);
        for (const Coo offset : SuperRotationSystem::ALL[allblocks_index][crbl_rotation * 2 + clockwise]) {
            if (is_block_movable_to(crbl_center + offset, next_rotation)) {
                crbl_center += offset;
                crbl_rotation = next_rotation;
                adjust_crbl_shape_position();
                adjust_phbl();
                return;
            }
        }
    }

    int get_next_rotation(bool clockwise) { return good_mod(crbl_rotation + (clockwise ? 1 : -1), 4); }

    void select_new_crbl() {
        crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
        crbl_rotation = 0;
        allblocks_index = allblock_distrib(rng);
        adjust_crbl_shape_position();
        adjust_crbl_shape_color();
        adjust_phbl();
    }

    void adjust_crbl_shape_position() {
        for (auto [i,cell] : get_block_relative_cells(crbl_rotation) | std::ranges::views::enumerate) {
            // ugly but useful implicit int to float conversion
            crbl_shape[i].setPosition(sf::Vector2f(CELL_SIZE * (crbl_center.x + cell.x), CELL_SIZE * (crbl_center.y + cell.y)));
        }
    }

    void adjust_crbl_shape_color() {
        for(sf::RectangleShape& cell : crbl_shape) {
            cell.setFillColor(get_block_color());
        }
    }

    void adjust_phbl_shape_position() {
        for (auto [i, cell] : get_block_relative_cells(crbl_rotation) | std::ranges::views::enumerate) {
            // ugly but useful implicit int to float conversion
            phbl_shape[i].setPosition(sf::Vector2f(CELL_SIZE * (phbl_center.x + cell.x), CELL_SIZE * (phbl_center.y + cell.y)));
        }
    }


    void place_crbl_on_grid() {
        for (Coo cell : get_block_relative_cells(crbl_rotation)) {
            at(crbl_center + cell).setFillColor(get_block_color());
        }
    }

    std::array<sf::RectangleShape, 4> const& get_crbl_shapes() {
        return crbl_shape;
    }


    std::array<sf::RectangleShape, 4> const& get_phbl_shapes() {
        return phbl_shape;
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

    void adjust_phbl() {
        Coo potential_phbl_center { crbl_center.x, GRID_HEIGHT - 1 };
        while (!is_block_movable_to(potential_phbl_center, crbl_rotation)) {
            std::cout << "{ " << potential_phbl_center.x << ", " << potential_phbl_center.y << " }\n"; 
            potential_phbl_center.y -= 1;
        }
        phbl_center = potential_phbl_center;
        adjust_phbl_shape_position();
    }

    void switch_phantom_block() { phantom_enabled = !phantom_enabled; }

public:
    Grid(Grid const&) = delete;
    Grid(Grid&&) = delete;
    Grid operator=(Grid) = delete;

    static Grid& Get() {
        static Grid instance;
        return instance;
    }

private:
    Grid() {
        for (sf::RectangleShape& cell : val) {
            cell = create_grid_cell();
            cell.setFillColor(EMPTY_CELL_COLOR);
        }

        for (sf::RectangleShape& shape : crbl_shape) {
            shape = create_grid_cell();
        }

        for (sf::RectangleShape& shape : phbl_shape) {
            shape = create_grid_cell();
            shape.setFillColor(PHANTOM_BLOCK_COLOR);
        }

        select_new_crbl();
    }
    ~Grid() = default;
};
