#include "tretomino_render_shape.hpp"

constexpr float REC_SIZE = 160;

std::array<sf::Vector2f, TRETOMINO_COUNT> const TRETOMINO_RENDER_OFFSET { {
    { REC_SIZE * 1.7, REC_SIZE * 1.9 },
    { REC_SIZE * 1.2, REC_SIZE * 1.1 },
    { REC_SIZE * 1.1, REC_SIZE * 1.7 },
    { REC_SIZE * 1.6, REC_SIZE * 2.0 },
    { REC_SIZE * 1.7, REC_SIZE * 2.1 },
    { REC_SIZE * 1.8, REC_SIZE * 2.1 },
    { REC_SIZE * 1.7, REC_SIZE * 2.2 },
} };

void TretominoRenderShape::set_tretomino(Tretomino tretomino) {
    for (int i = 0; i < 4; i++) {
        int x_block = Tretominos::ALL.at(tretomino)[0].at(i).x;
        int y_block = Tretominos::ALL.at(tretomino)[0].at(i).y;

        float x_pos = static_cast<float>(x_block) * REC_SIZE +
                      TRETOMINO_RENDER_OFFSET.at(tretomino).x;
        float y_pos = static_cast<float>(y_block) * REC_SIZE +
                      TRETOMINO_RENDER_OFFSET.at(tretomino).y;

        shape.at(i).setPosition({ x_pos, y_pos });
        shape.at(i).setFillColor(Tretominos::ALL_COLORS.at(tretomino));
    }
}

void TretominoRenderShape::set_origin(sf::Vector2f origin) {
    for (sf::RectangleShape& cell : shape) {
        cell.setOrigin(origin);
    }
}

TretominoRenderShape::TretominoRenderShape() {
    for (sf::RectangleShape& shape : shape) {
        shape.setSize({
            REC_SIZE - BETWEEN_CELL_LINE_THICKNESS,
            REC_SIZE - BETWEEN_CELL_LINE_THICKNESS,
        });
        shape.setOutlineColor(BETWEEN_CELL_LINE_COLOR);
        shape.setOutlineThickness(BETWEEN_CELL_LINE_THICKNESS);
    }
};
