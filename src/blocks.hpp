#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>

using Coo = sf::Vector2i;
using Block = std::array<Coo, 4>;
using Rotations = std::array<Block, 4>;

namespace Blocks {

const Block T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };

const Block T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };

const Block T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, -1 } } };

const Block T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };

const Rotations T { T_up, T_right, T_down, T_left };
}  // namespace Blocks
