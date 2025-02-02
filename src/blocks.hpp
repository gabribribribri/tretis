#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <ranges>

using Coo = sf::Vector2i;
using TretominoRotation = std::array<Coo, 4>;
using AllTretominoRotations = std::array<TretominoRotation, 4>;

using CenterOffSets = std::array<Coo, 5>;
using OneTretominoRotations = std::array<CenterOffSets, 8>;

/// CONSTANTS ///



// Grid logic related
const int GRID_HEIGHT = 22;
const int GRID_WIDTH = 10;
const Coo NEW_CRBL_INITIAL_CENTER_POSITION = Coo { 4, 1 };

// Game Rendering related
const sf::Vector2f GRID_ORIGIN = sf::Vector2f(-50.0, -50.0);
const int CELL_SIZE = 35;
const sf::Vector2f GAME_DELIMITER_SIZE = sf::Vector2f(720, 870);
const float BETWEEN_CELL_LINE_THICKNESS = 2.0;
const float GAME_DELIMITER_LINE_THICHNESS = 4.0;

// Colors
const sf::Color EMPTY_CELL_COLOR = sf::Color::Black;
const sf::Color PHANTOM_BLOCK_COLOR = sf::Color(64, 64, 64);
const sf::Color BETWEEN_CELL_LINE_COLOR = sf::Color(32, 32, 32);

// Movements
const Coo MOVE_UP = Coo { 0, -1 };
const Coo MOVE_LEFT = Coo { -1, 0 };
const Coo MOVE_RIGHT = Coo { 1, 0 };
const Coo MOVE_DOWN = Coo { 0, 1 };

/*
    0: T
    1: O
    2: I
    3: L
    4: J
    5: S
    6: Z
*/

const int TRETOMINO_COUNT = 7;

CenterOffSets operator*(CenterOffSets lhs, Coo rhs) {
    CenterOffSets ret;
    for (auto [i, coo] : lhs | std::views::enumerate) {
        ret[i] = Coo { coo.x * rhs.x, coo.y * rhs.y };
    }
    return ret;
}

const Coo POSX_POSY { 1, 1 };
const Coo NEGX_POSY { -1, 1 };
const Coo POSX_NEGY { 1, -1 };
const Coo NEGX_NEGY { -1, -1 };

namespace SuperRotationSystem {

const CenterOffSets T_north_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };
OneTretominoRotations const& T { {
    T_north_to_west * POSX_POSY, T_north_to_west* NEGX_POSY,  //
    T_north_to_west* POSX_NEGY, T_north_to_west* POSX_NEGY,   //
    T_north_to_west* NEGX_POSY, T_north_to_west* POSX_POSY,   //
    T_north_to_west* NEGX_NEGY, T_north_to_west* NEGX_NEGY,   //
} };

const CenterOffSets O_north_to_west { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
OneTretominoRotations const& O { {
    O_north_to_west, O_north_to_west,  //
    O_north_to_west, O_north_to_west,  //
    O_north_to_west, O_north_to_west,  //
    O_north_to_west, O_north_to_west,  //
} };

const CenterOffSets I_north_to_west { { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, -2 }, { 1, 2 } } };
const CenterOffSets I_north_to_east { { { 1, 0 }, { -1, 0 }, { 2, 0 }, { -1, 1 }, { 2, -2 } } };
const CenterOffSets I_east_to_north { { { -1, 0 }, { 1, 0 }, { -2, 0 }, { 1, -1 }, { -2, 2 } } };
const CenterOffSets I_east_to_south { { { -1, 1 }, { -2, 1 }, { 1, 1 }, { -2, -1 }, { 1, 2 } } };
const CenterOffSets I_south_to_east { { { 1, -1 }, { 2, -1 }, { -1, -1 }, { -2, -1 }, { -1, -2 } } };
const CenterOffSets I_south_to_west { { { 0, -1 }, { 2, -1 }, { -1, -1 }, { 2, -2 }, { -1, 1 } } };
const CenterOffSets I_west_to_south { { { 0, 1 }, { -2, 1 }, { 1, 1 }, { -2, 2 }, { 1, -1 } } };
const CenterOffSets I_west_to_north { { { 0, 0 }, { 1, 0 }, { -2, 0 }, { 1, 2 }, { -2, -1 } } };
OneTretominoRotations const& I { {
    I_north_to_west, I_north_to_east,  //
    I_east_to_north, I_east_to_south,  //
    I_south_to_east, I_south_to_west,  //
    I_west_to_south, I_west_to_north,  //

} };

const CenterOffSets L_north_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };
OneTretominoRotations const& L { {
    L_north_to_west * POSX_POSY, L_north_to_west* NEGX_POSY,  //
    L_north_to_west* POSX_NEGY, L_north_to_west* POSX_NEGY,   //
    L_north_to_west* NEGX_POSY, L_north_to_west* POSX_POSY,   //
    L_north_to_west* NEGX_NEGY, L_north_to_west* NEGX_NEGY,   //
} };

const std::array<OneTretominoRotations, TRETOMINO_COUNT> ALL { {
    T,  //
    O,  //
    I,  //
    L,  //
    L,  // J has the same pattern as L
    L,  // S has the same pattern as L
    L,  // Z has the same pattern as L
} };

}  // namespace SuperRotationSystem

namespace Tretominos {

const TretominoRotation T_north { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };
const TretominoRotation T_east { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };
const TretominoRotation T_south { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };
const TretominoRotation T_west { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };
AllTretominoRotations const& T { T_north, T_east, T_south, T_west };

const TretominoRotation O_north { { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } };
AllTretominoRotations const& O { { O_north, O_north, O_north, O_north } };

const TretominoRotation I_north { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } } };
const TretominoRotation I_east { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } };
AllTretominoRotations const& I { { I_north, I_east, I_north, I_east } };

const TretominoRotation L_north { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, -1 } } };
const TretominoRotation L_east { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } } };
const TretominoRotation L_south { { { 1, 0 }, { 0, 0 }, { -1, 0 }, { -1, 1 } } };
const TretominoRotation L_west { { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, -1 } } };
AllTretominoRotations const& L { { L_north, L_east, L_south, L_west } };

const TretominoRotation J_north { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, -1 } } };
const TretominoRotation J_east { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, -1 } } };
const TretominoRotation J_south { { { 1, 0 }, { 0, 0 }, { -1, 0 }, { 1, 1 } } };
const TretominoRotation J_west { { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, 1 } } };
AllTretominoRotations const& J { { J_north, J_east, J_south, J_west } };

const TretominoRotation S_north { { { 0, 0 }, { -1, 0 }, { 0, -1 }, { 1, -1 } } };
const TretominoRotation S_east { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -1 } } };
const TretominoRotation S_south { { { 0, 0 }, { 1, 0 }, { 0, 1 }, { -1, 1 } } };
const TretominoRotation S_west { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 1 } } };
AllTretominoRotations const& S { { S_north, S_east, S_south, S_west } };

const TretominoRotation Z_north { { { 0, 0 }, { 0, -1 }, { -1, -1 }, { 1, 0 } } };
const TretominoRotation Z_east { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 1 } } };
const TretominoRotation Z_south { { { 0, 0 }, { -1, 0 }, { 0, 1 }, { 1, 1 } } };
const TretominoRotation Z_west { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -1 } } };
AllTretominoRotations const& Z { { Z_north, Z_east, Z_south, Z_west } };

const std::array<AllTretominoRotations, TRETOMINO_COUNT> ALL { {
    T,
    O,
    I,
    L,
    J,
    S,
    Z,
} };

}  // namespace Tretominos

namespace BlockColors {

const std::array<sf::Color, TRETOMINO_COUNT> ALL { {
    sf::Color::Magenta,
    sf::Color::Yellow,
    sf::Color::Cyan,
    sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Blue,
    sf::Color::Green,
    sf::Color::Red,
} };

}
