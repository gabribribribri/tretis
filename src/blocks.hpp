#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

using Coo = sf::Vector2i;
using Block = std::array<Coo, 4>;
using BlockRotations = std::array<Block, 4>;

using BlockCenterOffSets = std::array<Coo, 5>;
using LeftRightRotaBlockCenter = std::array<BlockCenterOffSets, 2>;
using BaseRotation = std::array<LeftRightRotaBlockCenter, 4>;


/*
    0: T
*/

namespace SuperRotationSystem {

const BlockCenterOffSets T_north_to_west {{
    {0, 0},
    {1, 0},
    {1, -1},
    {0, 2},
    {1, 2}
}};

const BlockCenterOffSets T_north_to_east {{
    {0, 0},
    {-1, 0},
    {-1, -1},
    {0, 2},
    {-1, 2}
}};

const BlockCenterOffSets T_east_to_north {{
    {0, 0},
    {1, 0},
    {1, 1},
    {0, -2},
    {1, -2},
}};

const BlockCenterOffSets T_east_to_south {{
    {0, 0},
    {1, 0},
    {1, 1},
    {0, -2},
    {1, -2},
}};

const BlockCenterOffSets T_south_to_east {{
    {0, 0},
    {-1, 0},
    {-1, -1},
    {0, 2},
    {-1, 2},
}};

const BlockCenterOffSets T_south_west {{
    {0, 0},
    {1, 0},
    {1, -1},
    {0, 2},
    {1, 2}
}};

const BaseRotation T {{
    {{ T_north_to_west, T_north_to_east}},
    {{ T_east_to_north, T_east_to_south}},
    {{T_south_to_east, T_south_west}},
    {{}}, 
}};

const std::array<BaseRotation, 1> ALL {{T }};
    
}

namespace BlockCoos {

const Block T_up { { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 0 } } };

const Block T_right { { { 0, -1 }, { 1, 0 }, { 0, 1 }, { 0, 0 } } };

const Block T_down { { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 0, 1 } } };

const Block T_left { { { -1, 0 }, { 0, 0 }, { 0, -1 }, { 0, 1 } } };

const BlockRotations T { T_up, T_right, T_down, T_left };

const std::array<BlockRotations, 1> ALL {{ T }};

}  // namespace Blocks

namespace BlockColors {

const std::array<sf::Color, 1> ALL {{
    sf::Color::Magenta,
}};

}

