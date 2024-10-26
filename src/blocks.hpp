#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <ranges>

using Coo = sf::Vector2i;
using Tretomino = std::array<Coo, 4>;
using Rotations = std::array<Tretomino, 4>;

using CenterOffSets = std::array<Coo, 5>;
using OneTretominoRotations = std::array<CenterOffSets, 8>;

/*
    0: T
*/

CenterOffSets operator*(CenterOffSets lhs, Coo rhs) {
    CenterOffSets ret;
    for (auto [i, coo] : lhs | std::views::enumerate) {
        ret[i] = Coo { coo.x * rhs.x, coo.y * rhs.y };
    }
    return ret;
}

const Coo POSX_POSY { 1, 1};
const Coo NEGX_POSY { -1, 1 };
const Coo POSX_NEGY { 1, -1 };
const Coo NEGX_NEGY { -1, -1 };

namespace SuperRotationSystem {
const CenterOffSets T_north_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };  // --
// const CenterOffSets T_north_to_east { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } } }; // -x y
// const CenterOffSets T_east_to_north { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } };  // x -y
// const CenterOffSets T_east_to_south { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } };  // x -y
// const CenterOffSets T_south_to_east { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } } };  // -x y
// const CenterOffSets T_south_to_west { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } };  // x y
// const CenterOffSets T_west_to_south { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 } } };  // -x -y
// const CenterOffSets T_west_to_north { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 } } };  // -x -y

const OneTretominoRotations T { {
    T_north_to_west * POSX_POSY, T_north_to_west * NEGX_POSY,  //
    T_north_to_west * POSX_NEGY, T_north_to_west * POSX_NEGY,  //
    T_north_to_west * NEGX_POSY, T_north_to_west * POSX_POSY,  //
    T_north_to_west * NEGX_NEGY, T_north_to_west * NEGX_NEGY,  //
} };

const std::array<OneTretominoRotations, 1> ALL { { T } };

}  // namespace SuperRotationSystem

namespace Tretominos {

const Tretomino T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };

const Tretomino T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };

const Tretomino T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };

const Tretomino T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };

const Rotations T { T_up, T_right, T_down, T_left };

const std::array<Rotations, 1> ALL { { T } };

}  // namespace Tretominos

namespace BlockColors {

const std::array<sf::Color, 1> ALL { {
    sf::Color::Magenta,
} };

}
