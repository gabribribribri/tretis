#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>

#include "blocks.hpp"

const float REC_SIZE = 40;

const std::array<sf::Vector2f, TRETOMINO_COUNT> TRETOMINO_RENDER_OFFSET { {
    { REC_SIZE, REC_SIZE },
    { static_cast<float>(REC_SIZE * 0.5), static_cast<float>(REC_SIZE * 0.5) },
    { -static_cast<float>(REC_SIZE * 0.5), static_cast<float>(REC_SIZE * 1.5) },
    { REC_SIZE, REC_SIZE },
    { REC_SIZE, REC_SIZE },
    { REC_SIZE, REC_SIZE },
    { REC_SIZE, REC_SIZE },
} };

class TretominoRenderShape {
public:
    std::array<sf::RectangleShape, 4> shape;
    sf::Vector2f pos;

public:
    void set_tretomino(Tretomino tretomino) {
        for (int i = 0; i < 4; i++) {
            float x_pos = Tretominos::ALL[tretomino][0][i].x * REC_SIZE + TRETOMINO_RENDER_OFFSET[i].x;
            float y_pos = Tretominos::ALL[tretomino][0][i].y * REC_SIZE + TRETOMINO_RENDER_OFFSET[i].y;
            
            shape[i].setPosition({x_pos, y_pos});
            shape[i].setFillColor(Tretominos::ALL_COLORS[tretomino]);
        }
    }

    void set_origin(sf::Vector2f origin) {
        for (auto cell : shape) {
            cell.setOrigin(origin);
        }
    }

public:
    TretominoRenderShape() {
        for (sf::RectangleShape& shape : shape) {
            shape.setSize({REC_SIZE, REC_SIZE});
            shape.setOutlineColor(BETWEEN_CELL_LINE_COLOR);
            shape.setOutlineThickness(BETWEEN_CELL_LINE_THICKNESS);
        }
    };
};
