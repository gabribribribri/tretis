#include "grid.hpp"
#include <algorithm>
#include "blocks.hpp"
#include "logging.hpp"
#include "movements.hpp"
#include "score.hpp"
#include "selection.hpp"

namespace {

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

bool is_in_grid(Coo coo) {
    return coo.x >= 0 and coo.x < GRID_WIDTH and coo.y >= 0 and coo.y < GRID_HEIGHT;
}

}  // namespace

// NOLINTNEXTLINE(cert-err58-cpp)
std::array<sf::Vector2i, 4> const T_SPIN_RECOGNITION_PATTERN = {
    sf::Vector2i(-1, -1),
    sf::Vector2i(+1, -1),
    sf::Vector2i(+1, +1),
    sf::Vector2i(-1, +1)
};

TretominoRotation const& Grid::get_block_relative_cells(int rotation) const {
    return Tretominos::ALL.at(crbl_tretomino).at(rotation);
}

sf::Color Grid::get_crbl_color() const {
    return Tretominos::ALL_COLORS.at(crbl_tretomino);
}

bool Grid::is_block_movable_to(Coo center, int rotation) {
    auto const& cells = get_block_relative_cells(rotation);
    // if ONE OF THEM is true, then we return false
    return not std::ranges::any_of(cells.begin(), cells.end(), [center, this](Coo block_cell) {
        return not is_in_grid(center + block_cell) or
               grid_at(center + block_cell).getFillColor() != EMPTY_CELL_COLOR;
    });
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
    using namespace std::ranges;
    int next_rotation = get_next_rotation(clockwise);
    CenterOffsets center_offsets = SuperRotationSystem::ALL.at(crbl_tretomino).at((crbl_rotation * 2) + static_cast<int>(clockwise));
    for (const auto [rotation_point, offset] : center_offsets | views::enumerate) {
        if (is_block_movable_to(crbl_center + offset, next_rotation)) {
            crbl_center += offset;
            crbl_rotation = next_rotation;
            Log::Debug("Rotation to ", next_rotation, " with offset ", offset.x, " ", offset.y);

            // Checks for T-Spins
            if (crbl_tretomino == Tretomino::T) {
                Score::Get().did_t_just_rotate();
                if (rotation_point == 4) {
                    Score::Get().did_t_rotation_point_5();
                }
            }
            return;
        }
    }
}

int Grid::get_next_rotation(bool clockwise) const {
    return pos_rem_mod(crbl_rotation + (clockwise ? 1 : -1), 4);
}

bool Grid::move_crbl_down_or_place() {
    if (not move_crbl(MOVE_DOWN)) {
        place_and_select_crbl();
        return true;
    }
    return false;
}

void Grid::hard_drop() {
    Score::Get().add_hard_drop(phbl_center.y - crbl_center.y);
    Movements::Get().restart_crbl_fall_by_one_countdown();
    crbl_center = phbl_center;
}

void Grid::hard_drop_ifnlocked() {
    if (Movements::Get().is_hard_drop_locked()) {
        return;
    }
    Movements::Get().set_hard_drop_lock(true);
    Movements::Get().restart_crbl_fall_by_one_countdown();
    Grid::Get().hard_drop();
}

void Grid::place_and_select_crbl() {
    // Placing the block, getting back modified lines
    uint64_t modified_lines_index_mask = place_crbl_on_grid();

    // We shall detect if some T-Spin happened
    detect_t_spins();

    // Clear full modified lines, getting back how many
    uint8_t num_cleared_lines = clear_full_modified_lines(modified_lines_index_mask);

    Score::Get().report_score(num_cleared_lines);

    // Selecting new current block
    select_new_crbl(std::nullopt);
}

uint64_t Grid::place_crbl_on_grid() {
    uint64_t modified_lines_index_mask = 0b0;
    for (Coo cell_relative_position : get_block_relative_cells(crbl_rotation)) {
        sf::Vector2i cell_absolute_position = crbl_center + cell_relative_position;
        modified_lines_index_mask |= 0b1 << cell_absolute_position.y;
        grid_at(cell_absolute_position).setFillColor(get_crbl_color());
    }
    return modified_lines_index_mask;
}

void Grid::select_new_crbl(std::optional<Tretomino> tretomino) {
    Log::Debug("New crbl selected");

    crbl_center = NEW_CRBL_INITIAL_CENTER_POSITION;
    crbl_rotation = 0;
    if (tretomino.has_value()) {
        crbl_tretomino = *tretomino;
    } else {
        crbl_tretomino = Selection::Get().next_tretomino();
    }

    // Release "once per block" hold lock
    Selection::Get().hold_locked = false;
    // Release hold lock
    Movements::Get().set_hard_drop_lock(false);
    // Restart fall by one countdown
    Movements::Get().restart_crbl_fall_by_one_countdown();

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

void Grid::hold_crbl_ifnlocked() {
    Selection& selec = Selection::Get();
    if (selec.hold_locked) {
        return;
    }
    auto new_current = selec.replace_hold_tretomino(crbl_tretomino);
    select_new_crbl(new_current);
    selec.hold_locked = true;
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
        grid_at(x_index, to).setFillColor(grid_at(x_index, from).getFillColor());
    }
}

void Grid::detect_t_spins() {
    if (crbl_tretomino != Tretomino::T) {
        return;
    }

    // Absolutely unreadable but too tired to make it better
    bool a_side = grid_at(crbl_center + T_SPIN_RECOGNITION_PATTERN.at((crbl_rotation + 0) % 4)).getFillColor() != EMPTY_CELL_COLOR;
    bool b_side = grid_at(crbl_center + T_SPIN_RECOGNITION_PATTERN.at((crbl_rotation + 1) % 4)).getFillColor() != EMPTY_CELL_COLOR;
    //!\\ C and D are inverted because C is bottom left and D is bottom right
    Coo d_side_coo = crbl_center + T_SPIN_RECOGNITION_PATTERN.at((crbl_rotation + 2) % 4);
    bool d_side = is_in_grid(d_side_coo) ? grid_at(d_side_coo).getFillColor() != EMPTY_CELL_COLOR : true;
    Coo c_side_coo = crbl_center + T_SPIN_RECOGNITION_PATTERN.at((crbl_rotation + 3) % 4);
    bool c_side = is_in_grid(c_side_coo) ? grid_at(c_side_coo).getFillColor() != EMPTY_CELL_COLOR : true;

    Log::Warn("a_side=", a_side, " b_side=", b_side, " c_side=", c_side, " d_side=", d_side);

    if (a_side and b_side and (c_side or d_side)) {
        Log::Warn("T-Spin side acted !");
        Score::Get().did_t_spin_sides();
        return;
    }

    if (c_side and d_side and (a_side or b_side)) {
        Log::Warn("Mini T-Spin side acted !");
        Score::Get().did_t_spin_mini_sides();
        return;
    }
}

void Grid::adjust_crbl_shape_position() {
    for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) | std::ranges::views::enumerate) {
        auto pos_x = static_cast<float>(CELL_SIZE * (crbl_shape_center.x + cell.x));
        auto pos_y = static_cast<float>(CELL_SIZE * (crbl_shape_center.y + cell.y));
        crbl_shape.at(i).setPosition(sf::Vector2f(pos_x, pos_y));
    }
}

void Grid::adjust_crbl_shape_color() {
    assert(crbl_shape_color == get_crbl_color());
    for (sf::RectangleShape& cell : crbl_shape) {
        cell.setFillColor(get_crbl_color());
    }
}

void Grid::adjust_phbl_shape_position() {
    for (auto [i, cell] : get_block_relative_cells(crbl_shape_rotation) | std::ranges::views::enumerate) {
        auto pos_x = static_cast<float>(CELL_SIZE * (phbl_center.x + cell.x));
        auto pos_y = static_cast<float>(CELL_SIZE * (phbl_center.y + cell.y));
        phbl_shape.at(i).setPosition(sf::Vector2f(pos_x, pos_y));
    }
}

void Grid::adjust_phbl_center() {
    assert(crbl_rotation == crbl_shape_rotation);
    assert(crbl_center == crbl_shape_center);
    Coo potential_phbl_center = crbl_shape_center;
    while (is_block_movable_to(potential_phbl_center + MOVE_DOWN, crbl_shape_rotation)) {
        potential_phbl_center.y += 1;
    }
    phbl_center = potential_phbl_center;
    Log::Debug("Adjusting Phantom Block center at x=", phbl_center.x, " to y=", phbl_center.y);
}

void Grid::adjust_everything_if_moved() {
    if (crbl_shape_rotation != crbl_rotation or crbl_center.x != crbl_shape_center.x) {
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

GridData const& Grid::get_data() const {
    return data;
}

GridData& Grid::get_data() {
    return data;
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
    return data.at((y * GRID_WIDTH) + x);
}

sf::RectangleShape& Grid::grid_at(Coo coo) {
    assert(coo.x >= 0);
    assert(coo.x < GRID_WIDTH);
    assert(coo.y >= 0);
    assert(coo.y < GRID_HEIGHT);
    return data.at((coo.y * GRID_WIDTH) + coo.x);
}

void Grid::switch_phantom_block() { phantom_enabled = not phantom_enabled; }
bool Grid::is_phantom_enabled() const { return phantom_enabled; }

Grid& Grid::Get() {
    static Grid instance;
    return instance;
}

Grid::Grid() {
    // Init every cell of the grid
    for (sf::RectangleShape& cell : data) {
        cell = create_grid_cell();
        cell.setFillColor(EMPTY_CELL_COLOR);
    }

    // Set Cells positions
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid_at(x, y).setOrigin(GRID_ORIGIN);
            auto x_pos = static_cast<float>(x * CELL_SIZE);
            auto y_pos = static_cast<float>(y * CELL_SIZE);
            grid_at(x, y).setPosition(sf::Vector2f(x_pos, y_pos));
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
