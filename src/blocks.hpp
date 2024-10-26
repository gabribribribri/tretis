#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

using Coo = sf::Vector2i;
using Tretomino = std::array<Coo, 4>;
using BlockRotations = std::array<Tretomino, 4>;

using CenterOffSets = std::array<Coo, 5>;
using OneTretominoRotations = std::array<CenterOffSets, 8>;

/*
    0: T
*/

namespace SuperRotationSystem {

const CenterOffSets T_north_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };

const CenterOffSets T_north_to_east { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } } };

const CenterOffSets T_east_to_north { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } };

const CenterOffSets T_east_to_south { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } };

const CenterOffSets T_south_to_east { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } } };

const CenterOffSets T_south_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };

const CenterOffSets T_west_to_south { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { -2, 0 }, { -2, -1 } } };

const CenterOffSets T_west_to_north { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { -2, 0 }, { -2, -1 } } };

const OneTretominoRotations T { {
    T_north_to_west, T_north_to_east,  //
    T_east_to_north, T_east_to_south,  //
    T_south_to_east, T_south_to_west,  //
    T_west_to_south, T_west_to_north,  //
} };

const std::array<OneTretominoRotations, 1> ALL { { T } };

}  // namespace SuperRotationSystem

namespace Tretominos {

const Tretomino T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };

const Tretomino T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };

const Tretomino T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };

const Tretomino T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };

const BlockRotations T { T_up, T_right, T_down, T_left };

const std::array<BlockRotations, 1> ALL { { T } };

}  // namespace Tretominos

namespace BlockColors {

const std::array<sf::Color, 1> ALL { {
    sf::Color::Magenta,
} };

}
