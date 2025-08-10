#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Cursor.hpp>
#include <array>
#include <cassert>
#include <cstdint>

#include "blocks.hpp"

/// GRID ///
class Grid {
public:
    std::array<sf::RectangleShape, GRID_HEIGHT * GRID_WIDTH>
        val;  // initialized in constructor

    bool hard_drop_locked = false;

private:
    // CRBL means CURRENT_BLOCK, the block that is falling.
    Coo crbl_center;
    TretominoGridShape crbl_shape;
    Coo crbl_shape_center;
    int crbl_shape_rotation;

    // PHBL means PHANTOM_BLOCK
    Coo phbl_center;
    sf::Color crbl_shape_color;
    TretominoGridShape phbl_shape;

    // phbl use crbl_rotation because always the same
    int crbl_rotation = 0;
    Tretomino crbl_tretomino;

    bool phantom_enabled = true;

public:
    TretominoRotation const& get_block_relative_cells(int rotation) const;

    sf::Color get_crbl_color() const;

    bool is_block_movable_to(Coo center, int rotation);

    // returns success
    bool move_crbl(Coo direction);

    void super_rotate_block(bool clockwise);

    int get_next_rotation(bool clockwise);

    /// returns true if block has been placed, false if not
    bool move_crbl_down_or_place();

    void hard_drop();

    void place_and_select_crbl();

    // returns a 64 bits (just in case) bitmask of every line index that has
    // been modified
    //  0b0000000000000000000000000000000000000000000000000000000000000000
    //    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^---------------------^
    //                                             useless from here     y=0 of
    //                                             grid because height=22
    //                                             because 1<<0
    uint64_t place_crbl_on_grid();

    void select_new_crbl(std::optional<Tretomino> tretomino);

    void hold_crbl_ifnlocked();

    // returns number of lines that have been cleared
    int clear_full_modified_lines(uint64_t modified_lines_index_mask);

    bool is_line_full(int y_index);

    void clear_lines(uint64_t lines_to_clear_index_mask);

    void move_line(int from, int to);

    void create_score_report(uint8_t num_cleared_lines);

    void adjust_crbl_shape_position();

    void adjust_crbl_shape_color();

    /// DOESN'T MODIFY phbl_center
    /// Put phbl_shape at the correct place
    void adjust_phbl_shape_position();

    void adjust_phbl_center();

    void adjust_everything_if_moved();

    void adjust_everything_new_crbl();

    std::array<sf::RectangleShape, 4> const& get_crbl_shapes() const;

    std::array<sf::RectangleShape, 4> const& get_phbl_shapes() const;

    sf::RectangleShape& grid_at(int x, int y);

    sf::RectangleShape& grid_at(Coo coo);

    void switch_phantom_block();
    bool is_phantom_enabled() const;

public:
    Grid(Grid const&) = delete;
    Grid(Grid&&) = delete;
    Grid operator=(Grid) = delete;

    static Grid& Get();

private:
    Grid();
    ~Grid() = default;
};
