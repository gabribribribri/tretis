#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Cursor.hpp>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>

#include "blocks.hpp"
#include "selection.hpp"

/// MODULUS WITH POSITIVE REMAINDER ///
int pos_rem_mod(int lhs, int rhs) {
    // Can't believe C++ doesn't have one of those
    int res = lhs % rhs;
    return res >= 0 ? res : res + rhs;
}

sf::RectangleShape create_grid_cell() {
    sf::RectangleShape cell { sf::Vector2f(CELL_SIZE, CELL_SIZE) };
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
    Tretomino allblocks_index;

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
        return Tretominos::ALL_COLORS[allblocks_index];
    }

    void set_cells_positions() {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                grid_at(x, y).setOrigin(GRID_ORIGIN);
                grid_at(x, y).setPosition(
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
            if (grid_at(new_pos).getFillColor() != EMPTY_CELL_COLOR) {
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
        return pos_rem_mod(crbl_rotation + (clockwise ? 1 : -1), 4);
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
        // Placing the block
        uint64_t modified_lines_index_mask = place_crbl_on_grid();
        potential_clear_modified_lines(modified_lines_index_mask);
        select_new_crbl(std::nullopt);
        Selection::Get().hold_locked = false;  // Releasing the "once per drop" hold lock

        // Readjusting the grid shapes...
        // (I wish I found a better option than this... is messy)
        crbl_shape_center = crbl_center;
        crbl_shape_rotation = crbl_rotation;
        adjust_crbl_shape_position();
        adjust_phbl_center();
        adjust_phbl_shape_position();
    }

    // returns a 64 bits (just in case) bitmask of every line index that has
    // been modified
    //  0b0000000000000000000000000000000000000000000000000000000000000000
    //    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^---------------------^
    //                                             useless from here     y=0 of
    //                                             grid because height=22
    //                                             because 1<<0
    uint64_t place_crbl_on_grid() {
        uint64_t modified_lines_index_mask = 0b0;
        for (Coo cell_relative_position :
             get_block_relative_cells(crbl_rotation)) {
            sf::Vector2i cell_absolute_position =
                crbl_center + cell_relative_position;
            modified_lines_index_mask |= 0b1 << cell_absolute_position.y;
            grid_at(cell_absolute_position).setFillColor(get_block_color());
        }
        return modified_lines_index_mask;
    }

    void select_new_crbl(std::optional<Tretomino> tretomino) {
        crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
        crbl_rotation = 0;
        if (tretomino.has_value()) {
            allblocks_index = *tretomino;
        } else {
            allblocks_index = Selection::Get().next_tretomino();
        }
    }

    void hold_crbl_ifnlocked() {
        Selection& selec = Selection::Get();
        if (!selec.hold_locked) {
            auto new_current = selec.replace_hold_tretomino(allblocks_index);
            select_new_crbl(new_current);
            selec.hold_locked = true;
        }
    }

    void potential_clear_modified_lines(uint64_t modified_lines_index_mask) {
        uint64_t lines_to_clear_index_mask = 0b0;
        for (uint8_t y_index = 0; y_index < GRID_HEIGHT; y_index++) {
            if ((modified_lines_index_mask & (0b1 << y_index)) != 0) {
                if (is_line_full(y_index)) {
                    lines_to_clear_index_mask |= 0b1 << y_index;
                }
            }
        }
        if (lines_to_clear_index_mask != 0) {
            clear_lines(lines_to_clear_index_mask);
        }
    }

    bool is_line_full(int y_index) {
        for (int x_slider = 0; x_slider < GRID_WIDTH; x_slider++) {
            if (grid_at(x_slider, y_index).getFillColor() == EMPTY_CELL_COLOR) {
                return false;
            }
        }
        return true;
    }

    void clear_lines(uint64_t lines_to_clear_index_mask) {
        std::cout << "Woaw ! We are going to clear some liiiiines\n";
        int lines_to_clear_under = 0;
        for (int y_index = GRID_HEIGHT - 1; y_index >= 0; y_index--) {
            if ((lines_to_clear_index_mask & (0b1 << y_index)) != 0) {
                lines_to_clear_under += 1;
            } else if (lines_to_clear_under != 0) {
                move_line(y_index, y_index + lines_to_clear_under);
            }
        }
    }

    void move_line(int from, int to) {
        for (int x_index = 0; x_index < GRID_WIDTH; x_index++) {
            grid_at(x_index, to)
                .setFillColor(grid_at(x_index, from).getFillColor());
        }
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
        Coo potential_phbl_center = crbl_shape_center;
        while (is_block_movable_to(potential_phbl_center + MOVE_DOWN,
                                   crbl_shape_rotation)) {
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
        return crbl_shape;
    }

    // THERE IS A CALL TO adjust_everything_if_moved EEEVERY FRAME ????
    // THIS IS UNACCEPTABLE AND NEED TO BE PATCHED !!!!
    // I WILL PUT IT IN  THE TODO LIST
    std::array<sf::RectangleShape, 4> const& get_phbl_shapes() {
        return phbl_shape;
    }

    sf::RectangleShape& grid_at(int x, int y) {
        assert(x >= 0);
        assert(x < GRID_WIDTH);
        assert(y >= 0);
        assert(y < GRID_HEIGHT);
        return val[y * GRID_WIDTH + x];
    }

    sf::RectangleShape& grid_at(Coo coo) {
        assert(coo.x >= 0);
        assert(coo.x < GRID_WIDTH);
        assert(coo.y >= 0);
        assert(coo.y < GRID_HEIGHT);
        return val[coo.y * GRID_WIDTH + coo.x];
    }

    void switch_phantom_block() { phantom_enabled = !phantom_enabled; }
    bool is_phantom_enabled() const { return phantom_enabled; }

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
        select_new_crbl(std::nullopt);
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
