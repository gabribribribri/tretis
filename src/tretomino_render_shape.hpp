#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

#include "blocks.hpp"

// T  0  Magenta
// O  1  Yellow
// I  2  Cyan
// L  3  Orange
// J  4  Blue
// S  5  Green
// Z  6  Red

class TretominoRenderShape {
public:
    std::array<sf::RectangleShape, 4> shape;

public:
    void set_tretomino(Tretomino tretomino);

    void set_origin(sf::Vector2f origin);

public:
    TretominoRenderShape();
};
