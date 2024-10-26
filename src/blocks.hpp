#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <ranges>

using Coo = sf::Vector2i;
using TretominoRotation = std::array<Coo, 4>;
using AllTretominoRotations = std::array<TretominoRotation, 4>;

using CenterOffSets = std::array<Coo, 5>;
using OneTretominoRotations = std::array<CenterOffSets, 8>;

/*
    0: T
    1: O
    2: I
*/
const int TRETOMINO_COUNT = 3;

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

const std::array<OneTretominoRotations, TRETOMINO_COUNT> ALL { { T, O, I } };

}  // namespace SuperRotationSystem

namespace Tretominos {

const TretominoRotation T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };
const TretominoRotation T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };
const TretominoRotation T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };
const TretominoRotation T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };
AllTretominoRotations const& T { T_up, T_right, T_down, T_left };

const TretominoRotation O_up { { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } };
AllTretominoRotations const& O { { O_up, O_up, O_up, O_up } };

const TretominoRotation I_up { { { 0, -1 }, { 0, 0 }, { 0, 1 }, { 0, 2 } } };
const TretominoRotation I_right { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } } };
AllTretominoRotations const& I { { I_up, I_right, I_up, I_right } };

const std::array<AllTretominoRotations, TRETOMINO_COUNT> ALL { { T, O, I } };

}  // namespace Tretominos

namespace BlockColors {

const std::array<sf::Color, TRETOMINO_COUNT> ALL { { sf::Color::Magenta, sf::Color::Yellow, sf::Color::Cyan } };

}
