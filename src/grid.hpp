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

/// WHY IS RNG IN C++ SO AWFUL ///
inline std::random_device random_device;
inline std::mt19937 rng(random_device());
// I can't believe it's an inclusive range this is disgusting
inline std::uniform_int_distribution<int> allblock_distrib(
    0, Tretominos::ALL.size() - 1);

/// MODULUS WITH POSITIVE REMAINDER ///
int good_mod(int lhs, int rhs) {
    // Can't believe C++ doesn't have one of those
    int res = lhs % rhs;
    return res >= 0 ? res : res + rhs;
}

sf::RectangleShape create_grid_cell() {
    sf::RectangleShape cell { sf::Vector2f(CELL_SIZE, CELL_SIZE) };
    /// Outline has been removed
    // cell.setOutlineThickness(2.0);
    // cell.setOutlineColor(sf::Color(32, 32, 32));
    return cell;
}

/// GRID ///
class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH>
        val;  // initialized in constructor

private:
    // CRBL means CURRENT_BLOCK, the block that is falling.
    Coo crbl_center;  
    // PHBL means PHANTOM_BLOCK
    Coo phbl_center;  

    // phbl use crbl_rotation because always the same
    int crbl_rotation = 0;
    int allblocks_index;  

    TretominoShape crbl_shape;  
    Coo crbl_shape_center;
    int crbl_shape_rotation;
    sf::Color crbl_shape_color;
    TretominoShape phbl_shape;  

    bool phantom_enabled = true;

public:
    TretominoRotation const& get_block_relative_cells(int rotation) const {
        return Tretominos::ALL[allblocks_index][rotation];
    }

    sf::Color get_block_color() const {
        return BlockColors::ALL[allblocks_index];
    }

    void set_cells_positions() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                at(x, y).setOrigin(GRID_ORIGIN);
                at(x, y).setPosition(
                    sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
            }
        }
    }

    bool is_block_movable_to(Coo center, int rotation) {
        for (Coo block_cell : get_block_relative_cells(rotation)) {
            Coo new_pos = center + block_cell;
            if (new_pos.x < 0 or new_pos.x >= GRID_WIDTH or new_pos.y < 0 or
                new_pos.y >= GRID_HEIGHT) {
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
        if (is_block_movable_to(crbl_center + direction, crbl_rotation)) {
            crbl_center += direction;
            return true;
        }
        return false;
    }

    void super_rotate_block(bool clockwise) {
        std::cout << "SUPER ROTATION SYSTEM\n";
        int next_rotation = get_next_rotation(clockwise);
        for (const Coo offset :
             SuperRotationSystem::ALL[allblocks_index]
                                     [crbl_rotation * 2 + clockwise]) {
            if (is_block_movable_to(crbl_center + offset, next_rotation)) {
                crbl_center += offset;
                crbl_rotation = next_rotation;
                return;
            }
        }
    }

    int get_next_rotation(bool clockwise) {
        return good_mod(crbl_rotation + (clockwise ? 1 : -1), 4);
    }

    /// returns true if block has been placed, false if not
    bool move_crbl_down_or_place() {
        if (!move_crbl(MOVE_DOWN)) {
            place_and_select_crbl();
            return true;
        }
        return false;
    }

    void hard_drop() {
        crbl_center = phbl_center;
        place_and_select_crbl();
    }

    void place_and_select_crbl() {
        place_crbl_on_grid();
        select_new_crbl();
        crbl_shape_center = crbl_center;
        crbl_shape_rotation = crbl_rotation;
        adjust_crbl_shape_position();
        adjust_phbl_center();
        adjust_phbl_shape_position();
    }

    void place_crbl_on_grid() {
        for (Coo cell : get_block_relative_cells(crbl_rotation)) {
            at(crbl_center + cell).setFillColor(get_block_color());
        }
    }

    void select_new_crbl() {
        crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
        crbl_rotation = 0;
        allblocks_index = allblock_distrib(rng);
    }

    void adjust_crbl_shape_position() {
        for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) |
                                  std::ranges::views::enumerate) {
            // ugly but useful implicit int to float conversion
            crbl_shape[i].setPosition(
                sf::Vector2f(CELL_SIZE * (crbl_shape_center.x + cell.x),
                             CELL_SIZE * (crbl_shape_center.y + cell.y)));
        }
    }

    void adjust_crbl_shape_color() {
        assert(crbl_shape_color == get_block_color());
        for (sf::RectangleShape& cell : crbl_shape) {
            cell.setFillColor(get_block_color());
        }
    }

    /// DOESN'T MODIFY phbl_center
    /// Put phbl_shape at the correct place
    void adjust_phbl_shape_position() {
        for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) |
                                  std::ranges::views::enumerate) {
            // ugly but useful implicit int to float conversion
            phbl_shape[i].setPosition(
                sf::Vector2f(CELL_SIZE * (phbl_center.x + cell.x),
                             CELL_SIZE * (phbl_center.y + cell.y)));
        }
    }

    void adjust_phbl_center() {
        std::cout << "ADJUSTING PHBL CENTER\n";
        assert(crbl_rotation == crbl_shape_rotation);
        assert(crbl_center == crbl_shape_center);
        Coo potential_phbl_center { crbl_shape_center.x, 0 };
        while (
            is_block_movable_to(potential_phbl_center + MOVE_DOWN, crbl_shape_rotation)) {
            potential_phbl_center.y += 1;
        }
        phbl_center = potential_phbl_center;
    }

    void adjust_everything_if_moved() {
        if (crbl_shape_rotation != crbl_rotation or
            crbl_center.x != crbl_shape_center.x) {
            crbl_shape_center = crbl_center;
            crbl_shape_rotation = crbl_rotation;
            adjust_crbl_shape_position();
            adjust_phbl_center();
            adjust_phbl_shape_position();
        } else if (crbl_shape_center != crbl_center) {
            crbl_shape_center = crbl_center;
            adjust_crbl_shape_position();
        }
        if (get_block_color() != crbl_shape_color) {
            crbl_shape_color = get_block_color();
            adjust_crbl_shape_color();
        }
    }

    std::array<sf::RectangleShape, 4> const& get_crbl_shapes() {
        adjust_everything_if_moved();
        return crbl_shape;
    }

    std::array<sf::RectangleShape, 4> const& get_phbl_shapes() {
        adjust_everything_if_moved();
        return phbl_shape;
    }

    sf::RectangleShape& at(int x, int y) {
        assert(x >= 0 and x < GRID_WIDTH);
        assert(y >= 0 and y < GRID_HEIGHT);
        return val[y * GRID_WIDTH + x];
    }

    sf::RectangleShape& at(Coo coo) {
        assert(coo.x >= 0);
        assert(coo.x < GRID_WIDTH);
        assert(coo.y >= 0);
        assert(coo.y < GRID_HEIGHT);
        return val[coo.y * GRID_WIDTH + coo.x];
    }

    void switch_phantom_block() { phantom_enabled = !phantom_enabled; }
    bool is_phantom_enabled() { return phantom_enabled; }

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
        // Init every cell of the grid
        for (sf::RectangleShape& cell : val) {
            cell = create_grid_cell();
            cell.setFillColor(EMPTY_CELL_COLOR);
        }
        set_cells_positions();

        // Init the current block
        for (sf::RectangleShape& cell : crbl_shape) {
            cell = create_grid_cell();
            cell.setOrigin(GRID_ORIGIN);
        }
        select_new_crbl();
        crbl_shape_color = get_block_color();
        adjust_crbl_shape_color();
        crbl_shape_center = crbl_center;
        crbl_shape_rotation = crbl_rotation;
        adjust_crbl_shape_position();

        // Init the phantom block
        for (sf::RectangleShape& cell : phbl_shape) {
            cell = create_grid_cell();
            cell.setFillColor(PHANTOM_BLOCK_COLOR);
            cell.setOrigin(GRID_ORIGIN);
        }
        adjust_phbl_center();
        adjust_phbl_shape_position();
    }
    ~Grid() = default;
};
