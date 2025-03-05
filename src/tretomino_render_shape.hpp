#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

#include "blocks.hpp"

constexpr float REC_SIZE = 40;

// T  0  Magenta
// O  1  Yellow
// I  2  Cyan
// L  3  Orange
// J  4  Blue
// S  5  Green
// Z  6  Red

const std::array<sf::Vector2f, TRETOMINO_COUNT> TRETOMINO_RENDER_OFFSET { {
    { REC_SIZE * 1.7, REC_SIZE * 1.9 },
    { REC_SIZE * 1.2, REC_SIZE * 1.1 },
    { REC_SIZE * 1.1, REC_SIZE * 1.7 },
    { REC_SIZE * 1.6, REC_SIZE * 2.0 },
    { REC_SIZE * 1.7, REC_SIZE * 2.1 },
    { REC_SIZE * 1.8, REC_SIZE * 2.1 },
    { REC_SIZE * 1.7, REC_SIZE * 2.2 },
} };

class TretominoRenderShape {
public:
    std::array<sf::RectangleShape, 4> shape;
    sf::Vector2f pos;

public:
    void set_tretomino(Tretomino tretomino) {
        for (int i = 0; i < 4; i++) {
            float x_pos = Tretominos::ALL[tretomino][0][i].x * REC_SIZE +
                          TRETOMINO_RENDER_OFFSET[tretomino].x;
            float y_pos = Tretominos::ALL[tretomino][0][i].y * REC_SIZE +
                          TRETOMINO_RENDER_OFFSET[tretomino].y;

            shape[i].setPosition({ x_pos, y_pos });
            shape[i].setFillColor(Tretominos::ALL_COLORS[tretomino]);
        }
    }

    void set_origin(sf::Vector2f origin) {
        for (sf::RectangleShape& cell : shape) {
            cell.setOrigin(origin);
        }
    }

public:
    TretominoRenderShape() {
        for (sf::RectangleShape& shape : shape) {
            shape.setSize({
                REC_SIZE - BETWEEN_CELL_LINE_THICKNESS,
                REC_SIZE - BETWEEN_CELL_LINE_THICKNESS,
            });
            shape.setOutlineColor(BETWEEN_CELL_LINE_COLOR);
            shape.setOutlineThickness(BETWEEN_CELL_LINE_THICKNESS);
        }
    };
};
