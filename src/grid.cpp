#include "grid.hpp"
#include "logging.hpp"
#include "score.hpp"
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

std::array<sf::Vector2i, 4> const T_SPIN_RECOGNITION_PATTERN = {
    sf::Vector2i(-1, -1),  //
    sf::Vector2i(+1, -1),  //
    sf::Vector2i(+1, +1),  //
    sf::Vector2i(-1, +1)   //
};

TretominoRotation const& Grid::get_block_relative_cells(int rotation) const {
    return Tretominos::ALL[crbl_tretomino][rotation];
}

sf::Color Grid::get_crbl_color() const {
    return Tretominos::ALL_COLORS[crbl_tretomino];
}

bool Grid::is_block_movable_to(Coo center, int rotation) {
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


bool Grid::move_crbl(Coo direction) {
    if (is_block_movable_to(crbl_center + direction, crbl_rotation)) {
        crbl_center += direction;
        Score::Get().did_just_move();
        return true;
    }
    return false;
}

void Grid::super_rotate_block(bool clockwise) {
    // I just read this and even though right now I understand it, good luck
    // for anyone else or me in 2 years to figure out what the fuck is going
    // on, tips : it's pretty smart.
    int next_rotation = get_next_rotation(clockwise);
    for (const auto [rotation_point, offset] :
         SuperRotationSystem::ALL[crbl_tretomino]
                                 [crbl_rotation * 2 + clockwise] |
             std::ranges::views::enumerate) {
        if (is_block_movable_to(crbl_center + offset, next_rotation)) {
            crbl_center += offset;
            crbl_rotation = next_rotation;
            Log::Debug("Rotation to ", next_rotation, " with offset ",
                       offset.x, " ", offset.y);
            if (crbl_tretomino == Tretomino::T) {
                Score::Get().did_just_rotate();
                if (rotation_point == 4) {
                    Score::Get().using_rotation_point_5();
                }
            }
            return;
        }
    }
}

int Grid::get_next_rotation(bool clockwise) {
    return pos_rem_mod(crbl_rotation + (clockwise ? 1 : -1), 4);
}

bool Grid::move_crbl_down_or_place() {
    if (!move_crbl(MOVE_DOWN)) {
        place_and_select_crbl();
        return true;
    }
    return false;
}

void Grid::hard_drop() {
    Score::Get().add_hard_drop(phbl_center.y - crbl_center.y);
    crbl_center = phbl_center;
}


void Grid::place_and_select_crbl() {
    // Placing the block, getting back modified lines
    uint64_t modified_lines_index_mask = place_crbl_on_grid();

    // Clear full modified lines, getting back how many
    uint8_t num_cleared_lines =
        clear_full_modified_lines(modified_lines_index_mask);

    // Checking for T-Spins and adding score
    create_score_report(num_cleared_lines);

    // Selecting new current block
    select_new_crbl(std::nullopt);

    // Releasing "once per block" hold lock
    // and hard drop locks
    Selection::Get().hold_locked = false;
    hard_drop_locked = false;
}

uint64_t Grid::place_crbl_on_grid() {
    uint64_t modified_lines_index_mask = 0b0;
    for (Coo cell_relative_position :
         get_block_relative_cells(crbl_rotation)) {
        sf::Vector2i cell_absolute_position =
            crbl_center + cell_relative_position;
        modified_lines_index_mask |= 0b1 << cell_absolute_position.y;
        grid_at(cell_absolute_position).setFillColor(get_crbl_color());
    }
    return modified_lines_index_mask;
}


void Grid::select_new_crbl(std::optional<Tretomino> tretomino) {
    crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
    crbl_rotation = 0;
    if (tretomino.has_value()) {
        crbl_tretomino = *tretomino;
    } else {
        crbl_tretomino = Selection::Get().next_tretomino();
    }
    adjust_everything_new_crbl();
}

void Grid::hold_crbl_ifnlocked() {
    Selection& selec = Selection::Get();
    if (!selec.hold_locked) {
        auto new_current = selec.replace_hold_tretomino(crbl_tretomino);
        select_new_crbl(new_current);
        selec.hold_locked = true;
        hard_drop_locked = false;  // Ah Ah ! Did not think of that !
    }
}

int Grid::clear_full_modified_lines(uint64_t modified_lines_index_mask) {
    int number_of_cleared_lines = 0;
    uint64_t lines_to_clear_index_mask = 0b0;
    for (uint8_t y_index = 0; y_index < GRID_HEIGHT; y_index++) {
        if ((modified_lines_index_mask & (0b1 << y_index)) != 0) {
            if (is_line_full(y_index)) {
                number_of_cleared_lines += 1;
                lines_to_clear_index_mask |= 0b1 << y_index;
            }
        }
    }
    if (lines_to_clear_index_mask != 0) {
        clear_lines(lines_to_clear_index_mask);
    }
    return number_of_cleared_lines;
}

bool Grid::is_line_full(int y_index) {
    for (int x_slider = 0; x_slider < GRID_WIDTH; x_slider++) {
        if (grid_at(x_slider, y_index).getFillColor() == EMPTY_CELL_COLOR) {
            return false;
        }
    }
    return true;
}

void Grid::clear_lines(uint64_t lines_to_clear_index_mask) {
    Log::Debug("Lines to clear :", lines_to_clear_index_mask);
    int lines_to_clear_under = 0;
    for (int y_index = GRID_HEIGHT - 1; y_index >= 0; y_index--) {
        if ((lines_to_clear_index_mask & (0b1 << y_index)) != 0) {
            lines_to_clear_under += 1;
        } else if (lines_to_clear_under != 0) {
            move_line(y_index, y_index + lines_to_clear_under);
        }
    }
}

void Grid::move_line(int from, int to) {
    for (int x_index = 0; x_index < GRID_WIDTH; x_index++) {
        grid_at(x_index, to)
            .setFillColor(grid_at(x_index, from).getFillColor());
    }
}

void Grid::create_score_report(uint8_t num_cleared_lines) {
    if (crbl_tretomino == Tretomino::T) {
        //!\\ C and D are inverted because C is bottom left and D is bottom
        //! right
        bool a_side =
            grid_at(crbl_center +
                    T_SPIN_RECOGNITION_PATTERN[(crbl_rotation + 0) % 4])
                .getFillColor() != EMPTY_CELL_COLOR;
        bool b_side =
            grid_at(crbl_center +
                    T_SPIN_RECOGNITION_PATTERN[(crbl_rotation + 1) % 4])
                .getFillColor() != EMPTY_CELL_COLOR;
        Coo d_side_coo =
            crbl_center +
            T_SPIN_RECOGNITION_PATTERN[(crbl_rotation + 2) % 4];
        bool d_side =
            (d_side_coo.x >= 0 and d_side_coo.x < GRID_WIDTH and
             d_side_coo.y >= 0 and d_side_coo.y < GRID_HEIGHT)
                ? grid_at(d_side_coo).getFillColor() != EMPTY_CELL_COLOR
                : true;
        Coo c_side_coo =
            crbl_center +
            T_SPIN_RECOGNITION_PATTERN[(crbl_rotation + 3) % 4];
        bool c_side =
            (c_side_coo.x >= 0 and c_side_coo.x < GRID_WIDTH and
             c_side_coo.y >= 0 and c_side_coo.y < GRID_HEIGHT)
                ? grid_at(c_side_coo).getFillColor() != EMPTY_CELL_COLOR
                : true;

        Log::Warn("a_side=", a_side, " b_side=", b_side,
                  " c_side=", c_side, " d_side=", d_side);

        if (a_side and b_side and (c_side or d_side)) {
            Log::Warn("T-Spin side acted !");
            Score::Get().report_score(num_cleared_lines, true, false);
            return;
        }

        if (c_side and d_side and (a_side or b_side)) {
            Log::Warn("Mini T-Spin side acted !");
            Score::Get().report_score(num_cleared_lines, false, true);
            return;
        }
    }

    if (num_cleared_lines != 0) {
        Score::Get().report_score(num_cleared_lines, false, false);
    }
}

void Grid::adjust_crbl_shape_position() {
    for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) |
                              std::ranges::views::enumerate) {
        // ugly but useful implicit int to float conversion
        crbl_shape[i].setPosition(
            sf::Vector2f(CELL_SIZE * (crbl_shape_center.x + cell.x),
                         CELL_SIZE * (crbl_shape_center.y + cell.y)));
    }
}

void Grid::adjust_crbl_shape_color() {
    assert(crbl_shape_color == get_crbl_color());
    for (sf::RectangleShape& cell : crbl_shape) {
        cell.setFillColor(get_crbl_color());
    }
}

void Grid::adjust_phbl_shape_position() {
    for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) |
                              std::ranges::views::enumerate) {
        // ugly but useful implicit int to float conversion
        phbl_shape[i].setPosition(
            sf::Vector2f(CELL_SIZE * (phbl_center.x + cell.x),
                         CELL_SIZE * (phbl_center.y + cell.y)));
    }
}

void Grid::adjust_phbl_center() {
    assert(crbl_rotation == crbl_shape_rotation);
    assert(crbl_center == crbl_shape_center);
    Coo potential_phbl_center = crbl_shape_center;
    while (is_block_movable_to(potential_phbl_center + MOVE_DOWN,
                               crbl_shape_rotation)) {
        potential_phbl_center.y += 1;
    }
    phbl_center = potential_phbl_center;
    Log::Debug("Adjusting Phantom Block center at x=", phbl_center.x,
               " to y=", phbl_center.y);
}


void Grid::adjust_everything_if_moved() {
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
}

void Grid::adjust_everything_new_crbl() {
    Log::Debug("New crbl selected : Tretomino ", crbl_tretomino);
    // crbl color adjustement
    crbl_shape_color = get_crbl_color();
    adjust_crbl_shape_color();
    // crbl shape adjustement
    crbl_shape_center = crbl_center;
    crbl_shape_rotation = crbl_rotation;
    adjust_crbl_shape_position();
    // phbl adjustement (need crbl adjustement)
    adjust_phbl_center();
    adjust_phbl_shape_position();
}

std::array<sf::RectangleShape, 4> const& Grid::get_crbl_shapes() const {
    return crbl_shape;
}

std::array<sf::RectangleShape, 4> const& Grid::get_phbl_shapes() const {
    return phbl_shape;
}

sf::RectangleShape& Grid::grid_at(int x, int y) {
    assert(x >= 0);
    assert(x < GRID_WIDTH);
    assert(y >= 0);
    assert(y < GRID_HEIGHT);
    return val[y * GRID_WIDTH + x];
}

sf::RectangleShape& Grid::grid_at(Coo coo) {
    assert(coo.x >= 0);
    assert(coo.x < GRID_WIDTH);
    assert(coo.y >= 0);
    assert(coo.y < GRID_HEIGHT);
    return val[coo.y * GRID_WIDTH + coo.x];
}

void Grid::switch_phantom_block() { phantom_enabled = not phantom_enabled; }
bool Grid::is_phantom_enabled() const { return phantom_enabled; }

Grid& Grid::Get() {
    static Grid instance;
    return instance;
}
    
Grid::Grid() {
    // Init every cell of the grid
    for (sf::RectangleShape& cell : val) {
        cell = create_grid_cell();
        cell.setFillColor(EMPTY_CELL_COLOR);
    }

    // Set Cells positions
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid_at(x, y).setOrigin(GRID_ORIGIN);
            grid_at(x, y).setPosition(
                sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE));
        }
    }

    // Init the current block
    for (sf::RectangleShape& cell : crbl_shape) {
        cell = create_grid_cell();
        cell.setOrigin(GRID_ORIGIN);
    }

    // Init the phantom block
    for (sf::RectangleShape& cell : phbl_shape) {
        cell = create_grid_cell();
        cell.setFillColor(PHANTOM_BLOCK_COLOR);
        cell.setOrigin(GRID_ORIGIN);
    }

    select_new_crbl(std::nullopt);
}
