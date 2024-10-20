#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

using Coo = sf::Vector2i;
using Block = std::array<Coo, 4>;
using BlockRotations = std::array<Block, 4>;

namespace Blocks {

const Block T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };

const Block T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };

const Block T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };

const Block T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };

const BlockRotations T { T_up, T_right, T_down, T_left };

const std::array<BlockRotations, 1> ALL_BLOCKS {{ T }};

const std::array<sf::Color, 1> ALL_BLOCKS_COLOR {{
    sf::Color::Magenta,
}};

}  // namespace Blocks
