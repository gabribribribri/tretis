#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <ranges>

using Coo = sf::Vector2i;
using TretominoRotation = std::array<Coo, 4>;
using AllTretominoRotations = std::array<TretominoRotation, 4>;

using CenterOffSets = std::array<Coo, 5>;
using OneTretominoRotations = std::array<CenterOffSets, 8>;

using TretominoGridShape = std::array<sf::RectangleShape, 4>;

/// CONSTANTS ///

// I can't fix the SFML constructors, so
// NOLINTBEGIN(cert-err58-cpp)

// Grid logic related

// MUST NOT EXCEED 64 because of bitshifting dark
// magic happening in clearing lines process
const int GRID_HEIGHT = 22;
const int GRID_WIDTH = 10;
const Coo NEW_CRBL_INITIAL_CENTER_POSITION = Coo { 4, 1 }; 

// Game Rendering related
const int LEFT_OFFSET = 320;
const sf::Vector2f GRID_ORIGIN = sf::Vector2f(-1320.0, -200.0); 
const int CELL_SIZE = 140;
const sf::Vector2f GAME_DELIMITER_SIZE = sf::Vector2f(3920, 3480);
const float BETWEEN_CELL_LINE_THICKNESS = 8.0;
const float GAME_DELIMITER_LINE_THICHNESS = 16.0;
const sf::Vector2f HOLD_PIECE_DELIMITER_SIZE = sf::Vector2f(680, 680);
const sf::Vector2f HOLD_PIECE_DELIMITER_POS =
    sf::Vector2f(LEFT_OFFSET, 200 + BETWEEN_CELL_LINE_THICKNESS);
const sf::Vector2f NEXT_QUEUE_POS = sf::Vector2f(3000, 200);
const float NEXT_QUEUE_HEIGHT = 3080;

// Text related
const int TITLE_SIZE = 80;
const int VALUE_SIZE = 132;
const sf::Vector2f TEXT_POS = sf::Vector2f(
    LEFT_OFFSET, HOLD_PIECE_DELIMITER_SIZE.y + HOLD_PIECE_DELIMITER_POS.y + 80);

// Colors
const sf::Color EMPTY_CELL_COLOR = sf::Color(0, 0, 0);
const sf::Color PHANTOM_BLOCK_COLOR = sf::Color(64, 64, 64);
const sf::Color BETWEEN_CELL_LINE_COLOR = sf::Color(32, 32, 32);

// Movements
const Coo MOVE_UP = Coo { 0, -1 };
const Coo MOVE_LEFT = Coo { -1, 0 };
const Coo MOVE_RIGHT = Coo { 1, 0 };
const Coo MOVE_DOWN = Coo { 0, 1 };

enum Tretomino : std::uint8_t {
    T = 0,  // Magenta
    O = 1,  // Yellow
    I = 2,  // Cyan
    L = 3,  // Orange
    J = 4,  // Blue
    S = 5,  // Green
    Z = 6,  // Red
};

const int TRETOMINO_COUNT = 7;

constexpr CenterOffSets operator*(CenterOffSets lhs, Coo rhs) noexcept {
    CenterOffSets ret;
    for (auto [i, coo] : lhs | std::views::enumerate) {
        ret.at(i) = Coo { coo.x * rhs.x, coo.y * rhs.y };
    }
    return ret;
}

const Coo POSX_POSY { 1, 1 };
const Coo NEGX_POSY { -1, 1 };
const Coo POSX_NEGY { 1, -1 };
const Coo NEGX_NEGY { -1, -1 };

namespace SuperRotationSystem {

const CenterOffSets T_NORTH_TO_WEST { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };

const OneTretominoRotations T { {
    T_NORTH_TO_WEST * POSX_POSY,
    T_NORTH_TO_WEST* NEGX_POSY,  //
    T_NORTH_TO_WEST* POSX_NEGY,
    T_NORTH_TO_WEST* POSX_NEGY,  //
    T_NORTH_TO_WEST* NEGX_POSY,
    T_NORTH_TO_WEST* POSX_POSY,  //
    T_NORTH_TO_WEST* NEGX_NEGY,
    T_NORTH_TO_WEST* NEGX_NEGY,  //
} };

const CenterOffSets O_NORTH_TO_WEST { { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };

const OneTretominoRotations O { {
    O_NORTH_TO_WEST,
    O_NORTH_TO_WEST,  //
    O_NORTH_TO_WEST,
    O_NORTH_TO_WEST,  //
    O_NORTH_TO_WEST,
    O_NORTH_TO_WEST,  //
    O_NORTH_TO_WEST,
    O_NORTH_TO_WEST,  //
} };

const CenterOffSets I_NORTH_TO_WEST {
    { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, -2 }, { 1, 2 } }
};
const CenterOffSets I_NORTH_TO_EAST {
    { { 1, 0 }, { -1, 0 }, { 2, 0 }, { -1, 1 }, { 2, -2 } }
};
const CenterOffSets I_EAST_TO_NORTH {
    { { -1, 0 }, { 1, 0 }, { -2, 0 }, { 1, -1 }, { -2, 2 } }
};
const CenterOffSets I_EAST_TO_SOUTH {
    { { -1, 1 }, { -2, 1 }, { 1, 1 }, { -2, -1 }, { 1, 2 } }
};
const CenterOffSets I_SOUTH_TO_EAST {
    { { 1, -1 }, { 2, -1 }, { -1, -1 }, { -2, -1 }, { -1, -2 } }
};
const CenterOffSets I_SOUTH_TO_WEST {
    { { 0, -1 }, { 2, -1 }, { -1, -1 }, { 2, -2 }, { -1, 1 } }
};
const CenterOffSets I_WEST_TO_SOUTH {
    { { 0, 1 }, { -2, 1 }, { 1, 1 }, { -2, 2 }, { 1, -1 } }
};
const CenterOffSets I_WEST_TO_NORTH {
    { { 0, 0 }, { 1, 0 }, { -2, 0 }, { 1, 2 }, { -2, -1 } }
};

const OneTretominoRotations I { {
    I_NORTH_TO_WEST,
    I_NORTH_TO_EAST,  //
    I_EAST_TO_NORTH,
    I_EAST_TO_SOUTH,  //
    I_SOUTH_TO_EAST,
    I_SOUTH_TO_WEST,  //
    I_WEST_TO_SOUTH,
    I_WEST_TO_NORTH,  //

} };

const CenterOffSets L_NORTH_TO_WEST { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };

const OneTretominoRotations L { {
    L_NORTH_TO_WEST * POSX_POSY,
    L_NORTH_TO_WEST* NEGX_POSY,  //
    L_NORTH_TO_WEST* POSX_NEGY,
    L_NORTH_TO_WEST* POSX_NEGY,  //
    L_NORTH_TO_WEST* NEGX_POSY,
    L_NORTH_TO_WEST* POSX_POSY,  //
    L_NORTH_TO_WEST* NEGX_NEGY,
    L_NORTH_TO_WEST* NEGX_NEGY,  //
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

const TretominoRotation T_NORTH { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };
const TretominoRotation T_EAST { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };
const TretominoRotation T_SOUTH { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };
const TretominoRotation T_WEST { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };
const AllTretominoRotations T { { T_NORTH, T_EAST, T_SOUTH, T_WEST } };

const TretominoRotation O_NORTH { { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } };
const AllTretominoRotations O { { O_NORTH, O_NORTH, O_NORTH, O_NORTH } };

const TretominoRotation I_NORTH { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } } };
const TretominoRotation I_EAST { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } };
const AllTretominoRotations I { { I_NORTH, I_EAST, I_NORTH, I_EAST } };

const TretominoRotation L_NORTH { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 1, -1 } } };
const TretominoRotation L_EAST { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, 1 } } };
const TretominoRotation L_SOUTH { { { 1, 0 }, { 0, 0 }, { -1, 0 }, { -1, 1 } } };
const TretominoRotation L_WEST { { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, -1 } } };
const AllTretominoRotations L { { L_NORTH, L_EAST, L_SOUTH, L_WEST } };

const TretominoRotation J_NORTH { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, -1 } } };
const TretominoRotation J_EAST { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 1, -1 } } };
const TretominoRotation J_SOUTH { { { 1, 0 }, { 0, 0 }, { -1, 0 }, { 1, 1 } } };
const TretominoRotation J_WEST { { { 0, 1 }, { 0, 0 }, { 0, -1 }, { -1, 1 } } };
const AllTretominoRotations J { { J_NORTH, J_EAST, J_SOUTH, J_WEST } };

const TretominoRotation S_NORTH { { { 0, 0 }, { -1, 0 }, { 0, -1 }, { 1, -1 } } };
const TretominoRotation S_EAST { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -1 } } };
const TretominoRotation S_SOUTH { { { 0, 0 }, { 1, 0 }, { 0, 1 }, { -1, 1 } } };
const TretominoRotation S_WEST { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 1 } } };
const AllTretominoRotations S { { S_NORTH, S_EAST, S_SOUTH, S_WEST } };

const TretominoRotation Z_NORTH { { { 0, 0 }, { 0, -1 }, { -1, -1 }, { 1, 0 } } };
const TretominoRotation Z_EAST { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 1 } } };
const TretominoRotation Z_SOUTH { { { 0, 0 }, { -1, 0 }, { 0, 1 }, { 1, 1 } } };
const TretominoRotation Z_WEST { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -1 } } };
const AllTretominoRotations Z { { Z_NORTH, Z_EAST, Z_SOUTH, Z_WEST } };

const std::array<AllTretominoRotations, TRETOMINO_COUNT> ALL { {
    T,
    O,
    I,
    L,
    J,
    S,
    Z,
} };

const std::array<sf::Color, TRETOMINO_COUNT> ALL_COLORS { {
    sf::Color(255, 0, 255),    // Magenta
    sf::Color(255, 255, 0),    // Yellow
    sf::Color(0, 255, 255),    // Cyan
    sf::Color(0xff, 0x7f, 0),  // Orange
    sf::Color(0, 0, 255),      // Blue
    sf::Color(0, 255, 0),      // Green
    sf::Color(255, 0, 0),      // Red
} };

}  // namespace Tretominos

// NOLINTEND(cert-err58-cpp)
